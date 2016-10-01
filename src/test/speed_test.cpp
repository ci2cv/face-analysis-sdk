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

#include <avatar/Avatar.hpp>
#include <tracker/FaceTracker.hpp>
#include <tracker/myFaceTracker.hpp>
#include <opencv/highgui.h>
#include <iostream>
#include <fstream>

#include <test/command-line-options.hpp>

#define db at<double>
#define it at<int>

static
bool file_exists_and_is_readable_p(const std::string &filename) 
{
  std::ifstream stream(filename.c_str());
  return stream.good();
}

static
int how_many_screen_refreshes_have_occurred(int64 now_time, int64 last, int fps) 
{
  double number_of_frames = std::floor(fps*((double)now_time - (double)last)/cv::getTickFrequency());
  return std::max(0,-1 + (int)number_of_frames);
}

static
int how_many_frames_have_been_dropped(int64 now_time, int64 last, int fps)
{
  return how_many_screen_refreshes_have_occurred(now_time, last, fps);
}

//=============================================================================
void draw_shape(cv::Mat &image,cv::Mat &shape,cv::Mat &con)
{
  int i,n = shape.rows/2; cv::Point p1,p2; 
  for(i = 0; i < con.cols; i++){
    p1 = cv::Point(shape.at<double>(con.at<int>(0,i),0),
		   shape.at<double>(con.at<int>(0,i)+n,0));
    p2 = cv::Point(shape.at<double>(con.at<int>(1,i),0),
		   shape.at<double>(con.at<int>(1,i)+n,0));
    cv::line(image,p1,p2,CV_RGB(0,255,0),1);
  }
  for(i = 0; i < n; i++){    
    p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0));
    cv::circle(image,p1,2,CV_RGB(255,0,0));
  }return;
}
//==============================================================================
void draw_health(cv::Mat &I,int health)
{
  double v = double(health)/10.0;
  int w = 0.1*double(I.cols),h = 0.05*double(I.rows);
  cv::rectangle(I,cv::Point(h/2,h/2),cv::Point(w+h/2,h+h/2),CV_RGB(0,0,0),3);
  cv::Mat im = I(cv::Rect(h/2+3,h/2+3,v*(w-5),h-5)); im = CV_RGB(0,0,255);
  char str[256]; sprintf(str,"%d%%",(int)(100*v+0.5));
  cv::putText(I,str,cv::Point(w+h/2+w/20,h+h/2),
	      CV_FONT_HERSHEY_SIMPLEX,h*0.045,CV_RGB(0,0,255),2); return;
}
//==============================================================================
int main(int argc, char** argv)
{
  //get input parameters
  std::cout << "Usage: ./speed_test [options]" 
	    << std::endl
	    << "options: " << std::endl
	    << "  --camera camera_index_or_filename      which camera to use (default 0) or a pathname to a video file" << std::endl
            << "  --eye-mouth-refine integer             0=no, 1=yes (default 0)" << std::endl
	    << "  --output-images [filename_format]      Create images of the output. filename_format is passed to sprintf to generate the filename from the frame index. (default %05d.png)" << std::endl
	    << "  --help or -h                           Show this informative help message" << std::endl
	    << std::endl
	    << "advanced options: " << std::endl
            << "  --tracker-threshold integer            threshold used to reset tracking (default 6)" << std::endl
	    << "  --face-connections-file path           Connections file (default src/tracker/resources/face.con)" << std::endl
	    << "  --face-tracker-file path               Face Tracker Configuration File (default src/tracker/resources/face.mytracker.binary)" << std::endl
            << "  --face-tracker-parameters-file path    Face Tracker Parameters File (default src/tracker/resources/face.mytrackerparams.binary)" << std::endl
            << "  --avatar-file path                     Avatar Configuration File (default src/avatar/resources/CI2CV.avatar.binary)" << std::endl
	    << "  --pseudo-realtime [fps]                Make the program act like it is running in real time. (default 30)" << std::endl
	    << "  --initialise-user-on-first-frame       Initialise the avatar synthesis code on the first frame." << std::endl
	    << "  --hide-avatar-thumbnail                Hide the avatar thumbnail drawn on the input video." << std::endl
	    << "  --maximum-number-of-input-frames max   The maximum number of input frames to process." << std::endl
	    << std::endl;

  OptionDescriptions descriptions;
  descriptions.registerIdentifier("camera", "--camera", OptionDescription::ARGUMENT_REQUIRED);
  descriptions.registerIdentifier("tracker-threshold","--tracker-threshold", OptionDescription::ARGUMENT_REQUIRED);
  descriptions.registerIdentifier("eye-mouth-refine","--eye-mouth-refine", OptionDescription::ARGUMENT_REQUIRED);
  descriptions.registerIdentifier("output-images", "--output-images", OptionDescription::ARGUMENT_OPTIONAL);
  descriptions.registerIdentifier("face-connections-file","--face-connections-file", OptionDescription::ARGUMENT_REQUIRED);
  descriptions.registerIdentifier("face-tracker-parameters-file","--face-tracker-parameters-file", OptionDescription::ARGUMENT_REQUIRED);
  descriptions.registerIdentifier("face-tracker-file","--face-tracker-file", OptionDescription::ARGUMENT_REQUIRED);
  descriptions.registerIdentifier("avatar-file", "--avatar-file", OptionDescription::ARGUMENT_REQUIRED);
  descriptions.registerIdentifier("pseudo-realtime", "--pseudo-realtime", OptionDescription::ARGUMENT_OPTIONAL);
  descriptions.registerIdentifier("initialise-user-on-first-frame", "--initialise-user-on-first-frame", OptionDescription::ARGUMENT_NONE);
  descriptions.registerIdentifier("hide-avatar-thumbnail", "--hide-avatar-thumbnail", OptionDescription::ARGUMENT_NONE);
  descriptions.registerIdentifier("help","--help", OptionDescription::ARGUMENT_NONE);
  descriptions.registerOption("help","-h");
  descriptions.registerIdentifier("maximum-number-of-input-frames", "--maximum-number-of-input-frames", OptionDescription::ARGUMENT_REQUIRED);

  Options options;
  int camera_index;  
  std::string camera_file;
  int tracker_threshold;
  int eye_mouth_refine;
  std::string face_connections_file;
  std::string face_tracker_file;
  std::string face_tracker_parameters_file;
  std::string avatar_file;
  bool output_images_p;
  std::string output_images_filename_format;
  bool pseudo_realtime_p;
  int  pseudo_realtime_fps;
  bool hide_avatar_thumbnail;
  bool initialise_user_on_first_frame;
  int maximum_number_of_input_frames;
  try {
    descriptions.processOptions(argc, argv, options);

    if (options.isPresent("camera"))
      camera_file                  = options.argument("camera");
    
    try {
      camera_index                 = options.argument<int>("camera", 0);
    } catch (std::exception &e) {
      camera_index = -1;
    }

    tracker_threshold              = options.argument<int>("tracker-threshold",6);    
    eye_mouth_refine               = options.argument<int>("eye-mouth-refine",0);    
    output_images_p                = options.isPresent("output-images");
    output_images_filename_format  = options.argument("output-images","%05d.png");
    face_connections_file          = options.argument("face-connections-file","src/tracker/resources/face.con");
    face_tracker_file              = options.argument("face-tracker-file", "src/tracker/resources/face.mytracker.binary");
    face_tracker_parameters_file   = options.argument("face-tracker-parameters-file", "src/tracker/resources/face.mytrackerparams.binary");
    avatar_file                    = options.argument("avatar-file", "src/avatar/resources/CI2CV.avatar.binary");
    pseudo_realtime_p              = options.isPresent("pseudo-realtime");
    pseudo_realtime_fps            = options.argument<int>("pseudo-realtime",30);
    initialise_user_on_first_frame = options.isPresent("initialise-user-on-first-frame");
    hide_avatar_thumbnail          = options.isPresent("hide-avatar-thumbnail");
    maximum_number_of_input_frames = options.argument<int>("maximum-number-of-input-frames", std::numeric_limits<int>::max());

    if (options.isPresent("help"))
      return 0;

  } catch (std::exception &e) {
    std::cerr << "Option processing failed: " << e.what() << std::endl;
    return -1;
  }

  FACETRACKER::FaceTrackerParams * p = FACETRACKER::LoadFaceTrackerParams(face_tracker_parameters_file.c_str());
  FACETRACKER::myFaceTrackerParams* pp = dynamic_cast<FACETRACKER::myFaceTrackerParams *>(p);
  pp->shape_predict = eye_mouth_refine;

  char* fname = NULL;
  cv::Mat con = FACETRACKER::IO::LoadCon(face_connections_file.c_str());
  FACETRACKER::FaceTracker* tracker = 
    FACETRACKER::LoadFaceTracker(face_tracker_file.c_str());
  AVATAR::Avatar* avatar = 
    AVATAR::LoadAvatar(avatar_file.c_str());
  assert((p != NULL) && (tracker != NULL) && (avatar != NULL));
  cv::Mat im,draw; cvNamedWindow("test"); 
  cv::VideoCapture camera;
  if (camera_index == -1) {
    if (file_exists_and_is_readable_p(camera_file)) {
      camera.open(camera_file);
    } else {
      std::cerr << "Video file does not exist." << std::endl;
      return -1;
    }      
  } else {
    camera.open(camera_index);
  }

  bool init = false; cv::Mat thumb = avatar->Thumbnail(0);
  std::cout << "Usage:" << std::endl
	    << "d: Redetect" << std::endl
	    << "i: Initialise and start avatar " << std::endl;
  
  int frame = 0;
  int64 total_time=0;
  double total_fps=0;
  int64 capture_time=0;
  double capture_fps=0;
  int64 track_time=0;
  double track_fps=0;
  int64 animate_time=0;
  double animate_fps=0;
  char str[2000];

  int output_frame_number = 0;
  int64 output_frame_last_output = 0;
  cv::Mat output_frame_last_frame;
  int64 input_frame_last_captured = 0;
  int64 input_frame_index = 0;
  while(1){
    int64 total1 = cv::getTickCount(); 

    //get image
    int64 capture1 = cv::getTickCount(); 
    cv::Mat im; 
    if (pseudo_realtime_p && (input_frame_index > 0)) {
      int64 start_time = cv::getTickCount();

      int64 now = cv::getTickCount();
      int number_of_dropped_frames = how_many_frames_have_been_dropped(now, input_frame_last_captured, pseudo_realtime_fps);

      if (number_of_dropped_frames > 0)
	std::cout << "Dropping " << number_of_dropped_frames << " input frames" << std::endl;
      for (int dropped_frame_index = 0; dropped_frame_index < number_of_dropped_frames; dropped_frame_index++) {
	camera >> im;
	input_frame_index++;
      }

      int64 end_time = cv::getTickCount();
      output_frame_last_output += (end_time - start_time);
    }

    if (input_frame_index > maximum_number_of_input_frames) 
      break;

    camera >> im; 
    input_frame_index++;
    input_frame_last_captured = cv::getTickCount();

    if(fname){
      double frac = camera.get(CV_CAP_PROP_POS_AVI_RATIO);
      if( ((1.0-frac) < 1.0e-5) || (im.rows == 0) || (im.cols == 0))break;
    }else cv::flip(im,im,1); 
    int64 capture2 = cv::getTickCount(); 
    capture_time += capture2-capture1;

    //create drawing image
    if(draw.rows != im.rows)draw.create(im.rows,im.cols*2,CV_8UC3);
    draw = cv::Scalar(0);

    //track
    int64 track1 = cv::getTickCount(); 
    int health = tracker->Track(im,p);   
    bool failed = false;

    if (health < tracker_threshold){
      if(!(health == FACETRACKER::FaceTracker::TRACKER_FACE_OUT_OF_FRAME)){
	tracker->Reset(); failed = true;
      }
    }
    int64 track2 = cv::getTickCount();
    track_time += track2-track1; 

    cv::Mat uimg = draw(cv::Rect(0,0,im.cols,im.rows)); im.copyTo(uimg); 

    //animate
    if(!failed){
      if (initialise_user_on_first_frame && !init) {
	avatar->Initialise(im,tracker->_shape); 
	init = true;
      }
      draw_health(uimg,std::max(0,health)); 
      draw_shape(uimg,tracker->_shape,con);
      int64 animate1 = cv::getTickCount(); 
      if(init){
	cv::Mat aimg = draw(cv::Rect(im.cols,0,im.cols,im.rows));
	avatar->Animate(aimg,im,tracker->_shape);
      }
      int64 animate2 = cv::getTickCount();
      animate_time += animate2-animate1;
    }
    if (!hide_avatar_thumbnail) {
      cv::Mat timg = draw(cv::Rect(im.cols-thumb.cols,im.rows-thumb.rows,
				   thumb.cols,thumb.rows));
      thumb.copyTo(timg); 
    }

    //draw time
    /*
      Commented out as the cv::cvWaitKey(1) pauses the loop for 1000
      milliseconds causing grief with the numbers returned.
      
    sprintf(str,"total time: %d frames/sec",(int)round(total_fps)); 
    std::string total_text = str;
    cv::putText(draw,total_text,cv::Point(im.cols + 10,20),
		CV_FONT_HERSHEY_SIMPLEX,0.5,CV_RGB(255,255,255));
    sprintf(str,"capture time: %d frames/sec",(int)round(capture_fps)); 
    std::string capture_text = str;
    cv::putText(draw,capture_text,cv::Point(im.cols + 10,40),
		CV_FONT_HERSHEY_SIMPLEX,0.5,CV_RGB(255,255,255));
    */
    sprintf(str,"track time: %d frames/sec",(int)round(track_fps)); 
    std::string track_text = str;
    cv::putText(draw,track_text,cv::Point(im.cols + 10,60),
		CV_FONT_HERSHEY_SIMPLEX,0.5,CV_RGB(255,255,255));
    sprintf(str,"animate time: %d frames/sec",(int)round(animate_fps)); 
    std::string animate_text = str;
    cv::putText(draw,animate_text,cv::Point(im.cols + 10,80),
		CV_FONT_HERSHEY_SIMPLEX,0.5,CV_RGB(255,255,255));
    
    cv::imshow("test",draw); 

    if (output_images_p) {
      int64 start_time = cv::getTickCount();
	
      char buf[1000];
      
      if (pseudo_realtime_p && (output_frame_number > 0)) {
	int64 now_time = cv::getTickCount();
	int number_of_synthesized_images = how_many_screen_refreshes_have_occurred(now_time, output_frame_last_output, pseudo_realtime_fps);
	if (number_of_synthesized_images > 0) {
	  std::cout << "Padding output with " << number_of_synthesized_images << " frames" << std::endl;
	}
	for (int count = 0; count < number_of_synthesized_images; count++) {
	  sprintf(buf, output_images_filename_format.c_str(), output_frame_number);
	  cv::imwrite(buf, output_frame_last_frame);
	  output_frame_number++;	  	  
	}
      }
      sprintf(buf,output_images_filename_format.c_str(), output_frame_number);
      cv::imwrite(buf, draw);

      output_frame_last_output = cv::getTickCount();      
      output_frame_last_frame  = draw;					
      output_frame_number++;

      int64 end_time = cv::getTickCount();
      input_frame_last_captured += (end_time - start_time);
    }

    int c = cvWaitKey(1);
    if(c == 27)break; 
    else if(c == int('d'))tracker->Reset();
    else if(c == int('i')){
      avatar->Initialise(im,tracker->_shape); init = true;
    }
    input_frame_last_captured += 1.0/(1000.0*cv::getTickFrequency());
    output_frame_last_output += 1.0/(1000.0*cv::getTickFrequency());

    int64 total2 = cv::getTickCount(); 
    total_time += total2-total1;
    
    frame++;

#define UPDATE_EVERY_X_FRAMES 30
    if(frame == UPDATE_EVERY_X_FRAMES){
      total_fps = UPDATE_EVERY_X_FRAMES/((double(total_time)/cvGetTickFrequency())/1e+6); 
      capture_fps = UPDATE_EVERY_X_FRAMES/((double(capture_time)/cvGetTickFrequency())/1e+6); 
      track_fps = UPDATE_EVERY_X_FRAMES/((double(track_time)/cvGetTickFrequency())/1e+6); 
      animate_fps = UPDATE_EVERY_X_FRAMES/((double(animate_time)/cvGetTickFrequency())/1e+6); 
      total_time = 0;
      capture_time = 0;
      track_time = 0;
      animate_time = 0;
      frame = 0;
    }
  }return 0;
}
//==============================================================================
