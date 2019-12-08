/*******************************************************************\

Module: Abstract Interpretation

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Abstract Interpretation

#include "ai.h"

#include <cassert>
#include <memory>
#include <sstream>

#include <util/invariant.h>
#include <util/std_code.h>
#include <util/std_expr.h>

#include "is_threaded.h"

void ai_baset::output(
  const namespacet &ns,
  const goto_functionst &goto_functions,
  std::ostream &out) const
{
  forall_goto_functions(f_it, goto_functions)
  {
    if(f_it->second.body_available())
    {
      out << "////\n";
      out << "//// Function: " << f_it->first << "\n";
      out << "////\n";
      out << "\n";

      output(ns, f_it->first, f_it->second.body, out);
    }
  }
}

void ai_baset::output(
  const namespacet &ns,
  const irep_idt &function_id,
  const goto_programt &goto_program,
  std::ostream &out) const
{
  forall_goto_program_instructions(i_it, goto_program)
  {
    out << "**** " << i_it->location_number << " "
        << i_it->source_location << "\n";

    abstract_state_before(i_it)->output(out, *this, ns);
    out << "\n";
#if 1
    goto_program.output_instruction(ns, function_id, out, *i_it);
    out << "\n";
#endif
  }
}

jsont ai_baset::output_json(
  const namespacet &ns,
  const goto_functionst &goto_functions) const
{
  json_objectt result;

  forall_goto_functions(f_it, goto_functions)
  {
    if(f_it->second.body_available())
    {
      result[id2string(f_it->first)] =
        output_json(ns, f_it->first, f_it->second.body);
    }
    else
    {
      result[id2string(f_it->first)] = json_arrayt();
    }
  }

  return std::move(result);
}

jsont ai_baset::output_json(
  const namespacet &ns,
  const irep_idt &function_id,
  const goto_programt &goto_program) const
{
  json_arrayt contents;

  forall_goto_program_instructions(i_it, goto_program)
  {
    // Ideally we need output_instruction_json
    std::ostringstream out;
    goto_program.output_instruction(ns, function_id, out, *i_it);

    json_objectt location{
      {"locationNumber", json_numbert(std::to_string(i_it->location_number))},
      {"sourceLocation", json_stringt(i_it->source_location.as_string())},
      {"abstractState", abstract_state_before(i_it)->output_json(*this, ns)},
      {"instruction", json_stringt(out.str())}};

    contents.push_back(std::move(location));
  }

  return std::move(contents);
}

xmlt ai_baset::output_xml(
  const namespacet &ns,
  const goto_functionst &goto_functions) const
{
  xmlt program("program");

  forall_goto_functions(f_it, goto_functions)
  {
    xmlt function(
      "function",
      {{"name", id2string(f_it->first)},
       {"body_available", f_it->second.body_available() ? "true" : "false"}},
      {});

    if(f_it->second.body_available())
    {
      function.new_element(output_xml(ns, f_it->first, f_it->second.body));
    }

    program.new_element(function);
  }

  return program;
}

xmlt ai_baset::output_xml(
  const namespacet &ns,
  const irep_idt &function_id,
  const goto_programt &goto_program) const
{
  xmlt function_body;

  forall_goto_program_instructions(i_it, goto_program)
  {
    xmlt location(
      "",
      {{"location_number", std::to_string(i_it->location_number)},
       {"source_location", i_it->source_location.as_string()}},
      {abstract_state_before(i_it)->output_xml(*this, ns)});

    // Ideally we need output_instruction_xml
    std::ostringstream out;
    goto_program.output_instruction(ns, function_id, out, *i_it);
    location.set_attribute("instruction", out.str());

    function_body.new_element(location);
  }

  return function_body;
}

void ai_baset::entry_state(const goto_functionst &goto_functions)
{
  // find the 'entry function'

  goto_functionst::function_mapt::const_iterator f_it =
    goto_functions.function_map.find(goto_functions.entry_point());

  if(f_it != goto_functions.function_map.end())
    entry_state(f_it->second.body);
}

void ai_baset::entry_state(const goto_programt &goto_program)
{
  // The first instruction of 'goto_program' is the entry point
  get_state(goto_program.instructions.begin()).make_entry();
}

void ai_baset::initialize(
  const irep_idt &function_id,
  const goto_functionst::goto_functiont &goto_function)
{
  initialize(function_id, goto_function.body);
}

void ai_baset::initialize(const irep_idt &, const goto_programt &goto_program)
{
  // we mark everything as unreachable as starting point

  forall_goto_program_instructions(i_it, goto_program)
    get_state(i_it).make_bottom();
}

void ai_baset::initialize(const goto_functionst &goto_functions)
{
  progress_data.num_functions = goto_functions.function_map.size();

  forall_goto_functions(it, goto_functions)
    initialize(it->first, it->second);
}

void ai_baset::finalize()
{
  // Nothing to do per default
}

ai_baset::locationt ai_baset::get_next(working_sett &working_set)
{
  PRECONDITION(!working_set.empty());

  working_sett::iterator i = working_set.begin();
  locationt l = i->second;
  working_set.erase(i);

  return l;
}

bool ai_baset::fixedpoint(
  const irep_idt &function_id,
  const goto_programt &goto_program,
  const goto_functionst &goto_functions,
  const namespacet &ns)
{
  working_sett working_set;

  // Put the first location in the working set
  if(!goto_program.empty())
    put_in_working_set(
      working_set,
      goto_program.instructions.begin());

  bool new_data = false;

  while(!working_set.empty())
  {
    locationt l = get_next(working_set);

    // goto_program is really only needed for iterator manipulation
    if(visit(function_id, l, working_set, goto_program, goto_functions, ns))
      new_data = true;
  }

  return new_data;
}

bool ai_baset::visit(
  const irep_idt &function_id,
  locationt l,
  working_sett &working_set,
  const goto_programt &goto_program,
  const goto_functionst &goto_functions,
  const namespacet &ns)
{
  bool new_data = false;

  // Function call and end are special cases
  if(l->is_function_call() && !goto_functions.function_map.empty())
  {
    DATA_INVARIANT(
      goto_program.get_successors(l).size() == 1,
      "function calls only have one successor");
    DATA_INVARIANT(
      *(goto_program.get_successors(l).begin()) == std::next(l),
      "function call successor / return location must be the next instruction");

    const code_function_callt &code = to_code_function_call(l->code);

    locationt to_l = std::next(l);
    if(do_function_call_rec(
         function_id,
         l,
         to_l,
         code.function(),
         code.arguments(),
         goto_functions,
         ns))
    {
      new_data = true;
      put_in_working_set(working_set, to_l);
    }
  }
  else if(l->is_end_function())
  {
    DATA_INVARIANT(
      goto_program.get_successors(l).empty(),
      "The end function instruction should have no successors.");

    // Do nothing
  }
  else
  {
    // Successors can be empty, for example assume(0).
    // Successors can contain duplicates, for example GOTO next;
    for(const auto &to_l : goto_program.get_successors(l))
    {
      if(to_l == goto_program.instructions.end())
        continue;

      new_data |=
        visit_edge(function_id, l, function_id, to_l, ns, working_set);
    }
  }

  return new_data;
}

bool ai_baset::visit_edge(
  const irep_idt &function_id,
  locationt l,
  const irep_idt &to_function_id,
  const locationt &to_l,
  const namespacet &ns,
  working_sett &working_set)
{
  // Abstract domains are mutable so we must copy before we transform
  statet &current = get_state(l);

  std::unique_ptr<statet> tmp_state(make_temporary_state(current));
  statet &new_values = *tmp_state;

  // Apply transformer
  new_values.transform(function_id, l, to_function_id, to_l, *this, ns);

  // Initialize state(s), if necessary
  get_state(to_l);

  if(merge(new_values, l, to_l))
  {
    put_in_working_set(working_set, to_l);
    return true;
  }

  return false;
}

bool ai_baset::do_function_call(
  const irep_idt &calling_function_id,
  locationt l_call,
  locationt l_return,
  const goto_functionst &goto_functions,
  const goto_functionst::function_mapt::const_iterator f_it,
  const exprt::operandst &,
  const namespacet &ns)
{
  PRECONDITION(l_call->is_function_call());

  const goto_functionst::goto_functiont &goto_function = f_it->second;

  // This branch is for legacy support only and is deprecated

  if(!goto_function.body_available())
  {
    working_sett working_set; // Redundant; visit will add l_return

    // If we don't have a body, we just do an edge call -> return
    return visit_edge(
      calling_function_id,
      l_call,
      calling_function_id,
      l_return,
      ns,
      working_set);
  }

  DATA_INVARIANT(
    !goto_function.body.instructions.empty(),
    "By analysis time, all functions should have bodies");

  // This is the edge from call site to function head.

  {
    locationt l_begin = goto_function.body.instructions.begin();

    working_sett working_set; // Redundant; fixpoint will add l_begin

    // Do the edge from the call site to the beginning of the function
    bool new_data = visit_edge(
      calling_function_id, l_call, f_it->first, l_begin, ns, working_set);

    // do we need to do/re-do the fixedpoint of the body?
    if(new_data)
    {
      const code_function_callt &code=
        to_code_function_call(l_call->code);

      const symbol_exprt symbol_expr = to_symbol_expr(code.function());

      const irep_idt id = symbol_expr.get_identifier();

      progress_data.functions_entered.insert(id);
      progress_data.num_functions_entered++;
      progress_data.stack.push_front(id);

      progress_data.max_stack_depth
        = std::max(progress_data.max_stack_depth, progress_data.stack.size());

      print_progress_interval(ns);

      fixedpoint(f_it->first, goto_function.body, goto_functions, ns);

      progress_data.functions_returned.insert(id);
      progress_data.num_functions_returned++;
      progress_data.stack.pop_front();

      print_progress_interval(ns);
    }
  }

  // This is the edge from function end to return site.

  {
    // get location at end of the procedure we have called
    const locationt l_end = --goto_function.body.instructions.end();
    assert(l_end->is_end_function());

    // do edge from end of function to instruction after call
    const statet &end_state = get_state(l_end);

    if(end_state.is_bottom())
      return false; // function exit point not reachable

    working_sett working_set; // Redundant; visit will add l_return

    const std::unique_ptr<statet> tmp_state(make_temporary_state(end_state));
    tmp_state->transform(f_it->first, l_end, f_it->first, l_return, *this, ns);

    const std::unique_ptr<statet> pre_merge_state{
      make_temporary_state(*tmp_state)};

    const locationt l_begin = goto_function.body.instructions.begin();
    tmp_state->merge_three_way_function_return(
      get_state(l_call), get_state(l_begin), *pre_merge_state, ns);

    return merge(*tmp_state, l_end, l_return);
  }
}

bool ai_baset::do_function_call_rec(
  const irep_idt &calling_function_id,
  locationt l_call,
  locationt l_return,
  const exprt &function,
  const exprt::operandst &arguments,
  const goto_functionst &goto_functions,
  const namespacet &ns)
{
  PRECONDITION(!goto_functions.function_map.empty());

  // This is quite a strong assumption on the well-formedness of the program.
  // It means function pointers must be removed before use.
  DATA_INVARIANT(
    function.id() == ID_symbol,
    "Function pointers and indirect calls must be removed before analysis.");

  const irep_idt &identifier = to_symbol_expr(function).get_identifier();

  goto_functionst::function_mapt::const_iterator it =
    goto_functions.function_map.find(identifier);

  DATA_INVARIANT(
    it != goto_functions.function_map.end(),
    "Function " + id2string(identifier) + "not in function map");

  return do_function_call(
    calling_function_id, l_call, l_return, goto_functions, it, arguments, ns);
}

void ai_baset::sequential_fixedpoint(
  const goto_functionst &goto_functions,
  const namespacet &ns)
{
  goto_functionst::function_mapt::const_iterator f_it =
    goto_functions.function_map.find(goto_functions.entry_point());

  if(f_it != goto_functions.function_map.end())
    fixedpoint(f_it->first, f_it->second.body, goto_functions, ns);
}

void ai_baset::concurrent_fixedpoint(
  const goto_functionst &goto_functions,
  const namespacet &ns)
{
  sequential_fixedpoint(goto_functions, ns);

  is_threadedt is_threaded(goto_functions);

  // construct an initial shared state collecting the results of all
  // functions
  goto_programt tmp;
  tmp.add_instruction();
  goto_programt::const_targett sh_target = tmp.instructions.begin();
  statet &shared_state = get_state(sh_target);

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

        goto_programt::const_targett l_end = it->second.body.instructions.end();
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
      put_in_working_set(working_set, wl_entry.location);

      statet &begin_state = get_state(wl_entry.location);
      merge(begin_state, sh_target, wl_entry.location);

      while(!working_set.empty())
      {
        goto_programt::const_targett l = get_next(working_set);

        visit(
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

void ai_baset::print_progress() const
{
  static const std::size_t num_width = 16;
  static const std::size_t frame_width = 4;

  progress() << "Progress:"

    << "\nNumber of functions in program:           "
    << std::setw(num_width)
    << progress_data.num_functions

    << "\nNumber of unique functions entered:       "
    << std::setw(num_width)
    << progress_data.functions_entered.size()

    << "\nNumber of unique functions returned from: "
    << std::setw(num_width)
    << progress_data.functions_returned.size()

    << "\nNumber of functions entered:              "
    << std::setw(num_width)
    << progress_data.num_functions_entered

    << "\nNumber of functions returned from:        "
    << std::setw(num_width)
    << progress_data.num_functions_returned

    << "\nCurrent stack depth:                      "
    << std::setw(num_width)
    << progress_data.stack.size()

    << "\nMaximum stack depth:                      "
    << std::setw(num_width)
    << progress_data.max_stack_depth;

  // Print stack, most recent frame first
  progress() << "\nAnalysis stack:\n";
  std::size_t frame_idx = 0;

  for(const irep_idt &id : progress_data.stack)
  {
    progress() << "[" << std::setw(frame_width) << frame_idx << "] " << id <<
      "\n";
    frame_idx++;
  }

  progress() << std::flush;
}

void ai_baset::print_progress_interval(const namespacet &ns) const
{
  using namespace std::chrono;

  system_clock::time_point now = system_clock::now();
  auto diff = now - last_progress_output;
  if(diff >= config.progress_interval)
  {
    last_progress_output = now;
    if(config.print_progress)
    {
      print_progress();
    }
    if(config.print_memory_usage)
    {
      print_memory_usage(ns);
    }
    if(config.print_string_container_statistics)
    {
#ifndef USE_STD_STRING
      get_string_container().compute_statistics().dump_on_stream(progress());
#endif
    }
  }
}

void ai_baset::print_memory_usage(const namespacet &ns) const
{
}

ai_configt ai_configt::from_options(const optionst &options)
{
  ai_configt result = {};
  result.print_progress = options.get_bool_option("vs-progress");
  if(options.is_set("vs-progress-interval"))
  {
    result.progress_interval = ai_configt::secondst(
      std::stof(options.get_option("vs-progress-interval")));
  }
  result.print_memory_usage =
    options.get_bool_option("vs-progress-memory-usage");
  result.print_string_container_statistics =
    options.get_bool_option("vs-progress-string-statistics");
  return result;
}

ai_configt &ai_configt::with_print_progress(bool print_progress)
{
  this->print_progress = print_progress;
  return *this;
}

ai_configt &
ai_configt::with_progress_interval(ai_configt::secondst progress_interval)
{
  this->progress_interval = progress_interval;
  return *this;
}

ai_configt &ai_configt::with_periodic_task(bool periodic_task)
{
  this->periodic_task = periodic_task;
  return *this;
}

ai_configt &
ai_configt::with_print_string_container_statistics(
  bool print_string_container_statistics)
{
  this->print_string_container_statistics = print_string_container_statistics;
  return *this;
}

bool is_same_code_location(
  const goto_programt::const_targett &x,
  const goto_programt::const_targett &y)
{
  // if either x or y are end iterators this won't work
  return &(*x) == &(*y);
}
