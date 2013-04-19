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

#include "gui/windowed-gui-controller.hpp"

#include <QtCore/QDebug>
#include <stdexcept>
#include "gui/worker-thread.hpp"
#include "configuration.hpp"

using namespace CI2CVGui;

WindowedGuiController::WindowedGuiController(QObject *parent)
: GuiController(parent),
	gui_state_change_button(NULL),
	show_user_button(NULL),
	show_avatar_selection_button(NULL),
	tracking_health(NULL),
	status_message(NULL),
	user_input(NULL),
	avatar_animation(NULL),
	avatar_selector(NULL),
	tracking_mesh(NULL),
	face_out_of_bounds_warning_user(NULL),
	face_out_of_bounds_warning_avatar(NULL),
	animation_state(SHOW_USER_SHOW_AVATAR),
	gui_state(CAMERA_INITIALISATION),
	image_update_timer(this),
	show_face_out_frame_warning(false)
{
	image_update_timer.setInterval(1.0/applicationConfiguration()->refreshRate());
	image_update_timer.setSingleShot(false);
	
	connect(&image_update_timer,SIGNAL(timeout()),SLOT(imageUpdateTick()));
	QObject::connect(cameraController(),SIGNAL(cameraInitialised()),&image_update_timer,SLOT(start()));	
}

void
WindowedGuiController::showUserShowAvatar()
{
	animation_state = SHOW_USER_SHOW_AVATAR;
	updateItems();	
	show_user_button->setChecked(true);
	user_input->setOpacity(1.0,true);
	
	if (show_tracking_mesh) {
		tracking_mesh->setOpacity(1.0,true);
	}
}

void
WindowedGuiController::hideUserShowAvatar()
{
	animation_state = HIDE_USER_SHOW_AVATAR;
	updateItems();
	show_user_button->setChecked(false);
	user_input->setOpacity(0.0,true);	
	tracking_mesh->setOpacity(0.0,true);
}

void
WindowedGuiController::showAvatarSelection()
{
	avatar_selection_state = SHOW_AVATAR_SELECTION;
	updateItems();
	
	show_avatar_selection_button->setChecked(true);
	
	if (gui_state == ANIMATION)
		avatar_selector->setOpacity(1.0,true);
}

void
WindowedGuiController::hideAvatarSelection()
{
	avatar_selection_state = HIDE_AVATAR_SELECTION;
	updateItems();
	
	show_avatar_selection_button->setChecked(false);
	
	if (gui_state == ANIMATION)
		avatar_selector->setOpacity(0.0,true);
}

void
WindowedGuiController::showTrackerMesh()
{
	show_tracking_mesh = true;
	show_tracking_mesh_button->setChecked(true);
	if ((gui_state == ANIMATION) && (animation_state == SHOW_USER_SHOW_AVATAR))
		tracking_mesh->setOpacity(1.0,true);
	
	if (gui_state == USER_INITIALISATION)
		tracking_mesh->setOpacity(1.0,true);
}

void
WindowedGuiController::hideTrackerMesh()
{
	show_tracking_mesh = false;
	show_tracking_mesh_button->setChecked(false);	
	tracking_mesh->setOpacity(0.0,true);
}

void
WindowedGuiController::setFullscreenMode()
{

}

void
WindowedGuiController::setWindowedMode()
{

}

void
WindowedGuiController::showFramesPerSecond()
{

}
void
WindowedGuiController::hideFramesPerSecond()
{

}

void
WindowedGuiController::turnOnWarningAboutFaceOutOfFrame()
{
	switch (animation_state) {
		case SHOW_USER_SHOW_AVATAR:
			face_out_of_bounds_warning_user->setOpacity(1.0,true);
			face_out_of_bounds_warning_avatar->setOpacity(0.0,true);
			break;
		case HIDE_USER_SHOW_AVATAR:
			face_out_of_bounds_warning_user->setOpacity(0.0,true);
			face_out_of_bounds_warning_avatar->setOpacity(1.0,true);
			break;
	}
	show_face_out_frame_warning = true;
}

void
WindowedGuiController::turnOffWarningAboutFaceOutOfFrame()
{
	face_out_of_bounds_warning_user->setOpacity(0.0,true);
	face_out_of_bounds_warning_avatar->setOpacity(0.0,true);
	show_face_out_frame_warning = false;	
}


void
WindowedGuiController::configureForUserInitialisation()
{
	gui_state = USER_INITIALISATION;
	updateItems();
	
	avatar_animation->setOpacity(0.0,true);
	avatar_selector->setOpacity(0.0,true);
	user_input->setOpacity(1.0,true);
	
	if (show_tracking_mesh)
		tracking_mesh->setOpacity(1.0,true);
	else
		tracking_mesh->setOpacity(0.0,true);
	
	gui_state_change_button->setText("Calibrate");
	gui_state_change_button->setVisible(true);
	reset_tracker_button->setVisible(true);	
	show_user_button->setVisible(false);
	show_tracking_mesh_button->setVisible(true);	
		
	show_avatar_selection_button->setVisible(false);
	status_message->setVisible(true);
	status_message->setText("Please present a neutral expression and then press Calibrate");
	
	tracking_health->setVisible(true);
	
	gui_state_change_button->setEnabled(trackerController()->trackingQualityDecision());
	gui_state_change_button->connect(trackerController(),SIGNAL(trackingQualityDecisionChanged(bool)), SLOT(setEnabled(bool)));
	
	face_out_of_bounds_warning_avatar->setOpacity(0.0,true);
	if (show_face_out_frame_warning) 
		face_out_of_bounds_warning_user->setOpacity(1.0,true);
	else
		face_out_of_bounds_warning_user->setOpacity(0.0,true);
}

void
WindowedGuiController::configureForCameraInitialisation()
{
	gui_state = CAMERA_INITIALISATION;
	updateItems();
	
	avatar_animation->setOpacity(0.0);
	avatar_selector->setOpacity(0.0);
	user_input->setOpacity(0.0);
	reset_tracker_button->setVisible(false);
	gui_state_change_button->setVisible(false);
	show_user_button->setVisible(false);
	show_avatar_selection_button->setVisible(false);
	status_message->setVisible(true);
	status_message->setText("Waiting for camera");	
	tracking_health->setVisible(false);
	tracking_mesh->setOpacity(0.0);
	show_tracking_mesh_button->setVisible(false);
	
	face_out_of_bounds_warning_user->setOpacity(0.0);
	face_out_of_bounds_warning_avatar->setOpacity(0.0);
}

void
WindowedGuiController::configureForAnimation()
{
	gui_state = ANIMATION;
	updateItems();
	
	avatar_animation->setOpacity(1.0,true);
	
	if (avatar_selection_state == HIDE_AVATAR_SELECTION)
		avatar_selector->setOpacity(0.0,true);
	else
		avatar_selector->setOpacity(1.0,true);
	
	gui_state_change_button->setText("Recalibrate");
	gui_state_change_button->setEnabled(true);
	reset_tracker_button->setVisible(true);
	gui_state_change_button->setVisible(true);	
	show_user_button->setVisible(true);
	show_avatar_selection_button->setText("Avatar Selection");
	show_avatar_selection_button->setVisible(true);
	status_message->setVisible(false);
	tracking_health->setVisible(true);
	show_tracking_mesh_button->setVisible(true);
	
	if ((gui_state == ANIMATION) && (animation_state == SHOW_USER_SHOW_AVATAR))
		tracking_mesh->setOpacity(1.0,true);
	
	if (gui_state == USER_INITIALISATION)
		tracking_mesh->setOpacity(1.0,true);	
	
	if (show_face_out_frame_warning) {
		switch (animation_state) {
			case SHOW_USER_SHOW_AVATAR:
				face_out_of_bounds_warning_user->setOpacity(1.0,true);
				face_out_of_bounds_warning_avatar->setOpacity(0.0,true);
				break;
			case HIDE_USER_SHOW_AVATAR:
				face_out_of_bounds_warning_user->setOpacity(0.0,true);
				face_out_of_bounds_warning_avatar->setOpacity(1.0,true);
				break;
			default:
				break;
		}
	} else {
		face_out_of_bounds_warning_user->setOpacity(0.0,true);
		face_out_of_bounds_warning_avatar->setOpacity(0.0,true);	
	}
	
	trackerController()->disconnect(SIGNAL(trackingQualityDecisionChanged(bool)),gui_state_change_button);
}

void
WindowedGuiController::updateItems(qreal width, qreal height)
{
	drawing_area_width = width;
	drawing_area_height = height;
	updateItems(false);
}

void
WindowedGuiController::updateItems(bool animate)
{
	checkBindings();
	
	ItemPositions new_positions = computeItemPositions(drawing_area_width,drawing_area_height);
		
	user_input->setItemPosition(new_positions.user_image_position,animate);
	tracking_mesh->setItemPosition(new_positions.user_image_position, animate);
	avatar_animation->setItemPosition(new_positions.avatar_image_position,animate);
	avatar_selector->setItemPosition(new_positions.avatar_selector_position,animate);
	
	face_out_of_bounds_warning_user->setItemPosition(new_positions.face_out_of_frame_position_user,animate);
	face_out_of_bounds_warning_avatar->setItemPosition(new_positions.face_out_of_frame_position_avatar,animate);
}

ItemPositions
WindowedGuiController::computeItemPositions(qreal width, qreal height)
{		
	switch (gui_state) {
		case CAMERA_INITIALISATION:
			item_positions_calculator.stateCameraInitialisation();
			break;
		case USER_INITIALISATION:
			item_positions_calculator.stateUserInitialisation();
			break;
		case ANIMATION:
			switch (avatar_selection_state) {
				case SHOW_AVATAR_SELECTION:
					switch (animation_state) {
						case SHOW_USER_SHOW_AVATAR:
							item_positions_calculator.stateAnimationShowUserShowSelection();
							break;
						case HIDE_USER_SHOW_AVATAR:
							item_positions_calculator.stateAnimationHideUserShowSelection();
							break;
						default:
							throw std::logic_error("Invalid animation_state");							
					}
					break;
				case HIDE_AVATAR_SELECTION:
					switch (animation_state) {
						case SHOW_USER_SHOW_AVATAR:
							item_positions_calculator.stateAnimationShowUserHideSelection();
							break;
						case HIDE_USER_SHOW_AVATAR:
							item_positions_calculator.stateAnimationHideUserHideSelection();
							break;
						default:
							throw std::logic_error("Invalid animation_state");							
					}
					break;
				default:
					throw std::logic_error("Invalid avatar_selection_state");
			}			
	}
	return item_positions_calculator.calculateItemPositions(width,height,item_positions_constants);
}

void
WindowedGuiController::checkBindings()
{
	if (!reset_tracker_button)
		throw std::runtime_error("reset_tracker_button has not been assigned.");
	
	if (!gui_state_change_button)
		throw std::runtime_error("state_change_button has not been assigned.");
	
	if (!user_input)
		throw std::runtime_error("user_input item positioner has not been assigned.");
	
	if (!avatar_animation)
		throw std::runtime_error("avatar_animation has not been assigned.");
	
	if (!show_user_button)
		throw std::runtime_error("show_user_button has not been assigned.");
	
	if (!show_avatar_selection_button)
		throw std::runtime_error("show_avatar_selection_button has not been assigned.");
	
	if (!avatar_selector)
		throw std::runtime_error("avatar_selector has not been assigned.");
	
	if (!status_message)
		throw std::runtime_error("status_message has not been assigned.");
	
	if (!tracking_health)
		throw std::runtime_error("tracking_health has not been assigned.");
	
	if (!tracking_mesh)
		throw std::runtime_error("tracking_mesh has not been assigned.");
	
	if (!show_tracking_mesh_button)
		throw std::runtime_error("show_tracking_mesh_button has not been assigned.");
	
	if (!face_out_of_bounds_warning_user)
		throw std::runtime_error("face_out_of_bounds_warning_user has not been assigned.");
	
	if (!face_out_of_bounds_warning_avatar)
		throw std::runtime_error("face_out_of_bounds_warning_avatar has not been assigned.");
	
}

void
WindowedGuiController::bindController()
{
	checkBindings();
	
	connect(gui_state_change_button,      SIGNAL(clicked(bool)), this, SLOT(guiStateChangeClicked()));
	connect(show_user_button,             SIGNAL(clicked(bool)), this, SLOT(showUserChangeClicked(bool)));
	connect(show_avatar_selection_button, SIGNAL(clicked(bool)), this, SLOT(showAvatarSelectionClicked(bool)));
	connect(reset_tracker_button,         SIGNAL(clicked(bool)), trackerController(), SLOT(reset()));
	connect(trackerController(),          SIGNAL(trackingQualityChanged(qreal)), this, SLOT(trackingQualityUpdate(qreal)));
	connect(show_tracking_mesh_button,    SIGNAL(clicked(bool)), this, SLOT(trackingMeshButtonClicked(bool)));
	
	tracking_health->setMinimum(0);
	tracking_health->setMaximum(100);
	trackingQualityUpdate(trackerController()->trackingQuality());
	
	configureForCameraInitialisation();
}

void
WindowedGuiController::guiStateChangeClicked()
{	
	switch (gui_state) {
		case USER_INITIALISATION:
			emit initialiseUserNowRequest();
			break;
		case ANIMATION:
			emit reinitialiseUserRequest();
			break;
		default:
			throw std::logic_error("Invalid gui_state in guiStateChangeClicked()");
	}
}

void
WindowedGuiController::showUserChangeClicked(bool show)
{
	if (show)
		emit showUserImageRequest();
	else
		emit hideUserImageRequest();
}

void
WindowedGuiController::showAvatarSelectionClicked(bool show)
{
	if (show)
		emit showAvatarSelectionRequest();
	else
		emit hideAvatarSelectionRequest();
}

void
WindowedGuiController::imageUpdateTick()
{
	WorkerData *data = workerThread()->currentData();
	
	if (!workerThread()->isRunning() && (gui_state != CAMERA_INITIALISATION))
		workerThread()->start();
	
	if (!data)
		return;
	
	if (data->input_image.isNull())
		return;
	
	user_input->setPixmap(QPixmap::fromImage(data->input_image));
	tracking_mesh->setMesh(data->cv_tracked_shape);
	
	if (gui_state == ANIMATION) {
		avatar_animation->setPixmap(QPixmap::fromImage(data->animated_image));
	}
}

void
WindowedGuiController::trackingQualityUpdate(qreal new_value)
{
	tracking_health->setValue(new_value*100);
}

void
WindowedGuiController::trackingMeshButtonClicked(bool hide_or_show)
{
	if (hide_or_show)
		emit showTrackerMeshRequest();
	else
		emit hideTrackerMeshRequest();
}


