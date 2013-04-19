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

#ifndef _CI2CV_GUI_GUI_ITEM_POSITIONS_CALCULATOR_HPP_
#define _CI2CV_GUI_GUI_ITEM_POSITIONS_CALCULATOR_HPP_

#include <QtCore/QObject>
#include <QtCore/QRectF>
#include <QtCore/QScopedPointer>

namespace CI2CVGui {	
	class ItemPositionsConstants {
	public:
		ItemPositionsConstants();
		
		qreal marginFromRightPercentage() const;
		qreal marginFromLeftPercentage() const;
		qreal marginFromBottomPercentage() const;
		qreal marginFromTopPercentage() const;
		
		qreal marginBetweenUserAndAvatarPercentage() const;
		
		qreal imagesHeightPercentage() const;
		qreal selectionHeightPercentage() const;
		
	private:
		qreal margin_from_right_percentage;
		qreal margin_from_left_percentage;
		qreal margin_from_top_percentage;		
		qreal margin_from_bottom_percentage;
		
		qreal margin_between_user_and_avatar_percentage;
		
		qreal images_height_percentage;
		qreal selection_height_percentage;
	};
	
	class ItemPositions {
	public:
		QRectF user_image_position;
		QRectF avatar_image_position;		
		QRectF avatar_selector_position;
		QRectF face_out_of_frame_position_user;
		QRectF face_out_of_frame_position_avatar;
	};
	
	class ItemPositionsCalculator {
	public:
		virtual ~ItemPositionsCalculator();
	
		virtual ItemPositions calculateItemPositions(qreal width, qreal height, const ItemPositionsConstants &constants) const = 0;
	};
	
	class WindowedGuiController;
	
	class WindowedGuiControllerItemPositionsCalculator : public QObject {
	public:
		WindowedGuiControllerItemPositionsCalculator();
		~WindowedGuiControllerItemPositionsCalculator();
		
		ItemPositions calculateItemPositions(qreal width, qreal height, const ItemPositionsConstants &constants) const;
		
	public slots:
		void stateCameraInitialisation();
		void stateUserInitialisation();
		void stateAnimationShowUserShowSelection();
		void stateAnimationHideUserShowSelection();
		void stateAnimationShowUserHideSelection();
		void stateAnimationHideUserHideSelection();
		
		typedef QScopedPointer<ItemPositionsCalculator,QScopedPointerDeleter<ItemPositionsCalculator> > Pointer;		
		
	private:		
		ItemPositionsCalculator *calculator;
		
		enum States {
			UNDEFINED,
			CAMERA_INITIALISATION,
			USER_INITIALISATION,
			SHOW_USER_SHOW_SELECTION,
			HIDE_USER_SHOW_SELECTION,
			SHOW_USER_HIDE_SELECTION,
			HIDE_USER_HIDE_SELECTION
		};
		
		States state;
		
		void swap(States new_state, ItemPositionsCalculator *new_pointer);
	};
}

#endif
