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

#ifndef _CI2CV_GUI_GUI_GRAPHICS_SCROLLBAR_HPP_
#define _CI2CV_GUI_GUI_GRAPHICS_SCROLLBAR_HPP_

#include <QtCore/QObject>
#include <QtCore/QRectF>
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QBrush>
#include <QtGui/QPen>

namespace CI2CVGui {
	class GraphicsScrollbar;
	
	class GraphicsScroller : public QGraphicsPathItem
	{
	public:
		GraphicsScroller(GraphicsScrollbar *parent);
		
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
		void mouseMoveEvent(QGraphicsSceneMouseEvent *event);		
		
	private:
		GraphicsScrollbar *scrollbar;
	};
	
	class GraphicsScrollbar : public QObject,
							  public QGraphicsItemGroup
	{
		Q_OBJECT
				
	public:
		GraphicsScrollbar(QGraphicsItem *parent_item = NULL);
			
		qreal percentageDisplayed() const;
		qreal percentage() const;
		
		QRectF scrollbarPosition() const;
		
		qreal percentageFromCursorPosition(qreal x, qreal y) const;
		
	public slots:
		void setPercentage(qreal percentage);
		void setPercentageDisplayed(qreal percentage_shown);
		void setScrollbarPosition(QRectF position);
		
	signals:
		void percentageChanged(qreal new_percentage);	
		void percentageDisplayedChanged(qreal new_percentage_displayed);
		
	private:
		void updateGraphics();
		
	private:
		qreal percentage_of_data;
		qreal percentage_displayed;
		QRectF scrollbar_position;
		
		QGraphicsPathItem outer_path_item;
		GraphicsScroller inner_path_item;
		
		QBrush outer_path_brush;
		QBrush inner_path_brush;
		QPen outer_path_pen;
		QPen inner_path_pen;
	};
}

#endif
