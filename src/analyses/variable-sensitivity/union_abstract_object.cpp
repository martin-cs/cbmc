/*******************************************************************\

 Module: analyses variable-sensitivity

 Author: Fotis Koutoulakis, fotis.koutoulakis@diffblue.com

\*******************************************************************/

#include <analyses/variable-sensitivity/abstract_enviroment.h>
#include <util/namespace.h>
#include <util/std_expr.h>

#include "union_abstract_object.h"

/*******************************************************************\

Function: union_abstract_objectt::union_abstract_objectt

  Inputs:
   type - the type the abstract_object is representing

 Outputs:

 Purpose:

\*******************************************************************/

union_abstract_objectt::union_abstract_objectt(const typet &type)
  : abstract_objectt(type)
{
  PRECONDITION(type.id() == ID_union);
}

/*******************************************************************\

Function: union_abstract_objectt::union_abstract_objectt

  Inputs:
   type - the type the abstract_object is representing
   top - is the abstract_object starting as top
   bottom - is the abstract_object starting as bottom

 Outputs:

 Purpose: Start the abstract object at either top or bottom or neither
          Asserts if both top and bottom are true

\*******************************************************************/

union_abstract_objectt::union_abstract_objectt(
  const typet &type,
  bool top,
  bool bottom)
  : abstract_objectt(type, top, bottom)
{
  PRECONDITION(type.id() == ID_union);
}

/*******************************************************************\

Function: union_abstract_objectt::union_abstract_objectt

  Inputs:
   expr - the expression to use as the starting pointer for an abstract object
   environment - the environment the abstract object is going to be evaluated in.

 Outputs:

 Purpose:

\*******************************************************************/

union_abstract_objectt::union_abstract_objectt(
  const exprt &expr,
  const abstract_environmentt &environment,
  const namespacet &ns)
  : abstract_objectt(expr, environment, ns)
{
  PRECONDITION(ns.follow(expr.type()).id() == ID_union);
}

/**
 * A helper function to evaluate an abstract object contained
 * within a container object. More precise abstractions may override this
 * to return more precise results.
 *
 * \param env the abstract environment
 * \param specifier a modifier expression, such as an array index or field
 * specifier used to indicate access to a specific component
 * \param ns the current namespace
 *
 * \return the abstract_objectt representing the value of the read component.
 */
abstract_object_pointert union_abstract_objectt::read(
  const abstract_environmentt &env,
  const exprt &specifier,
  const namespacet &ns) const
{
  return read_component(env, to_member_expr(specifier), ns);
}

/**
 * A helper function to evaluate writing to a component of an
 * abstract object. More precise abstractions may override this to
 * update what they are storing for a specific component.
 *
 * \param environment the abstract environment
 * \param ns the current namespace
 * \param stack the remaining stack of expressions on the LHS to evaluate
 * \param specifier the expression uses to access a specific component
 * \param value the value we are trying to write to the component
 * \param merging_write if true, this and all future writes will be merged
 * with the current value
 *
 * \return the abstract_objectt representing the result of writing
 * to a specific component.
 */
abstract_object_pointert union_abstract_objectt::write(
  abstract_environmentt &environment,
  const namespacet &ns,
  const std::stack<exprt> stack,
  const exprt &specifier,
  const abstract_object_pointert value,
  bool merging_write) const
{
  return write_component(
    environment, ns, stack, to_member_expr(specifier), value, merging_write);
}

/*******************************************************************\

Function: union_abstract_objectt::read_component

  Inputs:
   environment - the abstract environment
   member_expr - the expression uses to access a specific component

 Outputs: The abstract object representing the value of that component. For
          this abstraction this will always be top since we are not tracking
          the union.

 Purpose: A helper function to evaluate the abstract object contained
          within a union. More precise abstractions may override this
          to return more precise results.

\*******************************************************************/

abstract_object_pointert union_abstract_objectt::read_component(
  const abstract_environmentt &environment,
  const member_exprt &member_expr,
  const namespacet &ns) const
{
  return environment.abstract_object_factory(
    member_expr.type(), ns, !is_bottom(), is_bottom());
}

/*******************************************************************\

Function: union_abstract_objectt::write_component

  Inputs:
   environment - the abstract environment
   stack - the remaining stack of expressions on the LHS to evaluate
   member_expr - the expression uses to access a specific component
   value - the value we are trying to write to the component

 Outputs: The union_abstract_objectt representing the result of writing
          to a specific component. In this case this will always be top
          as we are not tracking the value of this union.

 Purpose: A helper function to evaluate writing to a component of a union.
          More precise abstractions may override this to
          update what they are storing for a specific component.

\*******************************************************************/

sharing_ptrt<union_abstract_objectt> union_abstract_objectt::write_component(
  abstract_environmentt &environment,
  const namespacet &ns,
  const std::stack<exprt> &stack,
  const member_exprt &member_expr,
  const abstract_object_pointert value,
  bool merging_write) const
{
  if(is_top() || is_bottom())
  {
    return std::dynamic_pointer_cast<const union_abstract_objectt>(clone());
  }
  else
  {
    return sharing_ptrt<union_abstract_objectt>(
      new union_abstract_objectt(type(), true, false));
  }
}
