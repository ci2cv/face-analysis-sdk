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

#include "gui/item-positions-calculator.hpp"
#include "gui/windowed-gui-controller.hpp"

#include <QtCore/QDebug>

using namespace CI2CVGui;

static
ItemPositions
IPC_animation_show_user_hide_selection_helper(qreal width, qreal height, const ItemPositionsConstants &constants)
{
	ItemPositions rv;
	
	qreal margin_from_left   =  width * constants.marginFromLeftPercentage();
	qreal margin_from_right  =  width * constants.marginFromRightPercentage();
	qreal margin_from_top    = height * constants.marginFromTopPercentage();
	qreal margin_from_bottom = height * constants.marginFromBottomPercentage();
	
	QSizeF user_size     = QSizeF(640,480);
	QSizeF avatar_size   = QSizeF(640,480);
	qreal  new_width     = std::max(user_size.width(),avatar_size.width());
	QSizeF combined_size = QSizeF(2*new_width,
								  std::max(user_size.height(), avatar_size.height()));
	
	QSizeF combined_size_original = combined_size;
	
	qreal combined_size_scaling;
	qreal user_size_scaling;
	qreal avatar_size_scaling;
	
	QRectF available_area = QRectF(margin_from_left,
								   margin_from_top,
								   width - margin_from_right,
								   height - margin_from_bottom);
	QSizeF available_size = available_area.size();	
	QPointF middle(width/2.0,height/2.0);
	
	user_size_scaling = new_width/((1.0 + constants.marginBetweenUserAndAvatarPercentage()/2.0)*user_size.width());
	avatar_size_scaling = new_width/((1.0 + constants.marginBetweenUserAndAvatarPercentage()/2.0)*avatar_size.width());
	
	combined_size.scale(available_size,Qt::KeepAspectRatio);
	combined_size_scaling = combined_size.width()/combined_size_original.width();
	
	avatar_size *= combined_size_scaling*user_size_scaling;
	user_size   *= combined_size_scaling*avatar_size_scaling;
	
	qreal margin = combined_size.width()/2.0 - std::max(user_size.width(),avatar_size.width());
	qreal half_margin = margin/2.0;

	rv.avatar_image_position = QRectF(middle.x() + half_margin,
									  middle.y() - avatar_size.height()/2.0,
									  avatar_size.width(), avatar_size.height());
	
	rv.user_image_position = QRectF(middle.x() - half_margin - user_size.width(),
									middle.y() - user_size.height()/2.0,
									user_size.width(), user_size.height());	
	
	return rv;
}

static
ItemPositions
IPC_animation_show_user_hide_selection(qreal width, qreal height, const ItemPositionsConstants &constants)
{
	QSizeF images_area(width,height*constants.imagesHeightPercentage());
	QSizeF selection_area(width,height*constants.selectionHeightPercentage());
	
	ItemPositions rv = IPC_animation_show_user_hide_selection_helper(width, height, constants);
	
	rv.avatar_selector_position = QRectF(0.0,images_area.height(),width,selection_area.height());
	
	return rv;	
}

static
ItemPositions
IPC_animation_show_user_show_selection(qreal width, qreal height, const ItemPositionsConstants &constants)
{
	QSizeF images_area(width,height*constants.imagesHeightPercentage());
	QSizeF selection_area(width,height*constants.selectionHeightPercentage());
	
	ItemPositions rv = IPC_animation_show_user_hide_selection_helper(width, images_area.height(), constants);
	
	rv.avatar_selector_position = QRectF(0.0,images_area.height(),width,selection_area.height());
	
	return rv;
}

static
ItemPositions
IPC_animation_hide_user_hide_selection_helper(qreal width, qreal height, const ItemPositionsConstants &constants)
{
	ItemPositions rv;
	
	qreal margin_from_left   =  width * constants.marginFromLeftPercentage();
	qreal margin_from_right  =  width * constants.marginFromRightPercentage();
	qreal margin_from_top    = height * constants.marginFromTopPercentage();
	qreal margin_from_bottom = height * constants.marginFromBottomPercentage();
	
	QSizeF avatar_size = QSizeF(640,480);
	
	QRectF available_area = QRectF(margin_from_left, 
								   margin_from_top, 
								   width - margin_from_right, 
								   height - margin_from_bottom);
	QSizeF  available_size = available_area.size();
	
	avatar_size.scale(available_size, Qt::KeepAspectRatio);
	
	QPointF middle(width/2.0,height/2.0);
	
	rv.avatar_image_position = QRectF(middle.x()  - avatar_size.width()/2.0,
									middle.y() - avatar_size.height()/2.0,
									avatar_size.width(),
									avatar_size.height());
	
	rv.user_image_position = rv.avatar_image_position;			
	
	return rv;
}

static
ItemPositions
IPC_animation_hide_user_hide_selection(qreal width, qreal height, const ItemPositionsConstants &constants)
{
	QSizeF images_area(width,height*constants.imagesHeightPercentage());
	QSizeF selection_area(width,height*constants.selectionHeightPercentage());
	
	ItemPositions rv = IPC_animation_hide_user_hide_selection_helper(width, height, constants);	
	
	rv.avatar_selector_position = QRectF(0.0,images_area.height(),width,selection_area.height());
	
	return rv;	
}


static
ItemPositions
IPC_animation_hide_user_show_selection(qreal width, qreal height, const ItemPositionsConstants &constants)
{
	QSizeF images_area(width,height*constants.imagesHeightPercentage());
	QSizeF selection_area(width,height*constants.selectionHeightPercentage());
	
	ItemPositions rv = IPC_animation_hide_user_hide_selection_helper(width, images_area.height(), constants);	
	
	rv.avatar_selector_position = QRectF(0.0,images_area.height(),width,selection_area.height());	
	
	return rv;
}

static
ItemPositions
IPC_user_initialisation(qreal width, qreal height, const ItemPositionsConstants &constants)
{
	ItemPositions rv = IPC_animation_hide_user_hide_selection(width,height,constants);	
	
	return rv;
}

static
ItemPositions
IPC_camera_initialisation(qreal width, qreal height, const ItemPositionsConstants &constants)
{
	ItemPositions rv = IPC_user_initialisation(width,height,constants);
	
	return rv;
}


ItemPositionsConstants::ItemPositionsConstants()
:	margin_from_right_percentage(0.05),
	margin_from_left_percentage(0.05),
	margin_from_top_percentage(0.05),
	margin_from_bottom_percentage(0.05),
	margin_between_user_and_avatar_percentage(0.075),
	images_height_percentage(0.7),
	selection_height_percentage(0.26)
{
	
}

qreal
ItemPositionsConstants::marginFromRightPercentage() const
{
	return margin_from_right_percentage;
}

qreal
ItemPositionsConstants::marginFromLeftPercentage() const
{
	return margin_from_left_percentage;
}

qreal
ItemPositionsConstants::marginFromBottomPercentage() const
{
	return margin_from_bottom_percentage;
}

qreal
ItemPositionsConstants::marginFromTopPercentage() const
{
	return margin_from_top_percentage;
}

qreal
ItemPositionsConstants::imagesHeightPercentage() const
{
	return images_height_percentage;
}

qreal
ItemPositionsConstants::selectionHeightPercentage() const
{
	return selection_height_percentage;
}

qreal
ItemPositionsConstants::marginBetweenUserAndAvatarPercentage() const
{
	return margin_between_user_and_avatar_percentage;
}

template <typename A>
class ItemPositionsFunctor : public ItemPositionsCalculator
{
public:
	ItemPositionsFunctor(A a) : op(a) {}
	
	ItemPositions calculateItemPositions(qreal width, qreal height, const ItemPositionsConstants &constants) const
	{
		return op(width,height,constants);
	}
	
private:
	A op;
};

template <typename A>
ItemPositionsCalculator *
make_functor(A a)
{
	return new ItemPositionsFunctor<A>(a);
}

ItemPositions
WindowedGuiControllerItemPositionsCalculator::calculateItemPositions(qreal width, qreal height, const ItemPositionsConstants &constants) const
{
	ItemPositions rv = calculator->calculateItemPositions(width,height,constants);	
	rv.face_out_of_frame_position_user = rv.user_image_position;	
	rv.face_out_of_frame_position_avatar = rv.avatar_image_position;		
	return rv;
}

#define swapif(a,b) if (state != a) swap(a,b);
void
WindowedGuiControllerItemPositionsCalculator::stateCameraInitialisation()
{
	swapif(CAMERA_INITIALISATION, make_functor(IPC_camera_initialisation));
}

void
WindowedGuiControllerItemPositionsCalculator::stateUserInitialisation()
{
	swapif(USER_INITIALISATION, make_functor(IPC_user_initialisation));		
}

void
WindowedGuiControllerItemPositionsCalculator::stateAnimationShowUserShowSelection()
{		
	swapif(SHOW_USER_SHOW_SELECTION, make_functor(IPC_animation_show_user_show_selection));
}

void
WindowedGuiControllerItemPositionsCalculator::stateAnimationHideUserShowSelection()
{		
	swapif(HIDE_USER_SHOW_SELECTION, make_functor(IPC_animation_hide_user_show_selection));
}

void
WindowedGuiControllerItemPositionsCalculator::stateAnimationShowUserHideSelection()
{		
	swapif(SHOW_USER_HIDE_SELECTION, make_functor(IPC_animation_show_user_hide_selection));
}

void
WindowedGuiControllerItemPositionsCalculator::stateAnimationHideUserHideSelection()
{		
	swapif(HIDE_USER_HIDE_SELECTION, make_functor(IPC_animation_hide_user_hide_selection));
}


WindowedGuiControllerItemPositionsCalculator::WindowedGuiControllerItemPositionsCalculator()
: calculator(NULL),
  state(UNDEFINED)
{
	
}

WindowedGuiControllerItemPositionsCalculator::~WindowedGuiControllerItemPositionsCalculator()
{
	if (calculator)
		delete calculator;
}

void
WindowedGuiControllerItemPositionsCalculator::swap(States new_state, ItemPositionsCalculator *new_pointer)
{
	if (calculator)
		delete calculator;
	
	calculator = new_pointer;
	state = new_state;
}

ItemPositionsCalculator::~ItemPositionsCalculator()
{

}
