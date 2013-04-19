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

#ifndef _CI2CV_GUI_GUI_WINDOWED_GUI_CONTROLLER_HPP_
#define _CI2CV_GUI_GUI_WINDOWED_GUI_CONTROLLER_HPP_

#include <QtGui/QWidget>
#include <QtGui/QAbstractButton>
#include <QtCore/QTimer>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>

#include "controllers.hpp"
#include "gui/item-controllers.hpp"
#include "gui/item-positions-calculator.hpp"

namespace CI2CVGui {	
	
	
	class WindowedGuiController : public GuiController {
		Q_OBJECT
	public:	
		WindowedGuiController(QObject *parent = NULL);
		
		void showUserShowAvatar();		
		void hideUserShowAvatar();
		
		void showAvatarSelection();
		void hideAvatarSelection();
		
		void showTrackerMesh();
		void hideTrackerMesh();
		
		void setFullscreenMode();
		void setWindowedMode();
		
		void showFramesPerSecond();
		void hideFramesPerSecond();
		
		void configureForUserInitialisation();
		void configureForAnimation();
		void configureForCameraInitialisation();
		
		void turnOnWarningAboutFaceOutOfFrame();
		void turnOffWarningAboutFaceOutOfFrame();
		
		
	public:
		QAbstractButton *reset_tracker_button;
		QAbstractButton *gui_state_change_button; // the button used to switch between Gui States
		QAbstractButton *show_user_button; 
		QAbstractButton *show_avatar_selection_button;
		QAbstractButton *show_tracking_mesh_button;
		QProgressBar    *tracking_health;
		
		QLabel *status_message;
		
		PixmapItemController *user_input;
		PixmapItemController *avatar_animation;
		ItemController *avatar_selector;
		MeshItemController *tracking_mesh;
		ItemController *face_out_of_bounds_warning_user;
		ItemController *face_out_of_bounds_warning_avatar;
				
	private:
		void checkBindings();
		
	public slots:
		void updateItems(qreal width, qreal height);
		void updateItems(bool animate = true);
		
		void bindController();
								
	private:		
	private slots:
		void guiStateChangeClicked();
		void showUserChangeClicked(bool hide_or_show);
		void showAvatarSelectionClicked(bool show);
		void imageUpdateTick();
		void trackingMeshButtonClicked(bool hide_or_show);
		
		void trackingQualityUpdate(qreal new_value);
			
	private:
		enum AnimationState {
			SHOW_USER_SHOW_AVATAR,
			HIDE_USER_SHOW_AVATAR
		};
		
		enum GuiState {
			CAMERA_INITIALISATION,
			USER_INITIALISATION,
			ANIMATION
		};
		
		enum AvatarSelectionState {
			SHOW_AVATAR_SELECTION,
			HIDE_AVATAR_SELECTION
		};
		
		AnimationState animation_state;
		GuiState gui_state;
		AvatarSelectionState avatar_selection_state;
				
		ItemPositions computeItemPositions(qreal width, qreal height);
		ItemPositionsConstants item_positions_constants;
		WindowedGuiControllerItemPositionsCalculator item_positions_calculator;
							
		qreal drawing_area_width, drawing_area_height;
		
		QTimer image_update_timer;				
		
		bool show_tracking_mesh;
		
		bool show_face_out_frame_warning;
	};
}

#endif
