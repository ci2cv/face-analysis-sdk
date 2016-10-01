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

#include "helpers.hpp"
#include <fstream>
#include <libgen.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <functional>
#include <iostream>

bool
nan_p(double value)
{
  return std::isnan(value);
}

bool
nan_p(const cv::Mat_<double> &value)
{
  return some(value.begin(), value.end(), std::ptr_fun<double>(nan_p));
}

bool
plusp(int value)
{
  return value > 0;
}

cv::Mat_<double>
column_concatenate(const cv::Mat_<double> &a, const cv::Mat_<double> &b)
{
  if (a.rows != b.rows)
    throw make_runtime_error("Unable to concatenate matrix columns as the number of rows do not match. (%d != %d)", a.rows, b.rows);

  cv::Mat_<double> rv(a.rows, a.cols + b.cols);
  for (int i = 0; i < rv.rows; i++) {
    int offset;

    offset = 0;
    for (int j = 0; j < a.cols; j++) 
      rv(i, offset + j) = a(i,j);

    offset = a.cols;
    for (int j = 0; j < b.cols; j++)
      rv(i, offset + j) = b(i,j);
  }

  return rv;
}

cv::Mat_<double>
column_concatenate(const cv::Mat_<double> &a, const cv::Mat_<double> &b, const cv::Mat_<double> &c)
{
  return column_concatenate(column_concatenate(a, b), c);
}

cv::Mat_<double>
row_concatenate(const cv::Mat_<double> &a, const cv::Mat_<double> &b)
{
  if (a.cols != b.cols)
    throw make_runtime_error("Column length mismatch.");

  cv::Mat_<double> rv(a.rows + b.rows, a.cols);
  for (int i = 0; i < a.rows; i++) {
    for (int j = 0; j < a.cols; j++) {
      rv(i,j) = a(i,j);
    }
  }

  for (int i = 0; i < b.rows; i++) {
    for (int j = 0; j < b.cols; j++) {
      rv(a.rows + i, j) = b(i,j);
    }
  }

  return rv;
}

int
rank(const cv::Mat_<double> &matrix)
{
  cv::SVD svd(matrix, cv::SVD::NO_UV);
  int rv = 0;
  assert(svd.w.type() == cv::DataType<double>::type);
  assert(svd.w.cols == 1);
  
  for (int i = 0; i < svd.w.rows; i++) {
    if (svd.w.at<double>(i,0) > std::numeric_limits<double>::epsilon()) {
      rv++;
    }
  }
  return rv;
}

bool
valid_image_p(const cv::Mat &image)
{
  return !invalid_image_p(image);
}

bool
invalid_image_p(const cv::Mat &image)
{
  return (image.rows == 0) || (image.cols == 0);
}

bool
column_vector_with_length_p(const cv::Mat_<double> &matrix, int length)
{
  return matrix_with_dimensions_p(matrix, length, 1);
}

bool
matrix_with_dimensions_p(const cv::Mat_<double> &matrix, int rows, int columns)
{  
  return true
    && (matrix.rows == rows)
    && (matrix.cols == columns);
}

bool
matrix_with_dimensions_p(const cv::Mat_<double> &m, const cv::Size_<int> &size)
{
  return matrix_with_dimensions_p(m, size.height, size.width);
}

bool
matrices_with_dimensions_p(const std::vector<cv::Mat_<double> > &input, int rows, int columns)
{
  for (size_t i = 0; i < input.size(); i++) {
    if (!matrix_with_dimensions_p(input[i], rows, columns))
      return false;
  }
  return true;
}

bool
matrices_with_dimensions_p(const std::vector<cv::Mat_<double> > &input, const cv::Size_<int> &size)
{
  return matrices_with_dimensions_p(input, size.height, size.width);
}

bool
column_vector_p(const cv::Mat_<double> &matrix)
{
  return matrix.cols == 1;
}

bool
column_vectors_p(const std::vector<cv::Mat_<double> > &input)
{
  return every(input.begin(), input.end(), &column_vector_p);
}

bool
column_vectors_with_length_p(const std::vector<cv::Mat_<double> > &input, int length)
{
  for (size_t i = 0; i < input.size(); i++) {
    if (!column_vector_with_length_p(input[i], length))
      return false;
  }
  return true;
}

std::ostream &
operator<<(std::ostream &stream, const cv::Size &s)
{
  stream << "cv::Size(" << s.height << "," << s.width << ")";
  return stream;
}

bool
file_exists_p(const std::string &pathname)
{
  std::ifstream file(pathname.c_str());
  return file.is_open();
}

std::runtime_error
make_runtime_error(const char *format, ...)
{
  va_list args;
  va_start(args,format);

  std::runtime_error rv = vmake_runtime_error(format, args);
  va_end(args);
  return rv;
}

std::runtime_error
vmake_runtime_error(const char *format, va_list args)
{
  char buffer[2048];
  vsnprintf(buffer, sizeof(buffer)/sizeof(char), format, args);
  return std::runtime_error(buffer);
}

std::list<std::string>
read_list(const char *filename)
{
  std::ifstream stream(filename);
  if (!stream.is_open())
    throw make_runtime_error("Unable to open file %s", filename);

  return read_list(stream);
}

std::list<std::string>
read_list(std::istream &stream)
{
  std::list<std::string> rv;
  std::string line;
  while (!stream.eof()) {
    std::getline(stream, line);
    if (line.size() > 0) 
      rv.push_back(line);
  }
  return rv;
}

std::vector<std::string>
read_list_as_vector(const char *filename)
{
  std::vector<std::string> rv;
  read_and_transform_list(filename, std::back_inserter(rv), identity<std::string>());
  return rv;
}

std::string
pathname_type(const std::string &filename)
{
  std::string tmp(pathname_sans_directory(filename));
  
  if (std::count(tmp.begin(), tmp.end(), '.') > 1) 
    throw make_runtime_error("Unable to compute the extension of file '%s'", filename.c_str());
  
  std::string::size_type pos = tmp.find('.');
  if (pos == tmp.size()) {
    return "";
  } else {
    return tmp.substr(pos + 1);
  }
}

std::string
pathname_name(const std::string &pathname)
{
  std::string tmp(pathname_sans_directory(pathname));
  if (std::count(tmp.begin(), tmp.end(), '.') > 1)
    throw make_runtime_error("Unable to compute the name component of file '%s'", pathname.c_str());

  std::string::size_type pos = tmp.find('.');
  if (pos == tmp.size())
    return pathname;
  else
    return tmp.substr(0, pos);
}

std::string
pathname_directory(const std::string &pathname)
{
  char buffer[pathname.size() + 1];
  memset(buffer, 0, sizeof(buffer));
  std::copy(pathname.begin(), pathname.end(), buffer);
  return std::string(dirname(buffer));
}

std::string
pathname_sans_directory(const std::string &pathname)
{
  char buffer[pathname.size() + 1];
  memset(buffer, 0, sizeof(buffer));
  std::copy(pathname.begin(), pathname.end(), buffer);
  return std::string(basename(buffer));
}

std::string
make_pathname(const std::string &directory, const std::string &name, const std::string &type)
{
  std::stringstream s;
  s << directory << "/" << name;
  if (type != "")
    s << "." << type;

  return s.str();
}

cv::Mat_<uint8_t>
load_grayscale_image(const char *pathname, int colour_conversion_method, cv::Mat *image)
{
  *image = cv::imread(pathname);
  if ((image->rows == 0) || (image->cols == 0))
    throw make_runtime_error("Unable to load image '%s'", pathname);

  if (image->type() == cv::DataType<uint8_t>::type) {
    return image->clone();
  } else {
    cv::Mat rv;
    cv::cvtColor(*image, rv, colour_conversion_method);
    assert(rv.type() == cv::DataType<uint8_t>::type);
    return rv;
  }    
}

cv::Mat_<uint8_t>
load_grayscale_image(const char *pathname, int colour_conversion_method)
{
  cv::Mat original_image;
  return load_grayscale_image(pathname, colour_conversion_method, &original_image);
}

cv::Mat_<uint8_t>
load_grayscale_image(const char *pathname, cv::Mat *original_image)
{
  return load_grayscale_image(pathname, CV_BGR2GRAY, original_image);
}

cv::Mat_<uint8_t>
load_grayscale_image(const char *pathname)
{
  cv::Mat original_image;
  return load_grayscale_image(pathname, &original_image);
}

void
imshow_normalised(const char *window_name, const cv::Mat_<double> &image)
{
  cv::Mat_<double> normalised;
  cv::normalize(image, normalised, 0, 1, cv::NORM_MINMAX);
  cv::imshow(window_name, normalised);
}

void
imshow_jacobian(const char *window_name, const cv::Mat_<double> &jacobian, const cv::Size_<int> &size)
{
  char window_name_buffer[1000];
  
  for (int i = 0; i < jacobian.rows; i++) {
    cv::Mat_<double> img = jacobian.row(i).clone().reshape(0,size.height);
    sprintf(window_name_buffer, "%s %d", window_name, i);
    imshow_normalised(window_name_buffer, img);
  }
}

static const int DEFAULT_QUANTISATION = 10000;

cv::Mat_<double>
random_vector(int number_of_rows, double min, double max)
{
  return random_matrix(number_of_rows, 1, min, max);
}

std::vector<cv::Mat_<double> >
random_vectors(int number_of_vectors, int number_of_rows, double min, double max)
{
  return random_matrices(number_of_vectors, number_of_rows, 1, min, max);
}

cv::Mat_<double>
random_matrix(int number_of_rows, int number_of_columns, double min, double max)
{
  cv::Mat_<double> rv(number_of_rows,number_of_columns);  
  
  for (int i = 0; i < number_of_rows; i++) {
    for (int j = 0; j < number_of_columns; j++) {
      int v = rand() % DEFAULT_QUANTISATION;
      double percentage = ((double)v)/((double)DEFAULT_QUANTISATION);
      rv(i,j) = min + percentage*(max - min);
    }
  }

  return rv;
}

cv::Mat_<double>
random_matrix_of_maximum_rank(int number_of_rows, int number_of_columns, double min, double max)
{
  cv::Mat_<double> rv = random_matrix(number_of_rows, number_of_columns, min, max);
  if (rank(rv) == std::min(number_of_rows, number_of_columns))
    return rv;
  else
    return random_matrix_of_maximum_rank(number_of_rows, number_of_columns, min, max);
}

std::vector<cv::Mat_<double> >
random_matrices(int number_of_matrices, int number_of_rows, int number_of_columns, double min, double max)
{
  std::vector<cv::Mat_<double> > rv(number_of_matrices);
  for (int i = 0; i < number_of_matrices; i++) {
    rv[i] = random_matrix(number_of_rows, number_of_columns, min, max);
  }
  
  return rv;
}

double
l1_norm(const cv::Mat_<double> &input)
{
  return cv::norm(input, cv::NORM_L1);
}

double
l1_norm(const std::vector<cv::Mat_<double> > &input)
{
  double rv = 0;
  std::vector<cv::Mat_<double> >::const_iterator current;
  for (current = input.begin(); current != input.end(); current++) {
    rv += l1_norm(*current);
  }
  return rv;
}

double
l2_norm(const cv::Mat_<double> &input)
{
  return cv::norm(input, cv::NORM_L2);
}

double
l2_norm(const std::vector<cv::Mat_<double> > &input)
{
  double rv = 0;
  std::vector<cv::Mat_<double> >::const_iterator current;
  for (current = input.begin(); current != input.end(); current++) {
    rv += l2_norm(*current);
  }
  return rv;
}

double
l2_norm(const cv::Mat_<double> &input, const cv::Mat_<double> &prior)
{
  return l2_norm(prior * input);
}

double
l2_norm(const std::vector<cv::Mat_<double> > &input, const cv::Mat_<double> &prior)
{
  double rv = 0;
  std::vector<cv::Mat_<double> >::const_iterator current;
  for (current = input.begin(); current != input.end(); current++) {
    rv += l2_norm(*current, prior);
  }
  return rv;
}

double
l2_norm(const cv::Point_<double> &input)
{
  return std::sqrt(std::pow(input.x, 2) + std::pow(input.y, 2));
}

double
l2sq_norm(const cv::Mat_<double> input)
{
  return std::pow(l2_norm(input), 2);
}

double
l2sq_norm(const std::vector<cv::Mat_<double> > input)
{
  double rv = 0;
  std::vector<cv::Mat_<double> >::const_iterator it;
  for (it = input.begin(); it != input.end(); it++) {
    rv += l2sq_norm(*it);
  }
  return rv;
}

double
l2sq_norm(const cv::Mat_<double> input, const cv::Mat_<double> &prior)
{
  return std::pow(l2_norm(input, prior), 2);
}

double
l2sq_norm(const std::vector<cv::Mat_<double> > input, const cv::Mat_<double> &prior)
{
  double rv = 0;
  std::vector<cv::Mat_<double> >::const_iterator it;
  for (it = input.begin(); it != input.end(); it++) {
    rv += l2sq_norm(*it, prior);
  }
  return rv;
}

// Local Variables:
// compile-in-directory: "../"
// End:
