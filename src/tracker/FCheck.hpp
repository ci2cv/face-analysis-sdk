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

#ifndef _TRACKER_FCheck_h_
#define _TRACKER_FCheck_h_
#include <tracker/Warp.hpp>
#include <vector>
namespace FACETRACKER
{
  //===========================================================================
  /** 
      Checks for Tracking Failure
  */
  class FCheck{
  public:    
    PAW     _paw; /**< Piecewise affine warp */
    double  _b;   /**< SVM bias              */
    cv::Mat _w;   /**< SVM gain              */

    FCheck(){;}
    FCheck(const char* fname){this->Load(fname);}
    FCheck(double b, cv::Mat &w, PAW &paw){this->Init(b,w,paw);}
    FCheck& operator=(FCheck const&rhs);
    void Init(double b, cv::Mat &w, PAW &paw);
    void Load(const char* fname, bool binary = false);
    void Save(const char* fname, bool binary = false);
    void Write(std::ofstream &s, bool binary = false);
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s,bool readType = true);
    bool Check(cv::Mat &im,cv::Mat &s);
    
  private:
    cv::Mat crop_,vec_;
  };
  //===========================================================================
  /** 
      Checks for Multiview Tracking Failure
  */
  class MFCheck{
  public:    
    std::vector<FCheck> _fcheck; /**< FCheck for each view */
    
    MFCheck(){;}
    MFCheck(const char* fname){this->Load(fname);}
    MFCheck(std::vector<FCheck> &fcheck){this->Init(fcheck);}
    MFCheck& operator=(MFCheck const&rhs){      
      this->_fcheck = rhs._fcheck; return *this;
    }
    void Init(std::vector<FCheck> &fcheck){_fcheck = fcheck;}
    void Load(const char* fname, bool binary = false);
    void Save(const char* fname, bool binary = false);
    void Write(std::ofstream &s, bool binary = false);
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s,bool readType = true);
    bool Check(int idx,cv::Mat &im,cv::Mat &s);
  };
  //===========================================================================
}
#endif
