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

#include <tracker/FaceTracker.hpp>
#ifdef _WITH_AVATAR_
#include <avatar/Avatar.hpp>
#endif
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#define db at<double>
#define it at<int>
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
  double v = MAX(0,double(health)/10.0);
  int w = 0.1*double(I.cols),h = 0.05*double(I.rows);
  cv::rectangle(I,cv::Point(h/2,h/2),cv::Point(w+h/2,h+h/2),CV_RGB(0,0,0),3);
  cv::Mat im = I(cv::Rect(h/2+3,h/2+3,v*(w-5),h-5)); im = CV_RGB(0,0,255);
  char str[256]; sprintf(str,"%d%%",(int)(100*v+0.5));
  cv::putText(I,str,cv::Point(w+h/2+w/20,h+h/2),
	      CV_FONT_HERSHEY_SIMPLEX,h*0.045,CV_RGB(0,0,255),2); return;
}
//==============================================================================
int main(int argc, char *argv[])
{
  cv::Mat con = FACETRACKER::IO::LoadCon("src/tracker/resources/face.con");
  FACETRACKER::FaceTrackerParams* p = FACETRACKER::LoadFaceTrackerParams("src/tracker/resources/face.mytrackerparams.binary");
  FACETRACKER::FaceTracker* tracker = 
    FACETRACKER::LoadFaceTracker("src/tracker/resources/face.mytracker.binary");
  // FACETRACKER::LoadFaceTracker("test_data/face.mytracker.binary.noPRA");
  assert((p != NULL) && "FaceTracker parameters can't be NULL - Check file is correct");
  assert((tracker != NULL) && "tracker not initialised - check file is OK");


  // tracker->Save("test_data/face.mytracker.binary.noSINIT", true);
  // return 0;

  bool saveData = false;
  std::string outputFile;
  unsigned int frameCount = 0;
  cv::FileStorage fs;
  int waitTime = 0;
  
#ifdef _WITH_AVATAR_
  AVATAR::Avatar* avatar = 
    AVATAR::LoadAvatar("src/avatar/resources/CI2CV.avatar.binary");
    //AVATAR::LoadAvatar("test_data/kabuki2.avatar.binary");
  assert(avatar != NULL);
  int idx = 0;
  cv::Mat thumb = avatar->Thumbnail(idx);
  bool init = false; 
#endif

  cv::Mat im,draw; cvNamedWindow("test"); cv::VideoCapture camera;
  std::cout << "Usage: "<<argv[0] << " [video] [-o]" << std::endl
	    << "-o will output an XML file with point locations" << std::endl;

  std::cout << "d: Redetect" << std::endl
	    << "i: Initialise and start avatar " << std::endl
	    << "p: Next avatar" << std::endl
	    << "o: Previous avatar" << std::endl
	    << "ESC: Exit" << std::endl;

  if(argc == 1){
    camera.open(0);
    camera.set(CV_CAP_PROP_FRAME_WIDTH,640);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT,480);
  }

  if(argc > 1){
    if(!strcmp(argv[1], "-o")){
	saveData = true;
	char fname[128];
	time_t rawtime; time(&rawtime);
	struct tm * timeinfo = localtime(&rawtime); 
	strftime(fname, 128, "output_%Y_%m_%d_%H_%M_%S.xml", timeinfo);
	outputFile = fname;

	camera.open(0);
	camera.set(CV_CAP_PROP_FRAME_WIDTH,640);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    }
    else{
      camera.open(argv[1]);
      if(!camera.isOpened()){
	std::cerr << "Unable to open video file "<<argv[1]
		  << ", opening camera"<<std::endl;
	camera.open(0);
	camera.set(CV_CAP_PROP_FRAME_WIDTH,640);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT,480);
      }
      else
	outputFile = std::string(argv[1])+"_output.xml";
    }
  }
 
  if(argc >2){
    if(!strcmp(argv[2], "-o"))
      saveData = true;
  }
  
  
  if( !camera.isOpened()){
    std::cerr << "Unable to open camera"<<std::endl;	  
    return -1;	  
  }

  if(saveData){
    fs.open(outputFile, cv::FileStorage::WRITE);
    if(!fs.isOpened()){
      std::cerr << "I can't open file " << outputFile << std::endl;
      return -1;
    }
    cvWriteComment(fs.fs, "Points are a 132x1 matrix, with the coordinates\n"
		   "given as x0,x1,..., x65, y0, y1, ... y65\n",0);
    cvWriteComment(fs.fs, "Pose is a 6x1 vector with the weak perspective \n"
		   "similarity transformation parameters (scale, alpha, beta, gamma, tx, ty)",0);
    cvWriteComment(fs.fs, "A new frame_xxxxx structure is created per frame.\n"
		   "Frames where the tracker failed are represented with an empty\n"
		   "structure",0);
  }


  while(1){
    cv::Mat im; camera >> im; 
    if(im.empty()) break;
    
    cv::flip(im,im,1);
    //im = im.t();

    if(draw.rows != im.rows){
      int extend = 1;
#ifdef _WITH_AVATAR_
      extend = 2;
#endif
      draw.create(im.rows,im.cols*extend,CV_8UC3);
    }
    draw = cv::Scalar(0);
    int health = tracker->Track(im,p);    
    cv::Mat uimg = draw(cv::Rect(0,0,im.cols,im.rows));
    im.copyTo(uimg); draw_health(uimg,health); 
    if(health >= 0){
      draw_shape(uimg,tracker->_shape,con);
#ifdef _WITH_AVATAR_
      if(init){
	cv::Mat aimg = draw(cv::Rect(im.cols,0,im.cols,im.rows));
	avatar->Animate(aimg,im,tracker->_shape);
      }
#endif
    }

    
    if(saveData){
      char fnum[26];
      sprintf(fnum, "frame_%05d", frameCount);
      fs << std::string(fnum) << "{";
      if(health>=0){
	//save the 2D points to file
	fs << "points" << tracker->_shape;
	fs << "shape_params" << tracker->getShapeParameters();
	fs << "pose" << tracker->getPoseParameters();
      }
      fs << "}";
    }
    
    frameCount++;

#ifdef _WITH_AVATAR_
    cv::Mat timg = draw(cv::Rect(im.cols-thumb.cols,im.rows-thumb.rows,
				 thumb.cols,thumb.rows));
    thumb.copyTo(timg);
#endif

    tracker->_timer.write_fps(uimg); 
    cv::imshow("test",draw); 
    int c = cvWaitKey(waitTime);
    if(c == 27)break; 
    else if(c == int('d'))tracker->Reset();
    else if(c == int(' ')){
      if(waitTime == 0) waitTime = 5;
      else waitTime = 0;
    }
#ifdef _WITH_AVATAR_
    else if(c == int('i')){
      avatar->Initialise(im,tracker->_shape); init = true;
    }
    else if(c == int('p')){
      if(avatar->numberOfAvatars() > idx+1){
	idx++; avatar->setAvatar(idx); thumb = avatar->Thumbnail(idx);
      }
    }else if(c == int('o')){
      if(idx>0){idx--; avatar->setAvatar(idx); thumb = avatar->Thumbnail(idx);}
    }
#endif
  }
  
  if(saveData)
    fs.release();

  return 0;
}
//==============================================================================
