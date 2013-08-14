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

#ifndef _HELPERS_HPP_
#define _HELPERS_HPP_

#include <opencv2/core/core.hpp>
#include <list>
#include <stdexcept>
#include <cstdarg>

/** Evaluates predicate on all items within the range (begin,end)
    until predicate is false, otherwise returns true. */
template <typename Iterator, typename Predicate> 
bool every(Iterator begin, Iterator end, Predicate predicate);

template <typename Iterator1, typename Iterator2, typename Predicate>
bool every(Iterator1 begin1, Iterator1 end1, Iterator2 begin2, Iterator2 end2, Predicate predicate);

/** Evaluates predicate on all items within the range (begin,end)
    until predicate returns true, otherwise returns false. */
template <typename Iterator, typename Predicate>
bool some(Iterator begin, Iterator end, Predicate predicate);

template <typename InputIterator, typename OutputIterator, typename Predicate>
void remove_if_not(InputIterator begin, InputIterator end, OutputIterator out, Predicate predicate);

template <typename InputIterator1, typename InputIterator2, typename Function>
void for_each(InputIterator1 start1, InputIterator1 end1, InputIterator2 start2, InputIterator2 end2, Function function);

template <typename Iterator1, typename Iterator2, typename T, typename BinaryFunction, typename Key>
typename BinaryFunction::return_value reduce(Iterator1 begin, Iterator1 end, T init, BinaryFunction function, Key key);

template <typename T>
struct IdentityFunction : public std::unary_function<T,T>
{
  T operator()(const T &a);
};

template <typename T>
IdentityFunction<T> identity();

bool nan_p(double value);
bool nan_p(const cv::Mat_<double> &value);

bool plusp(int value);

template <typename T1, typename T2>
bool vector_lengths_equal_p(const std::vector<T1> &v1, const std::vector<T2> &v2);

template <typename T1, typename T2, typename T3>
bool vector_lengths_equal_p(const std::vector<T1> &v1, const std::vector<T2> &v2, const std::vector<T3> &v3);

template <typename T1, typename T2, typename T3, typename T4>
bool vector_lengths_equal_p(const std::vector<T1> &v1, const std::vector<T2> &v2, const std::vector<T3> &v3, const std::vector<T4> &v4);

template <typename T1, typename T2, typename T3, typename T4, typename T5>
bool vector_lengths_equal_p(const std::vector<T1> &v1, const std::vector<T2> &v2, const std::vector<T3> &v3, const std::vector<T4> &v4, const std::vector<T5> &v5);

/* matrix operations */
template <typename T>
bool valid_row_index_p(const cv::Mat_<T> &matrix, int index);

template <typename T>
bool valid_column_index_p(const cv::Mat_<T> &matrix, int index);

template <typename T>
bool valid_matrix_index_p(const cv::Mat_<T> &matrix, int row, int column);

template <typename T>
cv::Mat_<T> ones(int number_of_rows, int number_of_columns);

template <typename T>
cv::Mat_<T> zeros(int number_of_rows, int number_of_columns);

template <typename T, typename U>
cv::Mat_<T> zeros(const cv::Size_<U> &size);

template <typename T>
std::vector<cv::Mat_<T> > vector_of_zeros(int number_of_elements, int rows, int columns);

template <typename T>
std::vector<cv::Mat_<T> > vector_of_value(int number_of_elements, const cv::Mat_<T> &value);

template <typename T>
std::vector<cv::Mat_<T> > vector_of_value(int number_of_elements, const cv::MatExpr &expression);

template <typename T>
cv::Mat_<T> eye(int number_of_rows, int number_of_columns);

template <typename T>
bool less_than_or_equal_to(const cv::Mat_<T> &a, const cv::Mat_<T> &b);

cv::Mat_<double> column_concatenate(const cv::Mat_<double> &a, const cv::Mat_<double> &b);
cv::Mat_<double> column_concatenate(const cv::Mat_<double> &a, const cv::Mat_<double> &b, const cv::Mat_<double> &c);

cv::Mat_<double> row_concatenate(const cv::Mat_<double> &a, const cv::Mat_<double> &b);

/* Compute the rank of matrix. */
int rank(const cv::Mat_<double> &matrix);

bool valid_image_p(const cv::Mat &image);
bool invalid_image_p(const cv::Mat &image);

bool column_vector_with_length_p(const cv::Mat_<double> &m, int length);
bool matrix_with_dimensions_p(const cv::Mat_<double> &m, int rows, int columns);
bool matrix_with_dimensions_p(const cv::Mat_<double> &m, const cv::Size_<int> &size);
bool matrices_with_dimensions_p(const std::vector<cv::Mat_<double> > &input, int rows, int columns);
bool matrices_with_dimensions_p(const std::vector<cv::Mat_<double> > &input, const cv::Size_<int> &size);

template <typename T>
bool matrix_dimensions_equal_p(const std::vector<cv::Mat_<T> > &v);

bool column_vector_p(const cv::Mat_<double> &matrix);
bool column_vectors_p(const std::vector<cv::Mat_<double> > &input);
bool column_vectors_with_length_p(const std::vector<cv::Mat_<double> > &input, int length);

std::ostream &operator<<(std::ostream &stream, const cv::Size &s);

bool file_exists_p(const std::string &pathname);

std::runtime_error make_runtime_error(const char *format, ...);
std::runtime_error vmake_runtime_error(const char *format, va_list args);

std::list<std::string> read_list(std::istream &stream);
std::list<std::string> read_list(const char *filename);
std::vector<std::string> read_list_as_vector(const char *filename);

template <typename Iterator, typename Function>
void read_and_transform_list(const char *filename, Iterator output_iterator, Function function);

// pathnames
std::string pathname_type(const std::string &pathname);
std::string pathname_name(const std::string &pathname);
std::string pathname_directory(const std::string &pathname);
std::string pathname_sans_directory(const std::string &pathname);
std::string make_pathname(const std::string &directory, const std::string &name, const std::string &type);

// operations
template <typename Iterator> void call_delete_on_pointers(Iterator begin, Iterator end);
template <typename Sequence> void call_delete_on_pointers(Sequence &sequence);

template <typename T> std::vector<T> concatenate(const std::vector<T> &a, const std::vector<T> &b);

// Images
cv::Mat_<uint8_t> load_grayscale_image(const char *pathname, int colour_conversion_method, cv::Mat *original_image);
cv::Mat_<uint8_t> load_grayscale_image(const char *pathname, int colour_conversion_method);
cv::Mat_<uint8_t> load_grayscale_image(const char *pathname, cv::Mat *original_image);
cv::Mat_<uint8_t> load_grayscale_image(const char *pathname);

// visualisation
void imshow_normalised(const char *window_name, const cv::Mat_<double> &image);
void imshow_jacobian(const char *window_name, const cv::Mat_<double> &jacobian, const cv::Size_<int> &size);

/* Produces a random vector containing floats between min and max. */
cv::Mat_<double> random_vector(int number_of_rows, double min = -1.0, double max = 1.0);

std::vector<cv::Mat_<double> > random_vectors(int number_of_vectors, int number_of_rows, double min = -1.0, double max = 1.0);

/* Produces a random vector containing floats between min and max. */
cv::Mat_<double> random_matrix(int number_of_rows, int number_of_columns, double min = -1.0, double max = 1.0);

std::vector<cv::Mat_<double> > random_matrices(int number_of_matrices, int number_of_rows, int number_of_columns, double min = -1.0, double max = 1.0);

cv::Mat_<double> random_matrix_of_maximum_rank(int number_of_rows, int number_of_columns, double min = -1.0, double max = 1.0);

/* Cloning */
template <typename T> cv::Mat_<T> clone(const cv::Mat_<T> &input);
template <typename T> std::vector<cv::Mat_<T> > clone(const std::vector<cv::Mat_<T> > &input);

/* norms */
double l1_norm(const cv::Mat_<double> &input);
double l1_norm(const std::vector<cv::Mat_<double> > &input);

double l2_norm(const cv::Mat_<double> &input);
double l2_norm(const std::vector<cv::Mat_<double> > &input);

double l2_norm(const cv::Point_<double> &input);

/* l2_norm(prior * input) */
double l2_norm(const cv::Mat_<double> &input, const cv::Mat_<double> &prior);
double l2_norm(const std::vector<cv::Mat_<double> > &input, const cv::Mat_<double> &prior);

/* l2_sq_norm  == std::pow(l2_norm, 2) */
double l2sq_norm(const cv::Mat_<double> input);
double l2sq_norm(const std::vector<cv::Mat_<double> > input);
double l2sq_norm(const cv::Mat_<double> input, const cv::Mat_<double> &prior);
double l2sq_norm(const std::vector<cv::Mat_<double> > input, const cv::Mat_<double> &prior);

#endif

#include "helpers.tpp"

// Local Variables:
// compile-in-directory: "../"
// End:
