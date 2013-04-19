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

#include "gui/item-controllers.hpp"
#include <QtCore/QPropertyAnimation>
#include <QtCore/QDebug>
#include "gui/avatar-selection.hpp"
#include "gui/mesh-drawer.hpp"
#include "controllers.hpp"

using namespace CI2CVGui;

GraphicsRectItemController::GraphicsRectItemController(QGraphicsRectItem *rect_item, QObject *parent)
: ItemController(parent),
  rect_item(rect_item)
{

}

QRectF
GraphicsRectItemController::itemPosition() const
{
	return rect_item->rect();
}

void
GraphicsRectItemController::setItemPosition(QRectF position)
{
	rect_item->setRect(position);	
}

qreal
GraphicsRectItemController::opacity() const
{
	return rect_item->opacity();
}

void
GraphicsRectItemController::setOpacity(qreal opacity)
{
	opacity = std::max(opacity,0.0);
	opacity = std::min(opacity,1.0);

	rect_item->setOpacity(opacity);
}

// AvatarSelectionItemController
AvatarSelectionItemController::AvatarSelectionItemController(AvatarSelection *selector, QObject *parent)
:	ItemController(parent),
	selector(selector)
{
	
}

QRectF
AvatarSelectionItemController::itemPosition() const
{
	return selector->position();
}

void
AvatarSelectionItemController::setItemPosition(QRectF position)
{
	selector->setPosition(position);
}

qreal
AvatarSelectionItemController::opacity() const
{
	return selector->opacity();
}

void
AvatarSelectionItemController::setOpacity(qreal opacity)
{
	selector->setOpacity(opacity);	
}

// ItemController
ItemController::ItemController(QObject *parent)
: QObject(parent)
{
	
}

ItemController::~ItemController()
{
	
}

void
ItemController::setOpacity(qreal opacity, bool animate)
{
	if (animate) {
		QPropertyAnimation *p = new QPropertyAnimation(this);
		p->setPropertyName("opacity");
		p->setTargetObject(this);
		p->setStartValue(this->opacity());
		p->setEndValue(opacity);
		p->start();
	} else {
		setOpacity(opacity);
	}	
}

void
ItemController::setItemPosition(QRectF position,bool animate)
{
	if (animate) {
		QPropertyAnimation *p = new QPropertyAnimation(this);
		p->setTargetObject(this);
		p->setPropertyName("itemPosition");
		p->setStartValue(itemPosition());
		p->setEndValue(position);
		p->start();
	} else {
		setItemPosition(position);
	}
}

// PixmapItemController
PixmapItemController::PixmapItemController(QObject *parent)
: ItemController(parent)
{
	
}

// GraphicsPixmapItemController
GraphicsPixmapItemController::GraphicsPixmapItemController(QGraphicsPixmapItem *item, QObject *parent)
: PixmapItemController(parent),
	pixmap_item(item),
	original_pixmap(item->pixmap()),
	item_position(item->x(), item->y(), original_pixmap.width(), original_pixmap.height())
{
	
}

QRectF
GraphicsPixmapItemController::itemPosition() const
{
	return item_position;				  
}

void
GraphicsPixmapItemController::setItemPosition(QRectF position)
{
	item_position = position;
	
	if (original_pixmap.isNull())
		return;
	
	QSize new_size = QSize(position.width(),position.height());
	
	if ((new_size.width() == 0) || (original_pixmap.width() == 0))
		return;
	
	qreal scaling = ((qreal)new_size.width())/((qreal)original_pixmap.width());
	
	pixmap_item->setPos(position.topLeft());
	pixmap_item->setTransform(QTransform::fromScale(scaling,scaling));		
}

qreal
GraphicsPixmapItemController::opacity() const
{
	return pixmap_item->opacity();
}

void
GraphicsPixmapItemController::setOpacity(qreal opacity)
{
	pixmap_item->setOpacity(opacity);
}

void
GraphicsPixmapItemController::setPixmap(QPixmap pixmap)
{
	original_pixmap = pixmap;
	pixmap_item->setPixmap(pixmap);
	setItemPosition(item_position);
}

// GraphicsTextItemController
GraphicsTextItemController::GraphicsTextItemController(QGraphicsTextItem *item, QObject *parent)
: ItemController(parent),
  text_item(item)
{
	
}


QRectF
GraphicsTextItemController::itemPosition() const
{
	return item_position;
}

void
GraphicsTextItemController::setItemPosition(QRectF position)
{
	item_position = position;	
	text_item->setPos(position.x(),position.y());		
}

qreal
GraphicsTextItemController::opacity() const
{
	return text_item->opacity();
}

void
GraphicsTextItemController::setOpacity(qreal opacity)
{
	text_item->setOpacity(opacity);
}

MeshItemController::MeshItemController(QObject *parent)
: ItemController(parent)
{
	
}

MeshDrawerItemController::MeshDrawerItemController(MeshDrawer *drawer, QObject *parent)
:MeshItemController(parent),
 mesh_drawer(drawer),
	frame_size(0,0)
{
	
}

qreal
MeshDrawerItemController::opacity() const
{
	return mesh_drawer->opacity();
}

QRectF
MeshDrawerItemController::itemPosition() const
{
	return item_position;
}

void
MeshDrawerItemController::setOpacity(qreal opacity)
{
	mesh_drawer->setOpacity(opacity);
}

void
MeshDrawerItemController::setItemPosition(QRectF position)
{
	if (frame_size.width() == 0) {
		CameraController *camera = cameraController();
		if (!camera->isInitialised())
			return;
		
		frame_size = cameraController()->frameSize();
	}
	
	item_position = position;
	QSize new_size = QSize(position.width(),position.height());
		
	qreal scaling = ((qreal)new_size.width())/((qreal)frame_size.width());
		
	mesh_drawer->setPos(position.topLeft());
	mesh_drawer->setTransform(QTransform::fromScale(scaling,scaling));		
}

void
MeshDrawerItemController::setMesh(const cv::Mat &mesh)
{
	mesh_drawer->setMesh(mesh);
}

