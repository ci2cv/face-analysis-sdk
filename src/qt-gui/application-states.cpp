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

#include "application-states.hpp"

using namespace CI2CVGui;

ApplicationStates::ApplicationStates()
  : application_state(QState::ParallelStates),
	camera(&application_state),
	display(&application_state),
	tracker_mesh_display(&application_state),
    avatar_selection(&application_state),
	fullscreen(&application_state),
	frames_per_second(&application_state),
    face_out_of_bounds_warning(&application_state),

	camera_on(&camera),
	camera_off(&camera),

	tracker_mesh_display_off(&tracker_mesh_display),
	tracker_mesh_display_on(&tracker_mesh_display),

	display_animating(&display),
	display_initialising(&display),

	animating_show_avatar_and_user(&display_animating),
	animating_show_avatar(&display_animating),

	avatar_selection_hide(&avatar_selection),
	avatar_selection_show(&avatar_selection),

	fullscreen_true(&fullscreen),
	fullscreen_false(&fullscreen),

	frames_per_second_true(&frames_per_second),
	frames_per_second_false(&frames_per_second),

	face_out_of_bounds_warning_on(&face_out_of_bounds_warning),
	face_out_of_bounds_warning_off(&face_out_of_bounds_warning)
{
	camera.setInitialState(&camera_on);
	display.setInitialState(&display_initialising);		
	tracker_mesh_display.setInitialState(&tracker_mesh_display_off);	
	avatar_selection.setInitialState(&avatar_selection_hide);	
	fullscreen.setInitialState(&fullscreen_false);
	frames_per_second.setInitialState(&frames_per_second_false);
	display_animating.setInitialState(&animating_show_avatar);
	face_out_of_bounds_warning.setInitialState(&face_out_of_bounds_warning_off);
}

