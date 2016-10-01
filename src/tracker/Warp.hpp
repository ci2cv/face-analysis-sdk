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

#ifndef _TRACKER_Warp_h_
#define _TRACKER_Warp_h_
#include <tracker/IO.hpp>
namespace FACETRACKER
{
  //===========================================================================
  /** Related Functions */
  double pythag(double a, double b);
  bool sameSide(double x0, double y0, double x1, double y1,
		double x2, double y2, double x3, double y3);
  int isWithinTri(double x,double y,cv::Mat &tri,cv::Mat &shape);
  double bilinInterp(cv::Mat& I,const double x,const double y);
  double triFacing(const double sx0,const double sy0,
		   const double sx1,const double sy1,
		   const double sx2,const double sy2,
		   const double dx0,const double dy0,
		   const double dx1,const double dy1,
		   const double dx2,const double dy2);
  //===========================================================================
  /** 
      A pure virtual class for a shape based warping function
  */
  class Warp{
  public:
    int     _type;   /**< Type of warp                       */
    int     _w;      /**< Region width                       */
    int     _h;      /**< Region height                      */
    cv::Mat _src;    /**< Source points                      */
    cv::Mat _dst;    /**< destination points                 */
    cv::Mat _mapx;   /**< x-destination of warped points     */
    cv::Mat _mapy;   /**< y-destination of warped points     */

    inline int nPoints(){return _src.rows/2;}
    inline int Width(){return _w;}
    inline int Height(){return _h;}
    void Load(const char* fname, bool binary = false){
      std::ifstream s;
      if(!binary){s.open(fname); assert(s.is_open()); this->Read(s);}
      else{ s.open(fname, std::ios::binary); assert(s.is_open()); this->ReadBinary(s);}
      s.close(); 
      return;
    }
    void Save(const char* fname, bool binary = false){
      std::ofstream s(fname); assert(s.is_open()); this->Write(s, binary);s.close(); 
      return;
    }
    virtual int nPix(){return _w*_h;}
    virtual void SetDst(cv::Mat &dst){
      int n = _src.rows/2; 
      assert((dst.rows == 2*n) && (dst.cols == 1) && (dst.type() == CV_64F)); 
      _dst = dst.clone(); return;
    }
    virtual void Write(std::ofstream &s, bool binary = false) = 0;
    virtual void Read(std::ifstream &s,bool readType = true) = 0;
    virtual void ReadBinary(std::ifstream &s,bool readType = true) = 0;
    virtual void CalcCoeff() = 0;
    virtual void WarpPoint(double xi,double yi, double &xo, double &yo) = 0;
    virtual void WarpRegion(cv::Mat &mapx,cv::Mat &mapy) = 0;
    virtual void Crop(cv::Mat &src_im,cv::Mat &dst_im,cv::Mat &dst) = 0;
    virtual cv::Mat dWdx() = 0;
  };
  //===========================================================================
  /** 
      A Piecewise Affine Warp
  */
  class PAW : public Warp{
  public:    
    int     _nPix;   /**< Number of pixels                   */
    double  _xmin;   /**< Minimum x-coord for src            */
    double  _ymin;   /**< Minimum y-coord for src            */
    cv::Mat _tri;    /**< Triangulation                      */
    cv::Mat _tridx;  /**< Triangle for each valid pixel      */
    cv::Mat _mask;   /**< Valid region mask                  */
    cv::Mat _coeff;  /**< affine coeffs for all triangles    */
    cv::Mat _alpha;  /**< matrix of (c,x,y) coeffs for alpha */
    cv::Mat _beta;   /**< matrix of (c,x,y) coeffs for alpha */
    cv::Mat _mapx;   /**< x-destination of warped points     */
    cv::Mat _mapy;   /**< y-destination of warped points     */

    PAW(){_type = IO::PAW;}
    PAW(const char* fname, bool binary = false){_type = IO::PAW; this->Load(fname, binary);}
    PAW(cv::Mat &src,cv::Mat &tri){_type = IO::PAW; this->Init(src,tri);}
    PAW(cv::Mat &src,cv::Mat &tri,cv::Mat &mask){
      _type = IO::PAW; this->Init(src,tri,mask);
    }
    PAW& operator=(PAW const&rhs);
    inline int nTri(){return _tri.rows;}

    int nPix(){return _nPix;}
    void Write(std::ofstream &s, bool binary = false);
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s,bool readType = true);
   void CalcCoeff();
    void WarpPoint(double xi,double yi, double &xo, double &yo);
    void WarpPoint(double xi, double yi, double &xo, double &yo, int t);
    void WarpRegion(cv::Mat &mapx,cv::Mat &mapy);
    int Warp(cv::Mat &si,cv::Mat &so,cv::Mat *idx = NULL);
    int WarpPointCheck(double xi, double yi, double &xo, double &yo, int t=-1);
    int InverseWarpPoint(double xi,double yi, double &xo, double &yo,int t=-1);
    int InverseWarp(cv::Mat &si,cv::Mat &so,cv::Mat *idx=NULL);
    void Crop(cv::Mat &src_im,cv::Mat &dst_im,cv::Mat &dst);
    void Vectorize(cv::Mat &img,cv::Mat &vec);
    void VectorizeUchar(cv::Mat &img,cv::Mat &vec);
    void UnVectorize(cv::Mat &vec,cv::Mat &img,const int bck = -1);
    cv::Mat dWdx();

    void Init(cv::Mat &src,cv::Mat &tri);
    void Init(cv::Mat &src,cv::Mat &tri,cv::Mat &mask); 
    void Draw(cv::Mat &src,cv::Mat &dst,cv::Mat &s);
    void DrawRGB(std::vector<cv::Mat> &src,
		 std::vector<cv::Mat> &dst,cv::Mat &s);
    cv::Mat PixTri();
    cv::Mat FindVTri();
    cv::Mat dWdx(cv::Mat &pixTri);
  };
  //===========================================================================
}
#endif
