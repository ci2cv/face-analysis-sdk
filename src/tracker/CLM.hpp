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

#ifndef _TRACKER_CLM_h_
#define _TRACKER_CLM_h_
#include <tracker/ShapeModel.hpp>
#include <tracker/Patch.hpp>
#include <tracker/Detector.hpp>
#include <vector>
namespace FACETRACKER
{
  //===========================================================================
  /** Related Functions */
  void CalcSimT(cv::Mat &src,cv::Mat &dst,
		double &a,double &b,double &tx,double &ty);
  void invSimT(double a1,double b1,double tx1,double ty1,
	       double& a2,double& b2,double& tx2,double& ty2);
  void SimT(cv::Mat &s,double a,double b,double tx,double ty);
  //===========================================================================
  /** 
      A Constrained Local Model
  */
  class CLM{
  public:
    PDM3D                             _pdm;   /**< 3D Shape model           */
    cv::Mat                           _plocal;/**< local parameters         */
    cv::Mat                           _pglobl;/**< global parameters        */
    cv::Mat                           _refs;  /**< Reference shape          */
    std::vector<cv::Mat>              _cent;  /**< Centers/view (Euler)     */
    std::vector<cv::Mat>              _visi;  /**< Visibility for each view */
    std::vector<std::vector<MPatch> > _patch; /**< Patches/point/view       */

    CLM(){;}
    CLM(const char* fname){this->Load(fname);}
    CLM(PDM3D &s,cv::Mat &r, std::vector<cv::Mat> &c,
	std::vector<cv::Mat> &v,std::vector<std::vector<MPatch> > &p){
      this->Init(s,r,c,v,p);
    }
    CLM& operator=(CLM const&rhs);
    inline int nViews(){return _patch.size();}
    int GetViewIdx();
    void Load(const char* fname, bool binary = false);
    void Save(const char* fname, bool binary = false);
    void Write(std::ofstream &s, bool binary = false);
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s, bool readType = true);
    void Init(PDM3D &s,cv::Mat &r, std::vector<cv::Mat> &c,
	      std::vector<cv::Mat> &v,std::vector<std::vector<MPatch> > &p);
    void Fit(cv::Mat& im, std::vector<int> &wSize,
	     int nIter = 10,double clamp = 3.0,double fTol = 0.0);
    void Fit(cv::Mat& im, cv::Mat &mu,cv::Mat &cov,
	     std::vector<int> &wSize,
	     int nIter=10,double clamp=3,double fTol=0,double lambda=1);
    void FitFwdAdd(cv::Mat& im, std::vector<int> &wSize,
		   int nIter=10,double clamp=3,double fTol=0);
    void FitPrior(cv::Mat& im, std::vector<int> &wSize,
		  int nIter,double clamp,double fTol,double lambda,
		  void (*pfunc)(cv::Mat &im,cv::Mat &s, cv::Mat &dxdp,
				cv::Mat &H,cv::Mat &g,void* data),
		  void* data);
  private:
    cv::Mat cshape_,bshape_,oshape_,ms_,u_,g_,J_,H_; 
    std::vector<cv::Mat> prob_,pmem_,wmem_;
    void Optimize(int idx,int wSize,int nIter,
		  double fTol,double clamp,bool rigid);
    void Optimize(int idx,cv::Mat &mu,cv::Mat &covi,int wSize,int nIter,
		  double fTol,double clamp,double lambda);
    void OptimizeFwdAdd(std::vector<cv::Mat> &xloc,
			std::vector<cv::Mat> &yloc,
			int idx,int wSize,int nIter,
			double fTol,double clamp,bool rigid);
    void OptimizePrior(std::vector<cv::Mat> &xloc,
		       std::vector<cv::Mat> &yloc,
		       int idx,int wSize,int nIter,
		       double fTol,double clamp,bool rigid,
		       double lambda,cv::Mat &im,
		       void (*pfunc)(cv::Mat &im,cv::Mat &s, cv::Mat &dxdp,
				     cv::Mat &H,cv::Mat &g,void* data),
		       void* data);
   std::vector<DetectorNCC> _detectorsNCC;
    int _kWidth;
  };
  //===========================================================================
}
#endif
