/*******************************************************************\

Module: Format vector of numbers into a compressed range

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#ifndef CPROVER_UTIL_FORMAT_NUMBER_RANGE_H
#define CPROVER_UTIL_FORMAT_NUMBER_RANGE_H

#include <string>
#include <vector>
#include "invariant.h"

class format_number_ranget
{
public:
  std::string operator()(std::vector<unsigned> &);
};

#endif // CPROVER_UTIL_FORMAT_NUMBER_RANGE_H
