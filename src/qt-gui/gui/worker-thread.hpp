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

#ifndef _CI2CV_GUI_GUI_WORKER_THREAD_HPP_
#define _CI2CV_GUI_GUI_WORKER_THREAD_HPP_

#include "controllers.hpp"
#include "tracker/FaceTracker.hpp"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QtCore/QThread>
#include <QtGui/QImage>
#include <QtCore/QTimer>

namespace CI2CVGui {
	class WorkerThreadCameraController : public CameraController
	{
		Q_OBJECT
	public:
		WorkerThreadCameraController(QObject *parent = NULL);
		
		cv::VideoCapture &camera();
		
		QSize frameSize();	
		
		bool isInitialised() const;
		
	public slots:
		void stop();
		void start();
		void initialiseCamera();		
		
	private:
		cv::VideoCapture camera_object;
		QSize frame_size;		
	};
	
	class WorkerThreadTrackerController : public TrackerController
	{
		Q_OBJECT
		
	public:
		WorkerThreadTrackerController(QObject *parent = NULL);
		bool isStopped() const;		
		
		FACETRACKER::FaceTracker *tracker() const;
  	        FACETRACKER::FaceTrackerParams *trackerParameters() const;
		
		qreal trackingQuality() const;
		bool trackingQualityDecision() const;
		
		qreal trackingQualityThreshold() const;	
		
		cv::Mat trackerConnections() const;
		
		bool isFaceOutOfFrame() const;
				
	public slots:
		void stop();
		void start();	
		void reset();
		
		void setTrackingQualityThreshold(qreal new_threshold);			
		
	private slots:
		void trackingQualityTest();
		
	private:
		FACETRACKER::FaceTracker *tracker_object;
		FACETRACKER::FaceTrackerParams *tracker_parameters;
		
		QTimer good_tracking_tick;
		
		cv::Mat tracker_connections;		
	};			
	
	struct WorkerData {
		cv::Mat cv_input_image;
		cv::Mat cv_animated_image;
		cv::Mat cv_tracked_shape;
		
		QImage input_image;
		QImage animated_image;		
	};
	
	class WorkerThread : public QThread
	{
		Q_OBJECT
	public:
		WorkerThread(QObject *parent = NULL);
		
		WorkerData *currentData() const;		
		
		bool trackingQualityDecision() const;
		qreal trackingQuality() const;
		qreal trackingQualityThreshold() const;
		
		bool isFaceOutOfFrame() const;
		
	public slots:
		void stop();
		void startTracker();
		void stopTracker();
		
		void startAnimating();		
		void stopAnimating();
		
		void setAvatarIndex(int index);
		
		void resetTracker();
		
		void setTrackingQualityThreshold(qreal new_threshold);
		
		void initialiseAvatar();
						
	protected:
		void run();
		
	private:
		void runTracker();
		
	private:
		WorkerData * current_data;
		QScopedArrayPointer<WorkerData> data_objects;
		int current_index;
		
		bool is_stopped;
		bool tracker_stopped;		
		bool animation_stopped;	
		int new_avatar_index;
		
		bool reset_tracker;
		
		bool tracking_quality_decision;
		qreal tracking_quality;
		qreal tracking_health_threshold;
		
		bool initialise_avatar;
		
		bool is_face_out_of_frame;		
	};
	
	WorkerThread *workerThread();
}

#endif
