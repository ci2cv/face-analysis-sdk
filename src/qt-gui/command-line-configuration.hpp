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

#ifndef _COMMAND_LINE_CONFIGURATION_HPP_
#define _COMMAND_LINE_CONFIGURATION_HPP_

#include "configuration.hpp"

namespace CI2CVGui {
  class CommandLineConfiguration : public Configuration
  {
  public:
    CommandLineConfiguration();
    
    QString pathToAvatarData() const;
    QString pathToTrackerData() const;
    QString pathToTrackerParametersData() const;
    QString pathToTrackerConnections() const;		
    
    int cameraIndex() const;
    QString videoFile() const;	
    
    qreal refreshRate() const;
    
    qreal trackingQualityThreshold() const;	
    int trackingHealthUpdateMSecs() const;
    
    int framesUntilResetWhenFaceIsOutsideTheFrame() const;		
    
    BackgroundColour backgroundColour() const;    

  private:
    int _camera_index;
  };
}

#endif
