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
#include "utils/points.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

void
print_usage()
{
  std::string text =
    "Usage: [options] <image> <points> [output-image]\n"
    "\n"
    "Options:\n"
    "  --help                   This helpful message.\n"
    "  --radius <double>        The radius of the circle to draw. Default is 5\n"
    "  --colour <r> <g> <b>     The colour of the circle to draw. Default is 255 0 0.\n"
    "  --wait-time <double>     How long to wait when displaying the image. Default is infinity.\n"
    "\n"
    "\n"
    "Arguments:\n"
    "<image> is the image to draw the <points> on to.\n"    
    "<points> is the landmarks to draw.\n"
    "[output-image] is a drawn image to create.\n"
    "\n";

  std::cout << text << std::endl;
}

int
run_program(int argc, char **argv)
{
  CommandLineArgument<std::string> image_pathname;
  CommandLineArgument<std::string> points_pathname;
  CommandLineArgument<std::string> output_pathname;

  int radius = 5;
  cv::Scalar colour(0,0,255);
  int thickness = 1;
  int line_type = 8;
  int shift = 0;
  double wait_time;

  for (int i = 1; i < argc; i++) {
    std::string argument(argv[i]);

    if (argument == "--help") {
      print_usage();
      return 0;
    } else if (argument == "--radius") {
      radius = get_argument<int>(&i, argc, argv);
    } else if (argument == "--colour") {
      colour[2] = get_argument<int>(&i, argc, argv);
      colour[1] = get_argument<int>(&i, argc, argv);
      colour[0] = get_argument<int>(&i, argc, argv);
    } else if (argument == "--wait-time") {
      wait_time = get_argument<double>(&i, argc, argv);
    } else if (!assign_argument(argument, image_pathname, points_pathname, output_pathname)) {
      throw make_runtime_error("Do not know how to process argument '%s'",
			       argument.c_str());
    }
  }

  if (!have_arguments_p(image_pathname, points_pathname)) {
    print_usage();
    return 0;
  }

  cv::Mat img = cv::imread(image_pathname->c_str());
  if ((img.rows == 0) || (img.cols == 0))
    throw make_runtime_error("Unable to load image at path '%s'", image_pathname->c_str());

  std::vector<cv::Point_<double> > pts = load_points(points_pathname->c_str());

  for (size_t i = 0; i < pts.size(); i++) {
    cv::circle(img, pts[i], radius, colour, thickness, line_type, shift);
  }

  if (have_argument_p(output_pathname)) {
    cv::imwrite(output_pathname->c_str(), img);
  } else {
    cv::imshow("Image", img);
    cv::waitKey(1000*wait_time);
  }
  
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
