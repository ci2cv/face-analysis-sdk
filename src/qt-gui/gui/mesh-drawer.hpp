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

#ifndef _CI2CV_GUI_GUI_MESH_DRAWER_HPP_
#define _CI2CV_GUI_GUI_MESH_DRAWER_HPP_

#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QColor>
#include <QtGui/QBrush>
#include <QtGui/QPen>

#include <opencv/cv.h>

#include <vector>

namespace CI2CVGui {	
	class MeshDrawer : public QGraphicsItemGroup
	{
	public:
		MeshDrawer(QGraphicsItem *parent = NULL);
		
		cv::Mat mesh() const;
		cv::Mat connections() const;
		
		void setMesh(const cv::Mat &mesh);
		void setConnections(const cv::Mat &connections);
		
	private:		
		int numberOfLines() const;
		int numberOfPoints() const;
		void prepareLines(int number_of_lines);
		void preparePoints(int number_of_points);
		
	private:
		typedef std::vector<QGraphicsLineItem *> LineCollection;		
		LineCollection line_items;
		
		typedef std::vector<QGraphicsEllipseItem *> PointCollection;
		PointCollection point_items;
		
		cv::Mat current_mesh;
		cv::Mat current_connections;
		
		int point_radius;			
		QBrush point_brush;
		QPen   point_pen;
		
		QPen line_pen;
	};
}

#endif
