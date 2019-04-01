/*******************************************************************\

Module: Abstract Interpretation

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Abstract Interpretation

#ifndef CPROVER_ANALYSES_AI_H
#define CPROVER_ANALYSES_AI_H

#include <iosfwd>
#include <map>
#include <memory>

#include <util/json.h>
#include <util/xml.h>
#include <util/expr.h>
#include <util/make_unique.h>

#include <goto-programs/goto_model.h>

#include "ai_domain.h"
#include "is_threaded.h"

/// The basic interface of an abstract interpreter. This should be enough
/// to create, run and query an abstract interpreter.
///
/// Note: this is just a base class. \ref ait should be used instead.
class ai_baset
{
public:
  typedef ai_domain_baset statet;
  typedef goto_programt::const_targett locationt;

  ai_baset()
  {
  }

  virtual ~ai_baset()
  {
  }

  /// Run abstract interpretation on a single function
  void operator()(
    const irep_idt &function_id,
    const goto_programt &goto_program,
    const namespacet &ns)
  {
    goto_functionst goto_functions;
    initialize(function_id, goto_program);
    entry_state(goto_program);
    fixedpoint(function_id, goto_program, goto_functions, ns);
    finalize();
  }

  /// Run abstract interpretation on a whole program
  void operator()(
    const goto_functionst &goto_functions,
    const namespacet &ns)
  {
    initialize(goto_functions);
    entry_state(goto_functions);
    fixedpoint(goto_functions, ns);
    finalize();
  }

  /// Run abstract interpretation on a whole program
  void operator()(const goto_modelt &goto_model)
  {
    const namespacet ns(goto_model.symbol_table);
    initialize(goto_model.goto_functions);
    entry_state(goto_model.goto_functions);
    fixedpoint(goto_model.goto_functions, ns);
    finalize();
  }

  /// Run abstract interpretation on a single function
  void operator()(
    const irep_idt &function_id,
    const goto_functionst::goto_functiont &goto_function,
    const namespacet &ns)
  {
    goto_functionst goto_functions;
    initialize(function_id, goto_function);
    entry_state(goto_function.body);
    fixedpoint(function_id, goto_function.body, goto_functions, ns);
    finalize();
  }

  /// Get a copy of the abstract state before the given instruction, without
  /// needing to know what kind of domain or history is used. Note: intended
  /// for users of the abstract interpreter; derived classes should
  /// use \ref get_state to access the actual underlying state.
  /// PRECONDITION(l is dereferenceable)
  /// \param l: The location before which we want the abstract state
  /// \return The abstract state before `l`. We return a pointer to a copy as
  ///   the method should be const and there are some non-trivial cases
  ///   including merging abstract states, etc.
  virtual std::unique_ptr<statet> abstract_state_before(locationt l) const = 0;

  /// Get a copy of the abstract state after the given instruction, without
  /// needing to know what kind of domain or history is used. Note: intended
  /// for users of the abstract interpreter; derived classes should
  /// use \ref get_state to access the actual underlying state.
  /// \param l: The location before which we want the abstract state
  /// \return The abstract state after `l`. We return a pointer to a copy as
  ///   the method should be const and there are some non-trivial cases
  ///   including merging abstract states, etc.
  virtual std::unique_ptr<statet> abstract_state_after(locationt l) const
  {
    /// PRECONDITION(l is dereferenceable && std::next(l) is dereferenceable)
    /// Check relies on a DATA_INVARIANT of goto_programs
    INVARIANT(!l->is_end_function(), "No state after the last instruction");
    return abstract_state_before(std::next(l));
  }

  /// Reset the abstract state
  virtual void clear()
  {
  }

  /// Output the abstract states for a single function
  /// \param ns: The namespace
  /// \param function_id: The identifier used to find a symbol to
  ///   identify the \p goto_program's source language
  /// \param goto_program: The goto program
  /// \param out: The ostream to direct output to
  virtual void output(
    const namespacet &ns,
    const irep_idt &function_id,
    const goto_programt &goto_program,
    std::ostream &out) const;

  /// Output the abstract states for a whole program
  virtual void output(
    const namespacet &ns,
    const goto_functionst &goto_functions,
    std::ostream &out) const;

  /// Output the abstract states for a whole program
  void output(
    const goto_modelt &goto_model,
    std::ostream &out) const
  {
    const namespacet ns(goto_model.symbol_table);
    output(ns, goto_model.goto_functions, out);
  }

  /// Output the abstract states for a function
  void output(
    const namespacet &ns,
    const goto_functionst::goto_functiont &goto_function,
    std::ostream &out) const
  {
    output(ns, "", goto_function.body, out);
  }

  /// Output the abstract states for the whole program as JSON
  virtual jsont output_json(
    const namespacet &ns,
    const goto_functionst &goto_functions) const;

  /// Output the abstract states for a whole program as JSON
  jsont output_json(
    const goto_modelt &goto_model) const
  {
    const namespacet ns(goto_model.symbol_table);
    return output_json(ns, goto_model.goto_functions);
  }

  /// Output the abstract states for a single function as JSON
  jsont output_json(
    const namespacet &ns,
    const goto_programt &goto_program) const
  {
    return output_json(ns, "", goto_program);
  }

  /// Output the abstract states for a single function as JSON
  jsont output_json(
    const namespacet &ns,
    const goto_functionst::goto_functiont &goto_function) const
  {
    return output_json(ns, "", goto_function.body);
  }

  /// Output the abstract states for the whole program as XML
  virtual xmlt output_xml(
    const namespacet &ns,
    const goto_functionst &goto_functions) const;

  /// Output the abstract states for the whole program as XML
  xmlt output_xml(
    const goto_modelt &goto_model) const
  {
    const namespacet ns(goto_model.symbol_table);
    return output_xml(ns, goto_model.goto_functions);
  }

  /// Output the abstract states for a single function as XML
  xmlt output_xml(
    const namespacet &ns,
    const goto_programt &goto_program) const
  {
    return output_xml(ns, "", goto_program);
  }

  /// Output the abstract states for a single function as XML
  xmlt output_xml(
    const namespacet &ns,
    const goto_functionst::goto_functiont &goto_function) const
  {
    return output_xml(ns, "", goto_function.body);
  }

protected:
  /// Initialize all the abstract states for a single function. Override this to
  /// do custom per-domain initialization.
  virtual void
  initialize(const irep_idt &function_id, const goto_programt &goto_program);

  /// Initialize all the abstract states for a single function.
  virtual void initialize(
    const irep_idt &function_id,
    const goto_functionst::goto_functiont &goto_function);

  /// Initialize all the abstract states for a whole program. Override this to
  /// do custom per-analysis initialization.
  virtual void initialize(const goto_functionst &goto_functions);

  /// Override this to add a cleanup or post-processing step after fixedpoint
  /// has run
  virtual void finalize();

  /// Set the abstract state of the entry location of a single function to the
  /// entry state required by the analysis
  void entry_state(const goto_programt &goto_program);

  /// Set the abstract state of the entry location of a whole program to the
  /// entry state required by the analysis
  void entry_state(const goto_functionst &goto_functions);

  /// Output the abstract states for a single function as JSON
  /// \param ns: The namespace
  /// \param goto_program: The goto program
  /// \param function_id: The identifier used to find a symbol to
  ///   identify the source language
  /// \return The JSON object
  virtual jsont output_json(
    const namespacet &ns,
    const irep_idt &function_id,
    const goto_programt &goto_program) const;

  /// Output the abstract states for a single function as XML
  /// \param ns: The namespace
  /// \param goto_program: The goto program
  /// \param function_id: The identifier used to find a symbol to
  ///   identify the source language
  /// \return The XML object
  virtual xmlt output_xml(
    const namespacet &ns,
    const irep_idt &function_id,
    const goto_programt &goto_program) const;

  /// The work queue, sorted by location number
  typedef std::map<unsigned, locationt> working_sett;

  /// Get the next location from the work queue
  locationt get_next(working_sett &working_set);

  void put_in_working_set(
    working_sett &working_set,
    locationt l)
  {
    working_set.insert(
      std::pair<unsigned, locationt>(l->location_number, l));
  }

  /// Run the fixedpoint algorithm until it reaches a fixed point
  /// \return True if we found something new
  bool fixedpoint(
    const irep_idt &function_id,
    const goto_programt &goto_program,
    const goto_functionst &goto_functions,
    const namespacet &ns);

  virtual void fixedpoint(
    const goto_functionst &goto_functions,
    const namespacet &ns);

  /// Perform one step of abstract interpretation from location l
  /// Depending on the instruction type it may compute a number of "edges"
  /// or applications of the abstract transformer
  /// \return True if the state was changed
  bool visit(
    const irep_idt &function_id,
    locationt l,
    working_sett &working_set,
    const goto_programt &goto_program,
    const goto_functionst &goto_functions,
    const namespacet &ns);

  // The most basic step, computing one edge / transformer application.
  bool visit_edge(
    const irep_idt &function_id,
    locationt l,
    const irep_idt &to_function_id,
    const locationt &to_l,
    const namespacet &ns,
    working_sett &working_set);

  // function calls
  bool do_function_call_rec(
    const irep_idt &calling_function_id,
    locationt l_call,
    locationt l_return,
    const exprt &function,
    const exprt::operandst &arguments,
    const goto_functionst &goto_functions,
    const namespacet &ns);

  bool do_function_call(
    const irep_idt &calling_function_id,
    locationt l_call,
    locationt l_return,
    const goto_functionst &goto_functions,
    const goto_functionst::function_mapt::const_iterator f_it,
    const exprt::operandst &arguments,
    const namespacet &ns);

  // abstract methods

  virtual bool merge(const statet &src, locationt from, locationt to)=0;

  /// Get the state for the given location, creating it in a default way if it
  /// doesn't exist
  virtual statet &get_state(locationt l)=0;

  /// Make a copy of a state
  virtual std::unique_ptr<statet> make_temporary_state(const statet &s)=0;
};

/// Base class for abstract interpretation. An actual analysis
/// must (a) inherit from this class and (b) provide a domain class as a
/// type argument, which must, in turn, inherit from \ref ai_domain_baset.
///
/// From a user's perspective, this class provides three main groups of
/// functions:
///
/// 1. Running an analysis, via
///    \ref ai_baset#operator()(const irep_idt&,const goto_programt&, <!--
///    --> const namespacet&),
///    \ref ai_baset#operator()(const goto_functionst&,const namespacet&)
///    and \ref ai_baset#operator()(const goto_modelt&)
/// 2. Accessing the results of an analysis, by looking up the result
///    for a given location \p l using
///    \ref ait#operator[](goto_programt::const_targett).
/// 3. Outputting the results of the analysis; see
///    \ref ai_baset#output(const namespacet&, const irep_idt&,
///    const goto_programt&, std::ostream&)const et cetera.
///
/// A typical usage pattern would be to call the analysis first,
/// and use `operator[]` afterwards to retrieve the results. The fixed
/// point algorithm used is a standard worklist algorithm; the current
/// implementation is flow- and path-sensitive, but not context-sensitive.
///
/// From an analysis developer's perspective, an analysis is implemented by
/// inheriting from this class (or, if a concurrency-sensitive analysis is
/// required, from \ref concurrency_aware_ait), providing a class implementing
/// the abstract domain as the type for the \p domainT parameter. Most of the
/// actual analysis functions (in particular, the minimal element, the lattice
/// join, and the state transformer) are supplied using \p domainT.
///
/// To control the analysis in more detail, you can also override the following
/// methods:
/// - \ref ait#initialize(const irep_idt&, const goto_programt&),
///   \ref ait#initialize(const irep_idt&,
///   const goto_functionst::goto_functiont&) and
///   \ref ait#initialize(const goto_functionst&), for pre-analysis
///   initialization
/// - \ref ait#finalize(), for post-analysis cleanup.
///
template<typename domainT>
class ait:public ai_baset
{
public:
  // constructor
  ait():ai_baset()
  {
  }

  typedef goto_programt::const_targett locationt;

  /// Find the analysis result for a given location.
  domainT &operator[](locationt l)
  {
    typename state_mapt::iterator it=state_map.find(l);
    if(it==state_map.end())
      throw std::out_of_range("failed to find state");

    return it->second;
  }

  /// Find the analysis result for a given location.
  const domainT &operator[](locationt l) const
  {
    typename state_mapt::const_iterator it=state_map.find(l);
    if(it==state_map.end())
      throw std::out_of_range("failed to find state");

    return it->second;
  }

  /// Used internally by the analysis.
  std::unique_ptr<statet> abstract_state_before(locationt t) const override
  {
    typename state_mapt::const_iterator it = state_map.find(t);
    if(it == state_map.end())
    {
      std::unique_ptr<statet> d = util_make_unique<domainT>();
      CHECK_RETURN(d->is_bottom());
      return d;
    }

    return util_make_unique<domainT>(it->second);
  }

  /// Remove all analysis results.
  void clear() override
  {
    state_map.clear();
    ai_baset::clear();
  }

protected:
  /// Map from locations to domain elements, for the results of a static
  /// analysis.
  typedef std::
    unordered_map<locationt, domainT, const_target_hash, pointee_address_equalt>
      state_mapt;
  state_mapt state_map;

  /// Look up the analysis state for a given location, instantiating a new state
  /// if required. Used internally by the analysis.
  virtual statet &get_state(locationt l) override
  {
    return state_map[l]; // calls default constructor
  }

  /// Merge the state \p src, flowing from location \p from to
  /// location \p to, into the state currently stored for location \p to.
  bool merge(const statet &src, locationt from, locationt to) override
  {
    statet &dest=get_state(to);
    return static_cast<domainT &>(dest).merge(
      static_cast<const domainT &>(src), from, to);
  }

  /// Make a copy of \p s.
  std::unique_ptr<statet> make_temporary_state(const statet &s) override
  {
    return util_make_unique<domainT>(static_cast<const domainT &>(s));
  }

private:
  /// This function exists to enforce that `domainT` is derived from
  /// \ref ai_domain_baset
  void dummy(const domainT &s) { const statet &x=s; (void)x; }
};

/// Base class for concurrency-aware abstract interpretation. See
/// \ref ait for details.
/// The only difference is that after the sequential fixed point construction,
/// as done by \ref ait, another step is added to account for
/// concurrently-executed instructions.
/// Basically, it makes the analysis flow-insensitive by feeding results of a
/// sequential execution back into the entry point, thereby accounting for any
/// values computed by different threads. Compare
/// [Martin Rinard, "Analysis of Multi-Threaded Programs", SAS 2001](
/// http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.28.4747&<!--
/// -->rep=rep1&type=pdf).
template<typename domainT>
class concurrency_aware_ait:public ait<domainT>
{
public:
  using statet = typename ait<domainT>::statet;
  using locationt = typename statet::locationt;

  // constructor
  concurrency_aware_ait():ait<domainT>()
  {
  }

  virtual bool merge_shared(
    const statet &src,
    locationt from,
    locationt to,
    const namespacet &ns)
  {
    statet &dest=this->get_state(to);
    return static_cast<domainT &>(dest).merge_shared(
      static_cast<const domainT &>(src), from, to, ns);
  }

protected:
  using working_sett = ai_baset::working_sett;

  void fixedpoint(
    const goto_functionst &goto_functions,
    const namespacet &ns) override
  {
    ai_baset::fixedpoint(goto_functions, ns);

    is_threadedt is_threaded(goto_functions);

    // construct an initial shared state collecting the results of all
    // functions
    goto_programt tmp;
    tmp.add_instruction();
    goto_programt::const_targett sh_target = tmp.instructions.begin();
    statet &shared_state = ait<domainT>::get_state(sh_target);

    struct wl_entryt
    {
      wl_entryt(
        const irep_idt &_function_id,
        const goto_programt &_goto_program,
        locationt _location)
        : function_id(_function_id),
          goto_program(&_goto_program),
          location(_location)
      {
      }

      irep_idt function_id;
      const goto_programt *goto_program;
      locationt location;
    };

    typedef std::list<wl_entryt> thread_wlt;
    thread_wlt thread_wl;

    forall_goto_functions(it, goto_functions)
      forall_goto_program_instructions(t_it, it->second.body)
      {
        if(is_threaded(t_it))
        {
          thread_wl.push_back(wl_entryt(it->first, it->second.body, t_it));

          goto_programt::const_targett l_end =
            it->second.body.instructions.end();
          --l_end;

          merge_shared(shared_state, l_end, sh_target, ns);
        }
      }

    // now feed in the shared state into all concurrently executing
    // functions, and iterate until the shared state stabilizes
    bool new_shared = true;
    while(new_shared)
    {
      new_shared = false;

      for(const auto &wl_entry : thread_wl)
      {
        working_sett working_set;
        ai_baset::put_in_working_set(working_set, wl_entry.location);

        statet &begin_state = ait<domainT>::get_state(wl_entry.location);
        ait<domainT>::merge(begin_state, sh_target, wl_entry.location);

        while(!working_set.empty())
        {
          goto_programt::const_targett l = ai_baset::get_next(working_set);

          ai_baset::visit(
            wl_entry.function_id,
            l,
            working_set,
            *(wl_entry.goto_program),
            goto_functions,
            ns);

          // the underlying domain must make sure that the final state
          // carries all possible values; otherwise we would need to
          // merge over each and every state
          if(l->is_end_function())
            new_shared |= merge_shared(shared_state, l, sh_target, ns);
        }
      }
    }
  }
};

#endif // CPROVER_ANALYSES_AI_H
