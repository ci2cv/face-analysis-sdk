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
#include <iostream>
#include <iterator>
#include <cerrno>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void
print_usage()
{
  std::string text = 
    "Usage: [options] <N> <list 1> ... <list N> <command> [command options]\n"
    "\n"
    "Options:\n"
    "  --help    This helpful message.\n"
    "  --echo    Do not execute the command, just echo what would be executed\n"
    "Read arguments in lists to pass to command as arguments\n";
  
  std::cout << text << std::endl;
}

class Runner
{
public:
  virtual void perform(const std::string &command, const std::list<std::string> &arguments) = 0;
};

class EchoRunner : public Runner
{
public:
  EchoRunner(std::ostream &stream)
    : stream(stream)
  {

  }

  void perform(const std::string &command, const std::list<std::string> &arguments) {
    stream << command << " ";
    std::copy(arguments.begin(), arguments.end(), std::ostream_iterator<std::string>(stream, " "));
    stream << std::endl;
  }
  
private:
  std::ostream &stream;
};

class ForkRunner : public Runner
{
public:
  void perform(const std::string &command, const std::list<std::string> &arguments) {
    pid_t new_pid = fork();
    if (new_pid == -1) 
      throw make_runtime_error("Unable to fork a new process: %s.", strerror(errno));

    if (new_pid == 0) { // child process
      char *argv[arguments.size() + 2];
      argv[0] = (char *)command.c_str();
      argv[arguments.size() + 1] = 0;

      std::list<std::string>::const_iterator it = arguments.begin();
      for (size_t i = 0; i < arguments.size(); i++) {
	argv[i + 1] = (char *)it->c_str();
	it++;
      }
      
      int rv = execvp(command.c_str(), argv);
      if (rv == -1) 
	throw make_runtime_error("Unable to create new process: %s.", strerror(errno));
    } else { // parent process
      int status;
      pid_t wait_pid = waitpid(new_pid, &status, 0);      
      if (wait_pid == -1)
	throw make_runtime_error("Unable to wait for process.");

      if (WEXITSTATUS(status) != 0) {
	std::stringstream s;
	s << command << " ";
	std::copy(arguments.begin(), arguments.end(), std::ostream_iterator<std::string>(s, " "));

	throw make_runtime_error("map-list failed to execute the following command successfully: %s", s.str().c_str());
      }
    }
  }
};

int
run_program(int argc, char **argv)
{
  CommandLineArgument<int> number_of_list_files;
  int next_command_line_argument = 0;
  bool echo_commands = false;

  for (int i = 1; ((i < argc) && (!have_argument_p(number_of_list_files))); i++) {
    std::string argument(argv[i]);
    if (argument == "--help") {
      print_usage();
      return 0;
    } else if (argument == "--echo") {
      echo_commands = true;
    } else if (!have_argument_p(number_of_list_files)) {
      number_of_list_files = argument;
      next_command_line_argument = i + 1;
    }
  }

  if (!have_argument_p(number_of_list_files)) {
    print_usage();
    return -1;
  }
  
  if (*number_of_list_files == 0)
    return 0;

  const int minimum_number_of_arguments = 0
    + *number_of_list_files
    + 1 // the command;
    + 0;

  if ((argc - next_command_line_argument) < minimum_number_of_arguments)
    throw make_runtime_error("Not enough arguments available for processing.\n");

  std::list<std::string> standard_input_list;
  bool have_read_standard_input_list = false;

  std::vector<std::string> list_filenames(*number_of_list_files);
  std::vector<std::list<std::string> > lists(*number_of_list_files);  
  for (int i = 0; i < *number_of_list_files; i++) {    
    int argument_index = i + next_command_line_argument;
    std::string list_file = argv[argument_index];
    std::list<std::string> l;
    
    if ((list_file == "-") && (have_read_standard_input_list)) {
      l = standard_input_list;
    } else if ((list_file == "-")) {
      standard_input_list = read_list(std::cin);
      have_read_standard_input_list = true;
      l = standard_input_list;
    } else if (!file_exists_p(list_file)) {
      throw make_runtime_error("List file %s does not exist.", list_file.c_str());
    } else {
      l = read_list(list_file.c_str());
    }

    list_filenames[i] = list_file;
    lists[i] = l;
  }
  next_command_line_argument += *number_of_list_files;

  // read the command and its options.
  std::string command(argv[next_command_line_argument]);
  std::list<std::string> command_arguments;
  std::copy(argv + next_command_line_argument + 1, argv + argc, std::back_inserter(command_arguments));

  // check all lists are the same size.
  for (size_t i = 0; i < lists.size(); i++) {
    if (lists[i].size() != lists[0].size())
      throw make_runtime_error("The number of entires in list %s (%d) differs to %s (%d).", 
			       list_filenames[i].c_str(),
			       lists[i].size(),
			       list_filenames[0].c_str(),
			       lists[0].size());
  }

  EchoRunner echo_runner(std::cout);
  ForkRunner fork_runner;

  Runner *runner = 0;
  if (echo_commands) 
    runner = &echo_runner;  
  else
    runner = &fork_runner;

  assert(runner);
  std::vector<std::list<std::string>::const_iterator> iterators(*number_of_list_files);
  for (int i = 0; i < *number_of_list_files; i++)
    iterators[i] = lists[i].begin();

  for (size_t i = 0; i < lists[0].size(); i++) {
    std::string invocation_command = command;
    std::list<std::string> invocation_arguments;
    std::copy(command_arguments.begin(), command_arguments.end(), std::back_inserter(invocation_arguments));
    for (size_t j = 0; j < iterators.size(); j++) {
      invocation_arguments.push_back(*iterators[j]);
      iterators[j]++;
    }

    runner->perform(invocation_command, invocation_arguments);
  }

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
