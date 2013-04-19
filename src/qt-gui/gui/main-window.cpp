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

#include "gui/main-window.hpp"
#include "gui/windowed-gui-controller.hpp"
#include "controllers.hpp"
#include "gui/avatar-selection.hpp"
#include "gui/mesh-drawer.hpp"
#include "configuration.hpp"

#include <QtGui/QStyle>
#include <QtGui/QFont>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QApplication>
#include <QtCore/QDebug>

using namespace CI2CVGui;

static MainWindow        * main_window         = NULL;
static GuiController     * gui_controller      = NULL;

static
void
initialize_main_window()
{
	if (!main_window) {
		main_window = new MainWindow();
		WindowedGuiController *p = new WindowedGuiController();
		
		p->reset_tracker_button = main_window->reset_button;
		p->gui_state_change_button = main_window->gui_button;
		p->user_input = main_window->user_input_view;
		p->avatar_animation = main_window->avatar_view;
		p->show_user_button = main_window->show_user_button;
		p->show_avatar_selection_button = main_window->show_avatar_selection_button;
		p->avatar_selector = main_window->avatar_selector;
		p->status_message = main_window->status_message;
		p->tracking_health = main_window->tracking_health;
		p->tracking_mesh = main_window->tracking_mesh;
		p->show_tracking_mesh_button = main_window->show_tracking_mesh_button;
		p->face_out_of_bounds_warning_user = main_window->face_out_of_bounds_warning_user;
		p->face_out_of_bounds_warning_avatar = main_window->face_out_of_bounds_warning_avatar;
		p->bindController();
		
		QObject::connect(main_window,SIGNAL(resized(qreal,qreal)),p,SLOT(updateItems(qreal,qreal)));
		QObject::connect(main_window,SIGNAL(avatarSelectionChanged(int)), avatarController(), SLOT(setAvatarIndex(int)));
		
		gui_controller = p;

		main_window->show();
		main_window->resize(800,600);	
		QApplication::processEvents();
	}
}

CI2CVGui::GuiController *
CI2CVGui::guiController()
{
	initialize_main_window();
	return gui_controller;
}

MainWindowGraphicsView::MainWindowGraphicsView(QWidget *parent)
: QGraphicsView(parent)
{
  /*
        QBrush background_brush(backgroundBrush());
        Configuration *application_configuration = applicationConfiguration();
	switch (application_configuration->backgroundColour()) {
	case BACKGROUND_COLOUR_BLACK:
  	  background_brush.setColor(Qt::black);
	  break;
	case BACKGROUND_COLOUR_WHITE:
	  background_brush.setColor(Qt::white);
	  break;
	default:
	  throw std::logic_error("Invalid background colour specified.");
	}
	background_brush.setStyle(Qt::SolidPattern);
	setBackgroundBrush(background_brush);
  */
}

void
MainWindowGraphicsView::resizeEvent(QResizeEvent *event)
{
	QGraphicsView::resizeEvent(event);
	QWidget *w = this;		
	scene()->setSceneRect(0.0, 0.0,w->width(),w->height());
}


MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
: QMainWindow(parent,flags),
	reset_button(new QPushButton("Reset Tracking",this)),
	gui_button(new QPushButton(this)),
	show_user_button(new QCheckBox("Show User",this)),
	show_tracking_mesh_button(new QCheckBox("Show Tracking",this)),
	show_avatar_selection_button(new QPushButton(this)),
	graphics_view(new MainWindowGraphicsView(this)),
	graphics_scene(new QGraphicsScene(this)),
	tracking_health(new QProgressBar(this)),
	status_message(new QLabel("Waiting for Camera", this))
{	
	show_avatar_selection_button->setCheckable(true);
																			
	QGraphicsPixmapItem *graphics_input_item  = new QGraphicsPixmapItem();
	QGraphicsPixmapItem *graphics_avatar_item = new QGraphicsPixmapItem();
	QGraphicsTextItem   *graphics_face_out_of_bounds_user_item = new QGraphicsTextItem();
	QGraphicsTextItem   *graphics_face_out_of_bounds_avatar_item = new QGraphicsTextItem();	
	graphics_face_out_of_bounds_user_item->setPlainText("Face outside the image!");
	graphics_face_out_of_bounds_avatar_item->setPlainText("Face outside the image!");		
	{
		QFont  font;
		font.setPointSizeF(24.0);
				
		graphics_face_out_of_bounds_user_item->setFont(font);
		graphics_face_out_of_bounds_avatar_item->setFont(font);
		graphics_face_out_of_bounds_user_item->setDefaultTextColor(QColor::fromRgb(255, 0, 0));
		graphics_face_out_of_bounds_avatar_item->setDefaultTextColor(QColor::fromRgb(255, 0, 0));		
	}
	
	user_input_view = new GraphicsPixmapItemController(graphics_input_item, this);
	avatar_view     = new GraphicsPixmapItemController(graphics_avatar_item, this);
	face_out_of_bounds_warning_user = new GraphicsTextItemController(graphics_face_out_of_bounds_user_item, this);
	face_out_of_bounds_warning_avatar = new GraphicsTextItemController(graphics_face_out_of_bounds_avatar_item, this);	
	
	graphics_scene->addItem(graphics_input_item);
	graphics_scene->addItem(graphics_avatar_item);
	graphics_scene->addItem(graphics_face_out_of_bounds_user_item);
	graphics_scene->addItem(graphics_face_out_of_bounds_avatar_item);	
		
	AvatarSelection * selection = new AvatarSelection();
	avatar_selector = new AvatarSelectionItemController(selection);
	graphics_scene->addItem(selection);
	
	MeshDrawer *mesh_drawer = new MeshDrawer();
	tracking_mesh = new MeshDrawerItemController(mesh_drawer, this);
	graphics_scene->addItem(mesh_drawer);
			
	connect(graphics_scene,SIGNAL(sceneRectChanged(const QRectF &)),this,SLOT(sceneRectChanged(const QRectF &)));
	connect(selection,SIGNAL(selectionChanged(int)), SIGNAL(avatarSelectionChanged(int)));

	graphics_view->setScene(graphics_scene);
	graphics_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	graphics_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	graphics_view->setRenderHint(QPainter::Antialiasing);
	graphics_view->setInteractive(true);
	
	// Layout
	QVBoxLayout *vl = new QVBoxLayout();
	{
		QHBoxLayout *hl = new QHBoxLayout();
		hl->addStretch(1);
		hl->addWidget(status_message);
		hl->addStretch(1);
		
		vl->addLayout(hl,0);
	}
	{
		vl->addWidget(graphics_view,1);
	}
	{
		QHBoxLayout *hl = new QHBoxLayout();
		hl->addWidget(gui_button);
		hl->addWidget(show_avatar_selection_button);
		hl->addWidget(show_user_button);
		hl->addStretch(1);
		hl->addWidget(show_tracking_mesh_button);
		hl->addWidget(reset_button);
		hl->addWidget(tracking_health);
		vl->addLayout(hl,0);
	}

	QWidget *central_widget = new QWidget(this);
	central_widget->setLayout(vl);
	setCentralWidget(central_widget);
}

void
MainWindow::resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent(event);
	QWidget *w = graphics_view;
	graphics_scene->setSceneRect(0.0, 0.0,w->width(),w->height());
}

void
MainWindow::sceneRectChanged(const QRectF &rect)
{
	emit resized(rect.width(), rect.height());
}
