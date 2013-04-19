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

#include "gui/worker-thread.hpp"

#include <opencv/highgui.h>
#include <QtCore/QDebug>
#include <stdexcept>
#include <numeric>

#include "avatar/avatar.hpp"
#include "gui/avatar.hpp"
#include "configuration.hpp"

using namespace CI2CVGui;

#define NUMBER_OF_WORKER_DATA_OBJECTS 50

WorkerThreadCameraController::WorkerThreadCameraController(QObject *parent)
: CameraController(parent),
	frame_size(0,0)
{
	
}

QSize
WorkerThreadCameraController::frameSize()
{
	if (!isInitialised())
		throw std::logic_error("Attempting to request frameSize from an uninitialised camera.");
	
	return frame_size;
}

cv::VideoCapture &
WorkerThreadCameraController::camera()
{
	return camera_object;
}

void
WorkerThreadCameraController::stop()
{
	workerThread()->stop();
}

void
WorkerThreadCameraController::start()
{	
	workerThread()->start();
}

void
WorkerThreadCameraController::initialiseCamera()
{
	camera_object.open(applicationConfiguration()->cameraIndex());
	cv::Mat frame;
	camera_object >> frame;
	
	frame_size = QSize(frame.cols, frame.rows);
	emit cameraInitialised();
}

bool
WorkerThreadCameraController::isInitialised() const
{
	return frame_size.width() != 0;
}

// WorkerThreadTrackerController
WorkerThreadTrackerController::WorkerThreadTrackerController(QObject *parent)
: TrackerController(parent),
  good_tracking_tick(this)
{
	tracker_parameters  = FACETRACKER::LoadFaceTrackerParams(applicationConfiguration()->pathToTrackerParametersData().toStdString().c_str());
	tracker_object      = FACETRACKER::LoadFaceTracker(applicationConfiguration()->pathToTrackerData().toStdString().c_str());	
	tracker_connections = FACETRACKER::IO::LoadCon(applicationConfiguration()->pathToTrackerConnections().toStdString().c_str());
	
	if (!tracker_parameters)
		throw std::runtime_error("Failed to initialise tracker parameters.");
	
	if (!tracker_object)
		throw std::runtime_error("Failed to initialise tracker_object.");
	
	good_tracking_tick.setInterval(applicationConfiguration()->trackingHealthUpdateMSecs());
	good_tracking_tick.setSingleShot(false);
	connect(&good_tracking_tick, SIGNAL(timeout()), SLOT(trackingQualityTest()));
	good_tracking_tick.start();	
}

FACETRACKER::FaceTrackerParams *
WorkerThreadTrackerController::trackerParameters() const
{
	return tracker_parameters;
}

FACETRACKER::FaceTracker *
WorkerThreadTrackerController::tracker() const
{
	return tracker_object;
}

void
WorkerThreadTrackerController::stop()
{
	workerThread()->stopTracker();
}

void
WorkerThreadTrackerController::start()
{
	workerThread()->startTracker();
}

void
WorkerThreadTrackerController::reset()
{
	workerThread()->resetTracker();
}

void
WorkerThreadTrackerController::trackingQualityTest()
{
	emit trackingQualityDecisionChanged(workerThread()->trackingQualityDecision());
	emit trackingQualityChanged(workerThread()->trackingQuality());
	
	if (workerThread()->isFaceOutOfFrame())
		emit faceIsNowOutsideTheFrame();
	else
		emit faceIsNowInsideTheFrame();
}

qreal
WorkerThreadTrackerController::trackingQuality() const
{
	return workerThread()->trackingQuality();	
}

bool
WorkerThreadTrackerController::trackingQualityDecision() const
{
	return workerThread()->trackingQualityDecision();
}

qreal
WorkerThreadTrackerController::trackingQualityThreshold() const
{
	return workerThread()->trackingQualityThreshold();
}

void
WorkerThreadTrackerController::setTrackingQualityThreshold(qreal new_threshold)
{
	workerThread()->setTrackingQualityThreshold(new_threshold);
	emit trackingQualityThresholdChanged(new_threshold);
}

cv::Mat
WorkerThreadTrackerController::trackerConnections() const
{
	return tracker_connections;
}

bool
WorkerThreadTrackerController::isFaceOutOfFrame() const
{
	return workerThread()->isFaceOutOfFrame();
}

// WorkerThread

WorkerThread::WorkerThread(QObject *parent)
: QThread(parent),
	current_data(NULL),
	data_objects(new WorkerData[NUMBER_OF_WORKER_DATA_OBJECTS]),
	current_index(0),
	is_stopped(true),
	tracker_stopped(true),
	animation_stopped(true),
	reset_tracker(false),
	tracking_health_threshold(applicationConfiguration()->trackingQualityThreshold())
{
}

WorkerData *
WorkerThread::currentData() const
{
	return current_data;
}

void
WorkerThread::run()
{
	is_stopped = false;
	while (!is_stopped) {
		try {
			runTracker();
		} catch (std::exception &e) {
			qDebug() << "Caught Exception: " << e.what();
		} catch (...) {
			qDebug() << "Caught unidentified exception.";
		}
	}
}

static
void 
clear_avatar_image(WorkerData *data)
{
        cv::Scalar background_colour;
	switch (applicationConfiguration()->backgroundColour()) {
	case BACKGROUND_COLOUR_BLACK:
	  background_colour = cv::Scalar(0,0,0);
	  break;
	case BACKGROUND_COLOUR_WHITE:
	  background_colour = cv::Scalar(255,255,255);
	  break;
	default:
	  throw std::logic_error("Invalid background colour value.");
	}

	data->cv_animated_image = background_colour;

	data->animated_image = QImage(data->cv_animated_image.datastart,
								  data->cv_animated_image.cols,
								  data->cv_animated_image.rows,
								  QImage::Format_RGB888);	
}

void
WorkerThread::runTracker()
{	
	cv::Mat cv_input_image_bgr;
	cv::Mat cv_input_image_bgr_flip;	
	cv::Mat cv_animated_image_bgr;
			
	WorkerThreadCameraController *camera_controller = dynamic_cast<WorkerThreadCameraController *>(cameraController());
	cv::VideoCapture &camera = camera_controller->camera();
	
	WorkerThreadTrackerController *tracker_controller = dynamic_cast<WorkerThreadTrackerController *>(trackerController());
	FACETRACKER::FaceTracker *tracker = tracker_controller->tracker();
	FACETRACKER::FaceTrackerParams *tracker_parameters = tracker_controller->trackerParameters();
	
	tracker->Reset();
	
	Avatar *avatar_controller = dynamic_cast<Avatar *>(avatarController());
	AVATAR::Avatar *avatar = avatar_controller->avatar();
	
	tracking_quality = 0.0;
	tracking_quality_decision = false;
	
	int face_out_of_frame_count = 0;
	Configuration *application_configuration = applicationConfiguration();
	const int maximum_face_out_of_frame_count = application_configuration->framesUntilResetWhenFaceIsOutsideTheFrame();
	cv::Scalar background_colour;
	switch (application_configuration->backgroundColour()) {
	case BACKGROUND_COLOUR_BLACK:
	  background_colour = cv::Scalar(0,0,0);
	  break;
	case BACKGROUND_COLOUR_WHITE:
	  background_colour = cv::Scalar(255,255,255);
	  break;
	default:
	  throw std::logic_error("Invalid background colour");
	}
	is_face_out_of_frame = false;
	
	while (!is_stopped) {		
		current_index++;
		current_index = current_index % NUMBER_OF_WORKER_DATA_OBJECTS;
			
		WorkerData *new_data = &data_objects[current_index];		
				
		camera >> cv_input_image_bgr;
		
		cv::flip(cv_input_image_bgr, cv_input_image_bgr_flip, 1);
		
		if (cv_input_image_bgr_flip.rows != new_data->cv_input_image.rows)
			new_data->cv_input_image = cv_input_image_bgr_flip.clone();
		
		if (cv_input_image_bgr_flip.rows != new_data->cv_animated_image.rows) {
			new_data->cv_animated_image = cv_input_image_bgr_flip.clone();
			new_data->cv_animated_image = background_colour;
		}
		
		cv::cvtColor(cv_input_image_bgr_flip, new_data->cv_input_image, CV_BGR2RGB);
		
		new_data->input_image = QImage(new_data->cv_input_image.datastart,
									   new_data->cv_input_image.cols,
									   new_data->cv_input_image.rows,
									   QImage::Format_RGB888);							
						
		if (!tracker_stopped) {
			if (reset_tracker) {
				tracker->Reset();
				reset_tracker = false;
				face_out_of_frame_count = 0;
			}
			int health = tracker->Track(cv_input_image_bgr_flip,tracker_parameters);					
			tracking_quality   = health/10.0;
			
			if (health == FACETRACKER::FaceTracker::TRACKER_FACE_OUT_OF_FRAME) {
				face_out_of_frame_count++;
			} else 
				face_out_of_frame_count = 0;
			
			is_face_out_of_frame = face_out_of_frame_count > 0;
					
			
			tracking_quality_decision = ((tracking_quality > tracking_health_threshold)
										 || (   (health == FACETRACKER::FaceTracker::TRACKER_FACE_OUT_OF_FRAME)
											 && (face_out_of_frame_count <= maximum_face_out_of_frame_count)));
			
			if (!tracking_quality_decision) {
				clear_avatar_image(new_data);
				tracker->Reset();
				new_data->cv_tracked_shape = cv::Mat();
				face_out_of_frame_count = 0;
			} else {
				tracker->_shape.copyTo(new_data->cv_tracked_shape);
				
				if ((!animation_stopped) && (tracker->_shape.rows != 0) && (tracker->_shape.cols != 0)) {
					if (new_avatar_index >= 0) {					  
						avatar->setAvatar(new_avatar_index);
						new_avatar_index = -1;
					}
					if (initialise_avatar) {
					        FACETRACKER::PointVector points(new_data->cv_tracked_shape.rows / 2);
						for (size_t i = 0; i < points.size(); i++) 
						  points[i] = cv::Point_<double>(new_data->cv_tracked_shape.at<double>(i + 0, 0),
									   new_data->cv_tracked_shape.at<double>(i + points.size(), 0));

						avatar->Initialise(new_data->cv_input_image, points);
						initialise_avatar = false;
					}
					cv_animated_image_bgr = background_colour;
					avatar->Animate(cv_animated_image_bgr, cv_input_image_bgr_flip, tracker->getShape());
					cv::cvtColor(cv_animated_image_bgr, new_data->cv_animated_image, CV_BGR2RGB);
					
					new_data->animated_image = QImage(new_data->cv_animated_image.datastart,
													  new_data->cv_animated_image.cols,
													  new_data->cv_animated_image.rows,
													  QImage::Format_RGB888);
				} else {
					clear_avatar_image(new_data);
				}					
			}		
		} else { 
			clear_avatar_image(new_data);
		}

		current_data = new_data;
	}
}

void
WorkerThread::stop()
{
	is_stopped = true;
}

void
WorkerThread::startTracker()
{
	resetTracker();
	tracker_stopped = false;
}

void
WorkerThread::stopTracker()
{
	tracker_stopped = true;
}

void
WorkerThread::resetTracker()
{
	reset_tracker = true;
}

void
WorkerThread::startAnimating()
{
	animation_stopped = false;
}

void
WorkerThread::stopAnimating()
{
	animation_stopped = true;
}

void
WorkerThread::setAvatarIndex(int index)
{
	new_avatar_index = index;
}

qreal
WorkerThread::trackingQuality() const
{
	const qreal maximum_value = 1.0 - tracking_health_threshold;
	
	qreal rv = (tracking_quality - tracking_health_threshold)/maximum_value;
	rv = std::max(rv,0.0);
	rv = std::min(rv,1.0);
	return rv;
}

bool
WorkerThread::trackingQualityDecision() const
{
	return tracking_quality_decision;
}

qreal
WorkerThread::trackingQualityThreshold() const
{
	return tracking_health_threshold;
}

void
WorkerThread::setTrackingQualityThreshold(qreal new_threshold)
{
	tracking_health_threshold = new_threshold;
}

void
WorkerThread::initialiseAvatar()
{
	initialise_avatar = true;
}

bool
WorkerThread::isFaceOutOfFrame() const
{
	return is_face_out_of_frame;
}


static WorkerThread      * worker_thread       = NULL;
static CameraController  * camera_controller   = NULL;
static TrackerController * tracker_controller  = NULL;

WorkerThread *
CI2CVGui::workerThread()
{
	if (!worker_thread) {
		worker_thread = new WorkerThread();
	}
	return worker_thread;
}

CI2CVGui::CameraController *
CI2CVGui::cameraController()
{
	if (!camera_controller) {
		camera_controller = new WorkerThreadCameraController();
	}
	return camera_controller;
}

CI2CVGui::TrackerController *
CI2CVGui::trackerController()
{
	if (!tracker_controller) {
		tracker_controller = new WorkerThreadTrackerController();
	}
	return tracker_controller;
}


