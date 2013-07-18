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

#ifndef _TRACKER_FaceTracker_h_
#define _TRACKER_FaceTracker_h_
#include <tracker/IO.hpp>
namespace FACETRACKER
{
  //============================================================================
  /**
     Frames-per-second timer
  */
  class fpsTimer{
  public:
    int64 _t0,_t1,_cummul;  /**< Start and end time        */
    double _fps;    /**< Current frames per second */
    int _fnum;      /**< Frame number since _t0    */

    fpsTimer(){this->reset();}

    void
    start_frame(){
      _fnum++;
      _t0 = cvGetTickCount();
    }
    void 
    stop_frame(){
      _t1 = cvGetTickCount();
      _cummul += _t1-_t0;
      if(_fnum >= 29){
	_fps = 30.0/((double(_cummul)/cvGetTickFrequency())/1e+6); 
	_fnum = 0; _cummul = 0;
      }
    }
    void 
    reset(){_t0 = cvGetTickCount();_cummul=0; _fps=0; _fnum = 0;}

    void 
    write_fps(cv::Mat &im){ //image to draw FPS on top left corner
      char str[256];
      sprintf(str,"Tracker: %d fps",(int)round(_fps)); std::string text = str;
      cv::putText(im,text,cv::Point(10,im.rows-20),
		  CV_FONT_HERSHEY_SIMPLEX,0.5,CV_RGB(255,255,255)); return;
    }
  };

  class FaceTrackerParams {
  public:
    virtual ~FaceTrackerParams();
  };

  typedef std::vector<cv::Point_<double> > PointVector;

  struct Pose {
    double pitch;
    double yaw;  
    double roll;    
  };

  // Compute the vectors for each axis i.e. x-axis, y-axis then
  // z-axis. Alternatively the pitch, yaw, roll
  // 
  cv::Mat_<double> pose_axes(const Pose &pose);

  //============================================================================
  /**
     Base class for a face tracker
  */
  class FaceTracker{
  public:
    virtual ~FaceTracker();

    cv::Mat _shape;  /**< Current tracked shape */
    fpsTimer _timer; /**< Frames/second timer   */

    inline double               //frames-per-second
    fps(){return _timer._fps;}

    inline int                  //-1 on failure, health (0-10) otherwise
    Track(cv::Mat &im,          //grayscale image to track
	  FaceTrackerParams* params=NULL){   //additinal parameters
      _timer.start_frame(); 
      int r = this->NewFrame(im,params);
      _timer.stop_frame();
      return r;
    }
    void 
    Load(const char* fname, bool binary){ //file containing predictor model
      std::ifstream s;
      if(!binary){s.open(fname); assert(s.is_open()); this->Read(s);}
      else{s.open(fname, std::ios::binary);  assert(s.is_open()); this->ReadBinary(s);}
      s.close(); 
      return;
    }
    void 
    Save(const char* fname, bool binary = false){ //file to save predictor model to
      std::ofstream s;
      if(!binary) s.open(fname);
      else s.open(fname, std::ios::binary);

      assert(s.is_open()); this->Write(s, binary); s.close(); 
      return;
    }
    virtual void Reset()=0; //reset tracker
    enum {
      TRACKER_FAILED = -1,        // Failed to track the face.
      TRACKER_FACE_OUT_OF_FRAME = -2    // Face is outside the dimensions of the image.
    };

    virtual std::vector<cv::Point_<double> > getShape() const = 0;
    virtual std::vector<cv::Point3_<double> > get3DShape() const = 0;
    virtual Pose getPose() const = 0;

    // TODO Investigate why the params argument is needed at all. Why
    // can't it be const too? 
    
    virtual int                    // health TRACKER_FAILURE, TRACKER_OUT_OF_FRAME or 0-10 (tracking quality)
    NewFrame(cv::Mat &im,          //grayscale image to track
	     FaceTrackerParams *params=NULL)=0; //additinal parameters 
    virtual void 
    Read(std::ifstream &s,        //file stream to read from
	 bool readType = true)=0; //read type?
    virtual void
    ReadBinary(std::ifstream &s,        //file stream to read from
	 bool readType = true)=0;
    virtual void 
    Write(std::ofstream &s, //file stream to write to
	  bool binary = false)=0;

    //functions to get the clm parameters
    //returns _clm._plocal, the parameters of the current shape in the shape basis
    virtual cv::Mat
    getShapeParameters()=0;
    //returns _clm._pglobl, the (weak perspective) pose in 3D
    virtual cv::Mat
    getPoseParameters()=0; 
  };
  //============================================================================
  /** IO functions */
  FaceTracker* //contains pointer to facetracker obj on return, null on failure
  LoadFaceTracker(const char* fname); //filename containing facetracker model

  // TODO The FaceTracker and FaceTrackerParams interface really needs
  // to be improved/removed. Why is there no FaceTracker argument? The
  // API reads like the parameters are not associated with the type of
  // face tracker which isn't the case.

  FaceTrackerParams *  //contains pointer to facetracker param obj on return, null on failure
  LoadFaceTrackerParams(const char* fname); //file containing facetracker params
  //============================================================================

  std::string DefaultFaceTrackerModelPathname();
  std::string DefaultFaceTrackerParamsPathname();

  FaceTracker * LoadFaceTracker(); // load the default face tracker model.
  FaceTrackerParams *LoadFaceTrackerParams();   // load the default face tracker parameters.
  
}
#endif
