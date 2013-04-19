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

#include "gui/mesh-drawer.hpp"
#include "controllers.hpp"

#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsEllipseItem>

using namespace CI2CVGui;

#include <set>

namespace {
	int 
	number_of_lines(const cv::Mat &connections)
	{
		return connections.cols;
	}
	
	int
	number_of_points(const cv::Mat &connections)
	{
		std::set<int> point_indices;
		
		for (int i = 0; i < connections.rows; i++) {
			for (int j = 0; j < connections.cols; j++) {
				point_indices.insert(connections.at<int>(i,j));
			}
		}
		
		return (int)point_indices.size();
	}
}

MeshDrawer::MeshDrawer(QGraphicsItem *item)
: QGraphicsItemGroup(item)
{
	line_pen.setColor(qRgb(255,0,0));
	point_pen.setColor(qRgb(255,0,0));
	point_brush.setStyle(Qt::SolidPattern);
	point_brush.setColor(qRgb(255,0,0));
	point_radius = 2;
	
	setConnections(trackerController()->trackerConnections());
}

int
MeshDrawer::numberOfLines() const
{
	return number_of_lines(current_connections);
}

int
MeshDrawer::numberOfPoints() const
{
	return number_of_points(current_connections);
}

void
MeshDrawer::prepareLines(int number_of_lines)
{
	LineCollection::iterator it;
	for (it = line_items.begin(); it != line_items.end(); it++) {
		delete *it;
	}
	line_items.clear();
	
	for (int i = 0; i < number_of_lines; i++) {
		QGraphicsLineItem *new_item = new QGraphicsLineItem();
		new_item->setPen(line_pen);
		line_items.push_back(new_item);
		
		addToGroup(new_item);
	}
}

void
MeshDrawer::preparePoints(int number_of_points)
{
	PointCollection::iterator it;
	for (it = point_items.begin(); it != point_items.end(); it++) {
		delete *it;
	}
	point_items.clear();
	
	for (int i = 0; i < number_of_points; i++) {
		QGraphicsEllipseItem *new_item = new QGraphicsEllipseItem();
		new_item->setPen(point_pen);
		new_item->setBrush(point_brush);
		point_items.push_back(new_item);
		
		addToGroup(new_item);
	}	
}

void
MeshDrawer::setConnections(const cv::Mat &connections)
{
	current_connections = connections;
	prepareLines(numberOfLines());
	preparePoints(numberOfPoints());
}

void
MeshDrawer::setMesh(const cv::Mat &mesh)
{
	current_mesh = mesh;
		
	int number_of_points = (int)point_items.size();
	int number_of_lines  = (int)line_items.size();
	
	bool hide_mesh = ((mesh.rows == 0) || (mesh.cols == 0));
	
	// update lines
	for (int line_index = 0; line_index < number_of_lines; line_index++) {
		QGraphicsLineItem *line = line_items[line_index];
		
		if (hide_mesh) {
			line->setVisible(false);
		} else {
			QPointF	from_point(mesh.at<double>(current_connections.at<int>(0,line_index),0),
						   mesh.at<double>(current_connections.at<int>(0,line_index)+number_of_points,0));
			QPointF to_point(mesh.at<double>(current_connections.at<int>(1,line_index),0),
						 mesh.at<double>(current_connections.at<int>(1,line_index)+number_of_points,0));
			QLineF line_position(from_point,to_point);			
			line->setLine(line_position);
			line->setVisible(true);
		}
	}
	
	// update points
	for (int point_index = 0; point_index < number_of_points; point_index++) {
		QGraphicsEllipseItem *point = point_items[point_index];
		
		if (hide_mesh) {
			point->setVisible(false);
		} else {
			QPointF position(mesh.at<double>(point_index,0), mesh.at<double>(point_index+number_of_points,0));
		
			QRectF ellipse_rect(position.x() - point_radius,
								position.y() - point_radius,
								2*point_radius,
								2*point_radius);
		
			point->setRect(ellipse_rect);
			point->setVisible(true);
		}
	}
}
