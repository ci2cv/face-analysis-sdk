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

#include "tracker/CLM.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#define it at<int>
#define db at<double>
#define SQR(x) x*x
using namespace FACETRACKER;
using namespace std;
//=============================================================================
void FACETRACKER::CalcSimT(cv::Mat &src,cv::Mat &dst,
			   double &a,double &b,double &tx,double &ty)
{
  assert((src.type() == CV_64F) && (dst.type() == CV_64F) && 
	 (src.rows == dst.rows) && (src.cols == dst.cols) && (src.cols == 1));
  int i,n = src.rows/2;
  cv::Mat H(4,4,CV_64F,cv::Scalar(0));
  cv::Mat g(4,1,CV_64F,cv::Scalar(0));
  cv::Mat p(4,1,CV_64F);
  cv::MatIterator_<double> ptr1x = src.begin<double>();
  cv::MatIterator_<double> ptr1y = src.begin<double>()+n;
  cv::MatIterator_<double> ptr2x = dst.begin<double>();
  cv::MatIterator_<double> ptr2y = dst.begin<double>()+n;
  for(i = 0; i < n; i++,++ptr1x,++ptr1y,++ptr2x,++ptr2y){
    H.db(0,0) += SQR(*ptr1x) + SQR(*ptr1y);
    H.db(0,2) += *ptr1x; H.db(0,3) += *ptr1y;
    g.db(0,0) += (*ptr1x)*(*ptr2x) + (*ptr1y)*(*ptr2y);
    g.db(1,0) += (*ptr1x)*(*ptr2y) - (*ptr1y)*(*ptr2x);
    g.db(2,0) += *ptr2x; g.db(3,0) += *ptr2y;
  }
  H.db(1,1) = H.db(0,0); H.db(1,2) = H.db(2,1) = -1.0*(H.db(3,0) = H.db(0,3));
  H.db(1,3) = H.db(3,1) = H.db(2,0) = H.db(0,2); H.db(2,2) = H.db(3,3) = n;
	cv::solve(H,g,p,cv::DECOMP_CHOLESKY);
  a = p.db(0,0); b = p.db(1,0); tx = p.db(2,0); ty = p.db(3,0); return;
}
//=============================================================================
void FACETRACKER::invSimT(double a1,double b1,double tx1,double ty1,
			  double& a2,double& b2,double& tx2,double& ty2)
{
  cv::Mat M = (cv::Mat_<double>(2,2) << a1, -b1, b1, a1);
	cv::Mat N = M.inv(cv::DECOMP_SVD); a2 = N.db(0,0); b2 = N.db(1,0);
  tx2 = -1.0*(N.db(0,0)*tx1 + N.db(0,1)*ty1);
  ty2 = -1.0*(N.db(1,0)*tx1 + N.db(1,1)*ty1); return;
}
//=============================================================================
void FACETRACKER::SimT(cv::Mat &s,double a,double b,double tx,double ty)
{
  assert((s.type() == CV_64F) && (s.cols == 1));
  int i,n = s.rows/2; double x,y; 
  cv::MatIterator_<double> xp = s.begin<double>(),yp = s.begin<double>()+n;
  for(i = 0; i < n; i++,++xp,++yp){
    x = *xp; y = *yp; *xp = a*x - b*y + tx; *yp = b*x + a*y + ty;    
  }return;
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
CLM& CLM::operator= (CLM const& rhs)
{
  this->_pdm = rhs._pdm;
  this->_plocal = rhs._plocal.clone();
  this->_pglobl = rhs._pglobl.clone();
  this->_refs = rhs._refs.clone();
  this->_cent.resize(rhs._cent.size());
  this->_visi.resize(rhs._visi.size());
  this->_patch.resize(rhs._patch.size());
  for(size_t i = 0; i < rhs._cent.size(); i++){
    this->_cent[i] = rhs._cent[i].clone();
    this->_visi[i] = rhs._visi[i].clone();
    this->_patch[i].resize(rhs._patch[i].size());
    for(size_t j = 0; j < rhs._patch[i].size(); j++)
      this->_patch[i][j] = rhs._patch[i][j];
  }
  this->cshape_ = rhs.cshape_.clone();
  this->bshape_ = rhs.bshape_.clone();
  this->oshape_ = rhs.oshape_.clone();  
  this->ms_ = rhs.cshape_.clone();
  this->u_  = rhs.u_.clone();
  this->g_  = rhs.g_.clone();
  this->J_  = rhs.J_.clone();
  this->H_  = rhs.H_.clone();
  this->prob_.resize(rhs.prob_.size());
  this->pmem_.resize(rhs.pmem_.size());
  this->wmem_.resize(rhs.pmem_.size());
  for(size_t i = 0; i < rhs.prob_.size(); i++){
    this->prob_[i] = rhs.prob_[i].clone();
    this->pmem_[i] = rhs.pmem_[i].clone();
    this->wmem_[i] = rhs.wmem_[i].clone();
  }return *this;
}
//=============================================================================
void CLM::Init(PDM3D &s,cv::Mat &r, std::vector<cv::Mat> &c,
	       std::vector<cv::Mat> &v,std::vector<std::vector<MPatch> > &p)
{
  size_t n = p.size(); assert((c.size() == n) && (v.size() == n));
  assert((r.type() == CV_64F) && (r.rows == 2*s.nPoints()) && (r.cols == 1));
  for(size_t i = 0; i < n; i++){
    assert(p[i].size() == (size_t)s.nPoints());
    assert((c[i].type() == CV_64F) && (c[i].rows == 3) && (c[i].cols == 1));
    assert((v[i].type() == CV_32S) && (v[i].rows == s.nPoints()) && 
	   (v[i].cols == 1));
  }
  _pdm = s; _refs = r.clone();_cent.resize(n);_visi.resize(n);_patch.resize(n);
  for(size_t i = 0; i < n; i++){
    _cent[i] = c[i].clone(); _visi[i] = v[i].clone();
    _patch[i].resize(p[i].size());
    for(size_t j = 0; j < p[i].size(); j++)_patch[i][j] = p[i][j];
  }
  _plocal.create(_pdm.nModes(),1,CV_64F);
  _pglobl.create(6,1,CV_64F);
  cshape_.create(2*_pdm.nPoints(),1,CV_64F);
  bshape_.create(2*_pdm.nPoints(),1,CV_64F);
  oshape_.create(2*_pdm.nPoints(),1,CV_64F);
  ms_.create(2*_pdm.nPoints(),1,CV_64F);
  u_.create(6+_pdm.nModes(),1,CV_64F);
  g_.create(6+_pdm.nModes(),1,CV_64F);
  J_.create(2*_pdm.nPoints(),6+_pdm.nModes(),CV_64F);
  H_.create(6+_pdm.nModes(),6+_pdm.nModes(),CV_64F);
  prob_.resize(_pdm.nPoints()); pmem_.resize(_pdm.nPoints()); 
  wmem_.resize(_pdm.nPoints()); return;
}
//=============================================================================
void CLM::Load(const char* fname, bool binary)
{
  ifstream s;
  if(!binary){s.open(fname); assert(s.is_open()); this->Read(s);}
  else {s.open(fname, std::ios::binary); assert(s.is_open()); this->ReadBinary(s);}
  s.close(); return;
}
//=============================================================================
void CLM::Save(const char* fname, bool binary)
{
  ofstream s;
  if(!binary){s.open(fname);}
  else {s.open(fname, std::ios::binary);}
  assert(s.is_open()); this->Write(s, binary);s.close(); return;
}
//=============================================================================
void CLM::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << IO::CLM << " " << _patch.size() << " "; 
    _pdm.Write(s); 
    IO::WriteMat(s,_refs);
    for(size_t i = 0; i < _cent.size(); i++)
      IO::WriteMat(s,_cent[i]);
    for(size_t i = 0; i < _visi.size(); i++)
      IO::WriteMat(s,_visi[i]);
    for(size_t i = 0; i < _patch.size(); i++){
      for(int j = 0; j < _pdm.nPoints(); j++)
	_patch[i][j].Write(s, binary);
    }
  }
  else{
    int t = IOBinary::CLM;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    t = _patch.size();
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    _pdm.Write(s, binary); 
    IOBinary::WriteMat(s,_refs);
    for(size_t i = 0; i < _cent.size(); i++)
      IOBinary::WriteMat(s,_cent[i]);
    for(size_t i = 0; i < _visi.size(); i++)
      IOBinary::WriteMat(s,_visi[i]);
    for(size_t i = 0; i < _patch.size(); i++){
      for(int j = 0; j < _pdm.nPoints(); j++){
	_patch[i][j].Write(s, binary);
	// if(j%46==0 && _patch[i][j]._p.size()){
	//   std::cout << i << " " <<j << std::endl;
	//   std::cout << _patch[i][j]._p[0]._W << std::endl;
	// }
      }
    }
  }
return;
}
//=============================================================================
void CLM::Read(ifstream &s,bool readType)
{
  if (readType) {
    int type;
    s >> type;
    assert(type == IO::CLM);
  }

  int n;
  s >> n;
  
  _kWidth = 36.;
  _pdm.Read(s);
  _cent.resize(n);
  _visi.resize(n);
  _patch.resize(n);
  _detectorsNCC.resize(n);
  IO::ReadMat(s,_refs);
  for (size_t i = 0; i < _cent.size(); i++)
    IO::ReadMat(s,_cent[i]);

  for (size_t i = 0; i < _visi.size(); i++)
    IO::ReadMat(s,_visi[i]);

  for (size_t i = 0; i < _patch.size(); i++) {
    _patch[i].resize(_pdm.nPoints());
    
    for (int j = 0; j < _pdm.nPoints(); j++)
      _patch[i][j].Read(s);

    _detectorsNCC.at(i)._patch = _patch[i];
    _detectorsNCC.at(i).setReferenceShape(_refs);
  }
  _plocal.create(_pdm.nModes(),1,CV_64F);
  _pglobl.create(6,1,CV_64F);
  cshape_.create(2*_pdm.nPoints(),1,CV_64F);
  bshape_.create(2*_pdm.nPoints(),1,CV_64F);
  oshape_.create(2*_pdm.nPoints(),1,CV_64F);
  ms_.create(2*_pdm.nPoints(),1,CV_64F);
  u_.create(6+_pdm.nModes(),1,CV_64F);
  g_.create(6+_pdm.nModes(),1,CV_64F);
  J_.create(2*_pdm.nPoints(),6+_pdm.nModes(),CV_64F);
  H_.create(6+_pdm.nModes(),6+_pdm.nModes(),CV_64F);
  prob_.resize(_pdm.nPoints());
  pmem_.resize(_pdm.nPoints()); 
  wmem_.resize(_pdm.nPoints());
  return;
}
//=============================================================================
void CLM::ReadBinary(ifstream &s,bool readType)
{
  if (readType) {
    int type;
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::CLM);
  }
  int n;
  
  _kWidth = 36.;
  s.read(reinterpret_cast<char*>(&n), sizeof(n));
  _pdm.ReadBinary(s);
  _cent.resize(n);
  _visi.resize(n);
  _patch.resize(n);
  _detectorsNCC.resize(n);
  IOBinary::ReadMat(s,_refs);

  for(size_t i = 0; i < _cent.size(); i++)
    IOBinary::ReadMat(s,_cent[i]);

  for(size_t i = 0; i < _visi.size(); i++)
    IOBinary::ReadMat(s,_visi[i]);

  for(int i = 0; i < n; i++){
    _patch[i].resize(_pdm.nPoints());

    for(int j = 0; j < _pdm.nPoints(); j++){
      _patch[i][j].ReadBinary(s);
    }
    _detectorsNCC.at(i)._patch = _patch[i]; //.setPatchExperts(_patch[i]);
    _detectorsNCC.at(i).setReferenceShape(_refs);
  }
  
  _plocal.create(_pdm.nModes(),1,CV_64F);
  _pglobl.create(6,1,CV_64F);
  cshape_.create(2*_pdm.nPoints(),1,CV_64F);
  bshape_.create(2*_pdm.nPoints(),1,CV_64F);
  oshape_.create(2*_pdm.nPoints(),1,CV_64F);
  ms_.create(2*_pdm.nPoints(),1,CV_64F);
  u_.create(6+_pdm.nModes(),1,CV_64F);
  g_.create(6+_pdm.nModes(),1,CV_64F);
  J_.create(2*_pdm.nPoints(),6+_pdm.nModes(),CV_64F);
  H_.create(6+_pdm.nModes(),6+_pdm.nModes(),CV_64F);
  prob_.resize(_pdm.nPoints());
  pmem_.resize(_pdm.nPoints());
  wmem_.resize(_pdm.nPoints());
  return;
}
//=============================================================================
int CLM::GetViewIdx()
{
  int idx = 0;
  if(this->nViews() == 1)return 0;
  else{
    int i; double v1,v2,v3,d,dbest = -1.0;
    for(i = 0; i < this->nViews(); i++){
      v1 = _pglobl.db(1,0) - _cent[i].db(0,0);
      v2 = _pglobl.db(2,0) - _cent[i].db(1,0);
      v3 = _pglobl.db(3,0) - _cent[i].db(2,0);
      d = v1*v1 + v2*v2 + v3*v3;
      if(dbest < 0 || d < dbest){dbest = d; idx = i;}
    }return idx;
  }
}
//=============================================================================
void CLM::Fit(cv::Mat& im, std::vector<int> &wSize,
	      int nIter,double clamp,double fTol)
{
  assert(im.type()==CV_8U);
  int idx;
  //int i,idx,n = _pdm.nPoints();
  double a1,b1,tx1,ty1,a2,b2,tx2,ty2;
  for(size_t witer = 0; witer < wSize.size(); witer++){
    _pdm.CalcShape2D(cshape_,_plocal,_pglobl);
    CalcSimT(_refs,cshape_,a1,b1,tx1,ty1);
    invSimT(a1,b1,tx1,ty1,a2,b2,tx2,ty2);
    idx = this->GetViewIdx();
	
	cv::Size wsz = cv::Size(wSize[witer], wSize[witer]);
    _detectorsNCC.at(idx).response(im, cshape_,
								   wsz, _visi[idx]);
    prob_ = _detectorsNCC.at(idx).getResponsesForRefShape();
	
    SimT(cshape_,a2,b2,tx2,ty2);
    _pdm.ApplySimT(a2,b2,tx2,ty2,_pglobl);
    cshape_.copyTo(bshape_);
    this->Optimize(idx,wSize[witer],nIter,fTol,clamp,1);
    this->Optimize(idx,wSize[witer],nIter,fTol,clamp,0);
    _pdm.ApplySimT(a1,b1,tx1,ty1,_pglobl);
  }return;
}
////=============================================================================
//void CLM::FitFwdAdd(cv::Mat& im, std::vector<int> &wSize,
//		    int nIter,double clamp,double fTol)
//{
//  assert(im.type()==CV_8U);
//  int i,idx,n = _pdm.nPoints(); double a1,b1,tx1,ty1;
//  for(size_t witer = 0; witer < wSize.size(); witer++){
//    //get current shape and transformation from reference
//    _pdm.CalcShape2D(cshape_,_plocal,_pglobl);
//    CalcSimT(_refs,cshape_,a1,b1,tx1,ty1);
//    idx = this->GetViewIdx();
//
//    //compute patch responses in reference frame
//	cv::Size wsz = cv::Size(wSize[witer], wSize[witer]);
//    //compute patch responses in reference frame
//    _detectorsNCC[idx].response(im, cshape_, wsz, _visi[idx]);
//    //transform responses to CLM's own reference frame
//    prob_ = _detectorsNCC[idx].getResponsesForRefShape(_refs);
//	
//	//transform landmark candidates to image frame
//    std::vector<cv::Mat> xloc(n),yloc(n); //int wsize = wSize[witer];
//#ifdef _OPENMP
//#pragma omp parallel for
//#endif
//    for(int i = 0; i < n; i++){
//      if(_visi[idx].rows == n){if(_visi[idx].it(i,0) == 0)continue;}
//      xloc[i].create(wsize,wsize,CV_64F);
//      yloc[i].create(wsize,wsize,CV_64F);
//      double tx = cshape_.db(i,0), ty = cshape_.db(i+n,0);
//      cv::MatIterator_<double> px = xloc[i].begin<double>();
//      cv::MatIterator_<double> py = yloc[i].begin<double>();
//      for(int y = 0; y < wsize; y++){
//		double vy = y-(wsize-1)/2;
//		for(int x = 0; x < wsize; x++){
//		  double vx = x-(wsize-1)/2;
//		  *px++ = a1*vx - b1*vy + tx;
//		  *py++ = b1*vx + a1*vy + ty;
//		}
//      }
//    }
//    //optimise
//    cshape_.copyTo(bshape_);
//    this->OptimizeFwdAdd(xloc,yloc,idx,wsize,nIter,fTol,clamp,1);
//    this->OptimizeFwdAdd(xloc,yloc,idx,wsize,nIter,fTol,clamp,0);
//  }return;
//}
//=============================================================================
void CLM::FitPrior(cv::Mat& im, std::vector<int> &wSize,
		   int nIter,double clamp,double fTol,double lambda,
		   void (*pfunc)(cv::Mat &im,cv::Mat &s, cv::Mat &dxdp,
				 cv::Mat &H,cv::Mat &g,void* data),
		   void* data)
{
  assert(im.type()==CV_8U);
  int idx;
  int n = _pdm.nPoints();
  double a1,b1,tx1,ty1;
  for(size_t witer = 0; witer < wSize.size(); witer++){
    //get current shape and transformation from reference
    _pdm.CalcShape2D(cshape_,_plocal,_pglobl);
    CalcSimT(_refs,cshape_,a1,b1,tx1,ty1);
    idx = this->GetViewIdx();
	
	
    //    std::cout << "visibility"<<idx << _visi[idx] << std::endl;
    //std::cout << "with Prior: "<< idx << std::endl;
    cv::Size wsz = cv::Size(wSize[witer], wSize[witer]);
    //compute patch responses in reference frame
    _detectorsNCC[idx].response(im, cshape_, wsz, _visi[idx]);
    prob_ = _detectorsNCC[idx].getResponsesForRefShape(_refs);
	
    //transform landmark candidates to image frame
    std::vector<cv::Mat> xloc(n),yloc(n);
    //   int wsize = wSize[witer];
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for(int i = 0; i < n; i++){
      cv::Size wsz = prob_[i].size();
      if(_visi[idx].rows == n){if(_visi[idx].it(i,0) == 0)continue;}
      xloc[i].create(wsz,CV_64F);
      yloc[i].create(wsz,CV_64F);
      double tx = cshape_.db(i,0), ty = cshape_.db(i+n,0);
      cv::MatIterator_<double> px = xloc[i].begin<double>();
      cv::MatIterator_<double> py = yloc[i].begin<double>();
      for(int y = 0; y < wsz.height; y++){
		double vy = y-(wsz.height-1)/2;
		for(int x = 0; x < wsz.width; x++){
		  double vx = x-(wsz.width-1)/2;
		  *px++ = a1*vx - b1*vy + tx;
		  *py++ = b1*vx + a1*vy + ty;
		}
      }
    }
    //optimise
    cshape_.copyTo(bshape_);
    //this->OptimizePrior(xloc,yloc,idx,wsize,nIter,fTol,clamp,1,lambda,im,
    //			pfunc,data);
    this->OptimizePrior(xloc,yloc,idx,wSize[witer],nIter,fTol,clamp,0,lambda,im,
						pfunc,data);
  }return;
}
//=============================================================================
void CLM::OptimizePrior(std::vector<cv::Mat> &xloc,
			std::vector<cv::Mat> &yloc,
			int idx,int // wSize
			,int nIter,
			double fTol,double clamp,bool rigid,
			double lambda,cv::Mat &im,
			void (*pfunc)(cv::Mat &im,cv::Mat &s, cv::Mat &dxdp,
				      cv::Mat &H,cv::Mat &g,void* data),
			void* data)
{
  int i,m=_pdm.nModes(),n=_pdm.nPoints();  
  double var,sigma=_pglobl.db(0,0)*_pglobl.db(0,0)/_kWidth;
  cv::Mat u,g,J,H; 
  if(rigid){
    u = u_(cv::Rect(0,0,1,6));   g = g_(cv::Rect(0,0,1,6)); 
    J = J_(cv::Rect(0,0,6,2*n)); H = H_(cv::Rect(0,0,6,6));
  }else{u = u_; g = g_; J = J_; H = H_;}
  
  for(size_t i=0; i<prob_.size();i++){
	if(prob_[i].cols!=0){
	  sigma *= prob_[i].total();
	  break;
	}
  }
  // std::cout<<"sigma: " << sigma << std::endl;
  for(int iter = 0; iter < nIter; iter++){
    _pdm.CalcShape2D(cshape_,_plocal,_pglobl);
    if(iter > 0){
	  if(cv::norm(cshape_,oshape_) < fTol)
		break;
	}
    cshape_.copyTo(oshape_);
    if(rigid)_pdm.CalcRigidJacob(_plocal,_pglobl,J);
    else     _pdm.CalcJacob(_plocal,_pglobl,J);
    pfunc(im,cshape_,J,H,g,data);
	H *= lambda; g *= lambda;
	
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for(i = 0; i < n; i++){
	  bool discard = false;
	  if(_visi[idx].rows == n){
		if(_visi[idx].it(i,0) == 0){
		  discard = true;
		}
	  }
	  if (prob_[i].empty()) {
		discard = true;
	  }
	  if(discard){
		cv::Mat Jx = J.row(i  ); Jx = cvScalar(0);
		cv::Mat Jy = J.row(i+n); Jy = cvScalar(0);
		ms_.db(i,0) = 0.0; ms_.db(i+n,0) = 0.0; continue;
	  }
	  double dx = cshape_.db(i,0),dy = cshape_.db(i+n,0);
	  cv::Size wsz = prob_[i].size();
	  double sigmai = sigma*wsz.width*wsz.height;
	  int ii,jj; double v,vx,vy,lx,ly,mx=0.0,my=0.0,sum=0.0;
	  cv::MatIterator_<double> p = prob_[i].begin<double>();
	  cv::MatIterator_<double> px = xloc[i].begin<double>();
	  cv::MatIterator_<double> py = yloc[i].begin<double>();
	  /*
	   cv::Mat vx = (xloc - dx);
	   cv::Mat vy = (yloc - dy);
	   cv::Mat v = prob * cv::exp(-.5*(vx.mul(vx)+vy.mul(vy))/sigma);
	   sum = cv::sum(v); mx = cv::sum(v.mul(vx)); my = cv::sum(v.mul(vy));
	   ms._db(i,0) = mx/sum; ms_.db(i+n, 0) = my/sum;
	   */
      for(ii = 0; ii < wsz.height; ii++){
      	for(jj = 0; jj < wsz.width; jj++){
      	  lx = *px++ - dx; vx = lx*lx;
      	  ly = *py++ - dy; vy = ly*ly;
      	  v = *p++; v *= exp(-0.5*(vx+vy)/(sigmai));
      	  sum += v;  mx += v*lx;  my += v*ly;
      	}
      }
      ms_.db(i,0) = mx/sum; ms_.db(i+n,0) = my/sum;
	}
	g += (1.0-lambda)*(J.t()*ms_); H += (1.0-lambda)*(J.t()*J);
	
    if(!rigid){
      for(i = 0; i < m; i++){
		var = (1.0-lambda)*0.5*sigma/_pdm._E.db(0,i);
		H.db(6+i,6+i) += var; g.db(6+i,0) -= var*_plocal.db(i,0);
      }
    }
	u_ = cvScalar(0); cv::solve(H,g,u,cv::DECOMP_CHOLESKY);
    _pdm.CalcReferenceUpdate(u_,_plocal,_pglobl);
    if(!rigid)_pdm.Clamp(_plocal,clamp);
  }return;
}
////=============================================================================
//void CLM::OptimizeFwdAdd(std::vector<cv::Mat> &xloc,
//			 std::vector<cv::Mat> &yloc,
//			 int idx,int wSize,int nIter,
//			 double fTol,double clamp,bool rigid)
//{
//  int i,m=_pdm.nModes(),n=_pdm.nPoints();
//  double var,sigma=_pglobl.db(0,0)*_pglobl.db(0,0)*(wSize*wSize)/36.0;
//  cv::Mat u,g,J,H;
//  if(rigid){
//    u = u_(cv::Rect(0,0,1,6));   g = g_(cv::Rect(0,0,1,6));
//    J = J_(cv::Rect(0,0,6,2*n)); H = H_(cv::Rect(0,0,6,6));
//  }else{u = u_; g = g_; J = J_; H = H_;}
//  for(int iter = 0; iter < nIter; iter++){
//    _pdm.CalcShape2D(cshape_,_plocal,_pglobl);
//    if(iter > 0){if(cv::norm(cshape_,oshape_) < fTol)break;}
//    cshape_.copyTo(oshape_);
//    if(rigid)_pdm.CalcRigidJacob(_plocal,_pglobl,J);
//    else     _pdm.CalcJacob(_plocal,_pglobl,J);
//#ifdef _OPENMP
//#pragma omp parallel for
//#endif
//    for(i = 0; i < n; i++){
//      if(_visi[idx].rows == n){
//		if(_visi[idx].it(i,0) == 0){
//		  cv::Mat Jx = J.row(i  ); Jx = cvScalar(0);
//		  cv::Mat Jy = J.row(i+n); Jy = cvScalar(0);
//		  ms_.db(i,0) = 0.0; ms_.db(i+n,0) = 0.0; continue;
//		}
//      }
//      double dx = cshape_.db(i,0),dy = cshape_.db(i+n,0);
//      int ii,jj; double v,vx,vy,lx,ly,mx=0.0,my=0.0,sum=0.0;
//      cv::MatIterator_<double> p = prob_[i].begin<double>();
//      cv::MatIterator_<double> px = xloc[i].begin<double>();
//      cv::MatIterator_<double> py = yloc[i].begin<double>();
//      for(ii = 0; ii < wSize; ii++){
//		for(jj = 0; jj < wSize; jj++){
//		  lx = *px++ - dx; vx = lx*lx;
//		  ly = *py++ - dy; vy = ly*ly;
//		  v = *p++; v *= exp(-0.5*(vx+vy)/sigma);
//		  sum += v;  mx += v*lx;  my += v*ly;
//		}
//      }
//      ms_.db(i,0) = mx/sum; ms_.db(i+n,0) = my/sum;
//    }
//    g = J.t()*ms_; H = J.t()*J;
//    if(!rigid){
//      for(i = 0; i < m; i++){
//		var = 0.5*sigma/_pdm._E.db(0,i);
//		H.db(6+i,6+i) += var; g.db(6+i,0) -= var*_plocal.db(i,0);
//      }
//    }
//	u_ = cvScalar(0); cv::solve(H,g,u,cv::DECOMP_CHOLESKY);
//    _pdm.CalcReferenceUpdate(u_,_plocal,_pglobl);
//    if(!rigid)_pdm.Clamp(_plocal,clamp);
//  }return;
//}
//=============================================================================
//void CLM::Fit(cv::Mat& im, cv::Mat &mu,cv::Mat &cov,
//	      std::vector<int> &wSize,
//	      int nIter,double clamp,double fTol,double lambda)
//{
//  assert(im.type()==CV_8U); 
//  int i,idx,n = _pdm.nPoints(); double a1,b1,tx1,ty1,a2,b2,tx2,ty2;
//  for(size_t witer = 0; witer < wSize.size(); witer++){
//    _pdm.CalcShape2D(cshape_,_plocal,_pglobl);
//    CalcSimT(_refs,cshape_,a1,b1,tx1,ty1);
//    invSimT(a1,b1,tx1,ty1,a2,b2,tx2,ty2);
//    idx = this->GetViewIdx();
//#ifdef _OPENMP
//#pragma omp parallel for
//#endif
//    for(i = 0; i < n; i++){
//      if(_visi[idx].rows == n){if(_visi[idx].it(i,0) == 0)continue;}
//      int w = wSize[witer]+_patch[idx][i]._w - 1; 
//      int h = wSize[witer]+_patch[idx][i]._h - 1;
//      cv::Mat sim = 
//	(cv::Mat_<float>(2,3)<<a1,-b1,cshape_.db(i,0),b1,a1,cshape_.db(i+n,0));
//      if((w>wmem_[i].cols) || (h>wmem_[i].rows))wmem_[i].create(h,w,CV_32F);
//      cv::Mat wimg = wmem_[i](cv::Rect(0,0,w,h));
//      CvMat wimg_o = wimg,sim_o = sim; IplImage im_o = im;
//      cvGetQuadrangleSubPix(&im_o,&wimg_o,&sim_o);
//      if(wSize[witer] > pmem_[i].rows)
//	pmem_[i].create(wSize[witer],wSize[witer],CV_64F);
//      prob_[i] = pmem_[i](cv::Rect(0,0,wSize[witer],wSize[witer]));
//      _patch[idx][i].Response(wimg,prob_[i]);
//    }
//    SimT(cshape_,a2,b2,tx2,ty2); 
//    _pdm.ApplySimT(a2,b2,tx2,ty2,_pglobl);
//    cshape_.copyTo(bshape_);
//
//    //transform prior
//    cv::Mat T = cv::Mat::zeros(2*n,2*n,CV_64F);
//    for(i = 0; i < n; i++){
//      T.db(i  ,i  ) =  a2; T.db(i  ,i+n) = -b2;
//      T.db(i+n,i  ) =  b2; T.db(i+n,i+n) =  a2;
//    }
//    cv::Mat mu_s = T*mu,cov_s = T*cov*T.t();
//    for(i = 0; i < n; i++){
//      mu_s.db(i,0) += tx2; mu_s.db(i+n,0) += ty2;
//      //cov_s.db(i,i) += 1e-3;
//      //cov_s.db(i+n,i+n) += 1e-3;
//    }
//    ////////////////////////////////////////////////////////////////////////////
//    cv::Mat C = cv::Mat::zeros(2*n,2*n,CV_64F);
//    for(int i = 0; i < n; i++){ 
//      C.db(i,i) = cov_s.db(i,i); 
//      C.db(i,i+n) = cov_s.db(i,i+n);
//      C.db(i+n,i) = cov_s.db(i+n,i);
//      C.db(i+n,i+n) = cov_s.db(i+n,i+n);
//    }
//    cov_s = C;/////////////////////////////////////////////////////////////////
//    cv::Mat covi; cv::invert(cov_s,covi,cv::DECOMP_CHOLESKY);
//    this->Optimize(idx,mu_s,covi,wSize[witer],nIter,fTol,clamp,lambda);
//    _pdm.ApplySimT(a1,b1,tx1,ty1,_pglobl);
//  }return;
//}
//=============================================================================
//void CLM::Optimize(int idx,cv::Mat &mu,cv::Mat &covi,int wSize,int nIter,
//		   double fTol,double clamp,double lambda)
//{
//  int i,n=_pdm.nPoints();  
//  double sigma=(wSize*wSize)/36.0; cv::Mat u=u_,g=g_,J=J_,H=H_; 
//  for(int iter = 0; iter < nIter; iter++){
//    _pdm.CalcShape2D(cshape_,_plocal,_pglobl);
//    if(iter > 0){if(cv::norm(cshape_,oshape_) < fTol)break;}
//    cshape_.copyTo(oshape_); _pdm.CalcJacob(_plocal,_pglobl,J);
//    double gamma = sigma*lambda;
//    H = gamma*(J.t()*covi*J); g = gamma*(J.t()*covi*(mu-cshape_));
//#ifdef _OPENMP
//#pragma omp parallel for
//#endif
//    for(i = 0; i < n; i++){
//      if(_visi[idx].rows == n){
//		if(_visi[idx].it(i,0) == 0){
//		  cv::Mat Jx = J.row(i  ); Jx = cvScalar(0);
//		  cv::Mat Jy = J.row(i+n); Jy = cvScalar(0);
//		  ms_.db(i,0) = 0.0; ms_.db(i+n,0) = 0.0; continue;
//		}
//      }
//      double dx = cshape_.db(i  ,0) - bshape_.db(i  ,0) + (wSize-1)/2;
//      double dy = cshape_.db(i+n,0) - bshape_.db(i+n,0) + (wSize-1)/2;
//      int ii,jj; double v,vx,vy,mx=0.0,my=0.0,sum=0.0;      
//      cv::MatIterator_<double> p = prob_[i].begin<double>();
//      for(ii = 0; ii < wSize; ii++){
//		vx = (dy-ii)*(dy-ii);
//		for(jj = 0; jj < wSize; jj++){
//		  vy = (dx-jj)*(dx-jj);
//		  v = *p++; v *= exp(-0.5*(vx+vy)/sigma);
//		  sum += v;  mx += v*jj;  my += v*ii;
//		}
//      }
//      ms_.db(i,0) = mx/sum - dx; ms_.db(i+n,0) = my/sum - dy;
//    }
//    g += J.t()*ms_; H += J.t()*J;
//	  u_ = cvScalar(0); cv::solve(H,g,u,cv::DECOMP_CHOLESKY);
//    _pdm.CalcReferenceUpdate(u_,_plocal,_pglobl); _pdm.Clamp(_plocal,clamp);
//  }return;
//}
//=============================================================================
void CLM::Optimize(int idx,int wSize,int nIter,
		   double fTol,double clamp,bool rigid)
{
  int m=_pdm.nModes(),n=_pdm.nPoints();  
  double var,sigma=(wSize*wSize)/_kWidth; cv::Mat u,g,J,H;
  if(rigid){
    u = u_(cv::Rect(0,0,1,6));   g = g_(cv::Rect(0,0,1,6)); 
    J = J_(cv::Rect(0,0,6,2*n)); H = H_(cv::Rect(0,0,6,6));
  }else{u = u_; g = g_; J = J_; H = H_;}
  for (size_t i=0; i<prob_.size(); i++) {
	if(!prob_[i].empty()){
	  sigma = prob_[i].total()/_kWidth;
	  break;
	}
  }
  if(sigma ==0) sigma = wSize*wSize/_kWidth;
  
  for(int iter = 0; iter < nIter; iter++){
    _pdm.CalcShape2D(cshape_,_plocal,_pglobl);
    if(iter > 0){if(cv::norm(cshape_,oshape_) < fTol)break;}
    cshape_.copyTo(oshape_);
    if(rigid)_pdm.CalcRigidJacob(_plocal,_pglobl,J);
    else     _pdm.CalcJacob(_plocal,_pglobl,J);
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for(int i = 0; i < n; i++){
	  bool discard = false;
      if(_visi[idx].rows == n){
		if(_visi[idx].it(i,0) == 0){
		  discard = true;
		}
      }
	  if(prob_[i].empty()) discard = true;
	  if(discard){
		cv::Mat Jx = J.row(i  ); Jx = cvScalar(0);
		cv::Mat Jy = J.row(i+n); Jy = cvScalar(0);
		ms_.db(i,0) = 0.0; ms_.db(i+n,0) = 0.0; continue;
	  }
      double dx = cshape_.db(i  ,0) - bshape_.db(i  ,0) + (wSize-1)/2;
      double dy = cshape_.db(i+n,0) - bshape_.db(i+n,0) + (wSize-1)/2;
	  cv::Size wsz = prob_[i].size();
      double sigmai = wsz.width*wsz.height/_kWidth;
      int ii,jj; double v,vx,vy,mx=0.0,my=0.0,sum=0.0;      
      cv::MatIterator_<double> p = prob_[i].begin<double>();
      for(ii = 0; ii < wsz.height; ii++){
		vx = (dy-ii)*(dy-ii);
		for(jj = 0; jj < wsz.width; jj++){
		  vy = (dx-jj)*(dx-jj);
		  v = *p++; v *= exp(-0.5*(vx+vy)/sigmai);
		  sum += v;  mx += v*jj;  my += v*ii;
		}
	}
      ms_.db(i,0) = mx/sum - dx; ms_.db(i+n,0) = my/sum - dy;
    }
    g = J.t()*ms_; H = J.t()*J;
    if(!rigid){
      for(int i = 0; i < m; i++){
		var = 0.5*sigma/_pdm._E.db(0,i);
		H.db(6+i,6+i) += var; g.db(6+i,0) -= var*_plocal.db(i,0);
      }
    }
	  u_ = cvScalar(0); cv::solve(H,g,u,cv::DECOMP_CHOLESKY);
    _pdm.CalcReferenceUpdate(u_,_plocal,_pglobl);
    if(!rigid)_pdm.Clamp(_plocal,clamp);
  }return;
}
//==============================================================================
