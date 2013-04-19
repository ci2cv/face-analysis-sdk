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

#ifndef _TRACKER_RegistrationCheck_h_
#define _TRACKER_RegistrationCheck_h_
#include <tracker/Warp.hpp>
#include <vector>
namespace FACETRACKER
{
  //===========================================================================
  /**
     Check registration quality
  */
  class RegistrationCheck{
  public:    
    PAW     _paw; /**< Piecewise affine warp */
    double  _b;   /**< SVM bias              */
    double  _a;   /**< Probability gain      */
    double  _c;   /**< Probability bias      */
    cv::Mat _w;   /**< SVM gain              */
    
    RegistrationCheck(){;}
    RegistrationCheck(const char* fname, bool binary = false){this->Load(fname, binary);}
    RegistrationCheck(double a,   //probability gain
		      double b,   //svm bias
		      double c,   //probability bias
		      cv::Mat &w, //svm gain
		      PAW &paw){  //warping function
      this->Init(a,b,c,w,paw);
    }
    RegistrationCheck& operator=(RegistrationCheck const&rhs){
      this->_a = rhs._a; this->_b = rhs._b; this->_c = rhs._c;
      this->_w = rhs._w.clone(); this->_paw = rhs._paw; return *this;
    }
    void 
    Init(double a,   //probability gain
	 double b,   //svm bias
	 double c,   //probability bias
	 cv::Mat &w, //svm gain
	 PAW &paw){  //warping function
      _a = a; _b = b; _c = c; _paw = paw; _w = w.clone();
    }
    void 
    Load(const char* fname, bool binary = false){
      std::ifstream s(fname); assert(s.is_open()); 
      if(!binary) this->Read(s); 
      else this->ReadBinary(s);

      s.close(); 
      return;
    }
    void 
    Save(const char* fname, bool binary = false){
      std::ofstream s(fname); assert(s.is_open()); this->Write(s, binary);s.close(); 
      return;
    }
    void Write(std::ofstream &s, bool binary = false);
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s,bool readType = true);
    int                //-1 on failure, [0:10] health otherwise
    Check(cv::Mat &im, //image
	  cv::Mat &s); //shape
  private:
    cv::Mat crop_,vec_,x_;
  };
  //===========================================================================
  /**
     Multiview registration quality check
  */
  class mvRegistrationCheck{
  public:
    std::vector<RegistrationCheck> _rego; //per-view checkers
    
    mvRegistrationCheck(){;}
    mvRegistrationCheck(const char* fname, bool binary){this->Load(fname, binary);}
    mvRegistrationCheck(std::vector<RegistrationCheck> &rego){this->Init(rego);}
    mvRegistrationCheck& operator=(mvRegistrationCheck const&rhs){      
      this->_rego = rhs._rego; return *this;
    }
    void Init(std::vector<RegistrationCheck> &rego){_rego = rego;}

    void 
    Load(const char* fname, bool binary = false){
      std::ifstream s;
      if(!binary){s.open(fname); assert(s.is_open()); this->Read(s);}
      else { s.open(fname, std::ios::binary); assert(s.is_open()); this->ReadBinary(s);}
      s.close(); 
      return;
    }
    void 
    Save(const char* fname){
      std::ofstream s(fname); assert(s.is_open()); this->Write(s);s.close(); 
      return;
    }
    void Write(std::ofstream &s, bool binary = false);
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s,bool readType = true);
    int                //-1 on failure, [0:10] health otherwise
    Check(cv::Mat &im, //image
	  cv::Mat &s,  //shape
	  int idx);    //view index
  };
}
//==============================================================================
#endif
