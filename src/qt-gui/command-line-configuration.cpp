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

#include "command-line-configuration.hpp"
#include "tracker/Config.h"
#include "tracker/FaceTracker.hpp"
#include "avatar/Avatar.hpp"
#include <stdexcept>
#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>

using namespace CI2CVGui;

namespace {
  QString
  get_argument(int *i, const QStringList &args) {
    if (*i + 1 >= args.length()) {
      throw std::logic_error("No argument available.");
    }
    
    *i = *i + 1;
    return args[*i];
  }

  template <typename T> T get_argument(int *i, const QStringList &args);

  template <>
  int
  get_argument<>(int *i, const QStringList &args) {
    QString string = get_argument(i, args);
    bool ok;
    int rv = string.toInt(&ok);
    if (!ok)
      throw std::logic_error("Argument is not an integer.");
    return rv;
  }
}

CommandLineConfiguration::CommandLineConfiguration()
  : _camera_index(0)
{
  QStringList args = QCoreApplication::arguments();
  for (int i = 1; i < args.size(); i++) {
    QString arg(args[i]);

    if (arg == "--camera-index") {
      _camera_index = get_argument<int>(&i, args);
    }
  }
}

QString
CommandLineConfiguration::pathToAvatarData() const
{
  return AVATAR::DefaultAvatarModelPathname().c_str();
}

QString
CommandLineConfiguration::pathToTrackerData() const
{
  return FACETRACKER::DefaultFaceTrackerModelPathname().c_str();
}

QString
CommandLineConfiguration::pathToTrackerParametersData() const
{
  return FACETRACKER::DefaultFaceTrackerParamsPathname().c_str();
}

QString
CommandLineConfiguration::pathToTrackerConnections() const
{
  return FACETRACKER_DEFAULT_FACE_CON_PATHNAME;
}
    
int
CommandLineConfiguration::cameraIndex() const
{
  return _camera_index;
}

QString
CommandLineConfiguration::videoFile() const
{
  return "";
}
    
qreal
CommandLineConfiguration::refreshRate() const
{
  return 35;
}

qreal
CommandLineConfiguration::trackingQualityThreshold() const
{
  return 0.65;
}

int
CommandLineConfiguration::trackingHealthUpdateMSecs() const
{
  return 250;
}

int
CommandLineConfiguration::framesUntilResetWhenFaceIsOutsideTheFrame() const
{
  return 50;
}

BackgroundColour
CommandLineConfiguration::backgroundColour() const
{
  return BACKGROUND_COLOUR_WHITE;
}

static CommandLineConfiguration *application_configuration = NULL;

Configuration *
CI2CVGui::applicationConfiguration()
{
  if (!application_configuration) {
    application_configuration = new CommandLineConfiguration();
  }
  return application_configuration;
}
