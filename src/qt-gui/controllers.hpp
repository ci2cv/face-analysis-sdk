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

#ifndef _CI2CV_GUI_GUI_CONTROLLER_HPP_
#define _CI2CV_GUI_GUI_CONTROLLER_HPP_

#include <QtCore/QObject>
#include <QtGui/QImage>
#include <opencv2/core/core.hpp>

namespace CI2CVGui
{
	bool is_tracker_mesh_p(const cv::Mat &mesh);
	
	class GuiController : public QObject
	{
		Q_OBJECT
	public:
		GuiController(QObject *parent=NULL);
				
	signals:
		void fullscreenModeRequest();
		void windowModeRequest();
		
		void cameraOffRequest();
		void cameraOnRequest();
		
		void trackerOnRequest();
		void trackerOffRequest();
		
		void reinitialiseUserRequest();
		void initialiseUserNowRequest();
		void hideUserImageRequest();
		void showUserImageRequest();
		
		void showAvatarSelectionRequest();
		void hideAvatarSelectionRequest();
		
		void showTrackerMeshRequest();
		void hideTrackerMeshRequest();
		
		void showFramesPerSecondRequest();
		void hideFramesPerSecondRequest();
				
	public slots:
		virtual void showUserShowAvatar() = 0;		
		virtual void hideUserShowAvatar() = 0;
		
		virtual void showAvatarSelection() = 0;
		virtual void hideAvatarSelection() = 0;
		
		virtual void showTrackerMesh() = 0;
		virtual void hideTrackerMesh() = 0;
		
		virtual void setFullscreenMode() = 0;
		virtual void setWindowedMode() = 0;
		
		virtual void showFramesPerSecond() = 0;
		virtual void hideFramesPerSecond() = 0;
		
		virtual void turnOnWarningAboutFaceOutOfFrame() = 0;
		virtual void turnOffWarningAboutFaceOutOfFrame() = 0;
		
		virtual void configureForUserInitialisation() = 0;
		virtual void configureForAnimation() = 0;
		virtual void configureForCameraInitialisation() = 0;
		
	private:
	};
	
	GuiController *guiController();
	
	class TrackerController : public QObject
	{
		Q_OBJECT
	public:
		TrackerController(QObject *parent = NULL);		
		
		virtual qreal trackingQuality() const = 0;
		virtual bool trackingQualityDecision() const = 0;
		virtual qreal trackingQualityThreshold() const = 0;
		
		virtual cv::Mat trackerConnections() const = 0;		
		
		virtual bool isFaceOutOfFrame() const = 0;
		
	signals:
		void trackingQualityDecisionChanged(bool good_or_bad);
		void trackingQualityChanged(qreal quality);
		void trackingQualityThresholdChanged(qreal threshold);
		
		void faceIsNowOutsideTheFrame();
		void faceIsNowInsideTheFrame();
	
	public slots:
		virtual void stop() = 0;
		virtual void start() = 0;
		
		virtual void reset() = 0;
		
		virtual void setTrackingQualityThreshold(qreal threshold) = 0;
		
	private:
	};
	
	TrackerController *trackerController();
	
	class AvatarController : public QObject
	{
		Q_OBJECT
	public:
		AvatarController(QObject *parent = NULL);
		
		virtual QSize frameSize() = 0;
		
		virtual int numberOfAvatars() const = 0;
		
		virtual int avatarIndex() const = 0;
		
		virtual QImage avatarThumbnail(int index) = 0;
				
	public slots:
		virtual void initialiseUser() = 0;		
		virtual void setAvatarIndex(int index) = 0;
		
		virtual void start() = 0;
		virtual void stop() = 0;
		
	private:
	};
	
	AvatarController *avatarController();
	
	class CameraController : public QObject
	{
		Q_OBJECT		
	public:
		CameraController(QObject *parent = NULL);
		
		virtual QSize frameSize() = 0;	
		
		virtual bool isInitialised() const = 0;
		
	signals:
		void cameraInitialised();
			
	public slots:
		virtual void stop() = 0;
		virtual void start() = 0;
		virtual void initialiseCamera() = 0;
	};	
	
	CameraController *cameraController();		
}

#endif
