#include "utils/helpers.hpp"
#include "utils/command-line-arguments.hpp"
#include "utils/points.hpp"
#include "avatar/myAvatar.hpp"

#include <opencv2/highgui/highgui.hpp>

void
print_usage()
{
  std::string text = 
    "Usage: [options] <output-pathname> <avatar-image> <avatar-annotation> [eyes-annotation]\n"
    "\n"
    "Options:\n"
    "  --help     This helpful message.\n"
    "  --lists    Switch to lists mode.\n"
    "\n"
    "Default mode:\n"
    "<output-pathname>       The output avatar model pathname.\n"
    "<avatar-image>          The image of the avatar.\n"
    "<avatar-annotation>     The annotations of the avatar.\n"
    "<eye-annotations>       The annotations of the avatar's eye.\n"
    "\n"
    "Lists mode:\n"
    "<output-pathname>       The output avatar model pathname containing all avatars.\n"
    "<avatar-image>          A list of pathnames to avatar images.\n"
    "<avatar-annotation>     A list of pathnames to avatar image annoations.\n"
    "<eyes-annotation>       A list of pathnames to the avatar eye annotations.\n"
    "\n"
    "\n"
    "EYE ANNOTATIONS\n"
    "---------------\n"
    "The points file for the eye annotations must be in this order.\n"
    " - The pupil centre of the left eye.\n"
    " - The pupil centre of the right eye.\n"
    " - The pupil edge of the left eye.\n"
    " - The pupil edge of the right eye.\n"
    "\n"
    "Some avatars do not require eye annotations. It is ok if eyes-annotation is not specified.\n"
    "In list mode, leave a blank line for the avatars that do not require eye annotations.\n"
    "\n";
  
  std::cout << text << std::endl;
}

void
add_to_model(AVATAR::myAvatar *model, const std::string &image_pathname, const std::string &annotation_pathname, const std::string &eyes_pathname)
{
  cv::Mat image = cv::imread(image_pathname.c_str());
  if ((image.rows == 0) || (image.cols == 0))
    throw make_runtime_error("Failed to read image at pathname '%s'", image_pathname.c_str());

  std::vector<cv::Point_<double> > points = load_points(annotation_pathname.c_str());

  assert(0 == (model->_pdm._M.rows % 3));
  const int expected_number_of_annotations = (model->_pdm._M.rows/3);
  if (points.size() != (size_t)expected_number_of_annotations)
    throw make_runtime_error("Insufficient number of annotations of avatar. Expected %d, but got %d.\n",
			     expected_number_of_annotations,
			     points.size());

  cv::Mat_<double> saragih_points = vectorise_points(points);			     
  
  cv::Mat_<double> eyes;
  if (eyes_pathname != "") {
    eyes = vectorise_points(load_points(eyes_pathname.c_str()));
  }

  model->AddAvatar(image, saragih_points, eyes);
}

int
produce_model(const std::string &output_pathname,
	      const std::list<std::string> &images,
	      const std::list<std::string> &annotations,
	      const std::list<std::string> &eyes)
{
  if (images.size() != annotations.size())
    throw make_runtime_error("Number of images and the number of annotations do not match.");

  if (images.size() != eyes.size())
    throw make_runtime_error("Number of images and the number of eye annotations do not match.");

  // We cannot allocate a clean model, so take an existing one, empty
  // it and then write it to file. HACK CITY !! 
  // 
  // Aww man this is so crap. +1 for grey hair.
  AVATAR::Avatar *abstract_model = AVATAR::LoadAvatar(); 
  AVATAR::myAvatar *model = dynamic_cast<AVATAR::myAvatar *>(abstract_model);
  assert(model);

  // See avatar/myAvatar.hpp for thee list of fields to junk. anything starting with std::vector
  model->_scale.clear();
  model->_textr.clear();
  model->_images.clear();  
  model->_shapes.clear();
  model->_reg.clear();
  model->_expr.clear();
  model->_lpupil.clear();
  model->_rpupil.clear();
 
  std::list<std::string>::const_iterator image_it = images.begin();
  std::list<std::string>::const_iterator annotation_it = annotations.begin();
  std::list<std::string>::const_iterator eyes_it = eyes.begin();

  while (image_it != images.end()) {
    assert(annotation_it != annotations.end());
    assert(eyes_it != eyes.end());
    add_to_model(model, *image_it, *annotation_it, *eyes_it);

    image_it++;
    annotation_it++;
    eyes_it++;
  }

  std::ofstream out(output_pathname.c_str(), std::ios::out | std::ios::binary);
  model->Write(out, true);
  out.close();

  delete model;

  return 0;
}

int
run_program(int argc, char **argv)
{
  CommandLineArgument<std::string> output_pathname;
  CommandLineArgument<std::string> image_argument;
  CommandLineArgument<std::string> annotation_argument;
  CommandLineArgument<std::string> eyes_argument;

  bool lists_mode = false;

  for (int i = 1; i < argc; i++) {
    std::string argument(argv[i]);
    
    if (argument == "--help") {
      print_usage();
      return 0;
    } else if (argument == "--lists") {
      lists_mode = true;      
    } else if (!assign_argument(argument, output_pathname, image_argument, annotation_argument, eyes_argument)) {
      throw make_runtime_error("Unable to process argument %s", argument.c_str());
    }
  }

  if (!have_arguments_p(output_pathname, image_argument, annotation_argument)) {
    print_usage();
    return -1;
  }

  std::list<std::string> image_pathnames;
  std::list<std::string> annotation_pathnames;
  std::list<std::string> eye_annotation_pathnames;

  if (lists_mode) {
    image_pathnames = read_list(image_argument->c_str());
    annotation_pathnames = read_list(annotation_argument->c_str());
    if (have_argument_p(eyes_argument))
      eye_annotation_pathnames = read_list(eyes_argument->c_str());
    else
      eye_annotation_pathnames.resize(annotation_pathnames.size());
  } else {
    image_pathnames.push_back(*image_argument);
    annotation_pathnames.push_back(*annotation_argument);
    if (have_argument_p(eyes_argument))
      eye_annotation_pathnames.push_back(*eyes_argument);
    else
      eye_annotation_pathnames.push_back("");
  }

  return produce_model(*output_pathname, image_pathnames, annotation_pathnames, eye_annotation_pathnames);
}

int
main(int argc, char **argv)
{
  try {
    return run_program(argc, argv);
  } catch (std::exception &e) {
    std::cerr << "Caught unhandled exception: " << e.what() << std::endl;
  }
}
