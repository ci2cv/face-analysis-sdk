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

#include <tracker/Warp.hpp>
#define it at<int>
#define db at<double>
#define SQR(x) ((x) * (x))
using namespace FACETRACKER;
using namespace std;
//=============================================================================
double FACETRACKER::pythag(double a, double b) { 
  double absa=std::fabs(a),absb=std::fabs(b); 
  if(absa > absb)return absa * std::sqrt(1.0+SQR(absb/absa)); 
  else if (absb == 0.0)return 0.0;
  else return absb * std::sqrt(1.0+SQR(absa/absb));
}
//=============================================================================
bool FACETRACKER::sameSide(double x0, double y0, double x1, double y1,
			   double x2, double y2, double x3, double y3)
{
  double x = (x3-x2)*(y0-y2) - (x0-x2)*(y3-y2);
  double y = (x3-x2)*(y1-y2) - (x1-x2)*(y3-y2);
  if(x*y >= 0)return true; else return false;
}
//=============================================================================
int FACETRACKER::isWithinTri(double x,double y,cv::Mat &tri,cv::Mat &shape)
{
  int i,j,k,t,n = tri.rows,p = shape.rows/2; double s11,s12,s21,s22,s31,s32;
  for(t = 0; t < n; t++){
    i = tri.it(t,0); j = tri.it(t,1); k = tri.it(t,2);
    s11 = shape.db(i  ,0); s21 = shape.db(j  ,0); s31 = shape.db(k  ,0);
    s12 = shape.db(i+p,0); s22 = shape.db(j+p,0); s32 = shape.db(k+p,0);
    if(sameSide(x,y,s11,s12,s21,s22,s31,s32) &&
       sameSide(x,y,s21,s22,s11,s12,s31,s32) &&
       sameSide(x,y,s31,s32,s11,s12,s21,s22))return t;
  }return -1;
}
//===========================================================================
double FACETRACKER::bilinInterp(cv::Mat& I,const double x,const double y)
{
  assert(I.type() == CV_8U);
  if(x < 0 || x >= I.cols || y < 0 || y >= I.rows)return 0;
  int x1 = (int)std::floor(x),y1 = (int)std::floor(y);
  int x2 = (int)std::ceil(x) ,y2 = (int)std::ceil(y);
  if(x1 == x2){
    if(y1 == y2)return (double)I.at<uchar>(y1,x1);
    else return (double(y2)-y)*I.at<uchar>(y1,x1) + 
	   (y-double(y1))*I.at<uchar>(y2,x1);
  }else{
    if(y1 == y2)return (double(x2)-x)*I.at<uchar>(y1,x1) + 
		  (x-double(x1))*I.at<uchar>(y1,x2);
    else{
      double r1 = (double(x2)-x)*I.at<uchar>(y1,x1) + 
	(x-double(x1))*I.at<uchar>(y1,x2);
      double r2 = (double(x2)-x)*I.at<uchar>(y2,x1) + 
	(x-double(x1))*I.at<uchar>(y2,x2);
      return (double(y2)-y)*r1 + (y-double(y1))*r2;
    }
  }
}
//===========================================================================
double FACETRACKER::triFacing(const double sx0,const double sy0,
			      const double sx1,const double sy1,
			      const double sx2,const double sy2,
			      const double dx0,const double dy0,
			      const double dx1,const double dy1,
			      const double dx2,const double dy2)
{
  return ((dx1 - dx0)*(dy0 - dy2)+(dy1 - dy0)*(dx2 - dx0))/
    ((sx1 - sx0)*(sy0 - sy2)+(sy1 - sy0)*(sx2 - sx0));
}
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
PAW& PAW::operator= (PAW const& rhs)
{   
  this->_w    = rhs._w;
  this->_h    = rhs._h;
  this->_nPix = rhs._nPix;
  this->_xmin = rhs._xmin;
  this->_ymin = rhs._ymin;
  this->_src  = rhs._src.clone();
  this->_tri  = rhs._tri.clone();
  this->_tridx  = rhs._tridx.clone();
  this->_mask  = rhs._mask.clone();
  this->_alpha  = rhs._alpha.clone();
  this->_beta  = rhs._beta.clone();
  _mapx.create(_mask.rows,_mask.cols,CV_32F);
  _mapy.create(_mask.rows,_mask.cols,CV_32F);
  _coeff.create(this->nTri(),6,CV_64F);
  _dst = _src; return *this;
}
//===========================================================================
void PAW::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << IO::PAW << " " << _nPix << " " << _xmin << " " << _ymin << " ";
    IO::WriteMat(s,_src); IO::WriteMat(s,_tri); IO::WriteMat(s,_tridx);
    IO::WriteMat(s,_mask); IO::WriteMat(s,_alpha); IO::WriteMat(s,_beta);
  }
  else{
    int t = IOBinary::PAW;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    s.write(reinterpret_cast<char*>(&_nPix), sizeof(_nPix));
    s.write(reinterpret_cast<char*>(&_xmin), sizeof(_xmin));
    s.write(reinterpret_cast<char*>(&_ymin), sizeof(_ymin));

    IOBinary::WriteMat(s,_src); IOBinary::WriteMat(s,_tri); IOBinary::WriteMat(s,_tridx);
    IOBinary::WriteMat(s,_mask); IOBinary::WriteMat(s,_alpha); IOBinary::WriteMat(s,_beta);
  }
  return;
}
//===========================================================================
void PAW::ReadBinary(ifstream &s,bool readType)
{
 
  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::PAW);
  }
  
  s.read(reinterpret_cast<char*>(&_nPix), sizeof(_nPix));
  s.read(reinterpret_cast<char*>(&_xmin), sizeof(_xmin));
  s.read(reinterpret_cast<char*>(&_ymin), sizeof(_ymin));
  
  IOBinary::ReadMat(s,_src); IOBinary::ReadMat(s,_tri); IOBinary::ReadMat(s,_tridx);
  IOBinary::ReadMat(s,_mask); IOBinary::ReadMat(s,_alpha); IOBinary::ReadMat(s,_beta);    
  
  _w = _mask.cols; _h = _mask.rows;
  _mapx.create(_mask.rows,_mask.cols,CV_32F);
  _mapy.create(_mask.rows,_mask.cols,CV_32F);
  _coeff.create(this->nTri(),6,CV_64F); _dst = _src;
  return;
}//===========================================================================
void PAW::Read(ifstream &s,bool readType)
{
  int type = -1;
  if(readType){s >> type; assert(type == IO::PAW);}

  s >> _nPix >> _xmin >> _ymin;
  IO::ReadMat(s,_src); IO::ReadMat(s,_tri); IO::ReadMat(s,_tridx);
  IO::ReadMat(s,_mask); IO::ReadMat(s,_alpha); IO::ReadMat(s,_beta);
  
  _w = _mask.cols; _h = _mask.rows;
  _mapx.create(_mask.rows,_mask.cols,CV_32F);
  _mapy.create(_mask.rows,_mask.cols,CV_32F);
  _coeff.create(this->nTri(),6,CV_64F); _dst = _src;
  return;
}
//===========================================================================
void PAW::Init(cv::Mat &src,cv::Mat &tri)
{
  assert((src.type() == CV_64F) && (src.cols == 1));
  assert((tri.type() == CV_32S) && (tri.cols == 3));
  _src = src.clone(); _tri = tri.clone();
  int i,j,k,l,n = this->nPoints(); double c1,c2,c3,c4,c5; 
  _alpha.create(this->nTri(),3,CV_64F); _beta.create(this->nTri(),3,CV_64F);
  for(i = 0; i < this->nTri(); i++){
    j = _tri.it(i,0); k = _tri.it(i,1); l = _tri.it(i,2);
    c1 = _src.db(l+n,0) - _src.db(j+n,0); c2 = _src.db(l,0) - _src.db(j,0);
    c4 = _src.db(k+n,0) - _src.db(j+n,0); c3 = _src.db(k,0) - _src.db(j,0); 
    c5 = c3*c1 - c2*c4;
    _alpha.db(i,0) = (_src.db(j+n,0)*c2 - _src.db(j,0)*c1)/c5;
    _alpha.db(i,1) =  c1/c5; _alpha.db(i,2) = -c2/c5; 
    _beta.db(i,0)  = (_src.db(j,0)*c4 - _src.db(j+n,0)*c3)/c5;
    _beta.db(i,1)  = -c4/c5; _beta.db(i,2)  =  c3/c5;
  }
  cv::MatIterator_<double> x = _src.begin<double>(),y =_src.begin<double>()+n;
  double vx,vy,xmax=*x,ymax=*y,xmin=*x,ymin=*y;
  for(i = 0; i < n; i++){
    vx = *x++; vy = *y++;
    xmax = std::max(xmax,vx); ymax = std::max(ymax,vy);
    xmin = std::min(xmin,vx); ymin = std::min(ymin,vy);
  }
  int w = int(xmax - xmin + 1.0),h = int(ymax - ymin + 1.0);
  _mask.create(h,w,CV_8U); _tridx = cv::Mat::zeros(h,w,CV_32S);
  cv::MatIterator_<uchar> mp = _mask.begin<uchar>();
  cv::MatIterator_<int>   tp = _tridx.begin<int>();
  for(i = 0,_nPix = 0; i < h; i++){
    for(j = 0; j < w; j++,++mp,++tp){
      if((*tp = isWithinTri(double(j)+xmin,double(i)+ymin,tri,_src))==-1)*mp=0;
      else{*mp = 1; _nPix++;}
    }
  }
  _mapx.create(_mask.rows,_mask.cols,CV_32F);
  _mapy.create(_mask.rows,_mask.cols,CV_32F);
  _coeff.create(this->nTri(),6,CV_64F);
  _dst = _src; _xmin = xmin; _ymin = ymin;  
  _w = _mask.cols; _h = _mask.rows;
  return;
}
//===========================================================================
void PAW::Init(cv::Mat &src,cv::Mat &tri,cv::Mat &mask)
{
  assert((src.type() == CV_64F) && (src.cols == 1));
  assert((tri.type() == CV_32S) && (tri.cols == 3));
  _src = src.clone(); _tri = tri.clone();
  int i,j,k,l,n = this->nPoints(); double c1,c2,c3,c4,c5; 
  _alpha.create(this->nTri(),3,CV_64F); _beta.create(this->nTri(),3,CV_64F);
  for(i = 0; i < this->nTri(); i++){
    j = _tri.it(i,0); k = _tri.it(i,1); l = _tri.it(i,2);
    c1 = _src.db(l+n,0) - _src.db(j+n,0); c2 = _src.db(l,0) - _src.db(j,0);
    c4 = _src.db(k+n,0) - _src.db(j+n,0); c3 = _src.db(k,0) - _src.db(j,0); 
    c5 = c3*c1 - c2*c4;
    _alpha.db(i,0) = (_src.db(j+n,0)*c2 - _src.db(j,0)*c1)/c5;
    _alpha.db(i,1) =  c1/c5; _alpha.db(i,2) = -c2/c5; 
    _beta.db(i,0)  = (_src.db(j,0)*c4 - _src.db(j+n,0)*c3)/c5;
    _beta.db(i,1)  = -c4/c5; _beta.db(i,2)  =  c3/c5;
  }
  cv::MatIterator_<double> x = _src.begin<double>(),y =_src.begin<double>()+n;
  double vx,vy,xmax=*x,ymax=*y,xmin=*x,ymin=*y;
  for(i = 0; i < n; i++){
    vx = *x++; vy = *y++;
    xmax = std::max(xmax,vx); ymax = std::max(ymax,vy);
    xmin = std::min(xmin,vx); ymin = std::min(ymin,vy);
  }
  int w = int(xmax - xmin + 1.0),h = int(ymax - ymin + 1.0);
  _mask.create(h,w,CV_8U); _tridx = cv::Mat::zeros(h,w,CV_32S);
  cv::MatIterator_<uchar> Mp = mask.begin<uchar>();
  cv::MatIterator_<uchar> mp = _mask.begin<uchar>();
  cv::MatIterator_<int>   tp = _tridx.begin<int>();
  for(i = 0,_nPix = 0; i < h; i++){
    for(j = 0; j < w; j++,++Mp,++mp,++tp){
      if(*Mp){
	if((*tp = isWithinTri(double(j)+xmin,double(i)+ymin,tri,_src))==-1)
	  *mp=0;
	else{*mp = 1; _nPix++;}
      }
    }
  }
  _mapx.create(_mask.rows,_mask.cols,CV_32F);
  _mapy.create(_mask.rows,_mask.cols,CV_32F);
  _coeff.create(this->nTri(),6,CV_64F);
  _dst = _src; _xmin = xmin; _ymin = ymin;  
  _w = _mask.cols; _h = _mask.rows;
  return;
}
//=============================================================================
void PAW::Crop(cv::Mat &src, cv::Mat &dst, cv::Mat &s)
{
  assert((s.type() == CV_64F) && (s.rows == _src.rows) && (s.cols == 1) && 
	 (src.type() == dst.type()));
  this->SetDst(s); this->CalcCoeff(); this->WarpRegion(_mapx,_mapy);
  cv::remap(src,dst,_mapx,_mapy,CV_INTER_LINEAR); return;
}
//=============================================================================
void PAW::Vectorize(cv::Mat &img,cv::Mat &vec)
{
  assert((img.rows == _h) && (img.cols == _w));
  if((vec.rows!=_nPix)||(vec.cols!=1))vec.create(_nPix,1,CV_64F);
  int i,j;
  cv::MatIterator_<double> vp = vec.begin<double>();
  cv::MatIterator_<uchar>  mp = _mask.begin<uchar>();
  switch(img.type()){
  case CV_8U:
    {
      cv::MatIterator_<uchar>  cp = img.begin<uchar>();
      for(i=0;i<_h;i++){for(j=0;j<_w;j++,++mp,++cp){if(*mp)*vp++=(double)*cp;}}
      break;
    }
  case CV_32F:
    {
      cv::MatIterator_<float>  cp = img.begin<float>();
      for(i=0;i<_h;i++){for(j=0;j<_w;j++,++mp,++cp){if(*mp)*vp++=(double)*cp;}}
      break;
    }
  case CV_64F:
    {
      cv::MatIterator_<double>  cp = img.begin<double>();
      for(i=0;i<_h;i++){for(j=0;j<_w;j++,++mp,++cp){if(*mp)*vp++=(double)*cp;}}
      break;
    }
  default: printf("Unsupported image type %d!",img.type()); abort();
  }return;

  /*
  assert((img.rows == _h) && (img.cols == _w) && (img.type() == CV_8U));
  if((vec.rows!=_nPix)||(vec.cols!=1))vec.create(_nPix,1,CV_64F);
  int i,j;
  cv::MatIterator_<double> vp = vec.begin<double>();
  cv::MatIterator_<uchar>  cp = img.begin<uchar>();
  cv::MatIterator_<uchar>  mp = _mask.begin<uchar>();
  for(i=0;i<_h;i++){for(j=0;j<_w;j++,++mp,++cp){if(*mp)*vp++ = (double)*cp;}}
  return;
  */
}
//=============================================================================
void PAW::VectorizeUchar(cv::Mat &img,cv::Mat &vec)
{
  assert((img.rows == _h) && (img.cols == _w));
  if((vec.rows!=_nPix)||(vec.cols!=1))vec.create(_nPix,1,CV_8U);
  int i,j;
  cv::MatIterator_<uchar> vp = vec.begin<uchar>();
  cv::MatIterator_<uchar> mp = _mask.begin<uchar>();
  switch(img.type()){
  case CV_8U:
    {
      cv::MatIterator_<uchar>  cp = img.begin<uchar>();
      for(i=0;i<_h;i++){for(j=0;j<_w;j++,++mp,++cp){if(*mp)*vp++=(uchar)*cp;}}
      break;
    }
  case CV_32F:
    {
      cv::MatIterator_<float>  cp = img.begin<float>();
      for(i=0;i<_h;i++){for(j=0;j<_w;j++,++mp,++cp){if(*mp)*vp++=(uchar)*cp;}}
      break;
    }
  case CV_64F:
    {
      cv::MatIterator_<double>  cp = img.begin<double>();
      for(i=0;i<_h;i++){for(j=0;j<_w;j++,++mp,++cp){if(*mp)*vp++=(uchar)*cp;}}
      break;
    }
  default: printf("Unsupported image type %d!",img.type()); abort();
  }return;
}
//=============================================================================
void PAW::UnVectorize(cv::Mat &vec,cv::Mat &img,const int bck)
{
  assert((vec.rows==_nPix)&&(vec.cols==1));
  int i,j,w = this->Width(),h = this->Height();
  cv::MatIterator_<double> vp = vec.begin<double>();
  img = cv::Mat::zeros(h,w,CV_8U);
  if((bck >= 0) && (bck <= 255))img = cv::Scalar(bck);
  cv::MatIterator_<uchar> cp = img.begin<uchar>();
  cv::MatIterator_<uchar> mp = _mask.begin<uchar>();
  for(i=0;i<h;i++){
    for(j=0;j<w;j++,++mp,++cp){
      if(*mp){
	double v = *vp++;
	if(v < 0)*cp = 0;
	else if(v > 255)*cp = 255;
	else *cp = (uchar)floor(v+0.5);
      }
    }
  }return;
}
//=============================================================================
void PAW::CalcCoeff()
{
  int i,j,k,l,p=this->nPoints(); double c1,c2,c3,c4,c5,c6,*coeff,*alpha,*beta;
  for(l = 0; l < this->nTri(); l++){
    i = _tri.it(l,0); j = _tri.it(l,1); k = _tri.it(l,2);
    c1 = _dst.db(i  ,0); c2 = _dst.db(j  ,0) - c1; c3 = _dst.db(k  ,0) - c1;
    c4 = _dst.db(i+p,0); c5 = _dst.db(j+p,0) - c4; c6 = _dst.db(k+p,0) - c4;
    coeff = _coeff.ptr<double>(l);
    alpha = _alpha.ptr<double>(l);
    beta  = _beta.ptr<double>(l);
    coeff[0] = c1 + c2*alpha[0] + c3*beta[0];
    coeff[1] =      c2*alpha[1] + c3*beta[1];
    coeff[2] =      c2*alpha[2] + c3*beta[2];
    coeff[3] = c4 + c5*alpha[0] + c6*beta[0];
    coeff[4] =      c5*alpha[1] + c6*beta[1];
    coeff[5] =      c5*alpha[2] + c6*beta[2];
  }return;
}
//=============================================================================
void PAW::WarpRegion(cv::Mat &mapx,cv::Mat &mapy)
{
  assert((mapx.type() == CV_32F) && (mapy.type() == CV_32F));
  if((mapx.rows != _mask.rows) || (mapx.cols != _mask.cols))
    _mapx.create(_mask.rows,_mask.cols,CV_32F);
  if((mapy.rows != _mask.rows) || (mapy.cols != _mask.cols))
    _mapy.create(_mask.rows,_mask.cols,CV_32F);
  int x,y,j,k=-1; double yi,xi,xo,yo,*a=NULL,*ap;
  cv::MatIterator_<float> xp = mapx.begin<float>();
  cv::MatIterator_<float> yp = mapy.begin<float>();
  cv::MatIterator_<uchar> mp = _mask.begin<uchar>();
  cv::MatIterator_<int>   tp = _tridx.begin<int>();
  for(y = 0; y < _mask.rows; y++){   yi = double(y) + _ymin;
    for(x = 0; x < _mask.cols; x++){ xi = double(x) + _xmin;
      if(*mp == 0){*xp = -1; *yp = -1;}
      else{
	j = *tp; if(j != k){a = _coeff.ptr<double>(j); k = j;}  	
	ap = a;
	xo = *ap++; xo += *ap++ * xi; *xp = float(xo + *ap++ * yi);
	yo = *ap++; yo += *ap++ * xi; *yp = float(yo + *ap++ * yi);
      }
      mp++; tp++; xp++; yp++;
    }
  }return;
}
//===========================================================================
void PAW::WarpPoint(double xi,double yi, double &xo, double &yo)
{
  this->WarpPoint(xi,yi,xo,yo,-1); return;
}
//===========================================================================
void PAW::WarpPoint(double xi, double yi, double &xo, double &yo, int t)
{
  int i; 
  if(t < 0 || t >= this->nTri()){
    i = isWithinTri(xi,yi,_tri,_src); assert(i >= 0);
  }else i = t;
  double* a = _coeff.ptr<double>(i);
  xo = *a++; xo += *a++ * xi; xo += *a++ * yi;
  yo = *a++; yo += *a++ * xi; yo += *a++ * yi;
  return;
}
//===========================================================================
int PAW::InverseWarp(cv::Mat &si,cv::Mat &so,cv::Mat *idx)
{
  if((so.rows != si.rows) || (so.cols != si.cols))so.create(si.rows,1,CV_64F);
  for(int i = 0; i < si.rows/2; i++){
    if(this->InverseWarpPoint(si.db(i,0),si.db(i+si.rows/2,0),
			      so.db(i,0),so.db(i+so.rows/2,0)) < 0){
      if(idx == NULL)return -1;
      else{
	if(idx->rows != si.rows/2)return -1;
	so.db(i,0) = 0.0; so.db(i+so.rows/2,0) = 0.0;
	int k = idx->it(i,0),l = 0; double vx,vy;
	for(int j = 0; j < this->nTri(); j++){
	  if((k == _tri.it(j,0)) || (k == _tri.it(j,1)) || (k == _tri.it(j,2))){
	    this->InverseWarpPoint(si.db(i,0),si.db(i+si.rows/2,0),vx,vy,j);
	    so.db(i,0) += vx; so.db(i+so.rows/2,0) += vy; l++;
	  }
	}
	if(l == 0)return -1; 
	so.db(i,0) /= l; so.db(i+so.rows/2,0) /= l;
      }
    }
  }return 0;
}
//===========================================================================
int PAW::InverseWarpPoint(double xi,double yi, double &xo, double &yo,int t)
{
  int i,j,l,n = this->nPoints();  
  double dx[3],dy[3],sx[3],sy[3];
  cv::Mat A(2,3,CV_64F),Xi(3,3,CV_64F),X(3,3,CV_64F),Y(3,2,CV_64F);
  if(t < 0){l = isWithinTri(xi,yi,_tri,_dst); if(l < 0)return -1;} else l = t;
  for(i = 0; i < 3; i++){
    j = _tri.it(l,i);
    sx[i] = _src.db(j  ,0) - _xmin; sy[i] = _src.db(j+n,0) - _ymin;
    dx[i] = _dst.db(j  ,0);         dy[i] = _dst.db(j+n,0);
    X.db(i,0) = dx[i]; X.db(i,1) = dy[i]; X.db(i,2) = 1.0;
    Y.db(i,0) = sx[i]; Y.db(i,1) = sy[i];
  }
  cv::invert(X,Xi,cv::DECOMP_SVD); A = (Xi*Y).t();
  xo = A.db(0,0)*xi + A.db(0,1)*yi + A.db(0,2);
  yo = A.db(1,0)*xi + A.db(1,1)*yi + A.db(1,2); return 0;
}
//===========================================================================
int PAW::Warp(cv::Mat &si,cv::Mat &so,cv::Mat *idx)
{
  if((so.rows != si.rows) || (so.cols != si.cols))so.create(si.rows,1,CV_64F);
  for(int i = 0; i < si.rows/2; i++){
    if(this->WarpPointCheck(si.db(i,0),si.db(i+si.rows/2,0),
			    so.db(i,0),so.db(i+so.rows/2,0)) < 0){
      if(idx == NULL)return -1;
      else{
	if(idx->rows != si.rows/2)return -1;
	so.db(i,0) = 0.0; so.db(i+so.rows/2,0) = 0.0;
	int k = idx->it(i,0),l = 0; double vx,vy;
	for(int j = 0; j < this->nTri(); j++){
	  if((k == _tri.it(j,0)) || (k == _tri.it(j,1)) || (k == _tri.it(j,2))){
	    this->WarpPoint(si.db(i,0),si.db(i+si.rows/2,0),vx,vy,j);
	    so.db(i,0) += vx; so.db(i+so.rows/2,0) += vy; l++;
	  }
	}
	if(l == 0)return -1; 
	so.db(i,0) /= l; so.db(i+so.rows/2,0) /= l;
      }
    }
  }return 0;
}
//===========================================================================
int PAW::WarpPointCheck(double xi, double yi, double &xo, double &yo, int t)
{
  int i; 
  if(t < 0 || t >= this->nTri()){
    i = isWithinTri(xi,yi,_tri,_src); if(i < 0)return -1;
  }else i = t;
  double* a = _coeff.ptr<double>(i);
  xo = *a++; xo += *a++ * xi; xo += *a++ * yi;
  yo = *a++; yo += *a++ * xi; yo += *a++ * yi;
  return 0;
}
//===========================================================================
void PAW::Draw(cv::Mat &src,cv::Mat &dst,cv::Mat &s)
{
  assert((src.type() == CV_8U) && (dst.type() == CV_8U) &&
	 (src.rows == this->Height()) && (src.cols == this->Width()) &&
	 (s.type() == CV_64F) && (s.rows == _src.rows) && (s.cols == 1));
  int i,j,l,n = this->nPoints(),xmin,ymin,xmax,ymax;
  double dx[3],dy[3],sx[3],sy[3],x,y;
  cv::Mat A(2,3,CV_64F),Xi(3,3,CV_64F),X(3,3,CV_64F),Y(3,2,CV_64F);
  for(l = 0; l < this->nTri(); l++){
    for(i = 0; i < 3; i++){
      j = _tri.it(l,i);
      sx[i] = _src.db(j  ,0) - _xmin; sy[i] = _src.db(j+n,0) - _ymin;
      dx[i] =    s.db(j  ,0); dy[i] =    s.db(j+n,0);
    }
    xmax = (int)std::ceil( std::max(std::max(dx[0],dx[1]),dx[2]));
    ymax = (int)std::ceil( std::max(std::max(dy[0],dy[1]),dy[2]));
    xmin = (int)std::floor(std::min(std::min(dx[0],dx[1]),dx[2]));
    ymin = (int)std::floor(std::min(std::min(dy[0],dy[1]),dy[2]));
    if( (xmin < 0) || (xmax >= dst.cols) || (ymin < 0) || (ymax >= dst.rows) ||
       (((dx[1] - dx[0])*(dy[0] - dy[2])+(dy[1] - dy[0])*(dx[2] - dx[0]))/
	 ((sx[1] - sx[0])*(sy[0] - sy[2])+(sy[1] - sy[0])*(sx[2] - sx[0]))<=0)){
      continue;
    }
    for(i = 0; i < 3; i++){
      X.db(i,0) = dx[i]; X.db(i,1) = dy[i]; X.db(i,2) = 1.0;
      Y.db(i,0) = sx[i]; Y.db(i,1) = sy[i];
    }
    cv::invert(X,Xi,cv::DECOMP_SVD); A = (Xi*Y).t();
    for(i = ymin; i <= ymax; i++){
      for(j = xmin; j <= xmax; j++){
	if(sameSide(j,i,dx[0],dy[0],dx[1],dy[1],dx[2],dy[2]) &&
	   sameSide(j,i,dx[1],dy[1],dx[0],dy[0],dx[2],dy[2]) &&
	   sameSide(j,i,dx[2],dy[2],dx[0],dy[0],dx[1],dy[1])){
	  x = A.db(0,0)*j + A.db(0,1)*i + A.db(0,2);
	  y = A.db(1,0)*j + A.db(1,1)*i + A.db(1,2);
	  dst.at<uchar>(i,j) = (uchar)(bilinInterp(src,x,y)+0.5);
	}
      }
    }
  }return;
}
//===========================================================================
void PAW::DrawRGB(std::vector<cv::Mat> &src,
		  std::vector<cv::Mat> &dst,cv::Mat &s)
{
  assert((src.size() == 3) && (dst.size() == 3) &&
	 (s.type() == CV_64F) && (s.rows == _src.rows) && (s.cols == 1));
  for(int i = 0; i < 3; i++){
    assert((src[i].type() == CV_8U) && (dst[i].type() == CV_8U) &&
	   (src[i].rows == this->Height()) && (src[i].cols == this->Width()));
  }
  int i,j,k,l,n = this->nPoints(),xmin,ymin,xmax,ymax;
  double dx[3],dy[3],sx[3],sy[3],x,y;
  cv::Mat A(2,3,CV_64F),Xi(3,3,CV_64F),X(3,3,CV_64F),Y(3,2,CV_64F);
  for(l = 0; l < this->nTri(); l++){
    for(i = 0; i < 3; i++){
      j = _tri.it(l,i);
      sx[i] = _src.db(j  ,0) - _xmin; sy[i] = _src.db(j+n,0) - _ymin;
      dx[i] =    s.db(j  ,0); dy[i] =    s.db(j+n,0);
    }
    xmax = (int)std::ceil( std::max(std::max(dx[0],dx[1]),dx[2]));
    ymax = (int)std::ceil( std::max(std::max(dy[0],dy[1]),dy[2]));
    xmin = (int)std::floor(std::min(std::min(dx[0],dx[1]),dx[2]));
    ymin = (int)std::floor(std::min(std::min(dy[0],dy[1]),dy[2]));
    if( (xmin < 0) || (xmax >= dst[0].cols) || 
	(ymin < 0) || (ymax >= dst[0].rows) ||
       (((dx[1] - dx[0])*(dy[0] - dy[2])+(dy[1] - dy[0])*(dx[2] - dx[0]))/
	 ((sx[1] - sx[0])*(sy[0] - sy[2])+(sy[1] - sy[0])*(sx[2] - sx[0]))<=0)){
      continue;
    }
    for(i = 0; i < 3; i++){
      X.db(i,0) = dx[i]; X.db(i,1) = dy[i]; X.db(i,2) = 1.0;
      Y.db(i,0) = sx[i]; Y.db(i,1) = sy[i];
    }
    cv::invert(X,Xi,cv::DECOMP_SVD); A = (Xi*Y).t();
    for(i = ymin; i <= ymax; i++){
      for(j = xmin; j <= xmax; j++){
	if(sameSide(j,i,dx[0],dy[0],dx[1],dy[1],dx[2],dy[2]) &&
	   sameSide(j,i,dx[1],dy[1],dx[0],dy[0],dx[2],dy[2]) &&
	   sameSide(j,i,dx[2],dy[2],dx[0],dy[0],dx[1],dy[1])){
	  x = A.db(0,0)*j + A.db(0,1)*i + A.db(0,2);
	  y = A.db(1,0)*j + A.db(1,1)*i + A.db(1,2);
	  for(k = 0; k < 3; k++)
	    dst[k].at<uchar>(i,j) = (uchar)(bilinInterp(src[k],x,y)+0.5);
	}
      }
    }
  }return;
}
//=============================================================================
cv::Mat PAW::FindVTri()
{
  int i,j; cv::Mat vTri = cv::Mat::zeros(this->nPoints(),this->nTri(),CV_8U);
  for(i = 0; i < this->nPoints(); i++){
    for(j = 0; j < this->nTri(); j++){
      if((i == _tri.it(j,0)) || (i == _tri.it(j,1)) || (i == _tri.it(j,2)))
	vTri.at<uchar>(i,j) = 1;
    }
  }return vTri;
}
//=============================================================================
cv::Mat PAW::PixTri()
{
  int i,j; cv::Mat pixTri(_nPix,1,CV_32S);
  cv::MatIterator_<uchar> mp = _mask.begin<uchar>();
  cv::MatIterator_<int>   tp = _tridx.begin<int>();
  cv::MatIterator_<int>   pp = pixTri.begin<int>();
  for(i = 0; i < _h; i++){
    for(j = 0; j < _w; j++,++mp,++tp){
      if(*mp)*pp++ = isWithinTri(double(j)+_xmin,double(i)+_ymin,_tri,_src);
    }
  }return pixTri;
}
//=============================================================================
cv::Mat PAW::dWdx(cv::Mat &pixTri)
{
  int x,y,l,k=-1; double vx,vy,a1=0,a2=0,a3=0,b1=0,b2=0,b3=0; 
  cv::Mat dW(3,_nPix,CV_64F);
  cv::MatIterator_<double> wi = dW.begin<double>();
  cv::MatIterator_<double> wj = dW.begin<double>()+_nPix;
  cv::MatIterator_<double> wk = dW.begin<double>()+_nPix*2;
  cv::MatIterator_<uchar>  mp = _mask.begin<uchar>();
  cv::MatIterator_<int>    tp = pixTri.begin<int>();
  for(y = 0; y < _h; y++){        vy = (double)y + _ymin;
    for(x = 0; x < _w; x++,++mp){ vx = (double)x + _xmin;
      if(*mp){
	l = *tp++;
	if(k != l){
	  a1 = _alpha.db(l,0); a2 = _alpha.db(l,1); a3 = _alpha.db(l,2);
	  b1 =  _beta.db(l,0); b2 =  _beta.db(l,1); b3 =  _beta.db(l,2); k = l;
	}
	*wj = a1+a2*vx+a3*vy; *wk = b1+b2*vx+b3*vy; *wi = 1.0-(*wj + *wk);
	++wi; ++wj; ++wk;
      }
    }
  }return dW;
}
//=============================================================================
cv::Mat PAW::dWdx()
{
  cv::Mat pixTri = this->PixTri(); return this->dWdx(pixTri);
}
//==============================================================================
