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

#include <QtCore/QObject>
#include <QtGui/QApplication>
#include <QtCore/QStateMachine>
#include <QtCore/QDebug>

#include <stdexcept>
#include <iostream>

#include "application-states.hpp"
#include "controllers.hpp"
#include "configuration.hpp"

void
print_usage()
{
  std::string text =
    "Usage: [options]\n"
    "\n"
    "Options:\n"
    "  --camera-index <int>         The camera to use. Default is 0.\n"
    "\n";
  std::cout << text << std::endl;
}

int
main_program(int argc, char **argv)
{			
        for(int i = 1; i < argc; i++) {
	  std::string arg(argv[i]);
	  if (arg == "--help") {
	    print_usage();
	    return 0;
	  }
	}

	QApplication application(argc,argv);
			
	QStateMachine state_machine;
	
	CI2CVGui::ApplicationStates states;
	state_machine.addState(&states.initialisation_state);
	state_machine.addState(&states.application_state);
	state_machine.setInitialState(&states.initialisation_state);
		
	CI2CVGui::GuiController     *gui     = CI2CVGui::guiController();
	CI2CVGui::TrackerController *tracker = CI2CVGui::trackerController();
	CI2CVGui::CameraController  *camera  = CI2CVGui::cameraController();
	CI2CVGui::AvatarController  *avatar  = CI2CVGui::avatarController();
	
	states.initialisation_state.addTransition(camera,SIGNAL(cameraInitialised()),&states.application_state);
		
	states.avatar_selection_hide.addTransition(gui,SIGNAL(showAvatarSelectionRequest()),&states.avatar_selection_show);
	states.avatar_selection_show.addTransition(gui,SIGNAL(hideAvatarSelectionRequest()),&states.avatar_selection_hide);
	
	states.animating_show_avatar.addTransition(gui,SIGNAL(showUserImageRequest()),&states.animating_show_avatar_and_user);
	states.animating_show_avatar_and_user.addTransition(gui,SIGNAL(hideUserImageRequest()),&states.animating_show_avatar);
	
	states.display_initialising.addTransition(gui,SIGNAL(initialiseUserNowRequest()),&states.display_animating);
	states.display_animating.addTransition(gui,SIGNAL(reinitialiseUserRequest()),&states.display_initialising);
	
	states.camera_on.addTransition(gui,SIGNAL(cameraOffRequest()),&states.camera_off);
	states.camera_off.addTransition(gui,SIGNAL(cameraOnRequest()),&states.camera_on);
	
	states.frames_per_second_true.addTransition(gui,SIGNAL(hideFramesPerSecondRequest()),&states.frames_per_second_false);
	states.frames_per_second_false.addTransition(gui,SIGNAL(showFramesPerSecondRequest()),&states.frames_per_second_true);
	
	states.fullscreen_true.addTransition(gui,SIGNAL(windowModeRequest()),&states.fullscreen_false);
	states.fullscreen_false.addTransition(gui,SIGNAL(fullscreenModeRequest()),&states.fullscreen_true);
	
	states.tracker_mesh_display_off.addTransition(gui,SIGNAL(showTrackerMeshRequest()),&states.tracker_mesh_display_on);
	states.tracker_mesh_display_on.addTransition(gui,SIGNAL(hideTrackerMeshRequest()),&states.tracker_mesh_display_off);
	
	states.face_out_of_bounds_warning_on.addTransition(tracker,SIGNAL(faceIsNowInsideTheFrame()),&states.face_out_of_bounds_warning_off);
	states.face_out_of_bounds_warning_off.addTransition(tracker,SIGNAL(faceIsNowOutsideTheFrame()),&states.face_out_of_bounds_warning_on);
	
	tracker->connect(&states.display_initialising,SIGNAL(entered()), SLOT(reset()));
	
	tracker->connect(&states.camera_off,SIGNAL(entered()),SLOT(stop()));
	tracker->connect(&states.camera_on,SIGNAL(entered()),SLOT(start()));
	
	camera->connect(&states.camera_off,SIGNAL(entered()),SLOT(stop()));
	camera->connect(&states.camera_on,SIGNAL(entered()),SLOT(start()));	
	camera->connect(&states.initialisation_state,SIGNAL(entered()),SLOT(initialiseCamera()));
	camera->connect(&states.initialisation_state,SIGNAL(exited()),SLOT(start()));
	
	avatar->connect(&states.display_animating,SIGNAL(entered()),SLOT(initialiseUser()));
	avatar->connect(&states.display_initialising,SIGNAL(entered()),SLOT(stop()));
	avatar->connect(&states.display_animating,SIGNAL(entered()),SLOT(start()));
		
	gui->connect(&states.avatar_selection_hide,SIGNAL(entered()),SLOT(hideAvatarSelection()));
	gui->connect(&states.avatar_selection_show,SIGNAL(entered()),SLOT(showAvatarSelection()));	
	
	gui->connect(&states.tracker_mesh_display_off,SIGNAL(entered()),SLOT(hideTrackerMesh()));
	gui->connect(&states.tracker_mesh_display_on, SIGNAL(entered()),SLOT(showTrackerMesh()));	
	
	gui->connect(&states.initialisation_state,SIGNAL(entered()),SLOT(configureForCameraInitialisation()));	
	gui->connect(&states.display_initialising,SIGNAL(entered()),SLOT(configureForUserInitialisation()));
	gui->connect(&states.display_animating,SIGNAL(entered()),SLOT(configureForAnimation()));
	
	gui->connect(&states.animating_show_avatar_and_user,SIGNAL(entered()),SLOT(showUserShowAvatar()));
	gui->connect(&states.animating_show_avatar,SIGNAL(entered()),SLOT(hideUserShowAvatar()));
	
	gui->connect(&states.fullscreen_true,SIGNAL(entered()),SLOT(setFullscreenMode()));
	gui->connect(&states.fullscreen_false,SIGNAL(entered()),SLOT(setWindowedMode()));
	
	gui->connect(&states.face_out_of_bounds_warning_on, SIGNAL(entered()), SLOT(turnOnWarningAboutFaceOutOfFrame()));
	gui->connect(&states.face_out_of_bounds_warning_off, SIGNAL(entered()), SLOT(turnOffWarningAboutFaceOutOfFrame()));
	
	state_machine.start();		
	
	return application.exec();		
}

int
main(int argc, char **argv)
{
	try {
		return main_program(argc,argv);
	} catch (std::exception &e) {		
		qDebug() << "Caught Exception: " << e.what();
		return -1;
	} catch (...) {
		qDebug() << "Caught Unknown exception.";
		return -2;
	}
}
