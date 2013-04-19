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
#include "avatar/Avatar.hpp"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>

void
print_usage()
{
  std::string text =
    "Usage: [options] <calibration-image> <calibration-landmarks> \\ \n"
    "                 <image-argument> <landmarks-argument> <output-argument>\n"
    "\n"
    "Options:\n"
    "  --help              This helpful message.\n"
    "\n"
    "  --model <model>                     The collection of avatars to obtain the avatar from.\n"
    "  --index <index>                     The index of the avatar to use.\n"
    "  --background-colour <r> <g> <b>     The colour of the background.\n"
    "  --overlay                           Overlay the avatar over the user. Default is false.\n"
    "  --if-does-not-exists <action>       What to do if an input file does not exist.\n"
    "                                      Can be one of 'error', 'warn'. Default is error.\n"
    "\n"
    "  --lists             Switch to lists mode. See below.\n"
    "\n"
    "\n"
    "The arguments <calibration-image> and <calibration-landmarks> are as calibration data. The \n"
    "image and corresponding shape must be of an individual displaying a neutral expression.\n"
    "\n"
    "Default mode:\n"
    "Use the image <image-argument> and shape <landmarks-argument> to synthesize the expression of \n"
    "an avatar.\n"
    "\n"
    "Lists mode:\n"
    "The arguments <image-argument>, <landmarks-argument> and <output-argument> are now lists of \n"
    "pathnames to perform expression transfer on.\n"
    "\n";

  std::cout << text << std::endl;
}

struct Configuration
{
  std::string model_pathname;
  int model_index;
  cv::Vec<uint8_t, 3> background_colour;
  bool overlay;

  std::string if_does_not_exist;
};

bool
uint8p(int value)
{
  return true
    && (value >= 0)
    && (value <= 255);
}

int
run_program(int argc, char **argv)
{
  CommandLineArgument<std::string> calibration_image_pathname;
  CommandLineArgument<std::string> calibration_landmarks_pathname;
  CommandLineArgument<std::string> image_argument;
  CommandLineArgument<std::string> landmarks_argument;
  CommandLineArgument<std::string> output_argument;

  Configuration cfg;  
  cfg.model_pathname = AVATAR::DefaultAvatarModelPathname();
  cfg.model_index = 0;
  cfg.background_colour = cv::Vec<uint8_t,3>(0,0,0);
  cfg.overlay = false;
  cfg.if_does_not_exist = "error";

  bool lists_mode = false;

  for (int i = 1; i < argc; i++) {
    std::string argument(argv[i]);

    if (argument == "--help") {
      print_usage();
      return 0;
    } else if (argument == "--model") {
      cfg.model_pathname = get_argument(&i, argc, argv);
    } else if (argument == "--index") {
      cfg.model_index = get_argument<int>(&i, argc, argv);
    } else if (argument == "--lists") {
      lists_mode = true;
    } else if (argument == "--background-colour") {
      int red   = get_argument<int>(&i, argc, argv);
      int green = get_argument<int>(&i, argc, argv);
      int blue  = get_argument<int>(&i, argc, argv);
      assert(uint8p(red));
      assert(uint8p(green));
      assert(uint8p(blue));
      cfg.background_colour = cv::Vec<uint8_t,3>(blue, green, red);
    } else if (argument == "--overlay") {
      cfg.overlay = true;
    } else if (argument == "--if-does-not-exist") {
      cfg.if_does_not_exist = get_argument(&i, argc, argv);
    } else if (!assign_argument(argument, 
				calibration_image_pathname,
				calibration_landmarks_pathname,
				image_argument, landmarks_argument, output_argument)) {
      throw make_runtime_error("Unable to process argument '%s", argument.c_str());
    }
  }

  if (!have_arguments_p(calibration_image_pathname,
			calibration_landmarks_pathname,
			image_argument, landmarks_argument, output_argument)) {
    print_usage();
    return -1;
  }

  if ((cfg.if_does_not_exist != "error") && (cfg.if_does_not_exist != "warn"))
    throw make_runtime_error("Invalid value for --if-does-not-exist argument. Can only be one of 'error' or 'warn'.");

  std::list<std::string> image_pathnames;
  std::list<std::string> landmark_pathnames;
  std::list<std::string> output_pathnames;

  if (lists_mode) {
    image_pathnames = read_list(image_argument->c_str());
    landmark_pathnames = read_list(landmarks_argument->c_str());
    output_pathnames = read_list(output_argument->c_str());
  } else {
    image_pathnames.push_back(*image_argument);
    landmark_pathnames.push_back(*landmarks_argument);
    output_pathnames.push_back(*output_argument);
  }

  if (image_pathnames.size() != landmark_pathnames.size())
    throw make_runtime_error("Lists have different sizes: images (%d) versus landmarks (%d)",
			     image_pathnames.size(), landmark_pathnames.size());

  if (image_pathnames.size() != output_pathnames.size())
    throw make_runtime_error("Lists have difference sizes: images (%d) versus output (%d)",
			     image_pathnames.size(), output_pathnames.size());
  
  // Create the avatar instance
  AVATAR::Avatar *avatar = AVATAR::LoadAvatar(cfg.model_pathname.c_str());
  void *avatar_params    = 0;

  if (!avatar)
    throw make_runtime_error("Failed to load avatar.");

  if (avatar->numberOfAvatars() <= cfg.model_index)
    throw make_runtime_error("Invalid avatar index %d for '%s' model. File only containts %d avatars.",
			     cfg.model_index, cfg.model_pathname.c_str(), avatar->numberOfAvatars());  

  avatar->setAvatar(cfg.model_index);

  // Calibrate the expression transfer
  cv::Mat_<cv::Vec<uint8_t,3> > calibration_image = cv::imread(calibration_image_pathname->c_str());
  std::vector<cv::Point_<double> > calibration_points = load_points(calibration_landmarks_pathname->c_str());

  avatar->Initialise(calibration_image, calibration_points);

  // Perform expression transfer
  std::list<std::string>::const_iterator image_it     = image_pathnames.begin();
  std::list<std::string>::const_iterator landmarks_it = landmark_pathnames.begin();
  std::list<std::string>::const_iterator output_it    = output_pathnames.begin();

  while (image_it != image_pathnames.end()) {
    bool image_exists_p     = file_exists_p(image_it->c_str());
    bool landmarks_exists_p = file_exists_p(landmarks_it->c_str());

    if (!image_exists_p) {
      if (cfg.if_does_not_exist == "warn") {
	std::cout << "Image pathname '" << image_it->c_str() << "' does not exist." << std::endl;
      } else if (cfg.if_does_not_exist == "error") {
	throw make_runtime_error("Image pathname '%s' does not exist.", image_it->c_str());
      } else {
	throw make_runtime_error("Should not get here.");
      }
    }

    if (!landmarks_exists_p) {
      if (cfg.if_does_not_exist == "warn") {
	std::cout << "Landmarks pathname '" << landmarks_it->c_str() << "' does not exist." << std::endl;
      } else if (cfg.if_does_not_exist == "error") {
	throw make_runtime_error("Landmarks pathname '%s' does not exist.", landmarks_it->c_str());
      } else {
	throw make_runtime_error("Should not get here.");      
      }
    }
  
    if (image_exists_p && landmarks_exists_p) {
      cv::Mat image_unknown = cv::imread(image_it->c_str());
      if (image_unknown.type() != cv::DataType<cv::Vec<uint8_t,3> >::type)	
	throw make_runtime_error("This program only knows draw on 3 channel colour images. The file '%s' just doesn't satisfy this requirement. Sorry.", image_it->c_str());

      cv::Mat_<cv::Vec<uint8_t,3> > image  = image_unknown;
      std::vector<cv::Point_<double> > pts = load_points(landmarks_it->c_str());
      
      cv::Mat_<cv::Vec<uint8_t,3> > output;
      
      if (cfg.overlay) {
	output = image.clone();
      } else {
	output.create(image.rows, image.cols);		      
	output = cfg.background_colour;
      }

      avatar->Animate(output, image_unknown, pts, avatar_params);
      
      cv::imwrite(output_it->c_str(), output);
    }
   
    image_it++;
    landmarks_it++;
    output_it++;    
  }

  // Cleanup
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
