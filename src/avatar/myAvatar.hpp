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

#ifndef _AVATAR_myAvatar_h_
#define _AVATAR_myAvatar_h_
#include <avatar/Avatar.hpp>
#include <tracker/Warp.hpp>
#include <tracker/ShapeModel.hpp>
namespace AVATAR
{
  //============================================================================
  /**
     Kernel Ridge Regression
  */
  class KSmooth{
  public:
    double _sigma;           /**< RGB variance     */
    std::vector<cv::Mat> _X; /**< Training inputs  */
    std::vector<cv::Mat> _Y; /**< Training outputs */
    
    KSmooth(){;}
    KSmooth(const char* fname, bool binary = false){this->Load(fname, binary);}
    KSmooth& operator=(KSmooth const&rhs);
    void Load(const char* fname, bool binary = false){
      std::ifstream s(fname); assert(s.is_open()); 
      if(!binary)this->Read(s);
      else this->ReadBinary(s);
      s.close(); 
      return; 
    }
    void Save(const char* fname, bool binary = false){
      std::ofstream s(fname); assert(s.is_open()); this->Write(s, binary);s.close(); 
      return;
    }
    void Read(std::ifstream &s);
    void ReadBinary(std::ifstream &s, bool readType = true);
    void Write(std::ofstream &s, bool binary = false);
    
    void Train(std::vector<cv::Mat> &Y,std::vector<cv::Mat> &X,double sigma);
    cv::Mat Predict(cv::Mat &x);
    double Kernel(cv::Mat &x1,cv::Mat &x2,double sigma);
  };
  //============================================================================
  /**
     Expression generator
  */
  class ShapeExpMap{
  public:
    FACETRACKER::PDM3D _pdm;   /**< Shape model                    */
    std::vector<KSmooth> _reg; /**< Regressors for each expression */
    ShapeExpMap(){;}
    ShapeExpMap(const char* fname, bool binary = false){this->Load(fname, binary);}
    ShapeExpMap& operator=(ShapeExpMap const&rhs);
    void Load(const char* fname, bool binary = false){
      std::ifstream s(fname); assert(s.is_open()); 
      if(!binary)this->Read(s); 
      else this->ReadBinary(s);
      s.close(); 
      return; 
    }
    void Save(const char* fname, bool binary = false){
      std::ofstream s(fname); assert(s.is_open()); this->Write(s, binary);s.close(); 
      return;
    }
    void Read(std::ifstream &s);
    void ReadBinary(std::ifstream &s, bool readType = true);
    void Write(std::ofstream &s, bool binary = false);
    void Train(FACETRACKER::PDM3D &pdm,
	       std::vector<std::vector<cv::Mat> > express,
	       std::vector<std::vector<cv::Mat> > neutral,
	       double sigma);
    std::vector<cv::Mat> Generate(cv::Mat &s2D);
    cv::Mat CalcNormed3D(cv::Mat &s2D);
  protected:
    cv::Mat plocal_,pglobl_,shape_,R_,Ri_;
  };
  //============================================================================
  class myAvatar : public Avatar{
  public:
    struct pupil{         /**< Pupil info structure                   */
      double rad;                 /**< Pupil radius                           */
      double px;                  /**< x-coord in frontal gaze                */
      double py;                  /**< y-coord in frontal gaze                */
      cv::Mat idx;                /**< Eye indices in face shape              */
      cv::Mat tri;                /**< Eye triangulation                      */
      cv::Scalar scelera;         /**< Scelera color (RGB)                    */
      std::vector<cv::Mat> image; /**< Pupil image (RGB)                      */
    };
    int _idx;                     /**< avatar index                           */
    cv::Mat _user;                /**< user's neutral 3D shape                */
    cv::Mat _basis;               /**< appearance basis                       */
    FACETRACKER::PAW _warp;       /**< warping function                       */
    FACETRACKER::PDM3D _pdm;      /**< expression shape model                 */
    FACETRACKER::PDM3D _gpdm;     /**< generic shape model                    */
    FACETRACKER::PDM3D _updm;     /**< user PDM                               */
    std::vector<double>  _scale;  /**< lighting scale                         */
    std::vector<cv::Mat> _textr;  /**< vectorized neutral img for all avatar  */
    std::vector<cv::Mat> _images; /**< neutral expressions for all avatars    */
    std::vector<cv::Mat> _shapes; /**< neutral 3D shape for all avatars       */
    std::vector<cv::Mat> _reg;    /**< regressor between subspaces            */
    std::vector<cv::Mat> _expr;   /**< shapes for expressions of avatars      */
    std::vector<pupil> _lpupil;   /**< Left pupil                             */
    std::vector<pupil> _rpupil;   /**< Right pupil                            */
    pupil _base_lpupil;           /**< Base pupil to create new left pupils from.  This crap needs to go.*/
    pupil _base_rpupil;           /**< Base pupil to create new right pupils from. This crap needs to go to.*/
    cv::Point _lp0;               /**< User's frontal gaze left pupil loc     */
    cv::Point _rp0;               /**< User's frontal gaze right pupil loc    */
    cv::Mat _ocav_idx;            /**< Oral cavity point indices              */
    cv::Mat _ocav_tri;            /**< Oral cavity triangulation              */
    ShapeExpMap _gen;             /**< expression generator                   */

    myAvatar(){;}
    myAvatar(const char* fname, bool binary = false){this->Load(fname, binary);}

    int                         //-1 on failure, 0 otherwise
    Animate(cv::Mat &draw,      //rgb image to draw on
	    const cv::Mat_<cv::Vec<uint8_t,3> > &image,     //rgb image of user
	    const std::vector<cv::Point_<double> > &shape,     //user's facial shape
	    void* params=NULL); //additional parameters
    void                           //initialise avatar for a particular user
    Initialise(const cv::Mat_<cv::Vec<uint8_t,3> > &im,        //rgb image of user
	       const std::vector<cv::Point_<double> > &shape,     //shape describing user
	       void* params=NULL); //additional parameters
    void 
    Read(std::ifstream &s,      //file stream to read from
	 bool readType = true); //read type?
    void 
    ReadBinary(std::ifstream &s,      //file stream to read from
	       bool readType = true); //read type?
    void 
    Write(std::ofstream &s, bool binary = false);  //file stream to write to

    int numberOfAvatars() const;
    void setAvatar(int index);
    
    cv::Mat             //thumbnail image of avatar
    Thumbnail(int idx); //avatar index


  /**
       Add avatar to model
       @param image  Image containing avatar
       @param points Sparse annotations of avatar image
       @param eyes   Points describing eyes.
    */
    void AddAvatar(cv::Mat &image, 
		   cv::Mat &points, 
		   cv::Mat &eyes);

  private:
    cv::Mat plocal_,pglobl_,textr_,img_,gray_,p_,s3D_,epts_,grayImg_;
    cv::Mat gplocal_,gpglobl_,opts1_,opts2_; std::vector<cv::Mat> rgb_;
    double dx0_,dy0_;

    cv::Mat                           //animated avatar's shape
    AnimateShape(cv::Mat &shape,      //user's shape
		 int scale_eyes = 1); //eye scaling flag
    void 
    WarpTexture(cv::Mat &src_pts,              //src image points
		cv::Mat &dst_pts,              //destination image points
		std::vector<cv::Mat> &src_img, //channels of source image
		std::vector<cv::Mat> &dst_img, //channel of destination image
		cv::Mat &tri);                 //triangulation
    void 
    GetWidthHeight(cv::Mat &shape, //shape to bound
		   cv::Mat &idx,   //indices of shape points to bound
		   double &w,      //contains width on return
		   double &h);     //contains height on return
    void 
    GetIdxPts(cv::Mat &shape,  //original shape
	      cv::Mat &idx,    //indices in @shape
	      cv::Mat &ishape, //indexed shape on return
	      bool twoD);      //@shape is 2D?
    void 
    WarpBackPupils(cv::Point &pl, //left pupil coord in image, warped on return
		   cv::Point &pr, //right pupil coord in image, warped on return
		   cv::Mat &shape,//shape in image (2D)
		   cv::Mat &ref); //shape in reference (3D)
    cv::Point                         //warped pupil coord
    WarpBackLeftPupil(cv::Point &p,   //left pupil coord in image
		      cv::Mat &shape, //shape in image (2D)
		      cv::Mat &ref);  //shape in image (3D)
    cv::Point                         //warped pupil coord
    WarpBackRightPupil(cv::Point &p,   //right pupil coord in image
		       cv::Mat &shape, //shape in image (2D)
		       cv::Mat &ref);  //shape in image (3D)
    void 
    GetPupils(cv::Mat &im,   //get locations of left and right pupils
	      cv::Mat &shape, //shape describing face
	      cv::Point &pl,  //contains coords of left pupil on return
	      cv::Point &pr); //contains coords of right pupil on return
    cv::Point                //location of pupil in image
    GetPupil(cv::Mat &im,    //grayscaleimage containing face
	     cv::Mat &shape, //shape describing face (2D)
	     cv::Mat &idx,   //indices of eye points (left or right exclusive)
	     cv::Mat &tri);  //triangulation of eye
    cv::Rect                     //smallest rectangle containing all points
    GetBoundingBox(cv::Mat &pt); //point set to bound
    void 
    DrawPupil(double px,                   //x-coord of pupil in image
	      double py,                   //y-coord of pupil in image
	      double rad,                  //radius of pupil to draw
	      cv::Mat &pt,                 //shape describing face
	      std::vector<cv::Mat> &pupil, //pupil image (RGB)
	      std::vector<cv::Mat> &img,   //image to draw on (RGB)
	      cv::Scalar &scelera,         //scelera color
	      cv::Mat &idx,                //index of eye points in face
	      cv::Mat &tri);               //triangulation of eye
   /**
       Get avatar eyes
       @param pt       Avatar shape
       @param im       Avatar image (RGB)
       @param cxl      x-coord of center of left pupil
       @param cyl      y-coord of center of left pupil
       @param exl      x-coord of edge of left pupil
       @param eyl      y-coord of edge of left pupil
       @param cxr      x-coord of center of right pupil
       @param cyr      y-coord of center of right pupil
       @param exr      x-coord of edge of right pupil
       @param eyr      y-coord of edge of right pupil
       @param leye     Contains RGB image of left pupil on return
       @param reye     Contains RGB image of right pupil on return
       @param lscelera Scelera of left eye
       @param rscelera Scelera of right eye
       @param lrad     Radius of left pupil in avatar image
       @param rrad     Radius of right pupil in avatar image
       @parma d        Number of samples along radius for pupil image
       @param size     Size of pupil image
    */
    void GetEyes(cv::Mat &pt,cv::Mat &im,
		 double cxl,double cyl,double exl,double eyl,
		 double cxr,double cyr,double exr,double eyr,	     
		 cv::Mat &leye,cv::Mat &reye,
		 cv::Scalar &lscelera,cv::Scalar &rscelera,
		 double &lrad,double &rrad,
		 const int d,const int size);

  };
  //============================================================================
  class myAvatarParams{
  public:
    int type;           /**< Type of parameters          */
    bool animate_rigid; /**< Animate head pose motion?   */
    bool animate_exprs; /**< Animate facial expressions? */
    bool avatar_shape;  /**< User avatar's shape?        */
    bool oral_cavity;   /**< Do oral cavity replacement? */
    bool animate_textr; /**< Animate texture change?     */
    bool animate_eyes;  /**< Animate eyes?               */
    double alpha;       /**< Generic vs specific weight  */

    myAvatarParams();
    myAvatarParams(const char* fname, bool binary = false){this->Load(fname, binary);}
    void Save(const char* fname, bool binary = false);
    void Load(const char* fname, bool binary = false);
  };
  //============================================================================
}
#endif
