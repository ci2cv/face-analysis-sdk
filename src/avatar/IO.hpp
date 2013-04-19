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

#ifndef _AVATAR_IO_h_
#define _AVATAR_IO_h_
#include <opencv/cv.h>
#include <fstream>
#include <vector>
namespace AVATAR
{
  //===========================================================================
  /** 
      Input-output Operations
  */
  class IO{
  public:
    enum{MYAVATAR = 0,MYAVATARPARAMS, DUMMY_LAST_DONT_USE};
  };

  class IOBinary : public IO{
  public:
    enum{MYAVATAR = DUMMY_LAST_DONT_USE+1,MYAVATARPARAMS};
  };
  //===========================================================================
}
#endif
