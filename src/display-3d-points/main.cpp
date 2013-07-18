#include "utils/points.hpp"
#include "utils/command-line-arguments.hpp"
#include "utils/helpers.hpp"
#include "tracker/FaceTracker.hpp"

#include <opencv2/highgui/highgui.hpp>

void
print_usage()
{
  std::string text =
    "Usage: [options] <3d-points>\n"
    "\n"
    "Options:\n"
    "  --help                   This helpful message.\n"
    "  --width <int>            The width of the window. Default is 640\n"
    "  --height <int>           The height of the window. Default is 480\n"
    "  --window-title <title>   The title to use for the window.\n"
    "  --circle-radius <int>    The radius of the circle. Default is 2\n"
    
    "\n"
    "The argument <3d-points> contains the 3D points to display.\n"
    "\n"
    "\n"
    "The following keys control the projection used to draw the 3D shape:\n"
    " x-axis rotations: 'w' and 's'\n"
    " y-axis rotations: 'a' and 'd'\n"
    " z-axis rotations: 'q' and 'e'\n"
    "          zooming: 'z' and 'x'\n"
    "            reset: 'r'\n"
    "             quit: ESC\n"
    "\n";
  
  std::cout << text << std::endl;
}

struct Configuration
{
  int width;
  int height;
  std::string window_title;

  int circle_radius;
  int circle_thickness;
  int circle_linetype;
  int circle_shift;
  cv::Scalar circle_colour;
};

typedef std::vector<cv::Point3_<double> > Points3d;

cv::Point3_<double> calculate_mean(const Points3d &points);
cv::Mat_<double> scaling_matrix(double v);
cv::Mat_<double> calculate_similarity_transform(const Points3d &points, int height, int width);
cv::Mat_<double> rotation_about_x_axis(double theta);
cv::Mat_<double> rotation_about_y_axis(double theta);
cv::Mat_<double> rotation_about_z_axis(double theta);
void display_points(const Configuration &cfg, const cv::Mat_<double> &projection, const Points3d &points);

int
run_program(int argc, char **argv)
{
  CommandLineArgument<std::string> points_pathname;

  Configuration cfg;
  cfg.width = 640;
  cfg.height = 480;
  cfg.window_title = "";
  cfg.circle_radius = 2;
  cfg.circle_thickness = 1;
  cfg.circle_linetype = 8;
  cfg.circle_shift = 0;
  cfg.circle_colour = cv::Scalar(0, 0, 255);
  
  for (int i = 1; i < argc; i++) {
    std::string argument(argv[i]);
    if (argument == "--help") {
      print_usage();
      return 0;
    } else if (argument == "--width") {
      cfg.width = get_argument<int>(&i, argc, argv);
    } else if (argument == "--height") {
      cfg.height = get_argument<int>(&i, argc, argv);
    } else if (argument == "--window-title") {
      cfg.window_title = get_argument(&i, argc, argv);
    } else if (argument == "--circle-radius") {
      cfg.circle_radius = get_argument<int>(&i, argc, argv);
    } else if (!assign_argument(argument, points_pathname)) {
      throw make_runtime_error("Unable to process argument: %s\n", argument.c_str());
    }
  }

  if (!have_argument_p(points_pathname)) {
    print_usage();
    return -1;
  }

  if (cfg.window_title == "") 
    cfg.window_title = points_pathname->c_str();

  std::vector<cv::Point3_<double> > shape3D;
  try {
    shape3D = load_points3(points_pathname->c_str());
  } catch (std::exception &e) {
    throw make_runtime_error("Encountered error when reading file '%s': %s", points_pathname->c_str(), e.what());
  }

  cv::Point3_<double> mean = calculate_mean(shape3D);
  for (size_t i = 0; i < shape3D.size(); i++) {
    shape3D[i] -= mean;
  }

  cv::Mat_<double> projection = cv::Mat_<double>::eye(3,4);
  if (cfg.height < cfg.width) {
    projection(0,3) = double(cfg.width - cfg.height) / 2.0;
  } else {
    projection(1,3) = double(cfg.height - cfg.width) / 2.0;
  }

  cv::Mat_<double> starting_A = calculate_similarity_transform(shape3D, cfg.height, cfg.width);
  cv::Mat_<double> A(starting_A.clone());
  
  bool quit = false;
  while (!quit) {
    int key = cv::waitKey(1);
    switch (key) {
    case 27:
      quit = true;
      break;
    case 'z':
      A = A * scaling_matrix(1.1);
      break;
    case 'x':
      A = A * scaling_matrix(0.9);
      break;
    case 'a':
      A = A * rotation_about_y_axis(-0.1);
      break;
    case 'd':
      A = A * rotation_about_y_axis(0.1);
      break;
    case 'w':
      A = A * rotation_about_x_axis(-0.1);
      break;
    case 's':
      A = A * rotation_about_x_axis(0.1);
      break;
    case 'q':
      A = A * rotation_about_z_axis(0.1);
      break;
    case 'e':
      A = A * rotation_about_z_axis(-0.1);
      break;
    case 'r':
      A = starting_A.clone();
      break;
    }

    display_points(cfg, projection * A, shape3D);
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
    return 2;
  }
}

void
display_points(const Configuration &cfg, const cv::Mat_<double> &projection, const Points3d &points)
{
  cv::Mat_<cv::Vec<uint8_t, 3> > displayed_image = cv::Mat_<cv::Vec<uint8_t, 3> >::zeros(cfg.height, cfg.width);

  for (size_t i = 0; i < points.size(); i++) {
    cv::Mat_<double> input_x(4,1);
    input_x(0,0) = points[i].x;
    input_x(1,0) = points[i].y;
    input_x(2,0) = points[i].z;
    input_x(3,0) = 1;

    cv::Mat_<double> out_x = projection * input_x;

    cv::circle(displayed_image, cv::Point_<double>(out_x(0,0), out_x(1,0)),
	       cfg.circle_radius, cfg.circle_colour, cfg.circle_thickness, cfg.circle_linetype, cfg.circle_shift);
  }

  cv::imshow(cfg.window_title, displayed_image);
}

cv::Mat_<double>
scaling_matrix(double v)
{
  cv::Mat_<double> rv = cv::Mat_<double>::eye(4,4);
  rv(0,0) = v;
  rv(1,1) = v;
  rv(2,2) = v;
  return rv;
}

cv::Mat_<double>
rotation_about_x_axis(double theta)
{
  cv::Mat_<double> rv = cv::Mat_<double>::eye(4,4);
  rv(1,1) = cos(theta);
  rv(1,2) = sin(theta);
  rv(2,1) = -sin(theta);
  rv(2,2) = cos(theta);
  return rv;
}

cv::Mat_<double>
rotation_about_y_axis(double theta)
{
  cv::Mat_<double> rv = cv::Mat_<double>::eye(4,4);
  rv(0,0) = cos(theta);
  rv(0,2) = sin(theta);
  rv(2,0) = -sin(theta);
  rv(2,2) = cos(theta);
  return rv;
}

cv::Mat_<double>
rotation_about_z_axis(double theta)
{
  cv::Mat_<double> rv = cv::Mat_<double>::eye(4,4);
  rv(0,0) = cos(theta);
  rv(0,1) = sin(theta);
  rv(1,0) = -sin(theta);
  rv(1,1) = cos(theta);
  return rv;
}

cv::Point3_<double>
calculate_mean(const Points3d &points)
{
  cv::Point3_<double> sum;
  const size_t n = points.size();

  for (size_t i = 0; i < n; i++) {
    sum += points[i];
  }

  sum.x /= double(n);
  sum.y /= double(n);
  sum.z /= double(n);
  return sum;
}

cv::Mat_<double>
calculate_similarity_transform(const Points3d &points, int height, int width)
{
  cv::Point3_<double> mean = calculate_mean(points);

  double maximum = std::numeric_limits<double>::min();
  double minimum = std::numeric_limits<double>::max();

  for (size_t i = 0; i < points.size(); i++) {
    maximum = std::max(maximum, points[i].x - mean.x);
    maximum = std::max(maximum, points[i].y - mean.y);
    maximum = std::max(maximum, points[i].z - mean.z);

    minimum = std::min(minimum, points[i].x - mean.x);
    minimum = std::min(minimum, points[i].y - mean.y);
    minimum = std::min(minimum, points[i].z - mean.z);    
  }

  // calculate the hypotenuse of the cube defined by minimum and
  // maximum as this represents the widest two points.
  double side_length = maximum - minimum;
  double distance = std::sqrt( 3*side_length*side_length );

  // widen the debug to this distance.
  minimum -= (distance - side_length)/2.0;
  maximum += (distance - side_length)/2.0;

  // calculate the similarity transform
  double target_max = 0.98*std::min(height, width);
  double target_min = 0.02*std::min(height, width);

  double a = ( target_max - target_min ) / (maximum - minimum);
  double b = target_max - a*maximum;

  cv::Mat_<double> translation = cv::Mat_<double>::eye(4,4);
  translation(0,3) = -mean.x;
  translation(1,3) = -mean.y;
  translation(2,3) = -mean.z;

  cv::Mat_<double> scaling = cv::Mat_<double>::eye(4,4);
  scaling(0,0) = a;
  scaling(1,1) = a;
  scaling(2,2) = a;
  scaling(0,3) = b;
  scaling(1,3) = b;
  scaling(2,3) = b;

  return scaling * translation;
}
