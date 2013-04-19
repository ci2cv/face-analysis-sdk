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

#ifndef _CI2CV_GUI_GUI_ITEM_POSITIONERS_HPP_
#define _CI2CV_GUI_GUI_ITEM_POSITIONERS_HPP_

#include <QtCore/QObject>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsPixmapItem>

#include <opencv2/core/core.hpp>

#define ANIMATION_DURATION 10

namespace CI2CVGui
{
	class ItemController : public QObject
	{
		Q_OBJECT
	
		Q_PROPERTY(QRectF itemPosition
				   READ itemPosition
				   WRITE setItemPosition);
				
		Q_PROPERTY(qreal opacity
				   READ opacity
				   WRITE setOpacity);
	public:
		ItemController(QObject *parent = NULL);
		
		virtual ~ItemController();
		
		virtual QRectF itemPosition() const = 0;
		virtual void setItemPosition(QRectF position) = 0;
		
		virtual void setItemPosition(QRectF position,bool animate);
				
		virtual qreal opacity() const = 0;
		virtual void setOpacity(qreal opacity) = 0;
		virtual void setOpacity(qreal opacity,bool animate);
	};
	
	class PixmapItemController : public ItemController
	{
		Q_OBJECT
	public:
		PixmapItemController(QObject *parent = NULL);
		
		virtual void setPixmap(QPixmap pixmap) = 0;
	};
	
	class MeshItemController : public ItemController
	{
		Q_OBJECT
	public:
		MeshItemController(QObject *parent = NULL);
		
		virtual void setMesh(const cv::Mat &mesh) = 0;		
	}; 
	
	class GraphicsRectItemController : public ItemController
	{
		Q_OBJECT
		
	public:
		GraphicsRectItemController(QGraphicsRectItem *rect,QObject *parent = NULL);
		
		QRectF itemPosition() const;
		void setItemPosition(QRectF position);
				
		qreal opacity() const;
		void setOpacity(qreal opacity);		
		
	private:
		QGraphicsRectItem *rect_item;
	};
	
	class AvatarSelection;
	class AvatarSelectionItemController : public ItemController
	{
		Q_OBJECT
		
	public:
		AvatarSelectionItemController(AvatarSelection *selector,QObject *parent = NULL);
		
		QRectF itemPosition() const;
		void setItemPosition(QRectF position);	
		
		qreal opacity() const;
		void setOpacity(qreal opacity);		
		
	private:
		AvatarSelection *selector;
	};
	
	class GraphicsPixmapItemController : public PixmapItemController
	{
		Q_OBJECT
	public:
		GraphicsPixmapItemController(QGraphicsPixmapItem *item, QObject *parent = NULL);
		
		QRectF itemPosition() const;
		void setItemPosition(QRectF position);	
		
		qreal opacity() const;
		void setOpacity(qreal opacity);		
		
		void setPixmap(QPixmap pixmap);
		
	private:
		QGraphicsPixmapItem *pixmap_item;
		QPixmap original_pixmap;
		QRectF item_position;
	};
	
	class GraphicsTextItemController : public ItemController
	{
		Q_OBJECT
	public:
		GraphicsTextItemController(QGraphicsTextItem *item, QObject *parent = NULL);
		
		QRectF itemPosition() const;
		void setItemPosition(QRectF position);	
		
		qreal opacity() const;
		void setOpacity(qreal opacity);		
		
	private:
		QGraphicsTextItem *text_item;
		
		QRectF item_position;
	};
	
	class MeshDrawer;
	class MeshDrawerItemController : public MeshItemController
	{
		Q_OBJECT
	public:
		MeshDrawerItemController(MeshDrawer *drawer, QObject *parent = NULL);

		QRectF itemPosition() const;
		qreal opacity() const;
		
	public slots:		
		void setOpacity(qreal opacity);
		void setItemPosition(QRectF position);
		void setMesh(const cv::Mat &mesh);
		
	private:
		QRectF item_position;
		MeshDrawer *mesh_drawer;
		QSize frame_size;
	};
}

#endif
