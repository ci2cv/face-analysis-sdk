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

#include <tracker/ShapePredictor.hpp>
#include <tracker/CLM.hpp>
#define db at<double>
#define it at<int>
using namespace FACETRACKER;
using namespace std;
//=============================================================================
ShapePredictor& ShapePredictor::operator= (ShapePredictor const&rhs)
{
  _K = rhs._K;
  _idx = rhs._idx.clone();
  _rect = rhs._rect;
  _pdm = rhs._pdm;
  _warp = rhs._warp;
  _C.resize(_K); _R.resize(_K);
  for(int i = 0; i < _K; i++){
    _C[i] = rhs._C[i].clone();
    _R[i] = rhs._R[i].clone();
  }
  x_.create(_warp._nPix+1,1,CV_64F);
  y_.create(2*_idx.rows,1,CV_64F);
  z_.create(2*_idx.rows,1,CV_64F);
  plocal_.create(_pdm.nModes(),1,CV_64F);
  pglobl_.create(4,1,CV_64F); return *this;
}
//==============================================================================
void ShapePredictor::Load(const char* fname, bool binary)
{
  ifstream s;
  if(!binary){
    s.open(fname); assert(s.is_open()); this->Read(s);}
  else{
    s.open(fname, std::ios::binary);
    assert(s.is_open()); this->ReadBinary(s);
  } s.close(); return;  
}
//===========================================================================
void ShapePredictor::Save(const char* fname, bool binary)
{
  ofstream s;
  if(binary)
    s.open(fname, std::ios::binary);
  else 
    s.open(fname);
  assert(s.is_open()); this->Write(s);s.close(); return;
}
//==============================================================================
void ShapePredictor::Read(ifstream &s)
{
  s >> _K; _C.resize(_K); _R.resize(_K);
  for(int i = 0; i < _K; i++){
    FACETRACKER::IO::ReadMat(s,_C[i]);
    FACETRACKER::IO::ReadMat(s,_R[i]);
  }
  FACETRACKER::IO::ReadMat(s,_idx);
  _pdm.Read(s); _warp.Read(s);
  s >> _rect.x >> _rect.y >> _rect.width >> _rect.height;

  x_.create(_warp._nPix+1,1,CV_64F);
  y_.create(2*_idx.rows,1,CV_64F);
  z_.create(2*_idx.rows,1,CV_64F);
  plocal_.create(_pdm.nModes(),1,CV_64F);
  pglobl_.create(4,1,CV_64F); return;
}
//==============================================================================
void ShapePredictor::ReadBinary(ifstream &s, bool readType)
{

  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::SHAPEPREDICTOR);
  }
  
  s.read(reinterpret_cast<char*>(&_K), sizeof(_K));
  _C.resize(_K); _R.resize(_K);
  for(int i = 0; i < _K; i++){
    FACETRACKER::IOBinary::ReadMat(s,_C[i]);
    FACETRACKER::IOBinary::ReadMat(s,_R[i]);
  }
  FACETRACKER::IOBinary::ReadMat(s,_idx);
  _pdm.ReadBinary(s); _warp.ReadBinary(s);
  s.read(reinterpret_cast<char*>(&_rect), sizeof(_rect));
  
  x_.create(_warp._nPix+1,1,CV_64F);
  y_.create(2*_idx.rows,1,CV_64F);
  z_.create(2*_idx.rows,1,CV_64F);
  plocal_.create(_pdm.nModes(),1,CV_64F);
  pglobl_.create(4,1,CV_64F); return;
}
//==============================================================================
void ShapePredictor::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << _K << " ";
    for(int i = 0; i < _K; i++){
      FACETRACKER::IO::WriteMat(s,_C[i]);
      FACETRACKER::IO::WriteMat(s,_R[i]);
    }
    FACETRACKER::IO::WriteMat(s,_idx);
    _pdm.Write(s); _warp.Write(s); 
    s << _rect.x << " " <<  _rect.y << " " 
      <<  _rect.width << " " << _rect.height << " ";
  }
  else{
    int t = IOBinary::SHAPEPREDICTOR;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    s.write(reinterpret_cast<char*>(&_K), sizeof(_K));
    for(int i = 0; i < _K; i++){
      FACETRACKER::IOBinary::WriteMat(s,_C[i]);
      FACETRACKER::IOBinary::WriteMat(s,_R[i]);
    }
    FACETRACKER::IOBinary::WriteMat(s,_idx);
    _pdm.Write(s, binary); _warp.Write(s, binary);
    s.write(reinterpret_cast<char*>(&_rect), sizeof(_rect));
  }
  return;
}
//==============================================================================
cv::Mat ShapePredictor::Predict(cv::Mat &shape,cv::Mat &im)
{
  int n = _idx.rows,k = FindCluster(shape);
  _warp.Crop(im,crop_,shape);
  cv::Mat img = crop_(_rect);
  equalizeHist(img,img);
  cv::Mat x = x_(cv::Rect(0,0,1,_warp._nPix));
  _warp.Vectorize(crop_,x); cv::normalize(x,x); x_.db(_warp._nPix,0) = 1.0;
  y_ = _R[k]*x_; 
  for(int i = 0; i < n; i++){
    y_.db(i  ,0) += _warp._xmin;
    y_.db(i+n,0) += _warp._ymin;
  }
  _warp.Warp(y_,z_,&_idx);
  _pdm.CalcParams(z_,plocal_,pglobl_); _pdm.CalcShape2D(z_,plocal_,pglobl_);
  return z_;
}
//==============================================================================
int ShapePredictor::FindCluster(cv::Mat &shape)
{
  int n = _idx.rows;
  for(int j = 0; j < n; j++){
    z_.db(j  ,0) = shape.db(_idx.it(j,0)             ,0);
    z_.db(j+n,0) = shape.db(_idx.it(j,0)+shape.rows/2,0);
  }
  double a1,b1,tx1,ty1,a2,b2,tx2,ty2,v,vmin = 0; int l = -1; cv::Mat S;
  for(int k = 0; k < _K; k++){
    FACETRACKER::CalcSimT(_C[k],z_,a1,b1,tx1,ty1);
    FACETRACKER::invSimT(a1,b1,tx1,ty1,a2,b2,tx2,ty2);
    z_.copyTo(y_);
    FACETRACKER::SimT(y_,a2,b2,tx2,ty2); 
    v = cv::norm(y_,_C[k]);
    if((v < vmin) || (l < 0)){vmin = v; l = k;}
  }return l;
}
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
void ShapePredictorList::Load(const char* fname, bool binary)
{
  ifstream s;
  if(!binary){ s.open(fname); assert(s.is_open()); this->Read(s);}
  else { s.open(fname, std::ios::binary); assert(s.is_open()); this->ReadBinary(s);}
  s.close(); return;  
}
//===========================================================================
void ShapePredictorList::Save(const char* fname, bool binary)
{
  ofstream s;
  if(binary)
    s.open(fname, std::ios::binary);
  else s.open(fname);
  
  assert(s.is_open()); this->Write(s, binary);s.close(); return;
}
//===========================================================================
void ShapePredictorList::Read(ifstream &s)
{
  int N; 
  s >> N;
  
  _pred.resize(N);
  for(int i = 0; i < N; i++)_pred[i].Read(s);
  
  return;
}
//==============================================================================
//===========================================================================
void ShapePredictorList::ReadBinary(ifstream &s, bool readType)
{
  int N; 

  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::SHAPEPREDICTORLIST);
  }

  s.read(reinterpret_cast<char*>(&N), sizeof(int));

  _pred.resize(N);
  for(int i = 0; i < N; i++)_pred[i].ReadBinary(s);
  
  return;
}
//==============================================================================
void ShapePredictorList::Write(ofstream &s, bool binary)
{
  if(!binary)
    s << _pred.size() << " ";
  else{
    int t = IOBinary::SHAPEPREDICTORLIST;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    t = _pred.size();
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
  }
  for(size_t i = 0; i < _pred.size(); i++)_pred[i].Write(s, binary);
  return;
}
//==============================================================================
void ShapePredictorList::Predict(cv::Mat &shape,cv::Mat &im)
{
  size_t i;
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(i = 0; i < _pred.size(); i++){
    cv::Mat s = _pred[i].Predict(shape,im);
    int n = _pred[i]._idx.rows;
    for(int j = 0; j < n; j++){
      shape.db(_pred[i]._idx.it(j,0)             ,0) = s.db(j  ,0);
      shape.db(_pred[i]._idx.it(j,0)+shape.rows/2,0) = s.db(j+n,0);
    }
  }return;
}
//==============================================================================
