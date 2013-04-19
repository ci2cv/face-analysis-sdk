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

#ifndef _QT_GUI_APPLICATION_STATES_HPP_
#define _QT_GUI_APPLICATION_STATES_HPP_

#include <QtCore/QState>

namespace CI2CVGui 
{	
	class ApplicationStates {
	public:
		ApplicationStates();
		
		QState initialisation_state;
		
		/**
		 * The global state
		 */
		QState application_state;
		
		/**
		 * All of the states of the application which run parallel.
		 */
		QState camera;	
		QState display;		
		QState tracker_mesh_display;
		QState avatar_selection;
		QState fullscreen;
		QState frames_per_second;
		QState face_out_of_bounds_warning;
		
		/**
		 * States for the camera
		 */
		QState camera_on;
		QState camera_off;
				
		/**
		 * Show the tracker mesh output.
		 */
		QState tracker_mesh_display_off;
		QState tracker_mesh_display_on;
		
		
		/**
		 * Display states
		 */
		QState display_animating;
		QState display_initialising;
		
		/**
		 * Animating states
		 */
		QState animating_show_avatar_and_user;
		QState animating_show_avatar;
				
		/**
		 * Avatar selection
		 */
		QState avatar_selection_hide;
		QState avatar_selection_show;
		
		/**
		 * Full Screen
		 */
		QState fullscreen_true;
		QState fullscreen_false;
		
		/** 
		 * Frames Per Second
		 */
		QState frames_per_second_true;
		QState frames_per_second_false;
		
		/**
		 * Face out of bounds warnings
		 */
		QState face_out_of_bounds_warning_on;
		QState face_out_of_bounds_warning_off;
	};
}

#endif
