/*******************************************************************\

Module: Function Inlining

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#ifndef CPROVER_GOTO_INLINE_H
#define CPROVER_GOTO_INLINE_H

#include "goto_model.h"

class message_handlert;

// do a full inlining

void goto_inline(
  goto_modelt &goto_model,
  message_handlert &message_handler);

void goto_inline(
  goto_functionst &goto_functions,
  const namespacet &ns,
  message_handlert &message_handler);

// inline those functions marked as "inlined"
// and functions with less than _smallfunc_limit instructions

void goto_partial_inline(
  goto_modelt &goto_model,
  message_handlert &message_handler,
  unsigned smallfunc_limit=0);

void goto_partial_inline(
  goto_functionst &goto_functions,
  const namespacet &ns,
  message_handlert &message_handler,
  unsigned smallfunc_limit=0);

// transitively inline all calls the given function makes

void goto_function_inline(
  goto_modelt &goto_model,
  const irep_idt function,
  message_handlert &message_handler);

void goto_function_inline(
  goto_functionst &goto_functions,
  const irep_idt function,
  const namespacet &ns,
  message_handlert &message_handler);

#endif
