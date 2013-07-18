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

#include "utils/points.hpp"
#include "utils/helpers.hpp"
#include "tracker/IO.hpp"
#include <iostream>
#include <fstream>

std::vector<cv::Point_<double> >
load_points(const char *pathname)
{
  // Almost like IO::LoadPts except it throws exceptions rather than
  // aborts. 
  int i,n;
  char str[256];
  char c;

  std::fstream file(pathname, std::ios::in);
  if (!file.is_open())
    throw make_runtime_error("Unable to open pts file '%s'", pathname);

  file.exceptions(std::ios::badbit | std::ios::failbit);

  // read the number of points
  while (!file.eof()) { 
    file >> str;
    if (strncmp(str,"n_points:",9) == 0)
      break;
  }
  
  file >> n;

  std::vector<cv::Point_<double> > shape(n);

  // find the opening {
  while (!file.eof()) { 
    file >> c;
    if(c == '{')
      break;
  }
  
  for (i = 0; i < n; i++) 
    file >> shape[i].x >> shape[i].y;

  file.close();

  return shape;
}

void
save_points(const char *pathname, const std::vector<cv::Point_<double> > &points)
{ 
  std::fstream file(pathname, std::ios::out);
  if (!file.is_open()) 
    throw make_runtime_error("Unable to open file '%s' for writing!", pathname);

  file.exceptions(std::ios::badbit | std::ios::failbit);

  file << "n_points: " << points.size() << "\n{\n";
  for (size_t i = 0; i < points.size(); i++) 
    file << points[i].x << "\t" << points[i].y << "\n";

  file << "}\n";
  
  file.close();
  
  return;
}

cv::Mat_<double>
vectorise_points(const std::vector<cv::Point_<double> > &points)
{
  const size_t n = points.size();
  
  cv::Mat_<double> rv(points.size() * 2, 1);
  
  for (size_t i = 0; i < n; i++) {
    rv(i + 0) = points[i].x;
    rv(i + n) = points[i].y;
  }

  return rv;
}

// Points 3D
std::vector<cv::Point3_<double> >
load_points3(const char *pathname)
{
  int i,n;
  char str[256];
  char c;

  std::fstream file(pathname, std::ios::in);
  if (!file.is_open())
    throw make_runtime_error("Unable to open pts file '%s'", pathname);

  file.exceptions(std::ios::badbit | std::ios::failbit);

  // read the number of points
  while (!file.eof()) { 
    file >> str;
    if (strncmp(str,"n_points:",9) == 0)
      break;
  }
  
  file >> n;

  std::vector<cv::Point3_<double> > shape(n);

  // find the opening {
  while (!file.eof()) { 
    file >> c;
    if(c == '{')
      break;
  }
  
  for (i = 0; i < n; i++) 
    file >> shape[i].x >> shape[i].y >> shape[i].z;

  file.close();

  return shape;
}

void
save_points3(const char *pathname, const std::vector<cv::Point3_<double> > &points)
{ 
  std::fstream file(pathname, std::ios::out);
  if (!file.is_open()) 
    throw make_runtime_error("Unable to open file '%s' for writing!", pathname);

  file.exceptions(std::ios::badbit | std::ios::failbit);

  file << "n_points: " << points.size() << "\n{\n";
  for (size_t i = 0; i < points.size(); i++) 
    file << points[i].x << " " << points[i].y << " " << points[i].z << "\n";

  file << "}\n";
  
  file.close();
  
  return;
}
