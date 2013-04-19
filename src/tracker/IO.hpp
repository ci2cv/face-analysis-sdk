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

#ifndef _TRACKER_IO_h_
#define _TRACKER_IO_h_
#include <opencv/cv.h>
#include <fstream>
#include <vector>
namespace FACETRACKER
{
  //===========================================================================
  /** 
      Input-output Operations
  */
  class IO{
  public:
    enum{PDM3D = 0,PAW,PATCH,MPATCH,CLM,FDET,FCHECK,MFCHECK,TRACKER,TPS,AAM_POIC,PDM2D,AAM_POIC_F,SINIT,LINPATCH,CLP,CLD,TRACKER4,NONLINPATCH,MIXPATCH,CLM3,CLMix,PRA,ATM_PO,LBPHISTPATCH,FACEPREDICTORPRA,FACEPREDICTORKSMOOTH,MYFACETRACKER,MYFACETRACKERPARAMS,REGOCHECK,MVREGOCHECK,SHAPEPREDICTORLIST, SHAPEPREDICTOR, SHAPEEXPMAP, 
	 DUMMY_LAST_DONT_USE};
    static std::vector<std::string> GetList(const char* fname);
    static void ReadMat(std::ifstream& s,cv::Mat &M);
    static void WriteMat(std::ofstream& s,cv::Mat &M);
    static cv::Mat LoadCon(const char* fname);
    static cv::Mat LoadTri(const char* fname);
    static cv::Mat LoadVis(const char* fname);
    static cv::Mat LoadPts(const char* fname);
    static cv::Mat LoadPts3D(const char* fname);
    static cv::Mat LoadVisi(const char* fname);
    static void SavePts(const char* fname,cv::Mat &pts);
    static void SavePts(const char* fname, std::vector<cv::Point_<double> > &pts);
    static void SavePts3D(const char* fname,cv::Mat &pts);
    static std::vector<cv::Mat> LoadMatList(const char* fname);
  };
  //===========================================================================

	
  class IOBinary : public IO{
  public:
    //    enum{IO_BINARY_DATA = 0xFFFF0000};
    enum{PDM3D= IO::DUMMY_LAST_DONT_USE+1,PAW,PATCH,MPATCH,CLM,FDET,FCHECK,MFCHECK,TRACKER,TPS,AAM_POIC,PDM2D,AAM_POIC_F,SINIT,LINPATCH,CLP,CLD,TRACKER4,NONLINPATCH,MIXPATCH,CLM3,CLMix,PRA,ATM_PO,LBPHISTPATCH,FACEPREDICTORPRA,FACEPREDICTORKSMOOTH,MYFACETRACKER,MYFACETRACKERPARAMS,REGOCHECK,MVREGOCHECK, SHAPEPREDICTORLIST, SHAPEPREDICTOR, SHAPEEXPMAP, KSMOOTH,
	 DETECTOR_NCC, DETECTOR_HOG, DETECTOR_ASM, HOG_DET};
    static void ReadMat(std::ifstream& s,cv::Mat &M);
    static void WriteMat(std::ofstream& s,cv::Mat &M);
    //		static cv::Mat LoadCon(const char* fname);
    //		static cv::Mat LoadTri(const char* fname);
    //		static cv::Mat LoadVis(const char* fname);
    //		static cv::Mat LoadPts(const char* fname);
    //		static cv::Mat LoadPts3D(const char* fname);
    //		static cv::Mat LoadVisi(const char* fname);
    //		static void SavePts(const char* fname,cv::Mat &pts);
    //		static void SavePts3D(const char* fname,cv::Mat &pts);
    static std::vector<cv::Mat> LoadMatList(const char* fname);
		
		
  };

}
#endif
