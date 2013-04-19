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

#ifndef _CI2CV_GUI_GUI_MAIN_WINDOW_HPP_
#define _CI2CV_GUI_GUI_MAIN_WINDOW_HPP_

#include <QtGui/QMainWindow>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>

#include "gui/item-controllers.hpp"

namespace CI2CVGui {
	class MainWindowGraphicsView : public QGraphicsView
	{
		Q_OBJECT
	public:
		MainWindowGraphicsView(QWidget *parent = 0);
		
	protected:
		void resizeEvent(QResizeEvent *event);		
	};
	
	class MainWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		MainWindow(QWidget *parent = NULL, Qt::WindowFlags flags = 0);
		
		QPushButton *reset_button;
		QPushButton *gui_button;		
		QCheckBox *show_user_button;
		QCheckBox *show_tracking_mesh_button;
		QPushButton *show_avatar_selection_button;
		QGraphicsView *graphics_view;
		QGraphicsScene *graphics_scene;
		QProgressBar *tracking_health;
		
		PixmapItemController *user_input_view;
		PixmapItemController *avatar_view;
		ItemController *avatar_selector;
		MeshItemController *tracking_mesh;
		ItemController *face_out_of_bounds_warning_user;
		ItemController *face_out_of_bounds_warning_avatar;		
				
		QLabel *status_message;
				
	protected:
		void resizeEvent(QResizeEvent *event);
		
	signals:
		void resized(qreal width, qreal height);
		void avatarSelectionChanged(int index);
				
	private slots:
		void sceneRectChanged(const QRectF &rect);		
		
	private:
	};
	
	
}

#endif
