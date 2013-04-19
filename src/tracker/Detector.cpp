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

#include <opencv2/core/core.hpp>

#include "Detector.hpp"
#include "IO.hpp"

using namespace FACETRACKER;
using namespace std;
#include <tracker/BasicDefines.hpp>

//===========================================================================
int
nextMultipleOf4(int v)
{
  int k = (v%4)!=0;
  return (v/4)*4 + k*4;
}
//===========================================================================
void removeMean(cv::Mat &shape, double &tx, double &ty)
{
  cv::Mat avg;
  int n = shape.rows;
  shape = shape.reshape(0,2);
  cv::reduce(shape, avg, 1, CV_REDUCE_AVG);
  shape.row(0) = shape.row(0) - avg.rl(0,0);
  shape.row(1) = shape.row(1) - avg.rl(1,0);

  shape = shape.reshape(0, n);

  tx = avg.rl(0,0);
  ty = avg.rl(1,0);
}
//===========================================================================
void Align2DShapes(cv::Mat &src,cv::Mat &dst,double &scale,double &theta)
{
  int n = src.rows/2;
  assert((src.cols == 1) && (dst.rows == 2*n) && (dst.cols == 1));
  int i; double a,b,c;
  for(i = 0, a = b = c = 0.0; i < n; i++){
    a += src.rl(i,0)*src.rl(i  ,0)+src.rl(i+n,0)*src.rl(i+n,0);
    b += src.rl(i,0)*dst.rl(i  ,0)+src.rl(i+n,0)*dst.rl(i+n,0);
    c += src.rl(i,0)*dst.rl(i+n,0)-src.rl(i+n,0)*dst.rl(i  ,0);
  }
  b /= a; c /= a; scale = std::sqrt(b*b+c*c); theta = std::atan2(c,b); return;
}
//===========================================================================
void CalcSimT(cv::Mat&src, cv::Mat&dst,
	      cv::Mat &M)
{
  assert((src.type() == CV_REAL) && (dst.type() == CV_REAL) && 
	 (src.rows == dst.rows) && (src.cols == dst.cols) && (src.cols == 1));
  int i,n = src.rows/2;
  cv::Mat H(4,4,CV_REAL,cv::Scalar(0));
  cv::Mat g(4,1,CV_REAL,cv::Scalar(0));
  cv::Mat p(4,1,CV_REAL);
  cv::MatIterator_<double> ptr1x = src.begin<double>();
  cv::MatIterator_<double> ptr1y = src.begin<double>()+n;
  cv::MatIterator_<double> ptr2x = dst.begin<double>();
  cv::MatIterator_<double> ptr2y = dst.begin<double>()+n;
  for(i = 0; i < n; i++,++ptr1x,++ptr1y,++ptr2x,++ptr2y){
    H.rl(0,0) += SQR(*ptr1x) + SQR(*ptr1y);
    H.rl(0,2) += *ptr1x; H.rl(0,3) += *ptr1y;
    g.rl(0,0) += (*ptr1x)*(*ptr2x) + (*ptr1y)*(*ptr2y);
    g.rl(1,0) += (*ptr1x)*(*ptr2y) - (*ptr1y)*(*ptr2x);
    g.rl(2,0) += *ptr2x; g.rl(3,0) += *ptr2y;
  }
  H.rl(1,1) = H.rl(0,0); H.rl(1,2) = H.rl(2,1) = -1.0*(H.rl(3,0) = H.rl(0,3));
  H.rl(1,3) = H.rl(3,1) = H.rl(2,0) = H.rl(0,2); H.rl(2,2) = H.rl(3,3) = n;
  cv::solve(H,g,p,cv::DECOMP_CHOLESKY);
  M = (cv::Mat_<real>(2,3) << p.rl(0,0), -p.rl(1,0), p.rl(2,0), 
       p.rl(1,0), p.rl(0,0), p.rl(3,0));
  //  a = p.rl(0,0); b = p.rl(1,0); tx = p.rl(2,0); ty = p.rl(3,0); 
  return;  
}
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
std::vector<cv::Mat>
Detector::getResponsesForRefShape(double scale)
{
  std::vector<cv::Mat> resp(prob_.size());

  if(scale == 1.)
    return prob_;
  else{    
    cv::Size sz (prob_.at(0).cols / scale, prob_.at(0).rows/scale);

    bool t = false;
    if(sz.width%2==0){ sz.width++; t = true;}
    if(sz.height%2==0){ sz.height++; t = true;}
    //   std::cout << "expanded" << std::endl;
    for(size_t i=0; i<prob_.size(); i++){
      cv::Mat m;
      if(prob_.at(i).empty()) continue;
      cv::resize(prob_.at(i), m, sz);
      
      resp.at(i) = m;
    }
    return resp;
  }
}
//===========================================================================
std::vector<cv::Mat>
Detector::getResponsesForRefShape(cv::Mat r, double *sc_)
{
  std::vector<cv::Mat> resp(prob_.size());

  if(r.empty())
    return prob_;
  else{
    cv::Mat sh = r.clone();
    double tx, ty;
    removeMean(sh, tx, ty);
    double theta, scale;
    Align2DShapes(sh, _refs_zm, scale, theta);
    if(fabs(theta) > .1){
      std::cerr << "Reference shapes can only differ in scale" << std::endl;
      exit(-1);
    }
    if(sc_) *sc_ = scale;
    if(fabs(scale-1.)<1e-2)
      return prob_;

    return getResponsesForRefShape(scale);
  }
}
//===========================================================================

std::vector<cv::Mat>
Detector::getResponsesForRefShape(cv::Size wSize, cv::Mat r)
{
  std::vector<cv::Mat> resp(prob_.size());

  if(wSize.width ==0 || r.empty())
    return prob_;
  else if(!r.empty()){
     cv::Mat sh = r.clone(); 
    double tx, ty;
    removeMean(sh, tx, ty);
    double theta, scale;
    Align2DShapes(sh, _refs_zm, scale, theta);
    if(fabs(theta) > .1){
      std::cerr << "Reference shapes can only differ in scale" << std::endl;
      exit(-1);
    }
    if(fabs(scale-1.)<1e-2)
      return prob_;
    
    cv::Size sz;
    for (size_t i=0; i<prob_.size(); i++) {
      if(!prob_.at(i).empty()){
	sz = cv::Size(prob_.at(i).cols / scale, prob_.at(i).rows/scale);
	break;
      }
    }
    if(wSize.width == 0)
      wSize = sz;


    //if the target size wSize is bigger than the resize response, set
    //the border to zero. We have to allocate the responses in this case
    cv::Rect respRect, probRect;
    respRect.x = MAX((wSize.width - sz.width)/2, 0);
    respRect.y = MAX((wSize.height - sz.height)/2,0);
    
    probRect.x = MAX((sz.width - wSize.width)/2, 0);
    probRect.y = MAX((sz.height - wSize.height)/2, 0);

    probRect.width = respRect.width = MIN(wSize.width, sz.width);
    probRect.height = respRect.height = MIN(wSize.height, sz.height);

    for(size_t i=0; i<prob_.size(); i++){
      cv::Mat m;
      if(prob_.at(i).empty()) continue;
      cv::resize(prob_.at(i), m, sz);
      
      if(respRect.x>0 || respRect.y>0){
	resp.at(i) = cv::Mat::zeros(wSize, m.type());
	m(probRect).copyTo(resp.at(i)(respRect));
      }
      else
	resp.at(i) = m(probRect);
    }
    return resp;
  }
  else{
    std::vector<cv::Mat> resp(prob_.size());
    
    for(size_t i=0; i<prob_.size(); i++){
      if(prob_.at(i).empty()) continue;
      cv::resize(prob_.at(i), resp.at(i), wSize);
    }
    return resp;
  }

}
//===========================================================================

void
Detector::setReferenceShape(cv::Mat& s)
{
  // std::cerr << "Warning "<<__FUNCTION__<<": this functionality is deprecated" 
  //	       << std::endl;
  _refs = s.clone();
  _refs_zm = _refs.clone();
  double tx, ty;
  removeMean(_refs_zm, tx, ty);

}

//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================

DetectorNCC::DetectorNCC(std::string file, bool binary)
{

  Load(file,binary);
  
}

// void
// DetectorNCC::setPatchExperts(std::vector<MPatch>& p)
// {
//
//   std::cerr << "Warning "<<__FUNCTION__<<": this function is deprecated" 
// 	    << std::endl;
//   _patch = p;
//
// }

void
DetectorNCC::ReadBinary(std::ifstream &s, bool readType)
{
  if(readType){int type;
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::DETECTOR_NCC);
  }
  
  IOBinary::ReadMat(s, _refs);

  int t;
  s.read(reinterpret_cast<char*>(&t), sizeof(t));
  _patch.resize(t);
  for(int i=0; i<t; i++)
    _patch[i].ReadBinary(s);

  _refs_zm = _refs.clone();
  double tx, ty;
  removeMean(_refs_zm, tx, ty);

}

void
DetectorNCC::Write(std::ofstream &s, bool binary)
{
  assert(binary);

  int t = IOBinary::DETECTOR_NCC;
  s.write((char*)&t, sizeof(t));

  IOBinary::WriteMat(s, _refs);

  t = _patch.size();
  s.write((char*)&t, sizeof(t));
  for(size_t i=0; i< _patch.size(); i++)
    _patch[i].Write(s, true);
}

void
DetectorNCC::Read(std::ifstream &, bool)
{
  std::cerr << "Reading DetectorNCC objects from text files is not supported" 
	    << std::endl;
  exit(-1);
}

void
DetectorNCC::Load(std::string fname, bool binary)
{
  std::ifstream s;
  assert(binary);
  s.open(fname.c_str(), std::ios::binary);
  assert(s.is_open());
  this->ReadBinary(s);
  s.close();
}

void
DetectorNCC::Save(std::string fname, bool binary)
{
  std::ofstream s;
  assert(binary);
  s.open(fname.c_str(), std::ios::binary);
  assert(s.is_open());
  this->Write(s, true);
  s.close();
}

bool
DetectorNCC::response(cv::Mat & im, cv::Mat & sh,
		      cv::Size wSize,
		      cv::Mat& visi)
{

  cv::Mat simTransfMat;
  CalcSimT(_refs, sh, simTransfMat);

  int n = sh.rows/2;
  cv::Mat shape = sh.reshape(0,2);

  prob_.resize(n);
  wmem_.resize(n);
  pmem_.resize(n);

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(int i=0; i<n; i++){
    if(visi.it(i,0) ==0) continue;
    int w = wSize.width + _patch[i]._w - 1; 
    int h = wSize.height + _patch[i]._h - 1;
    simTransfMat.rl(0,2) = shape.rl(0,i);
    simTransfMat.rl(1,2) = shape.rl(1,i);
 
    if((w>wmem_[i].cols) || (h>wmem_[i].rows))
      wmem_[i].create(h,nextMultipleOf4(w),CV_32F);
    
    cv::Mat wimg = wmem_[i](cv::Rect(0,0,w,h));
    CvMat wimg_o = wimg,sim_o = simTransfMat; IplImage im_o = im;
    cvGetQuadrangleSubPix(&im_o,&wimg_o,&sim_o);
    if(wSize.height > pmem_[i].rows)
      pmem_[i].create(wSize.height, nextMultipleOf4(wSize.width), CV_REAL);
    
    prob_[i] = pmem_[i](cv::Rect(cv::Point(0,0),wSize));
    _patch[i].Response(wimg,prob_[i]);
  }

  return true;
}

