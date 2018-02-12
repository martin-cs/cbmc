/*******************************************************************\

Module: Range-based reaching definitions analysis (following Field-
        Sensitive Program Dependence Analysis, Litvak et al.,
        FSE 2010)

Author: Michael Tautschnig

  Date: February 2013

\*******************************************************************/

/// \file
/// Range-based reaching definitions analysis (following Field- Sensitive
///   Program Dependence Analysis, Litvak et al., FSE 2010)

#include "reaching_definitions.h"

#include <memory>

#include <util/pointer_offset_size.h>
#include <util/prefix.h>
#include <util/make_unique.h>


reaching_definitions_analysist::~reaching_definitions_analysist()=default;

//#define WITH_SCOPING
void rd_range_domaint::populate_cache(const irep_idt &identifier) const
{
  assert(bv_container);

  auto &r=values.find(identifier);

  if(!r.second)
    return;

  const values_innert &inner=r.first;

  if(inner.empty())
    return;

  ranges_at_loct &export_entry=export_cache[identifier];

  for(const auto &id : inner)
  {
    const reaching_definitiont &v=bv_container->get(id);

    export_entry[v.definition_at].insert(
      std::make_pair(v.bit_begin, v.bit_end));
  }
}

void rd_range_domaint::transform(
  locationt from,
  locationt to,
  ai_baset &ai,
  const namespacet &ns)
{
  reaching_definitions_analysist *rd=
    dynamic_cast<reaching_definitions_analysist*>(&ai);
  INVARIANT_STRUCTURED(
    rd!=nullptr,
    bad_cast_exceptiont,
    "ai has type reaching_definitions_analysist");

  assert(bv_container);

  // kill values
  if(from->is_dead())
    transform_dead(ns, from);
  // kill thread-local values
  else if(from->is_start_thread())
    transform_start_thread(ns, *rd);
  // do argument-to-parameter assignments
  else if(from->is_function_call())
    transform_function_call(ns, from, to, *rd);
  // cleanup parameters
  else if(from->is_end_function())
    transform_end_function(ns, from, to, *rd);
  // lhs assignments
  else if(from->is_assign())
    transform_assign(ns, from, from, *rd);
  // initial (non-deterministic) value
  else if(from->is_decl())
    transform_assign(ns, from, from, *rd);

#if 0
  // handle return values
  if(to->is_function_call())
  {
    const code_function_callt &code=to_code_function_call(to->code);

    if(code.lhs().is_not_nil())
    {
      rw_range_set_value_sett rw_set(ns, rd->get_value_sets());
      goto_rw(to, rw_set);
      const bool is_must_alias=rw_set.get_w_set().size()==1;

      forall_rw_range_set_w_objects(it, rw_set)
      {
        const irep_idt &identifier=it->first;
        // ignore symex::invalid_object
        const symbolt *symbol_ptr;
        if(ns.lookup(identifier, symbol_ptr))
          continue;
        assert(symbol_ptr!=0);

        const range_domaint &ranges=rw_set.get_ranges(it);

        if(is_must_alias &&
           (!rd->get_is_threaded()(from) ||
            (!symbol_ptr->is_shared() &&
             !rd->get_is_dirty()(identifier))))
          for(const auto &range : ranges)
            kill(identifier, range.first, range.second);
      }
    }
  }
#endif
}

void rd_range_domaint::transform_dead(
  const namespacet &ns,
  locationt from)
{
  const irep_idt &identifier=
    to_symbol_expr(to_code_dead(from->code).symbol()).get_identifier();

  values.erase(identifier);
  //export_cache.erase(identifier);
}

void rd_range_domaint::transform_start_thread(
  const namespacet &ns,
  reaching_definitions_analysist &rd)
{
  UNREACHABLE;  // Disabled for now

#if 0
  for(valuest::iterator it=values.begin();
      it!=values.end();
      ) // no ++it
  {
    const irep_idt &identifier=it->first;

    if(!ns.lookup(identifier).is_shared() &&
       !rd.get_is_dirty()(identifier))
    {
      export_cache.erase(identifier);

      valuest::iterator next=it;
      ++next;
      values.erase(it);
      it=next;
    }
    else
      ++it;
  }
#endif
}

void rd_range_domaint::transform_function_call(
  const namespacet &ns,
  locationt from,
  locationt to,
  reaching_definitions_analysist &rd)
{
  const code_function_callt &code=to_code_function_call(from->code);

  // only if there is an actual call, i.e., we have a body
  if(from->function != to->function)
  {
#ifdef WITH_SCOPING
    valuest::viewt view;
    values.get_view(view);

    for(const auto &p : view)
    {
      const irep_idt &identifier=p.first;

      const symbolt *sym;

      if((ns.lookup(identifier, sym) ||
         !sym->is_shared()) &&
         !rd.get_is_dirty()(identifier))
      {
        values.erase(identifier);
      }
    }
#endif

    const symbol_exprt &fn_symbol_expr=to_symbol_expr(code.function());
    const code_typet &code_type=
      to_code_type(ns.lookup(fn_symbol_expr.get_identifier()).type);

    for(const auto &param : code_type.parameters())
    {
      const irep_idt &identifier=param.get_identifier();

      if(identifier.empty())
        continue;

      range_spect size=
        to_range_spect(pointer_offset_bits(param.type(), ns));
      gen(from, identifier, 0, size);
    }
  }
  else
  {
    // handle return values of undefined functions
    const code_function_callt &code=to_code_function_call(from->code);

    if(code.lhs().is_not_nil())
      transform_assign(ns, from, from, rd);
  }
}

void rd_range_domaint::transform_end_function(
  const namespacet &ns,
  locationt from,
  locationt to,
  reaching_definitions_analysist &rd)
{
  goto_programt::const_targett call=to;
  --call;
  const code_function_callt &code=to_code_function_call(call->code);

#ifdef WITH_SCOPING
  valuest new_values;
  new_values.swap(values);
  values=rd[call].values;

  valuest::viewt view;
  new_values.get_view(view);

  for(const auto &new_value : view)
  {
    const irep_idt &identifier=new_value.first;

    if(!rd.get_is_threaded()(call) ||
       (!ns.lookup(identifier).is_shared() &&
        !rd.get_is_dirty()(identifier)))
    {
      for(const auto &id : new_value.second)
      {
        const reaching_definitiont &v=bv_container->get(id);
        kill(v.identifier, v.bit_begin, v.bit_end);
      }
    }

    for(const auto &id : new_value.second)
    {
      const reaching_definitiont &v=bv_container->get(id);
      gen(v.definition_at, v.identifier, v.bit_begin, v.bit_end);
    }
  }
#endif

  const code_typet &code_type=
    to_code_type(ns.lookup(from->function).type);

  for(const auto &param : code_type.parameters())
  {
    const irep_idt &identifier=param.get_identifier();

    if(identifier.empty())
      continue;

    values.erase(identifier);
    //export_cache.erase(identifier);
  }

  // handle return values
  if(code.lhs().is_not_nil())
  {
#if 0
    rd_range_domaint *rd_state=
      dynamic_cast<rd_range_domaint*>(&(rd.get_state(call)));
    assert(rd_state!=0);
    rd_state->
#endif
    transform_assign(ns, from, call, rd);
  }
}

void rd_range_domaint::transform_assign(
  const namespacet &ns,
  locationt from,
  locationt to,
  reaching_definitions_analysist &rd)
{
  rw_range_set_value_sett rw_set(ns, rd.get_value_sets());
  goto_rw(to, rd.get_goto_functions(), rw_set);
  const bool is_must_alias=rw_set.get_w_set().size()==1;

  forall_rw_range_set_w_objects(it, rw_set)
  {
    const irep_idt &identifier=it->first;
    // ignore symex::invalid_object
    const symbolt *symbol_ptr;
    if(ns.lookup(identifier, symbol_ptr))
      continue;
    INVARIANT_STRUCTURED(
      symbol_ptr!=nullptr,
      nullptr_exceptiont,
      "Symbol is in symbol table");

    const range_domaint &ranges=rw_set.get_ranges(it);

    if(is_must_alias &&
       (!rd.get_is_threaded()(from) ||
       (!symbol_ptr->is_shared() &&
       !rd.get_is_dirty()(identifier))))
      for(const auto &range : ranges)
      {
        const auto &r=range.second;
        kill(identifier, r.first, r.second);
      }

    for(const auto &range : ranges)
    {
      const auto &r=range.second;
      gen(from, identifier, r.first, r.second);
    }
  }
}

void rd_range_domaint::kill(
  const irep_idt &identifier,
  const range_spect &range_start,
  const range_spect &range_end)
{
  assert(range_start>=0);
  // -1 for objects of infinite/unknown size
  if(range_end==-1)
  {
    kill_inf(identifier, range_start);
    return;
  }

  assert(range_end>range_start);

  auto &r=values.find(identifier);
  if(!r.second)
    return;

  //bool clear_export_cache=false;

  values_innert &current_values=r.first;
  values_innert new_values;

  for(values_innert::iterator
      it=current_values.begin();
      it!=current_values.end();
     ) // no ++it
  {
    const reaching_definitiont &v=bv_container->get(*it);

    if(v.bit_begin>=range_end)
      ++it;
    else if(v.bit_end!=-1 &&
            v.bit_end<=range_start)
      ++it;
    else if(v.bit_begin>=range_start &&
            v.bit_end!=-1 &&
            v.bit_end<=range_end) // rs <= a < b <= re
    {
      //clear_export_cache=true;

      current_values.erase(it++);
    }
    else if(v.bit_begin >= range_start) // rs <= a <= re < b
    {
      //clear_export_cache=true;

      reaching_definitiont v_new=v;
      v_new.bit_begin=range_end;
      new_values.insert(bv_container->add(v_new));

      current_values.erase(it++);
    }
    else if(v.bit_end==-1 ||
            v.bit_end>range_end) // a <= rs < re < b
    {
      //clear_export_cache=true;

      reaching_definitiont v_new=v;
      v_new.bit_end=range_start;

      reaching_definitiont v_new2=v;
      v_new2.bit_begin=range_end;

      new_values.insert(bv_container->add(v_new));
      new_values.insert(bv_container->add(v_new2));

      current_values.erase(it++);
    }
    else // a <= rs < b <= re
    {
      //clear_export_cache=true;

      reaching_definitiont v_new=v;
      v_new.bit_end=range_start;
      new_values.insert(bv_container->add(v_new));

      current_values.erase(it++);
    }
  }

  //if(clear_export_cache)
  //  export_cache.erase(identifier);

  current_values.insert(new_values.begin(), new_values.end());
}

void rd_range_domaint::kill_inf(
  const irep_idt &identifier,
  const range_spect &range_start)
{
  assert(range_start>=0);

#if 0
  valuest::iterator entry=values.find(identifier);
  if(entry==values.end())
    return;

  // makes the analysis underapproximating
  rangest &ranges=entry->second;

  for(rangest::iterator it=ranges.begin();
      it!=ranges.end();
     ) // no ++it
    if(it->second.first!=-1 &&
       it->second.first <= range_start)
      ++it;
    else if(it->first >= range_start) // rs <= a < b <= re
    {
      ranges.erase(it++);
    }
    else // a <= rs < b < re
    {
      it->second.first=range_start;
      ++it;
    }
#endif
}

bool rd_range_domaint::gen(
  locationt from,
  const irep_idt &identifier,
  const range_spect &range_start,
  const range_spect &range_end)
{
  // objects of size 0 like union U { signed : 0; };
  if(range_start==0 && range_end==0)
    return false;

  assert(range_start>=0);

  // -1 for objects of infinite/unknown size
  assert(range_end>range_start || range_end==-1);

  reaching_definitiont v;
  v.identifier=identifier;
  v.definition_at=from;
  v.bit_begin=range_start;
  v.bit_end=range_end;

  size_t id=bv_container->add(v);

  const auto &r=values[identifier].insert(id);
  if(!r.second)
    return false;

  //export_cache.erase(identifier);

#if 0
  range_spect merged_range_end=range_end;

  std::pair<valuest::iterator, bool> entry=
    values.insert(std::make_pair(identifier, rangest()));
  rangest &ranges=entry.first->second;

  if(!entry.second)
  {
    for(rangest::iterator it=ranges.begin();
        it!=ranges.end();
        ) // no ++it
    {
      if(it->second.second!=from ||
         (it->second.first!=-1 && it->second.first <= range_start) ||
         (range_end!=-1 && it->first >= range_end))
        ++it;
      else if(it->first > range_start) // rs < a < b,re
      {
        if(range_end!=-1)
          merged_range_end=std::max(range_end, it->second.first);
        ranges.erase(it++);
      }
      else if(it->second.first==-1 ||
              (range_end!=-1 &&
               it->second.first >= range_end)) // a <= rs < re <= b
      {
        // nothing to do
        return false;
      }
      else // a <= rs < b < re
      {
        it->second.first=range_end;
        return true;
      }
    }
  }

  ranges.insert(std::make_pair(
      range_start,
      std::make_pair(merged_range_end, from)));
#endif

  return true;
}

void rd_range_domaint::output(std::ostream &out) const
{
  out << "Reaching definitions:\n";

  if(has_values.is_known())
  {
    out << has_values.to_string() << '\n';
    return;
  }

  valuest::viewt view;
  values.get_view(view);

  for(const auto &value : view)
  {
    const irep_idt &identifier=value.first;

    const ranges_at_loct &ranges=get(identifier);

    out << "  " << identifier << "[";

    for(ranges_at_loct::const_iterator itl=ranges.begin();
        itl!=ranges.end();
        ++itl)
    {
      for(rangest::const_iterator itr=itl->second.begin();
          itr!=itl->second.end();
          ++itr)
      {
        if(itr!=itl->second.begin() ||
           itl!=ranges.begin())
          out << ";";

        out << itr->first << ":" << itr->second;
        out << "@" << itl->first->location_number;
      }
    }

    out << "]\n";

    clear_cache(identifier);
  }
}

/// \return returns true iff there is something new
bool rd_range_domaint::merge(
  const rd_range_domaint &other,
  locationt from,
  locationt to)
{
  bool changed=false;

  if(other.has_values.is_false())
  {
    assert(other.values.empty());
    return false;
  }

  if(has_values.is_false())
  {
    assert(values.empty());
    values=other.values;
    assert(!other.has_values.is_true());
    has_values=other.has_values;
    return true;
  }
  rd_range_domaint &o=const_cast<rd_range_domaint &>(other);
  values.swap(o.values);
  valuest::delta_viewt delta_view;
  o.values.get_delta_view(values, delta_view);
  for(const auto &element : delta_view)
  {
    bool in_both=element.in_both;
    const irep_idt &k=element.k;
    const values_innert &inner_other=element.m; // in other
    const values_innert &inner=element.other_m; // in this

    if(!in_both)
    {
      values.insert(k, inner_other);
      changed=true;
    }
    else
    {
      if(inner!=inner_other)
      {
        auto &v=values.find(k);
        assert(v.second);

        values_innert &inner=v.first;

        size_t n=inner.size();
        inner.insert(inner_other.begin(), inner_other.end());
        if(inner.size()!=n)
          changed=true;
      }
    }
  }

  return changed;
}

/// \return returns true iff there is something new
bool rd_range_domaint::merge_shared(
  const rd_range_domaint &other,
  goto_programt::const_targett from,
  goto_programt::const_targett to,
  const namespacet &ns)
{
  UNREACHABLE;  // Disable for now
  return false;

#if 0
  // TODO: dirty vars
#if 0
  reaching_definitions_analysist *rd=
    dynamic_cast<reaching_definitions_analysist*>(&ai);
  assert(rd!=0);
#endif

  bool changed=has_values.is_false();
  has_values=tvt::unknown();

  valuest::iterator it=values.begin();
  for(const auto &value : other.values)
  {
    const irep_idt &identifier=value.first;

    if(!ns.lookup(identifier).is_shared()
       /*&& !rd.get_is_dirty()(identifier)*/)
      continue;

    while(it!=values.end() && it->first<value.first)
      ++it;
    if(it==values.end() || value.first<it->first)
    {
      values.insert(value);
      changed=true;
    }
    else if(it!=values.end())
    {
      assert(it->first==value.first);

      if(merge_inner(it->second, value.second))
      {
        changed=true;
        export_cache.erase(it->first);
      }

      ++it;
    }
  }

  return changed;
#endif
}

const rd_range_domaint::ranges_at_loct &rd_range_domaint::get(
  const irep_idt &identifier) const
{
  populate_cache(identifier);

  static ranges_at_loct empty;

  export_cachet::const_iterator entry=export_cache.find(identifier);

  if(entry==export_cache.end())
    return empty;
  else
    return entry->second;
}

void reaching_definitions_analysist::initialize(
  const goto_functionst &goto_functions)
{
  auto value_sets_=util_make_unique<value_set_analysis_fit>(ns);
  (*value_sets_)(goto_functions);
  value_sets=std::move(value_sets_);

  is_threaded=util_make_unique<is_threadedt>(goto_functions);

  is_dirty=util_make_unique<dirtyt>(goto_functions);

  concurrency_aware_ait<rd_range_domaint>::initialize(goto_functions);
}
