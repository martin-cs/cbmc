/*******************************************************************\

Module: Abstract Interpretation

Author: Martin Brain

\*******************************************************************/

/// \file
/// This is a skeleton implementation of an abstract interpretation
/// domain.  It is intended to be a start-point for writing your own
/// domain!

#ifndef CPROVER_ANALYSES_EXAMPLE_DOMAIN_H
#define CPROVER_ANALYSES_EXAMPLE_DOMAIN_H

#include "ai_domain.h"

class example_domaint : public ai_domain_baset
{
public:
  /// Constructors
  example_domaint();

  /// Destructor
  virtual ~example_domaint();

  /// Transform updates the domain with the effect of the instruction
  /// given by "from".
  /// As GOTO has multiple next instructions, the abstract trace "to"
  /// is passed so you can tell whether it is the branch taken or not.
  virtual void transform(
    const irep_idt &function_from,
    trace_ptrt from,
    const irep_idt &function_to,
    trace_ptrt to,
    ai_baset &ai,
    const namespacet &ns) override;

  /// Merges two domains together
  /// \return true if and only if *this has been modified / extended
  /// If it is true then it will requeue this domain for analysis
  bool merge(const example_domaint &b, trace_ptrt, trace_ptrt);

  /// Set the domain to be empty, i.e. representing nothing
  void make_bottom() override;

  /// Set the domain to allow all possibilities
  void make_top() override;

  /// Set up the domain for the start of the program
  void make_entry() override;

  /// Is the domain bottom or not
  bool is_bottom() const override;

  /// Is the domain top or not
  bool is_top() const override;

  /// Output the domain as a string
  void output(std::ostream &out, const ai_baset &ai, const namespacet &ns)
    const override;

protected:
  // Domain state goes here
  // For now we will track which states may (an overapproximation) be reachable.
  bool may_reach;
};

#endif // CPROVER_ANALYSES_EXAMPLE_DOMAIN_H
