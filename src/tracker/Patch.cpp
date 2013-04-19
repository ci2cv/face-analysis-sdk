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

#include <tracker/Patch.hpp>
#define SGN(x) ((x<0) ? 0:1)
using namespace FACETRACKER;
using namespace std;
//===========================================================================
void sum2one(cv::Mat &M)
{
  if(M.type() == CV_64F){
    double sum = 0; int cols = M.cols, rows = M.rows;
    if(M.isContinuous()){cols *= rows;rows = 1;}
    for(int i = 0; i < rows; i++){
      const double* Mi = M.ptr<double>(i);
      for(int j = 0; j < cols; j++)sum += *Mi++;
    }
    M /= sum; 
  }else if(M.type() == CV_32F){
    float sum = 0; int cols = M.cols, rows = M.rows;
    if(M.isContinuous()){cols *= rows;rows = 1;}
    for(int i = 0; i < rows; i++){
      const float* Mi = M.ptr<float>(i);
      for(int j = 0; j < cols; j++)sum += *Mi++;
    }
    M /= sum; 
  }else{    
    printf("ERROR(%s,%d): Unsupported patch type %d!\n",
	   __FILE__,__LINE__,M.type()); abort();
  }return;
}
//===========================================================================
void Grad(cv::Mat &im,cv::Mat &grad)
{
  assert((im.rows == grad.rows) && (im.cols == grad.cols));
  assert((im.type() == CV_32F) && (grad.type() == CV_32F));
  int x,y,h = im.rows,w = im.cols; float vx,vy; grad = cv::Scalar(0);
  cv::MatIterator_<float> gp  = grad.begin<float>() + w+1;
  cv::MatIterator_<float> px1 = im.begin<float>()   + w+2;
  cv::MatIterator_<float> px2 = im.begin<float>()   + w;
  cv::MatIterator_<float> py1 = im.begin<float>()   + 2*w+1;
  cv::MatIterator_<float> py2 = im.begin<float>()   + 1;
  for(y = 1; y < h-1; y++){ 
    for(x = 1; x < w-1; x++){
      vx = *px1++ - *px2++; vy = *py1++ - *py2++; *gp++ = vx*vx + vy*vy;
    }
    px1 += 2; px2 += 2; py1 += 2; py2 += 2; gp += 2;
  }return;
}
//===========================================================================
void LBP(cv::Mat &im,cv::Mat &lbp)
{
  assert((im.rows == lbp.rows) && (im.cols == lbp.cols));
  assert((im.type() == CV_32F) && (lbp.type() == CV_32F));
  int x,y,h = im.rows,w = im.cols; float v[9]; lbp = cv::Scalar(0);
  cv::MatIterator_<float> lp = lbp.begin<float>() + w+1;
  cv::MatIterator_<float> p1 = im.begin<float>();
  cv::MatIterator_<float> p2 = im.begin<float>()  + w;
  cv::MatIterator_<float> p3 = im.begin<float>()  + w*2;
 for(y = 1; y < h-1; y++){
    for(x = 1; x < w-1; x++){
      v[4] = *p2++; v[0] = *p2++; v[5] = *p2;
      v[1] = *p1++; v[2] = *p1++; v[3] = *p1;
      v[6] = *p3++; v[7] = *p3++; v[8] = *p3;
      *lp++ = 
	SGN(v[0]-v[1])*2   + SGN(v[0]-v[2])*4   + 
	SGN(v[0]-v[3])*8   + SGN(v[0]-v[4])*16  + 
	SGN(v[0]-v[5])*32  + SGN(v[0]-v[6])*64  + 
	SGN(v[0]-v[7])*128 + SGN(v[0]-v[8])*256 ;
      p1--; p2--; p3--;
    }
    p1 += 2; p2 += 2; p3 += 2; lp += 2;
  }return;
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
Patch& Patch::operator= (Patch const& rhs)
{   
  this->_t = rhs._t; this->_a = rhs._a; this->_b = rhs._b; 
  this->_W = rhs._W.clone(); this->im_ = rhs.im_.clone(); 
  this->res_ = rhs.res_.clone(); return *this;
}
//===========================================================================
void Patch::Load(const char* fname, bool binary)
{
  ifstream s;
  if(!binary){s.open(fname); assert(s.is_open()); this->Read(s); s.close();}
  else{s.open(fname, std::ios::binary);assert(s.is_open()); this->ReadBinary(s); s.close();}
  
  return;
}
//===========================================================================
void Patch::Save(const char* fname, bool binary){
  ofstream s(fname); assert(s.is_open()); this->Write(s, binary);s.close(); return;
}
//===========================================================================
void Patch::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << IO::PATCH << " " << _t << " " << _a << " " << _b <<" ";
    IO::WriteMat(s,_W);
  }
  else{
    int t =  IOBinary::PATCH;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    s.write(reinterpret_cast<char*>(&_t), sizeof(_t));
    s.write(reinterpret_cast<char*>(&_a), sizeof(_a));
    s.write(reinterpret_cast<char*>(&_b), sizeof(_b));
    IOBinary::WriteMat(s,_W); 
  }
  return;
}
//===========================================================================
void Patch::Read(ifstream &s,bool readType)
{
  int type =-1;
  if(readType){s >> type; assert(type == IO::PATCH);}
  
  s >> _t >> _a >> _b; 
  IO::ReadMat(s,_W); 
  
  return;
}
//===========================================================================
void Patch::ReadBinary(ifstream &s,bool readType)

{
  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::PATCH);
  }
  s.read(reinterpret_cast<char*>(&_t), sizeof(_t));
  s.read(reinterpret_cast<char*>(&_a), sizeof(_a));
  s.read(reinterpret_cast<char*>(&_b), sizeof(_b));
  IOBinary::ReadMat(s, _W);
  
}

//===========================================================================
void Patch::Init(int t, double a, double b, cv::Mat &W)
{
  assert((W.type() == CV_32F)); _t=t; _a=a; _b=b; _W=W.clone(); return;
}
//===========================================================================
void Patch::Response(cv::Mat &im,cv::Mat &resp)
{
  assert((im.type() == CV_32F) && (resp.type() == CV_64F));
  assert((im.rows>=_W.rows) && (im.cols>=_W.cols));
  int h = im.rows - _W.rows + 1, w = im.cols - _W.cols + 1; cv::Mat I;
  if(resp.rows != h || resp.cols != w)resp.create(h,w,CV_64F);
  if(res_.rows != h || res_.cols != w)res_.create(h,w,CV_32F);
  if(_t == 0)I = im;
  else{
    if(im_.rows == im.rows && im_.cols == im.cols)I = im_;
    else if(im_.rows >= im.rows && im_.cols >= im.cols)
      I = im_(cv::Rect(0,0,im.cols,im.rows));
    else{im_.create(im.rows,im.cols,CV_32F); I = im_;}
    if     (_t == 1)Grad(im,I);
    else if(_t == 2)LBP(im,I);
    else{
      printf("ERROR(%s,%d): Unsupported patch type %d!\n",
	     __FILE__,__LINE__,_t); abort();
    }
  }
  cv::matchTemplate(I,_W,res_,CV_TM_CCOEFF_NORMED);
  cv::MatIterator_<double> p = resp.begin<double>();
  cv::MatIterator_<float> q1 = res_.begin<float>();
  cv::MatIterator_<float> q2 = res_.end<float>();
  while(q1 != q2)*p++ = 1.0/(1.0 + exp( *q1++ * _a + _b ));
  return;
}
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
MPatch& MPatch::operator= (MPatch const& rhs)
{   
  _w = rhs._p[0]._W.cols; _h = rhs._p[0]._W.rows;
  for(size_t i = 1; i < rhs._p.size(); i++){
    if((rhs._p[i]._W.cols != _w) || (rhs._p[i]._W.rows != _h)){      
      printf("ERROR(%s,%d): Incompatible patch sizes!\n",
	     __FILE__,__LINE__); abort();
    }
  }
  _p = rhs._p; return *this;
}
//===========================================================================
void MPatch::Init(std::vector<Patch> &p)
{
  _w = p[0]._W.cols; _h = p[0]._W.rows;
  for(size_t i = 1; i < p.size(); i++){
    if((p[i]._W.cols != _w) || (p[i]._W.rows != _h)){      
      printf("ERROR(%s,%d): Incompatible patch sizes!\n",
	     __FILE__,__LINE__); abort();
    }
  }
  _p = p; return;
}
//===========================================================================
void MPatch::Load(const char* fname, bool binary)
{
  ifstream s;
  if(!binary){s.open(fname); assert(s.is_open()); this->Read(s); s.close();}
  else{s.open(fname, std::ios::binary);assert(s.is_open()); this->ReadBinary(s); 
    s.close();}
  return;
}
//===========================================================================
void MPatch::Save(const char* fname, bool binary){
  ofstream s(fname); assert(s.is_open()); this->Write(s, binary);s.close(); return;
}
//===========================================================================
void MPatch::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << IO::MPATCH<< " " << _w << " " << _h << " " << _p.size() << " ";
  }
  else{
    int t = IOBinary::MPATCH;
    s.write((char*)&t, sizeof(t));
    s.write((char*)&_w, sizeof(_w));
    s.write((char*)&_h, sizeof(_h));
    t = _p.size();
    s.write((char*)&t, sizeof(t));
  }
  //std::cout << _w << " " << _h << " " << _p.size() << std::endl;
  for(size_t i = 0; i < _p.size(); i++)
    _p[i].Write(s, binary); 
  
  return;
}
//===========================================================================
void MPatch::Read(ifstream &s,bool readType)
{
  int type;
  if(readType){s >> type; assert(type == IO::MPATCH);}
  
  int n; 
  if(type == IO::MPATCH)
    s >> _w >> _h >> n;

  _p.resize(n);
  for(int i = 0; i < n; i++)
    _p[i].Read(s); 
  return;
}
//===========================================================================
void MPatch::ReadBinary(ifstream &s,bool readType)
{
  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::MPATCH);
  }
  int n;
  
  s.read(reinterpret_cast<char*>(&_w), sizeof(_w));
  s.read(reinterpret_cast<char*>(&_h), sizeof(_h));
  s.read(reinterpret_cast<char*>(&n), sizeof(n));

  //std::cout << _w << " " << _h << " " << n << std::endl;
    
  _p.resize(n);
  for(int i = 0; i < n; i++)
    _p[i].ReadBinary(s); 
  return;

}
//===========================================================================
void MPatch::Response(cv::Mat &im,cv::Mat &resp)
{
  assert((im.type() == CV_32F) && (resp.type() == CV_64F));
  assert((im.rows >= _h) && (im.cols >= _w));
  int h = im.rows - _h + 1, w = im.cols - _w + 1;
  if(resp.rows != h || resp.cols != w)resp.create(h,w,CV_64F);
  if(res_.rows != h || res_.cols != w)res_.create(h,w,CV_64F);
  if(_p.size() == 1){_p[0].Response(im,resp); sum2one(resp);}
  else{
    resp = cvScalar(1.0);
    for(size_t i = 0; i < _p.size(); i++){
      _p[i].Response(im,res_); sum2one(res_); resp = resp.mul(res_);
    }
    sum2one(resp); 
  }return;
}
//=============================================================================
