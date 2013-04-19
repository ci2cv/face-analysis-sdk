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

#ifndef _COMMAND_LINE_ARGUMENTS_HPP_
#define _COMMAND_LINE_ARGUMENTS_HPP_

#include <string>
#include <stdexcept>
#include <sstream>

std::string get_argument(int *i, int argc, char **argv);

template <typename T>
T
get_argument(int *i, int argc, char **argv)
{
  std::stringstream s(get_argument(i, argc, argv));
  T rv;
  s >> rv;
  return rv;
}

template <typename T>
class CommandLineArgument;

template <>
class CommandLineArgument<std::string>
{
public:
  CommandLineArgument() : is_assigned(false) {}
  CommandLineArgument &operator=(const std::string &argument) {
    is_assigned = true;
    value = argument;
    
    return *this;
  }

  std::string &operator*() { 
    if (!isAssigned())
      throw std::runtime_error("Cannot obtain reference to value of expected command line argument as no value has been assigned.");

    return value;
  }

  const std::string &operator*() const { 
    if (!isAssigned())
      throw std::runtime_error("Cannot obtain reference to value of expected command line argument as no value has been assigned.");

    return value;
  }

  std::string *operator->() {
    if (!isAssigned())
      throw std::runtime_error("Cannot obtain pointer to value of expected command line argument as no value has been assigned.");

    return &value;
  }

  const std::string *operator->() const {
    if (!isAssigned())
      throw std::runtime_error("Cannot obtain pointer to value of expected command line argument as no value has been assigned.");

    return &value;
  }

  bool isAssigned() const {
    return is_assigned;
  }

private:
  bool is_assigned;
  std::string value;
};

template <typename T>
class CommandLineArgument
{
public:
  CommandLineArgument() : is_assigned(false) {}

  CommandLineArgument &operator=(const std::string &argument) {
    is_assigned = true;

    std::stringstream s(argument);
    s >> value;

    return *this;
  }

  CommandLineArgument &operator=(const T &argument) {
    is_assigned = true;
    value = argument;
    return *this;
  }

  T &operator*() { 
    if (!isAssigned())
      throw std::runtime_error("Cannot obtain reference to value of expected command line argument as no value has been assigned.");

    return value;
  }

  T *operator->() {
    if (!isAssigned())
      throw std::runtime_error("Cannot obtain pointer to value of expected command line argument as no value has been assigned.");

    return &value;
  }

  bool isAssigned() const {
    return is_assigned;
  }

private:
  bool is_assigned;
  T value;
};

/**
 ** HAVE_ARGUMENT_P boiler plate.
 **/

template <typename T>
bool have_argument_p(const CommandLineArgument<T> &argument)
{
  return argument.isAssigned();
}

template <typename T>
bool have_arguments_p(const CommandLineArgument<T> &argument)
{
  return have_argument_p(argument);
}

template <typename T1, typename T2>
bool have_arguments_p(const CommandLineArgument<T1> &argument1,
		      const CommandLineArgument<T2> &argument2)
{
  return true
    && have_argument_p(argument1)
    && have_argument_p(argument2);
}

template <typename T1, typename T2, typename T3>
bool have_arguments_p(const CommandLineArgument<T1> &argument1,
		      const CommandLineArgument<T2> &argument2,
		      const CommandLineArgument<T3> &argument3)
{
  return true
    && have_argument_p(argument1)
    && have_argument_p(argument2)
    && have_argument_p(argument3);
}

template <typename T1, typename T2, typename T3, typename T4>
bool have_arguments_p(const CommandLineArgument<T1> &argument1,
		      const CommandLineArgument<T2> &argument2,
		      const CommandLineArgument<T3> &argument3,
		      const CommandLineArgument<T4> &argument4)
{
  return true
    && have_argument_p(argument1)
    && have_argument_p(argument2)
    && have_argument_p(argument3)
    && have_argument_p(argument4);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
bool have_arguments_p(const CommandLineArgument<T1> &argument1,
		      const CommandLineArgument<T2> &argument2,
		      const CommandLineArgument<T3> &argument3,
		      const CommandLineArgument<T4> &argument4,
		      const CommandLineArgument<T5> &argument5)
{
  return true
    && have_argument_p(argument1)
    && have_argument_p(argument2)
    && have_argument_p(argument3)
    && have_argument_p(argument4)
    && have_argument_p(argument5);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
bool have_arguments_p(const CommandLineArgument<T1> &argument1,
		      const CommandLineArgument<T2> &argument2,
		      const CommandLineArgument<T3> &argument3,
		      const CommandLineArgument<T4> &argument4,
		      const CommandLineArgument<T5> &argument5,
		      const CommandLineArgument<T6> &argument6)
{
  return true
    && have_argument_p(argument1)
    && have_argument_p(argument2)
    && have_argument_p(argument3)
    && have_argument_p(argument4)
    && have_argument_p(argument5)
    && have_argument_p(argument6);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
bool have_arguments_p(const CommandLineArgument<T1> &argument1,
		      const CommandLineArgument<T2> &argument2,
		      const CommandLineArgument<T3> &argument3,
		      const CommandLineArgument<T4> &argument4,
		      const CommandLineArgument<T5> &argument5,
		      const CommandLineArgument<T6> &argument6,
		      const CommandLineArgument<T7> &argument7)
{
  return true
    && have_argument_p(argument1)
    && have_argument_p(argument2)
    && have_argument_p(argument3)
    && have_argument_p(argument4)
    && have_argument_p(argument5)
    && have_argument_p(argument6)
    && have_argument_p(argument7);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
bool have_arguments_p(const CommandLineArgument<T1> &argument1,
		      const CommandLineArgument<T2> &argument2,
		      const CommandLineArgument<T3> &argument3,
		      const CommandLineArgument<T4> &argument4,
		      const CommandLineArgument<T5> &argument5,
		      const CommandLineArgument<T6> &argument6,
		      const CommandLineArgument<T7> &argument7,
		      const CommandLineArgument<T8> &argument8)
{
  return true
    && have_argument_p(argument1)
    && have_argument_p(argument2)
    && have_argument_p(argument3)
    && have_argument_p(argument4)
    && have_argument_p(argument5)
    && have_argument_p(argument6)
    && have_argument_p(argument7)
    && have_argument_p(argument8);
}

/**
 ** ASSIGN_ARGUMENT boiler plate.
 **/

template <typename T>
bool assign_argument(const std::string &value, CommandLineArgument<T> &argument)
{
  if (have_argument_p(argument))
    return false;
  else {
    argument = value;
    return true;
  }
}

template <typename T1, typename T2>
bool assign_argument(const std::string &value, CommandLineArgument<T1> &argument1, CommandLineArgument<T2> &argument2)
{
  return false
    || assign_argument(value, argument1)
    || assign_argument(value, argument2);
}

template <typename T1, typename T2, typename T3>
bool assign_argument(const std::string &value,
		     CommandLineArgument<T1> &argument1,
		     CommandLineArgument<T2> &argument2,
		     CommandLineArgument<T3> &argument3)
{
  return false
    || assign_argument(value, argument1)
    || assign_argument(value, argument2)
    || assign_argument(value, argument3);
}

template <typename T1, typename T2, typename T3, typename T4>
bool assign_argument(const std::string &value,
		     CommandLineArgument<T1> &argument1,
		     CommandLineArgument<T2> &argument2,
		     CommandLineArgument<T3> &argument3,
		     CommandLineArgument<T4> &argument4)
{
  return false
    || assign_argument(value, argument1)
    || assign_argument(value, argument2)
    || assign_argument(value, argument3)
    || assign_argument(value, argument4);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
bool assign_argument(const std::string &value,
		     CommandLineArgument<T1> &argument1,
		     CommandLineArgument<T2> &argument2,
		     CommandLineArgument<T3> &argument3,
		     CommandLineArgument<T4> &argument4,
		     CommandLineArgument<T5> &argument5)
{
  return false
    || assign_argument(value, argument1)
    || assign_argument(value, argument2)
    || assign_argument(value, argument3)
    || assign_argument(value, argument4)
    || assign_argument(value, argument5);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
bool assign_argument(const std::string &value,
		     CommandLineArgument<T1> &argument1,
		     CommandLineArgument<T2> &argument2,
		     CommandLineArgument<T3> &argument3,
		     CommandLineArgument<T4> &argument4,
		     CommandLineArgument<T5> &argument5,
		     CommandLineArgument<T6> &argument6)
{
  return false
    || assign_argument(value, argument1)
    || assign_argument(value, argument2)
    || assign_argument(value, argument3)
    || assign_argument(value, argument4)
    || assign_argument(value, argument5)
    || assign_argument(value, argument6);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
bool assign_argument(const std::string &value,
		     CommandLineArgument<T1> &argument1,
		     CommandLineArgument<T2> &argument2,
		     CommandLineArgument<T3> &argument3,
		     CommandLineArgument<T4> &argument4,
		     CommandLineArgument<T5> &argument5,
		     CommandLineArgument<T6> &argument6,
		     CommandLineArgument<T7> &argument7)
{
  return false
    || assign_argument(value, argument1)
    || assign_argument(value, argument2)
    || assign_argument(value, argument3)
    || assign_argument(value, argument4)
    || assign_argument(value, argument5)
    || assign_argument(value, argument6)
    || assign_argument(value, argument7);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
bool assign_argument(const std::string &value,
		     CommandLineArgument<T1> &argument1,
		     CommandLineArgument<T2> &argument2,
		     CommandLineArgument<T3> &argument3,
		     CommandLineArgument<T4> &argument4,
		     CommandLineArgument<T5> &argument5,
		     CommandLineArgument<T6> &argument6,
		     CommandLineArgument<T7> &argument7,
		     CommandLineArgument<T8> &argument8)
{
  return false
    || assign_argument(value, argument1)
    || assign_argument(value, argument2)
    || assign_argument(value, argument3)
    || assign_argument(value, argument4)
    || assign_argument(value, argument5)
    || assign_argument(value, argument6)
    || assign_argument(value, argument7)
    || assign_argument(value, argument8);
}

#endif

// Local Variables:
// compile-in-directory: "../"
// End:
