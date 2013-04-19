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

#include "gui/graphics-scrollbar.hpp"
#include <QtGui/QPainterPath>
#include <QtCore/QDebug>
#include <QtGui/QGraphicsSceneMouseEvent>

using namespace CI2CVGui;

GraphicsScroller::GraphicsScroller(GraphicsScrollbar *parent)
: QGraphicsPathItem(parent),
	scrollbar(parent)
{
	setAcceptedMouseButtons(Qt::LeftButton);
}

void
GraphicsScroller::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void
GraphicsScroller::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
}

void
GraphicsScroller::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF parent_p = mapToParent(0.0,0.0);	
	QPointF p = event->lastPos() - event->buttonDownPos(Qt::LeftButton);	
	QPointF relative_position = parent_p + p;
	
	qreal new_percentage = scrollbar->percentageFromCursorPosition(relative_position.x(),relative_position.y());
	
	scrollbar->setPercentage(new_percentage);	
}


// GraphicsScrollbar

GraphicsScrollbar::GraphicsScrollbar(QGraphicsItem *parent)
: QGraphicsItemGroup(parent),
	percentage_of_data(0.0),
	percentage_displayed(0.5),
	outer_path_item(this),
	inner_path_item(this)
{
	inner_path_brush.setStyle(Qt::SolidPattern);
	setHandlesChildEvents(false);
}

qreal
GraphicsScrollbar::percentage() const
{
	return percentage_of_data;
}

qreal
GraphicsScrollbar::percentageDisplayed() const
{
	return percentage_displayed;
}

QRectF
GraphicsScrollbar::scrollbarPosition() const
{
	return scrollbar_position;
}

void
GraphicsScrollbar::setPercentage(qreal new_percentage)
{
	if (percentage_of_data == new_percentage)
		return;
	
	new_percentage = std::max(0.0,new_percentage);
	new_percentage = std::min(1.0,new_percentage);
		
	percentage_of_data = new_percentage;
	updateGraphics();
	emit percentageChanged(new_percentage);
}

void
GraphicsScrollbar::setPercentageDisplayed(qreal new_percentage_shown)
{
	if (percentage_displayed == new_percentage_shown)
		return;
	
	new_percentage_shown = std::max(0.0,new_percentage_shown);
	new_percentage_shown = std::min(1.0,new_percentage_shown);
	
	percentage_displayed = new_percentage_shown;
	updateGraphics();
	emit percentageDisplayedChanged(percentage_displayed);	
}

void
GraphicsScrollbar::setScrollbarPosition(QRectF new_position)
{
	scrollbar_position = new_position;
	updateGraphics();
}

void
GraphicsScrollbar::updateGraphics() 
{
	QPainterPath outer_path;
	qreal yradius = scrollbar_position.height();
	qreal xradius = yradius;
	
	outer_path.addRoundedRect(0,0,scrollbar_position.width(),scrollbar_position.height(), xradius, yradius);	
	outer_path_item.setPath(outer_path);
	outer_path_item.setBrush(outer_path_brush);
	outer_path_item.setPen(outer_path_pen);
	
	qreal percentage_to_show_width = percentage_displayed*scrollbar_position.width();
	qreal displayed_end   = scrollbar_position.width() - percentage_to_show_width;
	qreal position_x      = displayed_end*percentage_of_data;
		
	QRectF inner_rect(0.0,0.0,percentage_to_show_width,scrollbar_position.height());
	
	QPainterPath inner_path;
	inner_path.addRoundedRect(inner_rect,xradius,yradius);
	inner_path_item.setPath(inner_path);
	inner_path_item.setBrush(inner_path_brush);
	inner_path_item.setPen(inner_path_pen);
	inner_path_item.setX(position_x);
	
	setPos(scrollbar_position.x(),scrollbar_position.y());
}

qreal
GraphicsScrollbar::percentageFromCursorPosition(qreal x, qreal) const
{
	qreal percentage_to_show_width = percentage_displayed*scrollbar_position.width();
	qreal displayed_end            = scrollbar_position.width() - percentage_to_show_width;
	
	qreal rv = x/displayed_end;	
	
	rv = std::max(rv,0.0);
	rv = std::min(rv,1.0);

	return rv;
}

