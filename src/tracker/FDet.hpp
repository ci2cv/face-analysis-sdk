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

#ifndef _TRACKER_FDet_h_
#define _TRACKER_FDet_h_
#include <tracker/IO.hpp>
namespace FACETRACKER
{
  //===========================================================================
  /** 
      A wrapper for OpenCV's face detector
  */
  class FDet{
  public:    
    int                      _min_neighbours; /**< see OpenCV documentation */
    int                      _min_size;       /**< ...                      */
    double                   _img_scale;      /**< ...                      */
    double                   _scale_factor;   /**< ...                      */
    CvHaarClassifierCascade* _cascade;        /**< ...                      */

    FDet(){storage_=NULL;_cascade=NULL;}
    FDet(const char* fname){this->Load(fname);}
    FDet(const char*  cascFile,
	 const double img_scale, //1.3
	 const double scale_factor = 1.1,
	 const int    min_neighbours = 2,
	 const int    min_size = 100){
      this->Init(cascFile,img_scale,scale_factor,min_neighbours,min_size);
    }
    ~FDet();
    FDet& operator=(FDet const&rhs);
    void Init(const char* fname,
	      const double img_scale = 1.3,
	      const double scale_factor = 1.1,
	      const int    min_neighbours = 2,
	      const int    min_size = 30);
    cv::Rect Detect(cv::Mat im);
    void Load(const char* fname, bool binary = false);
    void Save(const char* fname, bool binary = false);
    void Write(std::ofstream &s, bool binary = false
	       );
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s,bool readType = true);
    
  private:
    cv::Mat small_img_; CvMemStorage* storage_;
  };
  //===========================================================================
  /** Shape initializer */
  class SInit{
  public:
    FDet       _fdet;
    cv::Mat    _rshape;
    cv::Scalar _simil;
    SInit(){;}
    SInit(const char* fname){this->Load(fname);}
    void Load(const char* fname, bool binary = false);
    void Save(const char* fname, bool binary = false);
    void Write(std::ofstream &s, bool binary = false);
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s,bool readType = true);
    int InitShape(cv::Mat &im,cv::Mat &shape, cv::Rect r = cv::Rect(0,0,0,0));
    cv::Rect Detect(cv::Mat &im){return _fdet.Detect(im);}
    cv::Rect ReDetect(cv::Mat &im);
    cv::Rect Update(cv::Mat &im,cv::Mat &s,bool rsize);
  protected:
    cv::Mat temp_,ncc_,small_;
  };
  //===========================================================================
}
#endif
