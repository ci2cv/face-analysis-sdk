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

#ifndef _CI2CV_GUI_CONFIGURATION_HPP_
#define _CI2CV_GUI_CONFIGURATION_HPP_

#include <QtCore/QString>

namespace CI2CVGui {

  enum BackgroundColour {
    BACKGROUND_COLOUR_WHITE = 0,
    BACKGROUND_COLOUR_BLACK = 1
  };

  class Configuration 
  {
  public:
    virtual ~Configuration();
    
    virtual QString pathToAvatarData() const = 0;
    virtual QString pathToTrackerData() const = 0;
    virtual QString pathToTrackerParametersData() const = 0;
    virtual QString pathToTrackerConnections() const = 0;
    
    virtual int cameraIndex() const = 0;
    virtual QString videoFile() const = 0;		
    
    virtual qreal refreshRate() const = 0;
    
    virtual qreal trackingQualityThreshold() const = 0;
    virtual int trackingHealthUpdateMSecs() const = 0;
    virtual int framesUntilResetWhenFaceIsOutsideTheFrame() const = 0;
    
    virtual BackgroundColour backgroundColour() const = 0;
  };
  
  Configuration *applicationConfiguration();
}

#endif
