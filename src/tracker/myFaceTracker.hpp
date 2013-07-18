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

#ifndef _TRACKER_myFaceTracker_h_
#define _TRACKER_myFaceTracker_h_
#include <tracker/IO.hpp>
#include <tracker/ATM.hpp>
#include <tracker/CLM.hpp>
#include <tracker/FDet.hpp>
#include <tracker/RegistrationCheck.hpp>
#include <tracker/FaceTracker.hpp>
#include <tracker/ShapePredictor.hpp>
namespace FACETRACKER
{
  //============================================================================
  class myFaceTracker : public FaceTracker{
  public:
    CLM _clm;                      /**< Constrained Local Model             */
    ATM _atm;                      /**< Active template model               */
    int64 _time;                   /**< Time since last re-initialisation   */
    SInit _sinit;                  /**< Face detector and shape initialiser */
    mvRegistrationCheck _fcheck;   /**< Failure checker                     */
    ShapePredictorList _spred;     /**< Refining shape predictors           */

    myFaceTracker(){_time=-1;}
    myFaceTracker(const char* fname, bool binary = false){this->Load(fname, binary);}
    myFaceTracker(const char* clmFile,     //CLM
		  const char* sInitFile,   //SInit
		  const char* FcheckFile,  //RegistrationCheck
		  //		  const char* praFile,     //praFacePredictor
		  const char* predFile,   //ShapePredictor
		  bool binary = false); // if the files are binary
    void Reset(); //reset tracker

    std::vector<cv::Point_<double> > getShape() const;
    std::vector<cv::Point3_<double> > get3DShape() const;
    Pose getPose() const;
    
    int                          //-1 on failure, 0 otherwise
    NewFrame(cv::Mat &im,        //grayscale image to track
	     FaceTrackerParams* params=NULL); //additinal parameters
    void 
    Read(std::ifstream &s,      //file stream to read from
	 bool readType = true); //read type?
    void 
    ReadBinary(std::ifstream &s,      //file stream to read from
	       bool readType = true); //read type?

    void 
    Write(std::ofstream &s,    //file stream to write to
	  bool binary = false);

    const cv::Mat mu(){return mu_;}
    const cv::Mat cov(){return cov_;}

    cv::Mat getShapeParameters(){return _clm._plocal.clone();}
    cv::Mat getPoseParameters(){return _clm._pglobl.clone();}
  protected:
    cv::Rect rect_; cv::Mat gray_,mu_,cov_,covi_,smooth_,dxdp_;
  };
  //============================================================================
  class myFaceTrackerParams : public FaceTrackerParams {
  public:
    int type;               /**< Type of object                           */
    int timeDet;            /**< Time between detections (seconds)        */
    int itol;               /**< Maximum number of iterations             */
    double ftol;            /**< Convergence tolerance                    */
    double clamp;           /**< Shape model clamping factor              */
    double init_lambda;     /**< Weight of pra model [0,1]                */
    double track_lambda;    /**< Weight of online model [0,1]             */
    double gamma;           /**< Weight of global vs local model [0,1]    */
    int init_type;          /**< 0=CLM only, 1=CLM+pra                    */
    int track_type;         /**< 0=CLM only, 1=CLM+atm, 2=CLM+atm+ksmooth */
    bool shape_predict;     /**< Use shape predictor for refinement?      */
    bool check_health;      /**< Check health of tracker                 */
    std::vector<int> init_wSize; /**< CLM search window sizes             */
    std::vector<int> track_wSize; /**< CLM search window sizes            */
    std::vector<cv::Mat> center; /**< Center view poses                   */
    std::vector<cv::Mat> visi;   /**< Patch visibility while tracking */

    cv::Mat atm_tri;        /**< Triangulation for ATM      */
    double atm_scale;       /**< Scale of ATM               */
    double atm_thresh;      /**< Threhold for ATM update    */
    int atm_ntemp;          /**< Number of templates in ATM */

    cv::Size ksmooth_size; /**< Size of ksmooth window         */
    double ksmooth_sigma;  /**< Variance of ksmooth kernel     */
    double ksmooth_noise;  /**< Noise variance of ksmooth      */
    double ksmooth_thresh; /**< Update threshold for ksmooth   */
    int ksmooth_ntemp;     /**< Number of templates in ksmooth */

    myFaceTrackerParams();
    myFaceTrackerParams(const char* fname, bool binary = false){this->Load(fname, binary);}
    void Save(const char* fname, bool binary = false);
    void Load(const char* fname, bool binary = false);
  };
  //============================================================================
}
#endif
