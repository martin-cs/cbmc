/*******************************************************************\

Module: ANSI-C Conversion / Type Checking

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <util/arith_tools.h>

#include "c_typecheck_base.h"

/*******************************************************************\

Function: c_typecheck_baset::add_argc_argv

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::add_argc_argv(const symbolt &main_symbol)
{
  const code_typet::parameterst &parameters=
    to_code_type(main_symbol.type).parameters();

  if(parameters.empty())
    return;

  if(parameters.size()!=2 &&
     parameters.size()!=3)
  {
    error().source_location=main_symbol.location;
    error() << "main expected to have no or two or three parameters" << eom;
    THROWZERO;
  }

  symbolt *argc_new_symbol;

  const exprt &op0=static_cast<const exprt &>(parameters[0]);
  const exprt &op1=static_cast<const exprt &>(parameters[1]);

  {
    symbolt argc_symbol;

    argc_symbol.base_name="argc";
    argc_symbol.name="argc'";
    argc_symbol.type=op0.type();
    argc_symbol.is_static_lifetime=true;
    argc_symbol.is_lvalue=true;

    if(argc_symbol.type.id()!=ID_signedbv &&
       argc_symbol.type.id()!=ID_unsignedbv)
    {
      error().source_location=main_symbol.location;
      error() << "argc argument expected to be integer type, but got `"
              << to_string(argc_symbol.type) << "'" << eom;
      THROWZERO;
    }

    move_symbol(argc_symbol, argc_new_symbol);
  }

  {
    if(op1.type().id()!=ID_pointer ||
       op1.type().subtype().id()!=ID_pointer)
    {
      error().source_location=main_symbol.location;
      error() << "argv argument expected to be pointer-to-pointer type, "
                 "but got `"
              << to_string(op1.type()) << '\'' << eom;
      THROWZERO;
    }

    // we make the type of this thing an array of pointers
    typet argv_type=array_typet();
    argv_type.subtype()=op1.type().subtype();

    // need to add one to the size -- the array is terminated
    // with NULL
    exprt one_expr=from_integer(1, argc_new_symbol->type);

    exprt size_expr(ID_plus, argc_new_symbol->type);
    size_expr.copy_to_operands(argc_new_symbol->symbol_expr(), one_expr);
    argv_type.add(ID_size).swap(size_expr);

    symbolt argv_symbol;

    argv_symbol.base_name="argv'";
    argv_symbol.name="argv'";
    argv_symbol.type=argv_type;
    argv_symbol.is_static_lifetime=true;
    argv_symbol.is_lvalue=true;

    symbolt *argv_new_symbol;
    move_symbol(argv_symbol, argv_new_symbol);
  }

  if(parameters.size()==3)
  {
    symbolt envp_symbol;
    envp_symbol.base_name="envp'";
    envp_symbol.name="envp'";
    envp_symbol.type=(static_cast<const exprt&>(parameters[2])).type();
    envp_symbol.is_static_lifetime=true;

    symbolt envp_size_symbol, *envp_new_size_symbol;
    envp_size_symbol.base_name="envp_size";
    envp_size_symbol.name="envp_size'";
    envp_size_symbol.type=op0.type(); // same type as argc!
    envp_size_symbol.is_static_lifetime=true;
    move_symbol(envp_size_symbol, envp_new_size_symbol);

    if(envp_symbol.type.id()!=ID_pointer)
    {
      error().source_location=main_symbol.location;
      error() << "envp argument expected to be pointer type, but got `"
              << to_string(envp_symbol.type) << '\'' << eom;
      THROWZERO;
    }

    exprt size_expr = envp_new_size_symbol->symbol_expr();

    envp_symbol.type.id(ID_array);
    envp_symbol.type.add(ID_size).swap(size_expr);

    symbolt *envp_new_symbol;
    move_symbol(envp_symbol, envp_new_symbol);
  }
}
