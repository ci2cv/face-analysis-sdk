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

#include <tracker/ATM.hpp>
#include <opencv/highgui.h>
#include <iostream>
#define it at<int>
#define db at<double>
using namespace FACETRACKER;
using namespace std;
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
void ATM::Init(std::vector<cv::Mat> &center,cv::Mat &pose,
		  cv::Mat &shape,cv::Mat &im,cv::Mat &tri,const double scale)
{  
  cv::Mat s = shape.clone(); s *= scale; _warp.Init(s,tri); 
  _center = center; this->AllocMemory(); _scale = scale;
  this->UpdateTemplate(im,shape,pose,1000,0); _init = true; return;
}
//==============================================================================
void ATM::UpdateTemplate(cv::Mat &im,cv::Mat &s,cv::Mat &pose,
			 const int tmax,const double tol)
{
  int idx=0; double dmin=0;
  for(int i = 0; i < int(_center.size()); i++){
    double d = cv::norm(_center[i],pose);
    if((i == 0) || (d < dmin)){idx = i; dmin = d;}
  }
  _warp.Crop(im,crop__,s); _warp.Vectorize(crop__,vec__); 
  if(int(_T[idx].size()) < tmax){
    cv::Mat tt = vec__.clone(); _T[idx].push_back(tt);
  }else{
    int imax=0,imin=0; double vmax=0,vmin=0;
    for(int i = 1; i < int(_T[idx].size()); i++){
      double v = cv::norm(_T[idx][i],vec__);
      if(i == 1){imax = i; imin = i; vmax = v; vmin = v;}
      else{
	if(vmax < v){imax = i; vmax = v;}
	if(vmin > v){imin = i; vmin = v;}
      }
    }
    if(vmin > tol)vec__.copyTo(_T[idx][imax]);
  }return;
}
//=============================================================================
void ATM::AllocMemory()
{
  vec__.create(_warp.nPix(),1,CV_64F);
  vecx__.create(_warp.nPix(),1,CV_64F);
  vecy__.create(_warp.nPix(),1,CV_64F);
  vecw__.create(_warp.nPix(),1,CV_64F);
  crop__.create(_warp.Height(),_warp.Width(),CV_8U);
  cropx__.create(_warp.Height(),_warp.Width(),CV_32F);
  cropy__.create(_warp.Height(),_warp.Width(),CV_32F);
  pixtri__ = _warp.PixTri(); dWdx__ = _warp.dWdx(pixtri__); 
  int N = _center.size();
  Ja__.resize(N); Ha__.resize(N); na__.resize(N); _T.resize(N);
  for(int i=0;i<N;i++){_T[i].resize(0); na__[i]=0;}
  return;
}
//=============================================================================
bool ATM::CheckUpdate(cv::Mat &im,cv::Mat &s,cv::Mat &pose,
		      double tol)
{
  int idx=0; double dmin=0;
  for(int i = 0; i < int(_center.size()); i++){
    double d = cv::norm(_center[i],pose);
    if((i == 0) || (d < dmin)){idx = i; dmin = d;}
  }
  if(na__[idx] < 1)return true;
  _warp.Crop(im,crop__,s); _warp.Vectorize(crop__,vec__); 
  double vmin=cv::norm(_T[idx][0],vec__);
  for(int i = 0; i < int(_T[idx].size()); i++){
    double v = cv::norm(_T[idx][i],vec__);
    if(vmin > v)vmin = v;
  }
  if(vmin > tol)return true; else return false;
}
//=============================================================================
int ATM::Update(cv::Mat &im,cv::Mat &s,
		cv::Mat &dxdp, cv::Mat &pose,
		const int tmax,const double tol)
{
  int idx=0; double dmin=0;
  for(int i = 0; i < int(_center.size()); i++){
    double d = cv::norm(_center[i],pose);
    if((i == 0) || (d < dmin)){idx = i; dmin = d;}
  }
  int updated = 0;
  if(int(_T[idx].size()) < tmax){
    this->CalcJacob(im,s,dxdp,J__,vec__);
    cv::Mat tt = vec__.clone(); _T[idx].push_back(tt);
    if(na__[idx] == 0){
      Ha__[idx] = J__.t()*J__; 
      Ja__[idx] = J__.clone();
      na__[idx] = 1;
    }else{
      Ha__[idx] = (J__.t()*J__  + na__[idx]*Ha__[idx])/(1.0+na__[idx]); 
      Ja__[idx] = (J__          + na__[idx]*Ja__[idx])/(1.0+na__[idx]); 
      na__[idx] += 1;
    }
    updated = 1;
  }else{
    _warp.Crop(im,crop__,s); _warp.Vectorize(crop__,vec__); 
    int imax=0,imin=0; double vmax=0,vmin=0;
    for(int i = 1; i < int(_T[idx].size()); i++){
      double v = cv::norm(_T[idx][i],vec__);
      if(i == 1){imax = i; imin = i; vmax = v; vmin = v;}
      else{
	if(vmax < v){imax = i; vmax = v;}
	if(vmin > v){imin = i; vmin = v;}
      }
    }
    if(vmin > tol){
      this->CalcJacob(im,s,dxdp,J__,vec__);
      vec__.copyTo(_T[idx][imax]);
      if(na__[idx] == 0){
	Ha__[idx] = J__.t()*J__; 
	Ja__[idx] = J__.clone();
	na__[idx] = 1;
      }else{
	/*
	if(na__[idx] < 10){
	  Ha__[idx] = (J__.t()*J__  + na__[idx]*Ha__[idx])/(1.0+na__[idx]); 
	  Ja__[idx] = (J__          + na__[idx]*Ja__[idx])/(1.0+na__[idx]); 
	  na__[idx] += 1;
	}else{
	  double alpha=0.3;
	  Ha__[idx] = alpha*J__.t()*J__ + (1.0-alpha)*Ha__[idx]; 
	  Ja__[idx] = alpha*J__ + (1.0-alpha)*Ja__[idx]; 
	  if(na__[idx] < 100)na__[idx] += 1; 
	}
	*/
	//better performance without average!!!!!!!!!!!!!!!!!!!!!!!!!!
	Ha__[idx] = J__.t()*J__; Ja__[idx] = J__; 
	if(na__[idx] < 100)na__[idx] += 1; 	
      }
      updated = 1;
    }
  }return updated;
}
//=============================================================================
void ATM::CalcJacob(cv::Mat &im,cv::Mat &s, cv::Mat &dxdp,
		    cv::Mat &J,cv::Mat &vec)
{
  cv::Sobel(im,imx__,CV_32F,1,0,3); imx__ /= 8;
  cv::Sobel(im,imy__,CV_32F,0,1,3); imy__ /= 8;
  if((J.cols != dxdp.cols) || (J.rows != _warp.nPix()))
    J.create(_warp.nPix(),dxdp.cols,CV_64F);
  _warp.Crop(im,crop__,s); 
  cv::remap(imx__,cropx__,_warp._mapx,_warp._mapy,CV_INTER_LINEAR);
  cv::remap(imy__,cropy__,_warp._mapx,_warp._mapy,CV_INTER_LINEAR);
  _warp.Vectorize(crop__,vec);
  _warp.Vectorize(cropx__,vecx__);
  _warp.Vectorize(cropy__,vecy__);
  int N = _warp.nPix(),n = s.rows/2,m=dxdp.cols;
  cv::MatIterator_<int> pt = pixtri__.begin<int>();
  cv::MatIterator_<double> wi = dWdx__.begin<double>();
  cv::MatIterator_<double> wj = dWdx__.begin<double>()+N;
  cv::MatIterator_<double> wk = dWdx__.begin<double>()+N*2;
  cv::MatIterator_<double> ix = vecx__.begin<double>();
  cv::MatIterator_<double> iy = vecy__.begin<double>();
  cv::MatIterator_<double> Jp = J.begin<double>();
  for(int q = 0; q < N; q++){
    int t = *pt++; double vi=*wi++,vj=*wj++,vk=*wk++,vx=*ix++,vy=*iy++; 
    int i=_warp._tri.it(t,0), j=_warp._tri.it(t,1), k=_warp._tri.it(t,2);
    cv::MatIterator_<double> xi = dxdp.begin<double>()+i*m;
    cv::MatIterator_<double> xj = dxdp.begin<double>()+j*m;
    cv::MatIterator_<double> xk = dxdp.begin<double>()+k*m;
    cv::MatIterator_<double> yi = dxdp.begin<double>()+(i+n)*m;
    cv::MatIterator_<double> yj = dxdp.begin<double>()+(j+n)*m;
    cv::MatIterator_<double> yk = dxdp.begin<double>()+(k+n)*m;
    for(int l=0;l<m;l++,++xi,++xj,++xk,++yi,++yj,++yk)
      *Jp++ = vx*(*xi*vi + *xj*vj + *xk*vk) + 
	      vy*(*yi*vi + *yj*vj + *yk*vk);
  }return;
}
//=============================================================================
void ATM::BuildLinearSystem(cv::Mat &im,cv::Mat &s,
			    cv::Mat &dxdp, cv::Mat &pose,
			    cv::Mat &H,cv::Mat &g)
{
  int idx=0; double dmin=0;
  for(int i = 0; i < int(_center.size()); i++){
    double d = cv::norm(_center[i],pose);
    if((i == 0) || (d < dmin)){idx = i; dmin = d;}
  }
  int N = 0; for(int i = 0; i < int(_T.size()); i++)N += int(_T[i].size());
  _warp.Crop(im,crop__,s); _warp.Vectorize(crop__,vec__);
  cv::Mat e(N,1,CV_64F); double sum = 0.0; int k = 0;
  for(int i = 0; i < int(_T.size()); i++){
    for(int j = 0; j < int(_T[i].size()); j++,k++){
      double v = cv::norm(_T[i][j],vec__); 
      e.db(k,0) = v*v; sum += (e.db(k,0));
    }
  }
  double var = sum/N; sum = 0.0; k = 0;
  for(int i = 0; i < int(_T.size()); i++){
    // WEIGHT BY DISTANCE FROM POSE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for(int j = 0; j < int(_T[i].size()); j++,k++){
      double v = exp(-0.5*e.db(k,0)/var); sum += (e.db(k,0) = v);
    }
  }
  if(sum < 1e-8){
    printf("ERROR(%s,%d): Weights are too small: %f!\n",
	   __FILE__,__LINE__,(float)sum); abort();
  }
  e /= sum; vecw__ = cv::Scalar(0); k = 0;
  for(int i = 0; i < int(_T.size()); i++){
    for(int j = 0; j < int(_T[i].size()); j++,k++)vecw__ += e.db(k,0)*_T[i][j];
  }
  if(na__[idx] < 1){
    this->CalcJacob(im,s,dxdp,J__,vec__);
    H = J__.t()*J__; g = J__.t()*(vecw__-vec__); 
  }else{Ha__[idx].copyTo(H); g = Ja__[idx].t()*(vecw__-vec__);}
  return;
}
//=============================================================================
