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

#include <Foundation/NSBundle.h>
#include <Foundation/NSString.h>
#include <Foundation/NSPathUtilities.h>
#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSDictionary.h>
#include <QtCore/QDebug>

#include "osx-configuration.hpp"

#include <stdexcept>

using namespace CI2CVGui;

struct OSXConfiguration::Private
{
	QString tracker_parameters_file;
	QString tracker_file;	
	QString avatar_file;	
	QString tracker_connections_file;
	
	int camera_index;
	
	qreal refresh_rate;
	
	qreal tracking_quality_threshold;	
	int tracking_health_update_msecs;
	int frames_until_reset_when_face_is_outside_the_frame;

	BackgroundColour background_colour;
};

static
QString
nsstring_to_qstring(NSString *string)
{
	char buf[1000];
	[string getCString: buf maxLength: 1000 encoding: NSUTF8StringEncoding];
	return QString::fromUtf8(buf);
}

static
QString
filepath_from_bundle(NSBundle *bundle, NSDictionary *dictionary, NSString *ns_key)
{
	QString key(nsstring_to_qstring(ns_key));
	NSString * ns_file = [dictionary objectForKey: ns_key];
	if (!ns_file) {
		qDebug() << "Key " << key << " does not exist.";
		throw std::runtime_error("Key does not exist.");
	}
	
	NSString * ns_filename  = [ns_file stringByDeletingPathExtension];
	NSString * ns_extension = [ns_file pathExtension];
	NSString * ns_path = [bundle pathForResource: ns_filename ofType: ns_extension];
	
	if (!ns_path) {
		qDebug() << "Resource (" << nsstring_to_qstring(ns_filename) 
				 << "," << nsstring_to_qstring(ns_extension)
				 << ")" << " path does not exist.";
		
		throw std::runtime_error("Resource path does not exist.");
	}
	
	return nsstring_to_qstring(ns_path);
}

static
NSNumber *
nsnumber_from_dictionary(NSDictionary *dictionary, NSString *ns_key)
{
	QString key = nsstring_to_qstring(ns_key);
	
	NSNumber *ns_number = [dictionary objectForKey: ns_key];
	
	if (!ns_number) {
		qDebug() << "Failed to find a value in dictionary for key " << key;
		throw std::runtime_error("Failed to find a value associated with a key in the Configuration.plist file.");
	}	
	
	return ns_number;
}

static
int
integer_from_dictionary(NSDictionary *dictionary, NSString *ns_key)
{
	NSNumber * ns_number = nsnumber_from_dictionary(dictionary, ns_key);	
	
	return [ns_number intValue];
}

static
qreal
qreal_from_dictionary(NSDictionary *dictionary, NSString *ns_key)
{
	NSNumber * ns_number = nsnumber_from_dictionary(dictionary, ns_key);	
	
	return [ns_number doubleValue];
}


OSXConfiguration::OSXConfiguration()
: pimpl(new Private())
{
	[[NSAutoreleasePool alloc] init];
	
	NSBundle *bundle = [NSBundle mainBundle];
	
	NSString *configuration_file = [bundle pathForResource: @"Configuration" ofType:@"plist"];
	
	if (!configuration_file) 
		throw std::runtime_error("Failed to find configuration file.");	
	
	NSDictionary *dictionary = [NSDictionary dictionaryWithContentsOfFile: configuration_file];
	
	if (!dictionary)
		throw std::runtime_error("Failed to load configuration dictionary.");
		
	pimpl->tracker_parameters_file  = filepath_from_bundle(bundle, dictionary, @"TrackerParametersFile");
	pimpl->tracker_file             = filepath_from_bundle(bundle, dictionary, @"TrackerFile");
	pimpl->avatar_file              = filepath_from_bundle(bundle, dictionary, @"AvatarFile");
	pimpl->tracker_connections_file = filepath_from_bundle(bundle, dictionary, @"TrackerConnectionsFile");

	pimpl->camera_index            = integer_from_dictionary(dictionary, @"CameraIndex");
	pimpl->refresh_rate            = qreal_from_dictionary(dictionary, @"RefreshRate");
	
	pimpl->tracking_quality_threshold = qreal_from_dictionary(dictionary, @"TrackingQualityThreshold");
	pimpl->tracking_health_update_msecs = integer_from_dictionary(dictionary, @"TrackingHealthUpdateMSecs");
	pimpl->frames_until_reset_when_face_is_outside_the_frame = integer_from_dictionary(dictionary, @"FramesUntilResetWhenFaceIsOutsideTheFrame");
	pimpl->background_colour       = (BackgroundColour)integer_from_dictionary(dictionary, @"BackgroundColour");
}

QString
OSXConfiguration::pathToAvatarData() const
{
	return pimpl->avatar_file;
}

QString
OSXConfiguration::pathToTrackerData() const
{
	return pimpl->tracker_file;
}

QString
OSXConfiguration::pathToTrackerParametersData() const
{
	return pimpl->tracker_parameters_file;
}

QString
OSXConfiguration::pathToTrackerConnections() const
{
	return pimpl->tracker_connections_file;
}


int
OSXConfiguration::cameraIndex() const
{
	return pimpl->camera_index;
}

QString
OSXConfiguration::videoFile() const
{
	throw std::runtime_error("Unsupported feature.");
}

qreal
OSXConfiguration::refreshRate() const
{
	return pimpl->refresh_rate;
}

qreal
OSXConfiguration::trackingQualityThreshold() const
{
	return pimpl->tracking_quality_threshold;
}

int
OSXConfiguration::trackingHealthUpdateMSecs() const
{
	return pimpl->tracking_health_update_msecs;
}

int
OSXConfiguration::framesUntilResetWhenFaceIsOutsideTheFrame() const
{
	return pimpl->frames_until_reset_when_face_is_outside_the_frame;
}

BackgroundColour
OSXConfiguration::backgroundColour() const
{
	return pimpl->background_colour;
}



static OSXConfiguration *application_configuration = NULL;

Configuration *
CI2CVGui::applicationConfiguration()
{
	if (!application_configuration) {
		application_configuration = new OSXConfiguration();
	}
	return application_configuration;
}
