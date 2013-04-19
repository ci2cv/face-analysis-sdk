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

// -*-c++-*-
#ifndef _HELPERS_TPP_
#define _HELPERS_TPP_


template <typename Iterator, typename Predicate> 
bool
every(Iterator begin, Iterator end, Predicate predicate)
{
  for (Iterator current = begin; current != end; current++) {
    if (!predicate(*current))
      return false;
  }
  return true;
}

template <typename Iterator1, typename Iterator2, typename Predicate>
bool
every(Iterator1 begin1, Iterator1 end1, Iterator2 begin2, Iterator2 end2, Predicate predicate)
{
  Iterator1 current1 = begin1;
  Iterator2 current2 = begin2;
  while ((current1 != end1) && (current2 != end2)) {
    if (!predicate(*current1, *current2))
      return false;

    current1++;
    current2++;
  }

  return true;
}


template <typename Iterator, typename Predicate>
bool
some(Iterator begin, Iterator end, Predicate predicate)
{
  for (Iterator current = begin; current != end; current++) {
    if (predicate(*current))
      return true;
  }
  
  return false;
}

template <typename InputIterator, typename OutputIterator, typename Predicate>
void
remove_if_not(InputIterator begin, InputIterator end, OutputIterator out, Predicate predicate)
{
  for (InputIterator current = begin; current != end; current++) {
    if (predicate(*current)) {
      *out = *current;
      out++;
    }
  }
}

template <typename InputIterator1, typename InputIterator2, typename Function>
void
for_each(InputIterator1 start1, InputIterator1 end1, InputIterator2 start2, InputIterator2 end2, Function function)
{
  InputIterator1 current1 = start1;
  InputIterator2 current2 = start2;
  while ((current1 != end1) && (current2 != end2)) {
    function(*current1, *current2);

    current1++;
    current2++;
  }
}

template <typename Iterator1, typename Iterator2, typename T, typename BinaryFunction, typename Key>
typename BinaryFunction::return_value
reduce(Iterator1 begin, Iterator1 end, T init, BinaryFunction function, Key key)
{
  T rv(init);
  for (Iterator1 current = begin; current != end; current++) {
    rv = function(rv, key(*current));
  }
  return rv;
}

template <typename T>
bool
valid_row_index_p(const cv::Mat_<T> &matrix, int index)
{
  return ((index >= 0) && (index < matrix.rows));
}

template <typename T>
bool
valid_column_index_p(const cv::Mat_<T> &matrix, int index)
{
  return ((index >= 0) && (index < matrix.cols));
}

template <typename T>
bool
valid_matrix_index_p(const cv::Mat_<T> &matrix, int row, int column)
{
  return true
    && valid_row_index_p(matrix, row)
    && valid_column_index_p(matrix, column);
}

template <typename T>
cv::Mat_<T>
ones(int number_of_rows, int number_of_columns)
{
  cv::Mat_<T> rv(number_of_rows, number_of_columns);
  for (int i = 0; i < number_of_rows; i++) {
    for (int j = 0; j < number_of_columns; j++) {
      rv(i,j) = 1;
    }
  }
  return rv;
}

template <typename T>
cv::Mat_<T>
zeros(int number_of_rows, int number_of_columns)
{
  return cv::Mat_<T>::zeros(number_of_rows, number_of_columns);
}

template <typename T, typename U>
cv::Mat_<T>
zeros(const cv::Size_<U> &size)
{
  return zeros<T>(size.height, size.width);
}

template <typename T>
std::vector<cv::Mat_<T> >
vector_of_zeros(int number_of_elements, int rows, int columns)
{
  return vector_of_value(number_of_elements, zeros<T>(rows, columns));
}

template <typename T>
std::vector<cv::Mat_<T> >
vector_of_value(int number_of_elements, const cv::Mat_<T> &value)
{
  std::vector<cv::Mat_<T> > rv(number_of_elements);
  for (int i = 0; i < number_of_elements; i++) {
    rv[i] = value.clone();
  }
  return rv;
}

template <typename T>
std::vector<cv::Mat_<T> >
vector_of_value(int number_of_elements, const cv::MatExpr &expression)
{
  cv::Mat_<T> value(expression);
  return vector_of_value(number_of_elements, value);
}

template <typename T>
cv::Mat_<T>
eye(int number_of_rows, int number_of_columns)
{
  return cv::Mat_<T>::eye(number_of_rows, number_of_columns);
}

template <typename T1, typename T2>
bool
vector_lengths_equal_p(const std::vector<T1> &v1, const std::vector<T2> &v2)
{
  return v1.size() == v2.size();
}

template <typename T1, typename T2, typename T3>
bool
vector_lengths_equal_p(const std::vector<T1> &v1, const std::vector<T2> &v2, const std::vector<T3> &v3)
{
  return true
    && vector_lengths_equal_p(v1, v2)
    && vector_lengths_equal_p(v1, v3);
}

template <typename T1, typename T2, typename T3, typename T4>
bool
vector_lengths_equal_p(const std::vector<T1> &v1, const std::vector<T2> &v2, const std::vector<T3> &v3, const std::vector<T4> &v4)
{
  return true
    && vector_lengths_equal_p(v1, v2)
    && vector_lengths_equal_p(v1, v3)
    && vector_lengths_equal_p(v1, v4);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
bool
vector_lengths_equal_p(const std::vector<T1> &v1, const std::vector<T2> &v2, const std::vector<T3> &v3, const std::vector<T4> &v4, const std::vector<T5> &v5)
{
  return true
    && vector_lengths_equal_p(v1, v2)
    && vector_lengths_equal_p(v1, v3)
    && vector_lengths_equal_p(v1, v4)
    && vector_lengths_equal_p(v1, v5);
}


template <typename T>
bool
matrix_dimensions_equal_p(const std::vector<cv::Mat_<T> > &v)
{
  if (v.size() <= 1)
    return true;

  for (size_t i = 1; i < v.size(); i++) {
    if (!matrix_with_dimensions_p(v[i], v[0].size()))
      return false;
  }
  return true;
}

template <typename T>
bool
less_than_or_equal_to(const cv::Mat_<T> &a, const cv::Mat_<T> &b)
{
  if (a.size() != b.size())
    throw make_runtime_error("Cannot perform comparison as matrix sizes do not match.");

  for (int i = 0; i < a.rows; i++) {
    for (int j = 0; j < a.cols; j++) {
      if (a(i,j) > b(i,j))
	return false;
    }
  }
  return true;
}

template <typename Iterator, typename Function>
void
read_and_transform_list(const char *filename, Iterator output_iterator, Function function)
{
  std::list<std::string> pathnames = read_list(filename);
  std::transform(pathnames.begin(), pathnames.end(), output_iterator, function);
}

template <typename Iterator>
void
call_delete_on_pointers(Iterator begin, Iterator end)
{
  for (Iterator current = begin; current != end; current++) {
    delete *current;
  }
}

template <typename Sequence>
void
call_delete_on_pointers(Sequence &sequence)
{
  call_delete_on_pointers(sequence.begin(), sequence.end());
}

template <typename T>
cv::Mat_<T>
clone(const cv::Mat_<T> &input)
{
  return input.clone();
}

template <typename T>
std::vector<cv::Mat_<T> >
clone(const std::vector<cv::Mat_<T> > &input)
{
  std::vector<cv::Mat_<T> > rv(input.size());
  for (size_t i = 0; i < input.size(); i++) {
    rv[i] = clone(input[i]);
  }
  return rv;
}

template <typename T>
std::vector<T>
concatenate(const std::vector<T> &a, const std::vector<T> &b)
{
  std::vector<T> rv(a);
  std::copy(b.begin(), b.end(), std::back_inserter(rv));
  return rv;
}

template <typename T>
T
IdentityFunction<T>::operator()(const T &a)
{
  return a;
}

template <typename T>
IdentityFunction<T> identity()
{
  return IdentityFunction<T>();
}

#endif

// Local Variables:
// compile-in-directory: "../"
// End:
