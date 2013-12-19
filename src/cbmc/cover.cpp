/*******************************************************************\

Module: Vacuity Checks

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <iostream>

#include <util/xml.h>
#include <util/std_expr.h>

#include <solvers/sat/satcheck.h>
#include <solvers/prop/cover_goals.h>

#include "bmc.h"
#include "bv_cbmc.h"

/*******************************************************************\

Function: bmct::cover_assertions

  Inputs:

 Outputs:

 Purpose: Try to cover all goals

\*******************************************************************/

void bmct::cover_assertions(const goto_functionst &goto_functions)
{
  satcheckt satcheck;
  
  satcheck.set_message_handler(get_message_handler());
  satcheck.set_verbosity(get_verbosity());
  
  bv_cbmct bv_cbmc(ns, satcheck);
  bv_cbmc.set_message_handler(get_message_handler());
  bv_cbmc.set_verbosity(get_verbosity());
  
  if(options.get_option("arrays-uf")=="never")
    bv_cbmc.unbounded_array=bv_cbmct::U_NONE;
  else if(options.get_option("arrays-uf")=="always")
    bv_cbmc.unbounded_array=bv_cbmct::U_ALL;

  prop_convt &prop_conv=bv_cbmc;

  // convert

  equation.convert_guards(prop_conv);
  equation.convert_assignments(prop_conv);
  equation.convert_decls(prop_conv);
  equation.convert_assumptions(prop_conv);

  // collect _all_ goals in `goal_map'
  typedef std::map<goto_programt::const_targett, exprt::operandst> goal_mapt;
  goal_mapt goal_map;
  
  forall_goto_functions(f_it, goto_functions)
    forall_goto_program_instructions(i_it, f_it->second.body)
      if(i_it->is_assert())
        goal_map[i_it]=exprt::operandst();

  // get the conditions for these goals from formula

  exprt assumption=true_exprt();

  for(symex_target_equationt::SSA_stepst::iterator
      it=equation.SSA_steps.begin();
      it!=equation.SSA_steps.end();
      it++)
  {
    if(it->is_assert())
    {
      // We just want reachability, i.e., the guard of the instruction,
      // not the assertion itself. The guard has been converted above.
      exprt goal=and_exprt(assumption, literal_exprt(it->guard_literal));

      goal_map[it->source.pc].push_back(goal);
    }
    else if(it->is_assume())
    {
      // Assumptions have been converted above.
      assumption=
        and_exprt(assumption, literal_exprt(it->cond_literal));
    }
  }
  
  // try to cover those
  cover_goalst cover_goals(prop_conv);
  cover_goals.set_message_handler(get_message_handler());
  cover_goals.set_verbosity(get_verbosity());

  for(goal_mapt::const_iterator
      it=goal_map.begin();
      it!=goal_map.end();
      it++)
  {
    // the following is FALSE if the bv is empty
    literalt condition=prop_conv.convert(disjunction(it->second));
    cover_goals.add(condition);
  }

  status() << "Total number of coverage goals: " << cover_goals.size() << eom;

  cover_goals();

  // report
  std::list<cover_goalst::cover_goalt>::const_iterator g_it=
    cover_goals.goals.begin();
      
  for(goal_mapt::const_iterator
      it=goal_map.begin();
      it!=goal_map.end();
      it++, g_it++)
  {
    if(ui==ui_message_handlert::XML_UI)
    {
      xmlt xml_result("result");
      xml_result.set_attribute("claim", id2string(it->first->location.get_claim()));

      xml_result.set_attribute("status",
        g_it->covered?"COVERED":"NOT_COVERED");
      
      std::cout << xml_result << "\n";
    }
    else
    {
      if(!g_it->covered)
        warning() << "!! failed to cover " << it->first->location;
    }
  }

  status() << eom;

  status() << "** Covered " << cover_goals.number_covered()
           << " of " << cover_goals.size() << " in "
           << cover_goals.iterations() << " iterations" << eom;
}
