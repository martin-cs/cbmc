/*******************************************************************\

Module: Abstract Interpretation

Author: Martin Brain

\*******************************************************************/

#include <iostream>

#include "example_domain.h"

example_domaint::example_domaint()
{
  // This must set the domain to bottom i.e. "no possible values"
  make_bottom();
}

example_domaint::~example_domaint()
{
  // You probably don't need anything here
}

// Transform updates the domain with the effect of the instruction "from"
void example_domaint::transform(
  const irep_idt &function_from,
  trace_ptrt from,
  const irep_idt &function_to,
  trace_ptrt to,
  ai_baset &ai,
  const namespacet &ns)
{
  std::cerr << "Example domain @ 0x" << this << " transform using instruction "
            << from->current_location()->location_number << '\n';

  // If e is an exprt (an expression) then
  //   std::cerr << e.pretty()
  // prints it out

  // Normally how the state of the domain is updated would depend on
  // what the instruction was (see below).  However the state we are
  // storing is so simple that we can just...
  this->may_reach = true;

  const goto_programt::instructiont &instruction = *(from->current_location());
  switch(instruction.type)
  {
    /** These are the instructions you actually need to implement **/
  case DECL:
    // Creates a new variable to_code_decl(instruction.code).symbol()
    break;

  case DEAD:
    // Says that to_code_dead(instruction.code).symbol() is no longer needed
    break;

  case ASSIGN:
    // Set to_code_assign(instruction.code).lhs() to
    //     to_code_assign(instruction.code).rhs()
    break;

  case GOTO:
  {
    // Comparing iterators is safe as the target must be within the same list
    // of instructions because this is a GOTO.
    locationt next = from->current_location();
    next++;
    if(from->current_location()->get_target() != next) // If equal then effectively a SKIP
    {
      if(next == to->current_location())
      {
        // Branch is not taken
      }
      else
      {
        // Branch is taken
      }
    }
    break;
  }

  case FUNCTION_CALL:
  {
    // Function calls are a bit of a fiddle...
    // const code_function_callt &code_function_call =
    //   to_code_function_call(instruction.code);
    break;
  }

  case ASSUME:
    // It is safe to over-approximate these by ... ignoring them!
    break;

    /** These are instructions you really can ignore **/
  case ASSERT: // An assert is a check; they don't alter execution
    // If goto-analyzer is run with --verify they will be checked after fixpoint
    // by using the ai_simplify() method given below.
    break;

    // Mostly markers
  case LOCATION:     // No action required
  case SKIP:         // No action required
  case END_FUNCTION: // No action required
    break;

  case CATCH:
  case THROW:
    DATA_INVARIANT(false, "Exceptions must be removed before analysis");
    break;
  case RETURN:
    DATA_INVARIANT(false, "Returns must be removed before analysis");
    break;

  case ATOMIC_BEGIN: // Ignoring is a valid over-approximation
  case ATOMIC_END:   // Ignoring is a valid over-approximation
  case START_THREAD: // Require a concurrent analysis at higher level
  case END_THREAD:   // Require a concurrent analysis at higher level
    break;

  case OTHER:
#if 0
    DATA_INVARIANT(false, "Unclear what is a safe over-approximation of OTHER");
#endif
    break;

  case INCOMPLETE_GOTO:
  case NO_INSTRUCTION_TYPE:
    DATA_INVARIANT(false, "Only complete instructions can be analyzed");
    break;
  }

  return;
}

// Merges the contents of b into this
bool example_domaint::merge(const example_domaint &b, trace_ptrt, trace_ptrt)
{
  std::cerr << "Example domain @ 0x" << this << " merge in example domain @ 0x"
            << &b << '\n';

  bool old = this->may_reach;
  this->may_reach |= b.may_reach;

  // Returning true here has consequences!
  // If merge is true then the domain will be requeued and transformed again.
  // Fixpoint is reached when all domains return false when merged.
  return (old != this->may_reach);
}

void example_domaint::make_bottom()
{
  this->may_reach = false;
}

void example_domaint::make_top()
{
  this->may_reach = true;
}

void example_domaint::make_entry()
{
  // This is fine for almost everyone
  make_top();
}

bool example_domaint::is_bottom() const
{
  return may_reach == false;
}

bool example_domaint::is_top() const
{
  return may_reach == true;
}

void example_domaint::output(
  std::ostream &out,
  const ai_baset &ai,
  const namespacet &ns) const
{
  out << "Example domain (" << may_reach << ") @ 0x" << this;
  return;
}
