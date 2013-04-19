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

#ifndef _TRACKER_ATM_h_
#define _TRACKER_ATM_h_
#include <tracker/ShapeModel.hpp>
#include <tracker/Warp.hpp>
#include <vector>
namespace FACETRACKER
{
  //===========================================================================
  /**
     Multiview active template model
  */
  class ATM{
  public:
    bool _init;                            /**< Has it been initialised? */
    PAW _warp;                             /**< Warping function         */
    double _scale;                         /**< Scale of reference frame */
    std::vector<cv::Mat> _center;          /**< Centers of each pose     */
    std::vector<std::vector<cv::Mat> > _T; /**< Templates                */

    ATM(){_init = false;}
    ATM(std::vector<cv::Mat> &center, //centers of each pose
	cv::Mat &pose,                //pose of first shape
	cv::Mat &shape,               //first shape
	cv::Mat &im,                  //first image
	cv::Mat &tri,                 //triangulation
	const double scale=1){        //scale of reference frame
      this->Init(center,pose,shape,im,tri,scale);
    }
    void 
    Init(std::vector<cv::Mat> &center, //centers of each pose
	 cv::Mat &pose,                //pose of first shape
	 cv::Mat &shape,               //first shape
	 cv::Mat &im,                  //first image
	 cv::Mat &tri,                 //triangulation
	 const double scale=1);        //scale of reference frame

    void 
    AllocMemory();

    int                            //0=not updated,1=updated
    Update(cv::Mat &im,            //image containing object
	   cv::Mat &s,             //shape desribing object
	   cv::Mat &dxdp,          //jacobian of shape model at shape
	   cv::Mat &pose,          //pose of object
	   const int tmax=10,      //maximum templates/view
	   const double tol=100);  //minimum difference for update

    void 
    UpdateTemplate(cv::Mat &im,       //image containing object
		   cv::Mat &s,        //shape describing object
		   cv::Mat &pose,     //pose of object
		   const int tmax,    //maximum number of templates
		   const double tol); //minimum difference for update
    
    void 
    CalcJacob(cv::Mat &im,   //image containing object
	      cv::Mat &s,    //shape describing object
	      cv::Mat &dxdp, //jacobian of shape model at shape
	      cv::Mat &J,    //contains jacobian of warp on return
	      cv::Mat &vec); //contains vectorised cropped image on return

    void 
    BuildLinearSystem(cv::Mat &im,    //image containing object
		      cv::Mat &s,     //shape describing object
		      cv::Mat &dxdp,  //shape jacobian at shape
		      cv::Mat &pose,  //object's pose
		      cv::Mat &H,     //contains Hessian on return
		      cv::Mat &g);    //contains gradient on return
    
    bool 
    CheckUpdate(cv::Mat &im,   //image containing object
		cv::Mat &s,    //shape describing object
		cv::Mat &pose, //object pose
		double tol);   //minimum difference for update

  protected:
    cv::Mat imx__,imy__,vec__,vecx__,vecy__,crop__,cropx__,cropy__;
    cv::Mat pixtri__,dWdx__,J__,vecw__;
    std::vector<cv::Mat> Ja__,Ha__;
    std::vector<int> na__;
  };
  //===========================================================================
}
#endif
