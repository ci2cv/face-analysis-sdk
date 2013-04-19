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

#include <tracker/RegistrationCheck.hpp>
using namespace FACETRACKER;
using namespace std;
//===========================================================================
void 
RegistrationCheck::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << IO::REGOCHECK << " " << _a << " " << _b << " " << _c << " ";
    IO::WriteMat(s,_w);
  }
  else{
    int t = IOBinary::REGOCHECK;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    s.write(reinterpret_cast<char*>(&_a), sizeof(_a));
    s.write(reinterpret_cast<char*>(&_b), sizeof(_b));
    s.write(reinterpret_cast<char*>(&_c), sizeof(_c));
    IOBinary::WriteMat(s, _w);
  }
  _paw.Write(s, binary); return;
}
//===========================================================================
void 
RegistrationCheck::Read(ifstream &s,
			bool readType)
{
  int type; 
  if(readType){s >> type; assert(type == IO::REGOCHECK);}
  s >> _a >> _b >> _c; IO::ReadMat(s,_w);
 
  _paw.Read(s); return;
}
//===========================================================================
void 
RegistrationCheck::ReadBinary(ifstream &s,
			      bool readType)
{
 
  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::REGOCHECK);
  }


    s.read(reinterpret_cast<char*>(&_a), sizeof(_a));
    s.read(reinterpret_cast<char*>(&_b), sizeof(_b));
    s.read(reinterpret_cast<char*>(&_c), sizeof(_c));
    IOBinary::ReadMat(s, _w);

  
  _paw.ReadBinary(s); return;
}
//===========================================================================
int                
RegistrationCheck::Check(cv::Mat &im,
			 cv::Mat &s)
{
  if((s.rows != _paw._src.rows) || (s.cols != 1) || (s.type() != CV_64F))
    return -1;
  _paw.Crop(im,crop_,s); _paw.VectorizeUchar(crop_,vec_);
  cv::equalizeHist(vec_,vec_); vec_.convertTo(x_,CV_64F);
  cv::normalize(x_,x_); double val = _w.dot(x_) + _b;
  double prob = 1.0/(1.0 + exp( val * _a + _c )); return int(prob*10 + 0.5);
}
//===========================================================================
void 
mvRegistrationCheck::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << IO::MVREGOCHECK << " " << int(_rego.size()) << " ";
  }
  else{
    int t = IOBinary::MVREGOCHECK;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    t = _rego.size();
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
  }
  for(int i = 0; i < int(_rego.size()); i++)_rego[i].Write(s, binary);
}
//===========================================================================
void 
mvRegistrationCheck::Read(ifstream &s,
			  bool readType)
{
  int type; 
  if(readType){s >> type; assert(type == IO::MVREGOCHECK);}
  int N; 
  s >> N; 
  _rego.resize(N); for(int i=0;i<N;i++)_rego[i].Read(s);
}
//===========================================================================
void 
mvRegistrationCheck::ReadBinary(ifstream &s,
			  bool readType)
{
  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::MVREGOCHECK);
  }
  
  int N;
  s.read(reinterpret_cast<char*>(&N), sizeof(N));
  
  _rego.resize(N); for(int i=0;i<N;i++)_rego[i].ReadBinary(s);
}
//===========================================================================
int
mvRegistrationCheck::Check(cv::Mat &im,
			   cv::Mat &s,
			   int idx)
{
  if((idx < 0) || (idx >= int(_rego.size())))return -1;
  else return _rego[idx].Check(im,s);
}
//===========================================================================
