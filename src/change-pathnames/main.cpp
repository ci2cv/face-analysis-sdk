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

#include "utils/helpers.hpp"
#include "utils/command-line-arguments.hpp"
#include <fstream>
#include <iterator>
#include <iostream>

void
print_usage()
{
  std::string text = 
    "Usage: [options] <path-list> <output-path-list>\n"
    "\n"
    "Options:\n"
    "  --help                 This helpful message.\n"
    "  --directory <string>   The new directory to set.\n"
    "  --type <string>        The new type to set.\n"
    "\n"
    "<path-list>          The set of a pathnames to change the directory of.\n"
    "<output-path-list>   The filename to save the new pathnames to.\n"
    "\n"
    "If <path-list> is the character '-', then the list is read from standard\n"
    "input.\n"
    "\n"
    "If <output-path-list> is the character '-', then the list is written to\n"
    "standard output.\n"
    "\n";

  std::cout << text << std::endl;
}

template <typename Sequence, typename Function>
void
output_new_pathnames(const Sequence &sequence, std::ostream &out, Function function)
{
  std::transform(sequence.begin(), sequence.end(), std::ostream_iterator<std::string>(out,"\n"), function);
}

struct DirectoryOnly : public std::unary_function<std::string, std::string>
{
  DirectoryOnly(const std::string &new_directory)
    : new_directory(new_directory)
  {}

  std::string operator()(const std::string &pathname) {
    return make_pathname(new_directory,
			 pathname_name(pathname),
			 pathname_type(pathname));
  }

private:
  std::string new_directory;
};

struct TypeOnly : public std::unary_function<std::string, std::string>
{
  TypeOnly(const std::string &new_type)
    : new_type(new_type)
  {}

  std::string operator()(const std::string &pathname) {
    return make_pathname(pathname_directory(pathname),
			 pathname_name(pathname),
			 new_type);
  }

private:
  std::string new_type;
};

struct DirectoryAndType : public std::unary_function<std::string, std::string>
{
  DirectoryAndType(const std::string &new_directory, const std::string &new_type) 
    : directory_only(new_directory), type_only(new_type)
  {}

  std::string operator()(const std::string &pathname) {
    return type_only(directory_only(pathname));
  }

private:
  DirectoryOnly directory_only;
  TypeOnly type_only;
};

int
run_program(int argc, char **argv)
{
  CommandLineArgument<std::string> input_pathnames_filename;
  CommandLineArgument<std::string> output_pathnames_filename;

  CommandLineArgument<std::string> new_directory;
  CommandLineArgument<std::string> new_type;

  for (int i = 1; i < argc; i++) {
    std::string argument = argv[i];

    if (argument == "--help") {
      print_usage();
      return 0;
    } else if (argument == "--directory") {
      new_directory = get_argument(&i, argc, argv);
    } else if (argument == "--type") {
      new_type = get_argument(&i, argc, argv);
    } else if (!assign_argument(argument, input_pathnames_filename, output_pathnames_filename)) {
      throw make_runtime_error("Unable to process argument '%s'", argument.c_str());
    }
  }

  if (!have_arguments_p(input_pathnames_filename, output_pathnames_filename)) {
    print_usage();
    return -1;
  }

  std::list<std::string> input_pathnames;
  if (*input_pathnames_filename == "-")
    input_pathnames = read_list(std::cin);
  else
    input_pathnames = read_list(input_pathnames_filename->c_str());

  std::ostream *out;
  std::ofstream out_f;
  if (*output_pathnames_filename == "-")
    out = &std::cout;
  else {
    out_f.open(output_pathnames_filename->c_str(), std::ios::out | std::ios::trunc);
    if (!out_f.is_open()) 
      throw make_runtime_error("Failed to open file '%s'", output_pathnames_filename->c_str());
    out = &out_f;
  }

  if (have_arguments_p(new_directory, new_type)) 
    output_new_pathnames(input_pathnames, *out, DirectoryAndType(*new_directory, *new_type));
  else if (have_argument_p(new_directory))
    output_new_pathnames(input_pathnames, *out, DirectoryOnly(*new_directory));
  else if (have_argument_p(new_type))
    output_new_pathnames(input_pathnames, *out, TypeOnly(*new_type));

  return 0;
}

int
main(int argc, char **argv)
{
  try {
    return run_program(argc, argv);
  } catch (std::exception &e) {
    std::cerr << "Caught unhandled exception: " << e.what() << std::endl;
    return -1;
  }
}

// Local Variables:
// compile-in-directory: "../"
// End:
