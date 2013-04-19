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

#ifndef _TRACKER_Patch_h_
#define _TRACKER_Patch_h_
#include <tracker/IO.hpp>
namespace FACETRACKER
{
  //===========================================================================
  /** 
      A Linear Patch Expert
  */
  class Patch{
  public:
    int     _t; /**< Type of patch (0=raw,1=grad,2=lbp) */
    double  _a; /**< scaling                            */
    double  _b; /**< bias                               */
    cv::Mat _W; /**< Gain                               */
    
    Patch(){;}
    Patch(const char* fname, bool binary = false){this->Load(fname, binary);}
    Patch(int t,double a,double b,cv::Mat &W){this->Init(t,a,b,W);}
    Patch& operator=(Patch const&rhs);
    inline int w(){return _W.cols;}
    inline int h(){return _W.rows;}
    void Load(const char* fname, bool binary = false);
    void Save(const char* fname, bool binary = false);
    void Write(std::ofstream &s, bool binary = false);
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s,bool readType = true);
    void Init(int t, double a, double b, cv::Mat &W);
    void Response(cv::Mat &im,cv::Mat &resp);    
    cv::Mat Response(){return res_.clone();}
  private:
    cv::Mat im_,res_;
  };
  //===========================================================================
  /**
     A Multi-patch Expert
  */
  class MPatch{
  public:
    int _w,_h;             /**< Width and height of patch */
    std::vector<Patch> _p; /**< List of patches           */
    
    MPatch(){;}
    MPatch(const char* fname, bool binary = false){this->Load(fname, binary);}
    MPatch(std::vector<Patch> &p){this->Init(p);}
    MPatch& operator=(MPatch const&rhs);
    inline int nPatch(){return _p.size();}
    void Load(const char* fname, bool binary = false);
    void Save(const char* fname, bool binary = false);
    void Write(std::ofstream &s, bool binary = false);
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s,bool readType = true);
    void Init(std::vector<Patch> &p);
    void Response(cv::Mat &im,cv::Mat &resp);    
  private:
    cv::Mat res_;
  };
  //===========================================================================
}
#endif
