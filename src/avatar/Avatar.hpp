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

#ifndef _AVATAR_Avatar_h_
#define _AVATAR_Avatar_h_

#include <avatar/IO.hpp>
namespace AVATAR
{

  typedef cv::Mat_<cv::Vec<uint8_t,3> > BGRImage;
  typedef std::vector<cv::Point_<double> > PointVector;

  //============================================================================
  /**
     Base class for Avatar
  */
  class Avatar{
  public:
    virtual ~Avatar();

    cv::Mat _shape; /**< Current avatar's shape */

    virtual int                   //-1 on failure, 0 otherwise
    Animate(cv::Mat &draw,        //rgb image to draw on
	    const cv::Mat_<cv::Vec<uint8_t,3> > &image,       //image of user
	    const std::vector<cv::Point_<double> > &shape,       //user's facial shape
	    void* params=NULL)=0; //additional parameters
    virtual void                    //initialise avatar for a particular user
    Initialise(const cv::Mat_<cv::Vec<uint8_t,3> > &im,         //image containing user
	       const std::vector<cv::Point_<double> > &shape,      //shape describing user
	       void* params=NULL)=0;//additional parameters
    virtual void
    Read(std::ifstream &s,        //file stream to read from
	 bool readType = true)=0;
    virtual void
    ReadBinary(std::ifstream &s,        //file stream to read from
	 bool readType = true)=0;
    virtual void 
    Write(std::ofstream &s, //file stream to write to
	  bool binary = false)=0;
    // virtual void 
    // Read(std::ifstream &s,        //file stream to read from
    // 	 bool readType = true)=0; //read type?

    // virtual void 
    // Write(std::ofstream &s)=0;    //file stream to write to

    virtual int numberOfAvatars() const=0;    // number of avatars
    virtual void setAvatar(int idx)=0; // setAvatar 

    virtual cv::Mat       //thumbnail image of avatar
    Thumbnail(int idx)=0; //avatar index

    void 
    Load(const char* fname, bool binary = false){ //file containing predictor model
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
    // void 
    // Load(const char* fname){ //file containing predictor model
    //   std::ifstream s(fname); assert(s.is_open()); this->Read(s); s.close(); 
    //   return;
    // }
    // void 
    // Save(const char* fname){ //file to save predictor model to
    //   std::ofstream s(fname); assert(s.is_open()); this->Write(s); s.close(); 
    //   return;
    // }
  };
  //============================================================================
  /** IO functions */
  Avatar* //contains pointer to avatar obj on return, null on failure
  LoadAvatar(const char* fname); //filename containing avatar model
  
  void*  //contains pointer to avatar param obj on return, null on failure
  LoadAvatarParams(const char* fname); //file containing avatar params
  //============================================================================

  std::string DefaultAvatarModelPathname();

  Avatar *LoadAvatar();
  
  // std::string DefaultAvatarParamsPathname();
  // void *LoadAvatarParams();
}
#endif
