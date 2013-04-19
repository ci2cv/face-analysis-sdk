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

#ifndef _TRACKER_ShapePredictor_h_
#define _TRACKER_ShapePredictor_h_
#include <tracker/ShapeModel.hpp>
#include <tracker/Warp.hpp>
namespace FACETRACKER
{  
  //===========================================================================
  class ShapePredictor{
  public:
    int _K;
    cv::Mat _idx;
    cv::Rect _rect;
    FACETRACKER::PDM2D _pdm;
    FACETRACKER::PAW _warp;
    std::vector<cv::Mat> _C,_R;
  
    ShapePredictor(){;}
    ShapePredictor(const char* fname, bool binary = false){this->Load(fname, binary);}
    ShapePredictor& operator=(ShapePredictor const&rhs);
    void Load(const char* fname, bool binary = false);
    void Save(const char* fname, bool binary = false);
    void Read(std::ifstream &s);
    void ReadBinary(std::ifstream &se, bool readType = true);
    void Write(std::ofstream &s, bool binary = false);
    cv::Mat Predict(cv::Mat &shape,cv::Mat &im);
  protected:
    cv::Mat crop_,x_,z_,y_,plocal_,pglobl_;
    int FindCluster(cv::Mat &shape);
  };
  //===========================================================================
  class ShapePredictorList{
  public:
    std::vector<ShapePredictor> _pred;
    ShapePredictorList(){;}
    ShapePredictorList(const char* fname, bool binary = false){this->Load(fname, binary);}
    void Load(const char* fname, bool binary = false);
    void Save(const char* fname, bool binary = false);
    void Read(std::ifstream &s);
    void ReadBinary(std::ifstream &s, bool readType = true);
    void Write(std::ofstream &s, bool binary = false);
    void Predict(cv::Mat &shape,cv::Mat &im);
  };
  //===========================================================================
}
#endif
