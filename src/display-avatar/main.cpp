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
#include "avatar/Avatar.hpp"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>

void
print_usage()
{
  std::string text =
    "Usage: [options] [model]\n"
    "\n"
    "Options:\n"
    "  --help        This helpful message.\n"
    "\n"
    "The argument [model] specifies the avatar model file to visualise.\n"
    "\n";

  std::cout << text << std::endl;
}

void
print_controls()
{
  std::string text =
    "a or '<-' for previous avatar.\n"
    "d or '->' for next avatar.\n"
    "q or ESC to quit.\n"
    "\n";

  std::cout << text << std::endl;
}

void
show_avatar(AVATAR::Avatar *avatar, int index)
{
  assert(index >= 0);
  assert(index < avatar->numberOfAvatars());
  printf(" Currently displaying avatar: %d\r", index);
  fflush(stdout);

  cv::imshow("Avatar", avatar->Thumbnail(index));
}

int
run_program(int argc, char **argv)
{
  CommandLineArgument<std::string> model_pathname;

  for (int i = 1; i < argc; i++) {
    std::string argument(argv[i]);

    if (argument == "--help") {
      print_usage();
      return 0;
    } else if (!assign_argument(argument, model_pathname)) {
      throw make_runtime_error("Do not know how to process argument '%s'",
			       argument.c_str());
    }
  }

  if (!have_argument_p(model_pathname))
    model_pathname = AVATAR::DefaultAvatarModelPathname();

  if (!file_exists_p(model_pathname->c_str()))
    throw make_runtime_error("Model file '%s' does not exist",
			     model_pathname->c_str());

  AVATAR::Avatar *avatar = AVATAR::LoadAvatar(model_pathname->c_str());
  if (avatar->numberOfAvatars() == 0)
    return 0;

  print_controls();

  const int minimum_avatar = 0;
  const int maximum_avatar = avatar->numberOfAvatars() - 1;
  int index = 0;
  bool quit_application_p = false;

  printf("Number of avatars: %d\n", avatar->numberOfAvatars());
  show_avatar(avatar, index);

  while (!quit_application_p) {    
    char ch = cv::waitKey(10);
    switch (ch) {
    case 'a':      
    case 2:
      index = std::max(minimum_avatar, index - 1);
      show_avatar(avatar, index);
      break;
    case 'd':
    case 3:
      index = std::min(maximum_avatar, index + 1);
      show_avatar(avatar, index);
      break;
    case 'q':
    case 27:
      quit_application_p = true;
      break;
    case -1:
      break;
    default:
      break;
    }
  }
  
  printf("\n");
  delete avatar;
  
  return 0;
}

int
main(int argc, char **argv)
{
  try {
    return run_program(argc, argv);
  } catch (std::exception &e) {
    std::cout << "Caught unhandled exception: " << e.what() << std::endl;
    return -1;
  }
}
