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

#ifndef _UTILS_POINTS_HPP_
#define _UTILS_POINTS_HPP_

#include <opencv2/core/core.hpp>
#include <vector>

typedef std::vector<cv::Point_<double> > PointVector;

std::vector<cv::Point_<double> > load_points(const char *pathname);
void save_points(const char *pathname, const std::vector<cv::Point_<double> > &points);

std::vector<cv::Point3_<double> > load_points3(const char *pathname);
void save_points3(const char *pathname, const std::vector<cv::Point3_<double> > &points);

cv::Mat_<double> vectorise_points(const std::vector<cv::Point_<double> > &points);

#endif
