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

#include "gui/avatar-selection.hpp"
#include "controllers.hpp"

#include <QtCore/QDebug>
#include <QtGui/QGraphicsSceneMouseEvent>

using namespace CI2CVGui;

AvatarThumbnail::AvatarThumbnail(QGraphicsItem *parent)
: QGraphicsPixmapItem(parent)
{
	setAcceptedMouseButtons(Qt::LeftButton);
}

int
AvatarThumbnail::avatarIndex() const
{
	return avatar_index;
}

void
AvatarThumbnail::setAvatarIndex(int index)
{
	avatar_index = index;
}

void
AvatarThumbnail::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void
AvatarThumbnail::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (contains(event->lastPos()))
		emit clicked(avatar_index);
}


AvatarSelection::AvatarSelection(QGraphicsItem *parent)
:   QGraphicsItemGroup(parent),
	current_selection(-1),
	scroller(this),
	selection_item(this),
	pixmaps(new QPixmap[avatarController()->numberOfAvatars()]),
	thumbnails(new AvatarThumbnail[avatarController()->numberOfAvatars()]),
	margin_from_top_percentage(0.1),
	margin_from_bottom_percentage(0.1),
	margin_between_thumbnails_percentage(0.25),
	scrollbar_height(12),
	scroller_margin_from_left_percentage(0.1),
	scroller_margin_from_right_percentage(0.1)
{
	setHandlesChildEvents(false);
	
	for (int i = 0; i < avatarController()->numberOfAvatars(); i++) {
		pixmaps[i] = QPixmap::fromImage(avatarController()->avatarThumbnail(i));
		addToGroup(&thumbnails[i]);			
		
		connect(&thumbnails[i],SIGNAL(clicked(int)),SLOT(avatarThumbnailClicked(int)));
		thumbnails[i].setAvatarIndex(i);
	}
	
	connect(&scroller,SIGNAL(percentageChanged(qreal)),SLOT(setScrollbarPosition(qreal)));
	
	updateItems();
	setSelection(avatarController()->avatarIndex());
}

void
AvatarSelection::setSelection(int index)
{
	current_selection = index;
	updateItems();
	emit selectionChanged(index);
}

QRectF
AvatarSelection::position() const
{
	return selection_position;
}

void
AvatarSelection::setPosition(QRectF position)
{
	selection_position = position;
	updateItems();
}

void
AvatarSelection::setScrollbarPosition(qreal new_percentage)
{
	scrollbar_percentage = new_percentage;
	updateItems();
}

static
qreal 
determine_opacity(qreal x_left_position, qreal x_right_position, qreal element_width, qreal total_width)
{
	if ((x_left_position < 0) && (x_right_position < 0))
		return 0.0;
	else if ((x_left_position < 0) && (x_right_position >= 0))
		return x_right_position/element_width;
	else if ((x_left_position < total_width) && (x_right_position <= total_width))
		return 1.0;
	else if ((x_left_position < total_width) && (x_right_position > total_width))
		return (total_width - x_left_position)/element_width;
	else 
		return 0.0;	
}

void
AvatarSelection::updateItems()
{
	QRectF thumbnail_area(selection_position.x(),selection_position.y(),selection_position.width(),selection_position.height() - scrollbar_height);
	
	qreal margin_from_top    = margin_from_top_percentage    * thumbnail_area.height();
	qreal margin_from_bottom = margin_from_bottom_percentage * thumbnail_area.height();
	qreal thumbnail_height = thumbnail_area.height() - margin_from_top - margin_from_bottom;
	
	qreal scroller_margin_from_left  = scroller_margin_from_left_percentage  * selection_position.width();
	qreal scroller_margin_from_right = scroller_margin_from_right_percentage * selection_position.width();
	QRectF scrollbar_area(selection_position.x() + scroller_margin_from_left,
						  selection_position.y() + thumbnail_area.height(),
						  selection_position.width() - scroller_margin_from_left - scroller_margin_from_right, 
						  scrollbar_height);
	
	int maximum_width = 0.0;
	for (int i = 0; i < avatarController()->numberOfAvatars(); i++) {
		QSize thumbnail_size   = pixmaps[i].size();
		thumbnail_size.scale(selection_position.width(), thumbnail_height, Qt::KeepAspectRatio);
		
		maximum_width = std::max(maximum_width,thumbnail_size.width());
	}
	
	qreal margin_between = margin_between_thumbnails_percentage*maximum_width;
	qreal total_width    = avatarController()->numberOfAvatars()*(maximum_width + margin_between);
	
	qreal x_offset       = (total_width - thumbnail_area.width() + margin_between) * scrollbar_percentage - margin_between;
	
	scroller.setPercentageDisplayed(thumbnail_area.width()/total_width);
	
	if (scroller.percentageDisplayed() == 1.0)
		x_offset = 0.0;
	
	
	for (int i = 0; i < avatarController()->numberOfAvatars(); i++) {
		QSize thumbnail_size   = pixmaps[i].size();
		thumbnail_size.scale(selection_position.width(),thumbnail_height,Qt::KeepAspectRatio);
		
		thumbnails[i].setPixmap(pixmaps[i].scaled(thumbnail_size,Qt::KeepAspectRatio));
		
		qreal x_left_position  = i*(maximum_width + margin_between) - x_offset;
		qreal x_right_position = x_left_position + maximum_width;
		
		qreal y_position = selection_position.y() + margin_from_top;
		
		thumbnails[i].setPos(x_left_position,y_position);
		
		qreal opacity = determine_opacity(x_left_position,x_right_position,maximum_width,thumbnail_area.width());		
		
		thumbnails[i].setOpacity(opacity);
	}
	
	if (current_selection >= 0) {
		qreal x_position = current_selection*(maximum_width + margin_between) - x_offset;
		qreal y_position = selection_position.y() + margin_from_top;
		QRectF select_item_rect(x_position,y_position,maximum_width,thumbnail_height);
		select_item_rect.adjust(-5,-5,5,5);
	
		QPen pen;
		pen.setColor(qRgb(255,0,0));
		pen.setWidth(3);
	
		selection_item.setPen(pen);
		selection_item.setRect(select_item_rect);
		
		qreal opacity = determine_opacity(x_position,x_position + select_item_rect.width(),maximum_width,thumbnail_area.width());		
		
		selection_item.setOpacity(opacity);		
	}
		
	scroller.setScrollbarPosition(scrollbar_area);
}

void
AvatarSelection::avatarThumbnailClicked(int index)
{
	current_selection = index;
	updateItems();

	emit selectionChanged(current_selection);
}
