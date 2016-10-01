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
#include "tracker/FaceTracker.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace FACETRACKER;

void
print_usage()
{
  std::string text =
    "Usage: [options] <image-argument> [landmarks-argument]\n"
    "\n"
    "Options:\n"
    "  --help                    This helpful message.\n"    
    "  --lists                   Switch to list processing mode. See below.\n"
    "  --video                   Switch to video processing mode. See below.\n"
    "  --wait-time <double>      How many seconds to wait when displaying the\n"
    "                            registration results. The default depends on the mode.\n"
    "  --model <pathname>        The pathname to the tracker model to use.\n"
    "  --params <pathname>       The pathname to the parameters to use.\n"
    "  --threshold <int>         The threshold of the error detector.\n"
    "                            Can range from 0 to 10 where 10 is extremely picky.\n"
    "                            The default is 5.\n"
    "  --title <string>          The window title to use.\n"                  
    "  --3d                      Save 3D shape instead of the 2D shape.\n"
    "  --verbose                 Display information whilst processing.\n"
    "\n"
    "Default mode:\n"
    "Perform fitting on an image located at <image-argument> and save\n"
    "the results to [landarks-argument] if specified, otherwise display\n"
    "the results.\n"
    "\n"
    "List mode:\n"
    "Perform fitting on the list of image pathnames contained in <image-argument>.\n"
    "If [landmarks-argument] is specified, then it must be a list with the same length as\n"
    "<image-argument> and contain pathnames to write the tracked points to.\n"
    "\n"
    "Video mode:\n"
    "Perform fitting on the video found at <image-pathname>. If\n"  
    "[landmarks-argument] is specified, then it represents a format string\n"
    "used by sprintf. The template must accept at most one unsigned integer\n"
    "value. If no [landmarks-argument] is given, then the tracking is displayed\n"
    "to the screen.\n"
    "\n";
  
  std::cout << text << std::endl;
}

class user_pressed_escape : public std::exception
{
public:
};

struct Configuration
{
  double wait_time;
  std::string model_pathname;
  std::string params_pathname;
  int tracking_threshold;
  std::string window_title;
  bool verbose;
  bool save_3d_points;

  int circle_radius;
  int circle_thickness;
  int circle_linetype;
  int circle_shift;
};

int run_lists_mode(const Configuration &cfg,
		   const CommandLineArgument<std::string> &image_argument,
		   const CommandLineArgument<std::string> &landmarks_argument);

int run_video_mode(const Configuration &cfg,
		   const CommandLineArgument<std::string> &image_argument,
		   const CommandLineArgument<std::string> &landmarks_argument);

int run_image_mode(const Configuration &cfg,
		   const CommandLineArgument<std::string> &image_argument,
		   const CommandLineArgument<std::string> &landmarks_argument);

void display_data(const Configuration &cfg,
		  const cv::Mat &image,
		  const std::vector<cv::Point_<double> > &points,
		  const Pose &pose);

int
run_program(int argc, char **argv)
{
  CommandLineArgument<std::string> image_argument;
  CommandLineArgument<std::string> landmarks_argument;

  bool lists_mode = false;
  bool video_mode = false;
  bool wait_time_specified = false;

  Configuration cfg;
  cfg.wait_time = 0;
  cfg.model_pathname = DefaultFaceTrackerModelPathname();
  cfg.params_pathname = DefaultFaceTrackerParamsPathname();
  cfg.tracking_threshold = 5;
  cfg.window_title = "CSIRO Face Fit";
  cfg.verbose = false;
  cfg.circle_radius = 2;
  cfg.circle_thickness = 1;
  cfg.circle_linetype = 8;
  cfg.circle_shift = 0;  
  cfg.save_3d_points = false;

  for (int i = 1; i < argc; i++) {
    std::string argument(argv[i]);

    if ((argument == "--help") || (argument == "-h")) {
      print_usage();
      return 0;
    } else if (argument == "--lists") {
      lists_mode = true;
    } else if (argument == "--video") {
      video_mode = true;
    } else if (argument == "--wait-time") {
      wait_time_specified = true;
      cfg.wait_time = get_argument<double>(&i, argc, argv);
    } else if (argument == "--model") {
      cfg.model_pathname = get_argument(&i, argc, argv);
    } else if (argument == "--params") {
      cfg.params_pathname = get_argument(&i, argc, argv);
    } else if (argument == "--title") {
      cfg.window_title = get_argument(&i, argc, argv);
    } else if (argument == "--threshold") {
      cfg.tracking_threshold = get_argument<int>(&i, argc, argv);
    } else if (argument == "--verbose") {
      cfg.verbose = true;
    } else if (argument == "--3d") {
      cfg.save_3d_points = true;
    } else if (!assign_argument(argument, image_argument, landmarks_argument)) {
      throw make_runtime_error("Unable to process argument '%s'", argument.c_str());
    }
  }

  if (!have_argument_p(image_argument)) {
    print_usage();
    return 0;
  }

  if (lists_mode && video_mode)
    throw make_runtime_error("The operator is confused as the switches --lists and --video are present on the command line.");

  if (lists_mode) {
    if (!wait_time_specified)
      cfg.wait_time = 1.0 / 30;
    return run_lists_mode(cfg, image_argument, landmarks_argument);
  } else if (video_mode) {
    if (!wait_time_specified)
      cfg.wait_time = 1.0 / 30;
    return run_video_mode(cfg, image_argument, landmarks_argument);
  } else {
    if (!wait_time_specified) 
      cfg.wait_time = 0;      
    return run_image_mode(cfg, image_argument, landmarks_argument);			  
  }

  return 0;
}

int
main(int argc, char **argv)
{
  try {
    return run_program(argc, argv);
  } catch (user_pressed_escape &e) {
    std::cout << "Stopping prematurely." << std::endl;
    return 1;
  } catch (std::exception &e) {
    std::cerr << "Caught unhandled exception: " << e.what() << std::endl;
    return 2;
  }
}

// Helpers
int
run_lists_mode(const Configuration &cfg,
	       const CommandLineArgument<std::string> &image_argument,
	       const CommandLineArgument<std::string> &landmarks_argument)
{
  FaceTracker * tracker = LoadFaceTracker(cfg.model_pathname.c_str());
  FaceTrackerParams *tracker_params  = LoadFaceTrackerParams(cfg.params_pathname.c_str());

  std::list<std::string> image_pathnames = read_list(image_argument->c_str());
  std::list<std::string> landmark_pathnames;
  if (have_argument_p(landmarks_argument)) {
    landmark_pathnames = read_list(landmarks_argument->c_str());
    if (landmark_pathnames.size() != image_pathnames.size())
      throw make_runtime_error("Number of pathnames in list '%s' does not match the number in '%s'",
			       image_argument->c_str(), landmarks_argument->c_str());
  }

  std::list<std::string>::const_iterator image_it     = image_pathnames.begin();
  std::list<std::string>::const_iterator landmarks_it = landmark_pathnames.begin();
  const int number_of_images = image_pathnames.size();
  int current_image_index = 1;

  for (; image_it != image_pathnames.end(); image_it++) {
    if (cfg.verbose) {
      printf(" Image %d/%d\r", current_image_index, number_of_images);    
      fflush(stdout);
    }
    current_image_index++;

    cv::Mat image;
    cv::Mat_<uint8_t> gray_image = load_grayscale_image(image_it->c_str(), &image);
    int result = tracker->NewFrame(gray_image, tracker_params);

    std::vector<cv::Point_<double> > shape;
    std::vector<cv::Point3_<double> > shape3D;
    Pose pose;
    if (result >= cfg.tracking_threshold) {
      shape = tracker->getShape();
      shape3D = tracker->get3DShape();
      pose = tracker->getPose();
    } else {
      tracker->Reset();
    }

    if (!have_argument_p(landmarks_argument)) {
      display_data(cfg, image, shape, pose);
    } else if (shape.size() > 0) {
      if (cfg.save_3d_points)	
	save_points3(landmarks_it->c_str(), shape3D);
      else
	save_points(landmarks_it->c_str(), shape);

      if (cfg.verbose)
	display_data(cfg, image, shape, pose);
    } else if (cfg.verbose) {
      display_data(cfg, image, shape, pose);
    }

    if (have_argument_p(landmarks_argument))
      landmarks_it++;
  }  

  delete tracker;
  delete tracker_params; 
  
  return 0;
}

int
run_video_mode(const Configuration &cfg,
	       const CommandLineArgument<std::string> &image_argument,
	       const CommandLineArgument<std::string> &landmarks_argument)
{
  FaceTracker *tracker = LoadFaceTracker(cfg.model_pathname.c_str());
  FaceTrackerParams *tracker_params = LoadFaceTrackerParams(cfg.params_pathname.c_str());

  assert(tracker);
  assert(tracker_params);

  cv::VideoCapture input(image_argument->c_str());
  if (!input.isOpened())
    throw make_runtime_error("Unable to open video file '%s'", image_argument->c_str());

  cv::Mat image;

  std::vector<char> pathname_buffer;
  pathname_buffer.resize(1000);

  input >> image;
  int frame_number = 1;

  while ((image.rows > 0) && (image.cols > 0)) {
    if (cfg.verbose) {
      printf(" Frame number %d\r", frame_number);
      fflush(stdout);
    }

    cv::Mat_<uint8_t> gray_image;
    if (image.type() == cv::DataType<cv::Vec<uint8_t,3> >::type)
      cv::cvtColor(image, gray_image, CV_BGR2GRAY);
    else if (image.type() == cv::DataType<uint8_t>::type)
      gray_image = image;
    else
      throw make_runtime_error("Do not know how to convert video frame to a grayscale image.");

    int result = tracker->Track(gray_image, tracker_params);

    std::vector<cv::Point_<double> > shape;
    std::vector<cv::Point3_<double> > shape3D;
    Pose pose;

    if (result >= cfg.tracking_threshold) {
      shape = tracker->getShape();
      shape3D = tracker->get3DShape();
      pose = tracker->getPose();
    } else {
      tracker->Reset();
    }

    if (!have_argument_p(landmarks_argument)) {
      display_data(cfg, image, shape, pose);
    } else if (shape.size() > 0) {
      snprintf(pathname_buffer.data(), pathname_buffer.size(), landmarks_argument->c_str(), frame_number);

      if (cfg.save_3d_points)	
	save_points3(pathname_buffer.data(), shape3D);
      else
	save_points(pathname_buffer.data(), shape);

      if (cfg.verbose)
	display_data(cfg, image, shape, pose);
    } else if (cfg.verbose) {
      display_data(cfg, image, shape, pose);
    }

    input >> image;
    frame_number++;
  }

  delete tracker;
  delete tracker_params; 

  return 0;
}

int
run_image_mode(const Configuration &cfg,
	       const CommandLineArgument<std::string> &image_argument,
	       const CommandLineArgument<std::string> &landmarks_argument)
{  
  FaceTracker * tracker = LoadFaceTracker(cfg.model_pathname.c_str());
  FaceTrackerParams *tracker_params  = LoadFaceTrackerParams(cfg.params_pathname.c_str());

  cv::Mat image;
  cv::Mat_<uint8_t> gray_image = load_grayscale_image(image_argument->c_str(), &image);

  int result = tracker->NewFrame(gray_image, tracker_params);

  std::vector<cv::Point_<double> > shape;
  std::vector<cv::Point3_<double> > shape3;
  Pose pose;
  
  if (result >= cfg.tracking_threshold) {
    shape = tracker->getShape();
    shape3 = tracker->get3DShape();
    pose = tracker->getPose();
  }

  if (!have_argument_p(landmarks_argument)) {
    display_data(cfg, image, shape, pose); 
  } else if (shape.size() > 0) {
    if (cfg.save_3d_points)
      save_points3(landmarks_argument->c_str(), shape3);
    else
      save_points(landmarks_argument->c_str(), shape);
  }
 
  delete tracker;
  delete tracker_params; 
  
  return 0;
}

cv::Mat
compute_pose_image(const Pose &pose, int height, int width)
{
  cv::Mat_<cv::Vec<uint8_t,3> > rv = cv::Mat_<cv::Vec<uint8_t,3> >::zeros(height,width);
  cv::Mat_<double> axes = pose_axes(pose);
  cv::Mat_<double> scaling = cv::Mat_<double>::eye(3,3);

  for (int i = 0; i < axes.cols; i++) {
    axes(0,i) = -0.5*double(width)*(axes(0,i) - 1);
    axes(1,i) = -0.5*double(height)*(axes(1,i) - 1);
  }
  
  cv::Point centre(width/2, height/2);
  // pitch
  cv::line(rv, centre, cv::Point(axes(0,0), axes(1,0)), cv::Scalar(255,0,0));
  // yaw
  cv::line(rv, centre, cv::Point(axes(0,1), axes(1,1)), cv::Scalar(0,255,0));
  // roll
  cv::line(rv, centre, cv::Point(axes(0,2), axes(1,2)), cv::Scalar(0,0,255));

  return rv;
}

void
display_data(const Configuration &cfg,
	     const cv::Mat &image,
	     const std::vector<cv::Point_<double> > &points,
	     const Pose &pose)
{

  cv::Scalar colour;
  if (image.type() == cv::DataType<uint8_t>::type)
    colour = cv::Scalar(255);
  else if (image.type() == cv::DataType<cv::Vec<uint8_t,3> >::type)
    colour = cv::Scalar(0,0,255);
  else
    colour = cv::Scalar(255);

  cv::Mat displayed_image;
  if (image.type() == cv::DataType<cv::Vec<uint8_t,3> >::type)
    displayed_image = image.clone();
  else if (image.type() == cv::DataType<uint8_t>::type)
    cv::cvtColor(image, displayed_image, CV_GRAY2BGR);
  else 
    throw make_runtime_error("Unsupported camera image type for display_data function.");

  for (size_t i = 0; i < points.size(); i++) {
    cv::circle(displayed_image, points[i], cfg.circle_radius, colour, cfg.circle_thickness, cfg.circle_linetype, cfg.circle_shift);
  }

  int pose_image_height = 100;
  int pose_image_width = 100;
  cv::Mat pose_image = compute_pose_image(pose, pose_image_height, pose_image_width);
  for (int i = 0; i < pose_image_height; i++) {
    for (int j = 0; j < pose_image_width; j++) {
      displayed_image.at<cv::Vec<uint8_t,3> >(displayed_image.rows - pose_image_height + i,
					      displayed_image.cols - pose_image_width + j)
			 
			 = pose_image.at<cv::Vec<uint8_t,3> >(i,j);
    }
  }

  cv::imshow(cfg.window_title, displayed_image);

  if (cfg.wait_time == 0)
    std::cout << "Press any key to continue." << std::endl;

  char ch = cv::waitKey(cfg.wait_time * 1000);

  if (ch == 27) // escape
    throw user_pressed_escape();
}
