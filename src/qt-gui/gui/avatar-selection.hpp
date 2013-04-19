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

#ifndef _CI2CV_GUI_GUI_AVATAR_SELECTION_HPP_
#define _CI2CV_GUI_GUI_AVATAR_SELECTION_HPP_

#include <QtCore/QObject>
#include <QtCore/QRectF>
#include <QtGui/QGraphicsItemGroup>
#include <QtCore/QScopedArrayPointer>

#include "gui/graphics-scrollbar.hpp"

namespace CI2CVGui {
	class AvatarThumbnail : public QObject, public QGraphicsPixmapItem
	{
		Q_OBJECT
		
	public:
		AvatarThumbnail(QGraphicsItem *parent = 0);
		
		int avatarIndex() const;
		void setAvatarIndex(int index);
		
		
	signals:
		void clicked(int avatar_index);
		
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
		
	private:
		int avatar_index;
	};
	
	class AvatarSelection : public QObject, public QGraphicsItemGroup
	{
		Q_OBJECT
	public:
		AvatarSelection(QGraphicsItem *parent = 0);
		
		int selection() const;
				
		QRectF position() const;
		
	public slots:
		void setSelection(int index);
		void setPosition(QRectF position);
		
	private slots:
		void setScrollbarPosition(qreal percentage);
		void avatarThumbnailClicked(int index);
		
	signals:
		void selectionChanged(int index);		
		
	private:
		void updateItems();
		
	private:
		int current_selection;
		QRectF selection_position;
		qreal scrollbar_percentage;
		
		GraphicsScrollbar scroller;
		QGraphicsRectItem selection_item;
		QScopedArrayPointer<QPixmap> pixmaps;
		QScopedArrayPointer<AvatarThumbnail> thumbnails;
				
		const qreal margin_from_top_percentage;
		const qreal margin_from_bottom_percentage;
		
		const qreal margin_between_thumbnails_percentage;
		const qreal scrollbar_height;
		const qreal scroller_margin_from_left_percentage;
		const qreal scroller_margin_from_right_percentage;		
	};
}

#endif
