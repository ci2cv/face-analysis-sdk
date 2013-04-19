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

#include <tracker/FCheck.hpp>
using namespace FACETRACKER;
using namespace std;
//===========================================================================
FCheck& FCheck::operator= (FCheck const& rhs)
{
  this->_b = rhs._b; this->_w = rhs._w.clone(); this->_paw = rhs._paw;
  crop_.create(_paw._mask.rows,_paw._mask.cols,CV_8U);
  vec_.create(_paw._nPix,1,CV_64F); return *this;
}
//===========================================================================
void FCheck::Init(double b, cv::Mat &w, PAW &paw)
{
  assert((w.type() == CV_64F) && (w.rows == paw._nPix));
  _b = b; _w = w.clone(); _paw = paw;
  crop_.create(_paw._mask.rows,_paw._mask.cols,CV_8U);
  vec_.create(_paw._nPix,1,CV_64F); return;
}
//===========================================================================
void FCheck::Load(const char* fname, bool binary)
{
  ifstream s;
  if(!binary){ s.open(fname); assert(s.is_open()); this->Read(s);}
  else { s.open(fname,std::ios::binary); assert(s.is_open()); this->ReadBinary(s);}
  s.close(); return;
}
//===========================================================================
void FCheck::Save(const char* fname, bool binary)
{
  ofstream s(fname); assert(s.is_open()); this->Write(s, binary);s.close(); return;
}
//===========================================================================
void FCheck::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << IO::FCHECK << " " << _b << " ";
    IO::WriteMat(s,_w);
  }
  else{
    int t = IOBinary::FCHECK;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    s.write(reinterpret_cast<char*>(&_b), sizeof(_b));
    IOBinary::WriteMat(s,_w); 
  }
  _paw.Write(s, binary);
  
 return;
}
//===========================================================================
void FCheck::Read(ifstream &s,bool readType)
{
  int type = -1;; 
  if(readType){s >> type; assert(type == IO::FCHECK);}
 
  s >> _b; IO::ReadMat(s,_w); _paw.Read(s);

  crop_.create(_paw._mask.rows,_paw._mask.cols,CV_8U);
  vec_.create(_paw._nPix,1,CV_64F); 
  return;
}
//===========================================================================
void FCheck::ReadBinary(ifstream &s,bool readType)
{
  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::FCHECK);
  }

  s.read(reinterpret_cast<char*>(&_b), sizeof(_b)); 
  IOBinary::ReadMat(s,_w); _paw.ReadBinary(s);

  crop_.create(_paw._mask.rows,_paw._mask.cols,CV_8U);
  vec_.create(_paw._nPix,1,CV_64F); 
  return;
}
//===========================================================================
bool FCheck::Check(cv::Mat &im,cv::Mat &s)
{
  assert((im.type() == CV_8U) && (s.type() == CV_64F) &&
	 (s.rows/2 == _paw.nPoints()) && (s.cols == 1));
  _paw.Crop(im,crop_,s);
  if((vec_.rows!=_paw._nPix)||(vec_.cols!=1))vec_.create(_paw._nPix,1,CV_64F);
  int i,j,w = crop_.cols,h = crop_.rows;
  cv::MatIterator_<double> vp = vec_.begin<double>();
  cv::MatIterator_<uchar>  cp = crop_.begin<uchar>();
  cv::MatIterator_<uchar>  mp = _paw._mask.begin<uchar>();
  for(i=0;i<h;i++){for(j=0;j<w;j++,++mp,++cp){if(*mp)*vp++ = (double)*cp;}}
  double var,mean=cv::sum(vec_)[0]/vec_.rows; vec_-=mean; var = vec_.dot(vec_); 
  if(var < 1.0e-10)vec_ = cvScalar(0); else vec_ /= sqrt(var);
  if((_w.dot(vec_)+ _b) > 0)return true; else return false;
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
//===========================================================================
void MFCheck::Load(const char* fname, bool binary)
{
  ifstream s;
  if(!binary){s.open(fname); assert(s.is_open()); this->Read(s);}
  else{ s.open(fname, std::ios::binary);  assert(s.is_open()); this->ReadBinary(s);}
  s.close(); return;
}
//===========================================================================
void MFCheck::Save(const char* fname, bool binary)
{
  ofstream s(fname); assert(s.is_open()); this->Write(s, binary);s.close(); return;
}
//===========================================================================
void MFCheck::Write(ofstream &s, bool binary)
{
  if(!binary)
    s << IO::MFCHECK << " " << _fcheck.size() << " ";
  else{
    int t =  IOBinary::MFCHECK;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    t = _fcheck.size();
    s.write((char*)(&t), sizeof(t));
  }
  
  for(size_t i = 0; i < _fcheck.size(); i++)_fcheck[i].Write(s, binary); return;
}
//===========================================================================
void MFCheck::Read(ifstream &s,bool readType)
{
  int type = -1;
  if(readType){s >> type; assert(type == IO::MFCHECK);}
  int n;
  
  s >> n;

  _fcheck.resize(n);
  
  for(int i = 0; i < n; i++)_fcheck[i].Read(s); return;
}
//===========================================================================
void MFCheck::ReadBinary(ifstream &s,bool readType)
{
  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::MFCHECK);
  }
  int n;

  s.read((char*)&n, sizeof(n));

  _fcheck.resize(n);
  
  for(int i = 0; i < n; i++)_fcheck[i].ReadBinary(s); return;
}
//===========================================================================
bool MFCheck::Check(int idx,cv::Mat &im,cv::Mat &s)
{
  assert((idx >= 0) && (idx < (int)_fcheck.size()));
  return _fcheck[idx].Check(im,s);
}
//===========================================================================
