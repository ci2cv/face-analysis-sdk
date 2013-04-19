// CSIRO has filed various patents which cover the Software. 

// CSIRO grants to you a license to any patents granted for inventions
// implemented by the Software for academic, research and non-commercial
// use only.

// CSIRO hereby reserves all rights to its inventions implemented by the
// Software and any patents subsequently granted for those inventions
// that are not expressly granted to you.  Should you wish to license the
// patents relating to the Software for commercial use please contact
// CSIRO IP & Licensing, Gautam Tendulkar (gautam.tendulkar@csiro.au) or
// Nick Marsh (nick.marsh@csiro.au)

// This software is provided under the CSIRO OPEN SOURCE LICENSE
// (GPL2) which can be found in the LICENSE file located in the top
// most directory of the source code.

// Copyright CSIRO 2013

#include "command-line-arguments.hpp"
#include "helpers.hpp"

std::string
get_argument(int *i, int argc, char **argv)
{
  if ((*i + 1) >= argc) 
    throw make_runtime_error("Not enough arguments to process argument: %s", argv[*i]);

  *i = (*i + 1);
  return argv[*i];
}

// Local Variables:
// compile-in-directory: "../"
// End:
