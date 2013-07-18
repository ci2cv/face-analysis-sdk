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

#include <tracker/ShapeModel.hpp>
#define db at<double>
using namespace FACETRACKER;
using namespace std;
//=============================================================================
void FACETRACKER::GramSchmidt(cv::Mat &m)
{
  m.col(0) /= sqrt(m.col(0).dot(m.col(0)));
  for(int i = 1; i < m.cols; i++){
    for(int j = 0; j < i; j++){
      double v = m.col(j).dot(m.col(i));
      m.col(i) -= v*m.col(j);
    }
    m.col(i) /= sqrt(m.col(i).dot(m.col(i)));
  }return;
}
//===========================================================================
void FACETRACKER::AddOrthRow(cv::Mat &R)
{
  assert((R.rows == 3) && (R.cols == 3));
  R.db(2,0) = R.db(0,1)*R.db(1,2) - R.db(0,2)*R.db(1,1);
  R.db(2,1) = R.db(0,2)*R.db(1,0) - R.db(0,0)*R.db(1,2);
  R.db(2,2) = R.db(0,0)*R.db(1,1) - R.db(0,1)*R.db(1,0);
  return;
}
//=============================================================================
void FACETRACKER::MetricUpgrade(cv::Mat &R)
{
  assert((R.rows == 3) && (R.cols == 3));
  cv::SVD svd(R,cv::SVD::MODIFY_A);
  cv::Mat X = svd.u*svd.vt,W = cv::Mat::eye(3,3,CV_64F); 
  W.db(2,2) = determinant(X); R = svd.u*W*svd.vt; return;
}
//===========================================================================
void FACETRACKER::Euler2Rot(cv::Mat &R,const double pitch,const double yaw,
			    const double roll,bool full)
{
  if(full){if((R.rows != 3) || (R.cols != 3))R.create(3,3,CV_64F);}
  else{if((R.rows != 2) || (R.cols != 3))R.create(2,3,CV_64F);}
  double sina = sin(pitch), sinb = sin(yaw), sinc = sin(roll);
  double cosa = cos(pitch), cosb = cos(yaw), cosc = cos(roll);
  R.db(0,0) = cosb*cosc; R.db(0,1) = -cosb*sinc; R.db(0,2) = sinb;
  R.db(1,0) = cosa*sinc + sina*sinb*cosc;
  R.db(1,1) = cosa*cosc - sina*sinb*sinc;
  R.db(1,2) = -sina*cosb; if(full)AddOrthRow(R); return;
}
//===========================================================================
void FACETRACKER::Euler2Rot(cv::Mat &R,cv::Mat &p,bool full)
{
  assert((p.rows == 6) && (p.cols == 1));
  Euler2Rot(R,p.db(1,0),p.db(2,0),p.db(3,0),full); return;
}
//=============================================================================
void FACETRACKER::Rot2Euler(cv::Mat &R,double& pitch,double& yaw,double& roll)
{
  assert((R.rows == 3) && (R.cols == 3));
  double q[4];
  q[0] = sqrt(1+R.db(0,0)+R.db(1,1)+R.db(2,2))/2;
  q[1] = (R.db(2,1) - R.db(1,2)) / (4*q[0]) ;
  q[2] = (R.db(0,2) - R.db(2,0)) / (4*q[0]) ;
  q[3] = (R.db(1,0) - R.db(0,1)) / (4*q[0]) ;
  yaw  = asin(2*(q[0]*q[2] + q[1]*q[3]));
  pitch= atan2(2*(q[0]*q[1]-q[2]*q[3]),
	       q[0]*q[0]-q[1]*q[1]-q[2]*q[2]+q[3]*q[3]); 
  roll = atan2(2*(q[0]*q[3]-q[1]*q[2]),
	       q[0]*q[0]+q[1]*q[1]-q[2]*q[2]-q[3]*q[3]);
  return;
}
//=============================================================================
void FACETRACKER::Rot2Euler(cv::Mat &R,cv::Mat &p)
{
  assert((p.rows == 6) && (p.cols == 1));
  Rot2Euler(R,p.db(1,0),p.db(2,0),p.db(3,0)); return;
}
//=============================================================================
void FACETRACKER::Align3Dto2DShapes(double& scale,double& pitch,double& yaw,
				    double& roll,double& x,double& y,
				    cv::Mat &s2D,cv::Mat &s3D)
{
  assert((s2D.cols == 1) && (s3D.rows == 3*(s2D.rows/2)) && (s3D.cols == 1));
  int i,n = s2D.rows/2; double t2[2],t3[3];
  cv::Mat s2D_cpy = s2D.clone(),s3D_cpy = s3D.clone();
  cv::Mat X = (s2D_cpy.reshape(1,2)).t(),S = (s3D_cpy.reshape(1,3)).t();
  for(i = 0; i < 2; i++){cv::Mat v = X.col(i); t2[i] = sum(v)[0]/n; v-=t2[i];}
  for(i = 0; i < 3; i++){cv::Mat v = S.col(i); t3[i] = sum(v)[0]/n; v-=t3[i];}
	cv::Mat M = ((S.t()*S).inv(cv::DECOMP_CHOLESKY))*S.t()*X;
  cv::Mat MtM = M.t()*M; cv::SVD svd(MtM,cv::SVD::MODIFY_A);
  svd.w.db(0,0) = 1.0/sqrt(svd.w.db(0,0));
  svd.w.db(1,0) = 1.0/sqrt(svd.w.db(1,0));
  cv::Mat T(3,3,CV_64F);
  T(cv::Rect(0,0,3,2)) = svd.u*cv::Mat::diag(svd.w)*svd.vt*M.t();
  scale = 0.5*sum(T(cv::Rect(0,0,3,2)).mul(M.t()))[0];
  AddOrthRow(T); Rot2Euler(T,pitch,yaw,roll); T *= scale;
  x = t2[0] - (T.db(0,0)*t3[0] + T.db(0,1)*t3[1] + T.db(0,2)*t3[2]);
  y = t2[1] - (T.db(1,0)*t3[0] + T.db(1,1)*t3[1] + T.db(1,2)*t3[2]); return;
}
//=============================================================================
void FACETRACKER::Align3DShapes(cv::Mat &src,cv::Mat &dst,
				double &scale, double &pitch, double& yaw, 
				double& roll,double &tx, double &ty, double &tz)
{
  int n = src.rows/3; 
  assert((src.cols == 1) && (dst.rows == 3*n) && (dst.cols == 1));
  int i; double s,x,y,z;
  cv::Mat A(n,3,CV_64F),B(n,3,CV_64F),J(n,1,CV_64F);
  J = cv::Scalar(1.0);
  for(i = 0; i < n; i++){
    A.db(i,0)=src.db(i,0); A.db(i,1)=src.db(i+n,0); A.db(i,2)=src.db(i+n*2,0);
    B.db(i,0)=dst.db(i,0); B.db(i,1)=dst.db(i+n,0); B.db(i,2)=dst.db(i+n*2,0);
  }  
  cv::Mat AtA = A.t()*A,S = A.t()*B,AtJ = A.t()*J,BtJ = B.t()*J; 
  S -= (AtJ*BtJ.t())/n; AtA -= (AtJ*AtJ.t())/n;
  cv::SVD svd(S); cv::Mat R = svd.u*svd.vt; cv::Mat W = R.t()*S;
  s = (W.db(0,0)+W.db(1,1)+W.db(2,2))/(AtA.db(0,0)+AtA.db(1,1)+AtA.db(2,2));
  S = R.t(); FACETRACKER::Rot2Euler(S,pitch,yaw,roll); scale = s;
  for(i = 0; i < n; i++){
    x = A.db(i,0); y = A.db(i,1); z = A.db(i,2);
    B.db(i,0) = s*(x*S.db(0,0) + y*S.db(1,0) + z*S.db(2,0));
    B.db(i,1) = s*(x*S.db(0,1) + y*S.db(1,1) + z*S.db(2,1));
    B.db(i,2) = s*(x*S.db(0,2) + y*S.db(1,2) + z*S.db(2,2));
  }
  AtJ = B.t()*J;
  tx = (BtJ.db(0,0)-AtJ.db(0,0))/n;
  ty = (BtJ.db(1,0)-AtJ.db(1,0))/n;
  tz = (BtJ.db(2,0)-AtJ.db(2,0))/n; return;
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//===========================================================================
void LinearShapeModel::Clamp(cv::Mat &p,double c)
{
  assert((p.rows == _E.cols) && (p.cols == 1) && (p.type() == CV_64F));
  cv::MatIterator_<double> e  = _E.begin<double>();
  cv::MatIterator_<double> p1 =  p.begin<double>();
  cv::MatIterator_<double> p2 =  p.end<double>(); double v;
  for(; p1 != p2; ++p1,++e){
    v = c*sqrt(*e); if(fabs(*p1) > v){if(*p1 > 0.0)*p1=v; else *p1=-v;}
  }return;
}
//===========================================================================
void LinearShapeModel::CalcShape(cv::Mat &s,cv::Mat &params)
{
  int n = params.rows-this->nModes();
  cv::Mat plocal = params(cv::Rect(0,n,1,this->nModes()));
  cv::Mat pglobl = params(cv::Rect(0,0,1,n));
  this->CalcShape2D(s,plocal,pglobl); return;
}
//===========================================================================
void LinearShapeModel::CalcParams(cv::Mat &s,cv::Mat &params)
{
  int n = params.rows-this->nModes();
  cv::Mat plocal = params(cv::Rect(0,n,1,this->nModes()));
  cv::Mat pglobl = params(cv::Rect(0,0,1,n));
  this->CalcParams(s,plocal,pglobl); return;
}
//===========================================================================
void LinearShapeModel::CalcJacob(cv::Mat &params,cv::Mat &Jacob)
{
  int n = params.rows-this->nModes();
  cv::Mat plocal = params(cv::Rect(0,n,1,this->nModes()));
  cv::Mat pglobl = params(cv::Rect(0,0,1,n));
  this->CalcJacob(plocal,pglobl,Jacob); return;
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
PDM2D& PDM2D::operator= (PDM2D const&rhs)
{
  this->_n  = rhs._n; this->_T = rhs._T;
  this->_V  = rhs._V.clone();  this->_E  = rhs._E.clone();
  this->_M  = rhs._M.clone();  this->s_  = rhs.s_.clone(); return *this;
}
//=============================================================================
void PDM2D::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << IO::PDM2D << " ";
    IO::WriteMat(s,_V); 
    IO::WriteMat(s,_E); 
    IO::WriteMat(s,_M); 
    IO::WriteMat(s,_T); 
  }
  else{
    int t = IOBinary::PDM2D;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    IOBinary::WriteMat(s,_V); 
    IOBinary::WriteMat(s,_E); 
    IOBinary::WriteMat(s,_M); 
    IOBinary::WriteMat(s,_T); 
  }
  return;
}
//=============================================================================
void PDM2D::Read(ifstream &s,bool readType)
{
  int type; 
  if(readType){s >> type; assert(type == IO::PDM2D || type == IOBinary::PDM2D);}
  
    IO::ReadMat(s,_V); 
    IO::ReadMat(s,_E); 
    IO::ReadMat(s,_M); 
    IO::ReadMat(s,_T);

  _n = _M.rows/2; s_.create(_M.rows,1,CV_64F); return;
}
//=============================================================================
void PDM2D::ReadBinary(ifstream &s,bool readType)
{
  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::PDM2D);
  }

  IOBinary::ReadMat(s,_V); 
  IOBinary::ReadMat(s,_E); 
  IOBinary::ReadMat(s,_M); 
  IOBinary::ReadMat(s,_T);

  _n = _M.rows/2; s_.create(_M.rows,1,CV_64F); return;
}

//=============================================================================
void PDM2D::CalcShape2D(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl)
{
  assert((s.type() == CV_64F) && (plocal.type() == CV_64F) && 
	 (pglobl.type() == CV_64F));
  assert((plocal.rows == _E.cols) && (plocal.cols == 1));
  assert((pglobl.rows == 4) && (pglobl.cols == 1));
  if((s.rows != _M.rows) || (s.cols = 1))s.create(2*_n,1,CV_64F);
  double a=pglobl.db(0,0),b = pglobl.db(1,0),x=pglobl.db(2,0),y=pglobl.db(3,0);
  s_ = _M+_V*plocal;
  for(int i = 0; i < _n; i++){
    s.db(i   ,0) = a* s_.db(i   ,0) - b*s_.db(i+_n,0) + x;
    s.db(i+_n,0) = a* s_.db(i+_n,0) + b*s_.db(i   ,0) + y;
  }return;
}
//=============================================================================
void PDM2D::CalcParams(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl)
{
  assert((s.type() == CV_64F) && (s.rows == 2*_n) && (s.cols = 1));
  if((pglobl.rows != 4) || (pglobl.cols != 1) || (pglobl.type() != CV_64F))
    pglobl.create(4,1,CV_64F);  
  cv::Mat q(4,1,CV_64F),T(2,1,CV_64F),Ti(2,1,CV_64F);
  cv::Mat R(2,2,CV_64F),Ri(2,2,CV_64F); int i;
  double v1,v2 = std::sqrt(_n);
  cv::MatIterator_<double> mp = _M.begin<double>();
  cv::MatIterator_<double> sp =  s.begin<double>();
  cv::MatIterator_<double> tp = s_.begin<double>();
  for(i=0 , v1=0.0; i < 2*_n; i++,++mp,++sp,++tp){v1+=(*mp)*(*mp); *tp=*sp-*mp;}
  v1 = std::sqrt(v1); q = _T*s_;
  pglobl.db(0,0) = q.db(0,0)/v1 + 1.0; pglobl.db(1,0) = q.db(1,0)/v1;
  pglobl.db(2,0) = q.db(2,0)/v2;       pglobl.db(3,0) = q.db(3,0)/v2;
  T.db(0,0) = pglobl.db(2,0); T.db(1,0) = pglobl.db(3,0);
  R.db(0,0) = (R.db(1,1) = pglobl.db(0,0));
  R.db(0,1) = -1.0*(R.db(1,0) = pglobl.db(1,0));
  cv::invert(R,Ri,cv::DECOMP_SVD); Ti = -1.0*Ri*T;
  cv::MatIterator_<double> sx = s.begin<double>();
  cv::MatIterator_<double> sy = s.begin<double>()+_n;
  cv::MatIterator_<double> tx = s_.begin<double>();
  cv::MatIterator_<double> ty = s_.begin<double>()+_n;
  cv::MatIterator_<double> mx = _M.begin<double>();
  cv::MatIterator_<double> my = _M.begin<double>()+_n;
  for(i = 0; i < _n; i++,++mx,++my,++sx,++sy){
    *tx++ = Ri.db(0,0)*(*sx) + Ri.db(0,1)*(*sy) + Ti.db(0,0) - *mx;
    *ty++ = Ri.db(1,0)*(*sx) + Ri.db(1,1)*(*sy) + Ti.db(1,0) - *my;
  }
  plocal = _V.t()*s_; return;
}
//=============================================================================
void PDM2D::Init(cv::Mat &M,cv::Mat &V,cv::Mat &E)
{
  assert((M.type() == CV_64F) && (V.type() == CV_64F) && (E.type() == CV_64F));
  assert((V.rows == M.rows) && (V.cols == E.cols));
  _M = M.clone(); _V = V.clone(); _E = E.clone(); _n = _M.rows/2;
  s_.create(_M.rows,1,CV_64F);  this->CalcSimil(); return;
}
//=============================================================================
void PDM2D::Identity(cv::Mat &plocal,cv::Mat &pglobl)
{
  plocal = cv::Mat::zeros(_V.cols,1,CV_64F);
  pglobl = (cv::Mat_<double>(4,1) << 1, 0, 0, 0); return;
}
//=============================================================================
void PDM2D::CalcJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob)
{
  int i,j,m = _V.cols;
  assert((plocal.rows == m  ) && (plocal.cols == 1) && 
	 (pglobl.rows == 4  ) && (pglobl.cols == 1) &&
	 (Jacob.rows == 2*_n) && (Jacob.cols == 4+m));
  s_ = _M + _V*plocal;
  double a = pglobl.db(0,0),b = pglobl.db(1,0);
  cv::MatIterator_<double> sx =  s_.begin<double>();
  cv::MatIterator_<double> sy =  sx + _n;
  cv::MatIterator_<double> Jx =  Jacob.begin<double>();
  cv::MatIterator_<double> Jy =  Jx + _n*(4+m);
  cv::MatIterator_<double> Vx =  _V.begin<double>();
  cv::MatIterator_<double> Vy =  Vx + _n*m;
  for(i = 0; i < _n; i++,++sx,++sy){
    *Jx++ = *sx;        *Jy++ = *sy;
    *Jx++ = -1.0*(*sy); *Jy++ = *sx;
    *Jx++ = 1.0;        *Jy++ = 0.0;
    *Jx++ = 0.0;        *Jy++ = 1.0;
    for(j = 0; j < m; j++,++Vx,++Vy){
      *Jx++ = a*(*Vx) - b*(*Vy); *Jy++ = b*(*Vx) + a*(*Vy);
    }
  }return;
}
//=============================================================================
void PDM2D::CalcRigidJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob)
{
  int i,n = _M.rows/2,m = _V.cols;
  assert((plocal.rows == m)  && (plocal.cols == 1) && 
	 (pglobl.rows == 4)  && (pglobl.cols == 1) &&
	 (Jacob.rows == 2*n) && (Jacob.cols == 4));
  s_ = _M + _V*plocal;
  cv::MatIterator_<double> sx =  s_.begin<double>();
  cv::MatIterator_<double> sy =  sx + n;
  cv::MatIterator_<double> Jx =  Jacob.begin<double>();
  cv::MatIterator_<double> Jy =  Jx + n*4;
  for(i = 0; i < _n; i++,++sx,++sy){
    *Jx++ = *sx;        *Jy++ = *sy;
    *Jx++ = -1.0*(*sy); *Jy++ = *sx;
    *Jx++ = 1.0;        *Jy++ = 0.0;
    *Jx++ = 0.0;        *Jy++ = 1.0;
  }return;
}
//=============================================================================
void PDM2D::CalcReferenceUpdate(cv::Mat &dp,cv::Mat &plocal,cv::Mat &pglobl)
{
  assert((dp.rows == 4+_V.cols) && (dp.cols == 1));
  plocal += dp(cv::Rect(0,4,1,_V.cols));
  pglobl += dp(cv::Rect(0,0,1,4)); return;
}
//=============================================================================
void PDM2D::ApplySimT(double a,double b,double tx,double ty,cv::Mat &pglobl)
{
  assert((pglobl.rows == 4) && (pglobl.cols == 1) && (pglobl.type() == CV_64F));
  double ac = pglobl.db(0,0),bc = pglobl.db(1,0);
  double xc = pglobl.db(2,0),yc = pglobl.db(3,0);
  pglobl.db(0,0) = a*ac - b*bc;      pglobl.db(1,0) = a*bc + ac*b;
  pglobl.db(2,0) = a*xc - b*yc + tx; pglobl.db(3,0) = b*xc + a*yc + ty; return;
}
//=============================================================================
void PDM2D::CalcSimil()
{
  cv::Mat simil(4,2*_n,CV_64F);
  for(int i = 0; i < _n; i++){
    simil.db(0,i   ) = _M.db(i   ,0);      simil.db(0,i+_n) = _M.db(i+_n,0);
    simil.db(1,i   ) = -1.0*_M.db(i+_n,0); simil.db(1,i+_n) = _M.db(i,0);
    simil.db(2,i   ) = 1;                  simil.db(2,i+_n) = 0;
    simil.db(3,i   ) = 0;                  simil.db(3,i+_n) = 1;
  }
  _T = simil.t(); FACETRACKER::GramSchmidt(_T); _T = _T.t(); return;
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
PDM3D& PDM3D::operator= (PDM3D const& rhs)
{   
  this->_n  = rhs._n;
  this->_V  = rhs._V.clone();  this->_E  = rhs._E.clone();
  this->_M  = rhs._M.clone();  this->S_  = rhs.S_.clone();
  this->R_  = rhs.R_.clone();  this->s_  = rhs.s_.clone();
  this->P_  = rhs.P_.clone();  this->Px_ = rhs.Px_.clone();
  this->Py_ = rhs.Py_.clone(); this->Pz_ = rhs.Pz_.clone();
  this->R1_ = rhs.R1_.clone(); this->R2_ = rhs.R2_.clone(); 
  this->R3_ = rhs.R3_.clone(); return *this;
}
//=============================================================================
void PDM3D::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << IO::PDM3D << " ";
    IO::WriteMat(s,_V); IO::WriteMat(s,_E); IO::WriteMat(s,_M);
  }
  else{
    int t = IOBinary::PDM3D;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    IOBinary::WriteMat(s,_V); IOBinary::WriteMat(s,_E); IOBinary::WriteMat(s,_M);
  }
 return;
}
//===========================================================================
void PDM3D::Read(ifstream &s,bool readType)
{
  int type = -1;
  if(readType){s >> type; assert(type == IO::PDM3D);}
  
  IO::ReadMat(s,_V); IO::ReadMat(s,_E); IO::ReadMat(s,_M);
  
  _n = _M.rows/3; 
  S_.create(_M.rows,1,CV_64F);  
  R_.create(3,3,CV_64F); s_.create(_M.rows,1,CV_64F); P_.create(2,3,CV_64F);
  Px_.create(2,3,CV_64F); Py_.create(2,3,CV_64F); Pz_.create(2,3,CV_64F);
  R1_.create(3,3,CV_64F); R2_.create(3,3,CV_64F); R3_.create(3,3,CV_64F);
  return;
}
//===========================================================================
void PDM3D::ReadBinary(ifstream &s,bool readType)
{
  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::PDM3D);
  }
  
  IOBinary::ReadMat(s,_V); IOBinary::ReadMat(s,_E); IOBinary::ReadMat(s,_M);
  
  _n = _M.rows/3; 
  S_.create(_M.rows,1,CV_64F);  
  R_.create(3,3,CV_64F); s_.create(_M.rows,1,CV_64F); P_.create(2,3,CV_64F);
  Px_.create(2,3,CV_64F); Py_.create(2,3,CV_64F); Pz_.create(2,3,CV_64F);
  R1_.create(3,3,CV_64F); R2_.create(3,3,CV_64F); R3_.create(3,3,CV_64F);
  return;
}
//===========================================================================
void PDM3D::Project2D(cv::Mat &s,cv::Mat &S,cv::Mat &pglobl)
{
  assert((s.type() == CV_64F) && (S.type() == CV_64F) && 
	 (pglobl.type() == CV_64F));
  assert((S.rows == _M.rows) && (S.cols == _M.cols));
  assert((pglobl.rows == 6) && (pglobl.cols == 1));
  int n = _M.rows/3; double a=pglobl.db(0,0),x=pglobl.db(4,0),y=pglobl.db(5,0);
  if((s.rows != 2*n) || (s.cols = 1))s.create(2*n,1,CV_64F);
  Euler2Rot(R_,pglobl); 
  for(int i = 0; i < n; i++){
    s.db(i  ,0) = a*( R_.db(0,0)*S.db(i    ,0) + R_.db(0,1)*S.db(i+n  ,0) +
		      R_.db(0,2)*S.db(i+n*2,0) )+x;
    s.db(i+n,0) = a*( R_.db(1,0)*S.db(i    ,0) + R_.db(1,1)*S.db(i+n  ,0) +
		      R_.db(1,2)*S.db(i+n*2,0) )+y;
  }return;
}
//===========================================================================
void PDM3D::CalcShape2D(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl)
{
  assert((s.type() == CV_64F) && (plocal.type() == CV_64F) && 
	 (pglobl.type() == CV_64F));
  assert((plocal.rows == _E.cols) && (plocal.cols == 1));
  assert((pglobl.rows == 6) && (pglobl.cols == 1));
  int n = _M.rows/3; double a=pglobl.db(0,0),x=pglobl.db(4,0),y=pglobl.db(5,0);
  Euler2Rot(R_,pglobl); S_ = _M + _V*plocal;
  if((s.rows != 2*n) || (s.cols = 1))s.create(2*n,1,CV_64F);
  for(int i = 0; i < n; i++){
    s.db(i  ,0) = a*( R_.db(0,0)*S_.db(i    ,0) + R_.db(0,1)*S_.db(i+n  ,0) +
		      R_.db(0,2)*S_.db(i+n*2,0) )+x;
    s.db(i+n,0) = a*( R_.db(1,0)*S_.db(i    ,0) + R_.db(1,1)*S_.db(i+n  ,0) +
		      R_.db(1,2)*S_.db(i+n*2,0) )+y;
  }return;
}
//===========================================================================
void PDM3D::CalcShape3D(cv::Mat &s,cv::Mat &plocal)
{
  assert((s.type() == CV_64F) && (plocal.type() == CV_64F));
  assert((s.rows == _M.rows) && (s.cols = 1));
  assert((plocal.rows == _E.cols) && (plocal.cols == 1));
  s = _M + _V*plocal; return;
}
//===========================================================================
void PDM3D::CalcParams(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl)
{
  assert((s.type() == CV_64F) && (s.rows == 2*(_M.rows/3)) && (s.cols = 1));
  if((pglobl.rows != 6) || (pglobl.cols != 1) || (pglobl.type() != CV_64F))
    pglobl.create(6,1,CV_64F);
  int j,n = _M.rows/3; double si,scale,pitch,yaw,roll,tx,ty,Tx,Ty,Tz; 
  cv::Mat R(3,3,CV_64F),z(n,1,CV_64F),t(3,1,CV_64F),p(_V.cols,1,CV_64F);  
  cv::Mat r = R.row(2), S(this->nPoints(),3,CV_64F);
  plocal = cv::Mat::zeros(_V.cols,1,CV_64F); 
  for(int iter = 0; iter < 100; iter++){
    this->CalcShape3D(S_,plocal);
    Align3Dto2DShapes(scale,pitch,yaw,roll,tx,ty,s,S_);
    Euler2Rot(R,pitch,yaw,roll); S = (S_.reshape(1,3)).t();
    z = scale*S*r.t(); si = 1.0/scale; 
    Tx = -si*(R.db(0,0)*tx + R.db(1,0)*ty);
    Ty = -si*(R.db(0,1)*tx + R.db(1,1)*ty);
    Tz = -si*(R.db(0,2)*tx + R.db(1,2)*ty);
    for(j = 0; j < n; j++){
      t.db(0,0) = s.db(j,0); t.db(1,0) = s.db(j+n,0); t.db(2,0) = z.db(j,0);
      S_.db(j    ,0) = si*t.dot(R.col(0))+Tx;
      S_.db(j+n  ,0) = si*t.dot(R.col(1))+Ty;
      S_.db(j+n*2,0) = si*t.dot(R.col(2))+Tz;
    }
    plocal = _V.t()*(S_-_M); 
    if(iter > 0){if(cv::norm(plocal-p) < 1.0e-5)break;}
    plocal.copyTo(p);
  }
  pglobl.db(0,0) = scale; pglobl.db(1,0) = pitch;
  pglobl.db(2,0) = yaw;   pglobl.db(3,0) = roll;
  pglobl.db(4,0) = tx;    pglobl.db(5,0) = ty;
  return;
}
//===========================================================================
void PDM3D::CalcParams3D(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl)
{
  assert((s.type() == CV_64F) && (s.rows == _M.rows) && (s.cols = 1));
  if((pglobl.rows != 6) || (pglobl.cols != 1) || (pglobl.type() != CV_64F))
    pglobl.create(6,1,CV_64F);
  if((plocal.rows != this->nModes()) || (plocal.cols != 1) || 
     (plocal.type() != CV_64F))plocal = cv::Mat::zeros(_V.cols,1,CV_64F); 
  else plocal = cv::Scalar(0);
  int n = this->nPoints();
  double scale,pitch,yaw,roll,tx,ty,tz;
  cv::Mat S(3*n,1,CV_64F),R(3,3,CV_64F),Rt,p(this->nModes(),1,CV_64F);
  for(int iter = 0; iter < 100; iter++){
    this->CalcShape3D(S_,plocal);
    Align3DShapes(S_,s,scale,pitch,yaw,roll,tx,ty,tz);
    Euler2Rot(R,pitch,yaw,roll); Rt = R.t()/scale;
    for(int i = 0; i < n; i++){
      for(int j = 0; j < 3; j++)
	S.db(i+j*n,0) = 
	  Rt.db(j,0)*(s.db(i    ,0) - tx)+ 
	  Rt.db(j,1)*(s.db(i+n  ,0) - ty)+ 
	  Rt.db(j,2)*(s.db(i+n*2,0) - tz);
    }
    plocal = _V.t()*(S-_M);    
    if(iter > 0){if(cv::norm(plocal-p) < 1.0e-5)break;}
    plocal.copyTo(p);
  }

  pglobl.db(0,0) = scale; pglobl.db(1,0) = pitch;
  pglobl.db(2,0) = yaw;   pglobl.db(3,0) = roll;
  pglobl.db(4,0) = tx;    pglobl.db(5,0) = ty;
  return;
}
//===========================================================================
void PDM3D::Init(cv::Mat &M,cv::Mat &V,cv::Mat &E)
{
  assert((M.type() == CV_64F) && (V.type() == CV_64F) && (E.type() == CV_64F));
  assert((V.rows == M.rows) && (V.cols == E.cols));
  _M = M.clone(); _V = V.clone(); _E = E.clone(); _n = _M.rows/3;
  S_.create(_M.rows,1,CV_64F);  
  R_.create(3,3,CV_64F); s_.create(_M.rows,1,CV_64F); P_.create(2,3,CV_64F);
  Px_.create(2,3,CV_64F); Py_.create(2,3,CV_64F); Pz_.create(2,3,CV_64F);
  R1_.create(3,3,CV_64F); R2_.create(3,3,CV_64F); R3_.create(3,3,CV_64F);  
  return;
}
//===========================================================================
void PDM3D::Identity(cv::Mat &plocal,cv::Mat &pglobl)
{
  plocal = cv::Mat::zeros(_V.cols,1,CV_64F);
  pglobl = (cv::Mat_<double>(6,1) << 1, 0, 0, 0, 0, 0);
}
//===========================================================================
void PDM3D::CalcJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob)
{
  int i,j,n = _M.rows/3,m = _V.cols; double X,Y,Z;
  assert((plocal.rows == m)  && (plocal.cols == 1) && 
	 (pglobl.rows == 6)  && (pglobl.cols == 1) &&
	 (Jacob.rows == 2*n) && (Jacob.cols == 6+m));
  double s = pglobl.db(0,0);
  double rx[3][3] = {{0,0,0},{0,0,-1},{0,1,0}}; cv::Mat Rx(3,3,CV_64F,rx);
  double ry[3][3] = {{0,0,1},{0,0,0},{-1,0,0}}; cv::Mat Ry(3,3,CV_64F,ry);
  double rz[3][3] = {{0,-1,0},{1,0,0},{0,0,0}}; cv::Mat Rz(3,3,CV_64F,rz);
  this->CalcShape3D(S_,plocal); Euler2Rot(R_,pglobl); 
  P_ = s*R_(cv::Rect(0,0,3,2)); Px_ = P_*Rx; Py_ = P_*Ry; Pz_ = P_*Rz;
  assert(R_.isContinuous() && Px_.isContinuous() && 
	 Py_.isContinuous() && Pz_.isContinuous() && P_.isContinuous());
  const double* px = Px_.ptr<double>(0);
  const double* py = Py_.ptr<double>(0);
  const double* pz = Pz_.ptr<double>(0);
  const double* p  =  P_.ptr<double>(0);
  const double* r  =  R_.ptr<double>(0);
  cv::MatIterator_<double> Jx =  Jacob.begin<double>();
  cv::MatIterator_<double> Jy =  Jx + n*(6+m);
  cv::MatIterator_<double> Vx =  _V.begin<double>();
  cv::MatIterator_<double> Vy =  Vx + n*m;
  cv::MatIterator_<double> Vz =  Vy + n*m;
  for(i = 0; i < n; i++){
    X=S_.db(i,0); Y=S_.db(i+n,0); Z=S_.db(i+n*2,0);    
    *Jx++ =  r[0]*X +  r[1]*Y +  r[2]*Z;
    *Jy++ =  r[3]*X +  r[4]*Y +  r[5]*Z;
    *Jx++ = px[0]*X + px[1]*Y + px[2]*Z;
    *Jy++ = px[3]*X + px[4]*Y + px[5]*Z;
    *Jx++ = py[0]*X + py[1]*Y + py[2]*Z;
    *Jy++ = py[3]*X + py[4]*Y + py[5]*Z;
    *Jx++ = pz[0]*X + pz[1]*Y + pz[2]*Z;
    *Jy++ = pz[3]*X + pz[4]*Y + pz[5]*Z;
    *Jx++ = 1.0; *Jy++ = 0.0; *Jx++ = 0.0; *Jy++ = 1.0;
    for(j = 0; j < m; j++,++Vx,++Vy,++Vz){
      *Jx++ = p[0]*(*Vx) + p[1]*(*Vy) + p[2]*(*Vz);
      *Jy++ = p[3]*(*Vx) + p[4]*(*Vy) + p[5]*(*Vz);
    }
  }return;
}
//===========================================================================
void PDM3D::CalcRigidJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob)
{
  int i,n = _M.rows/3,m = _V.cols; double X,Y,Z;
  assert((plocal.rows == m)  && (plocal.cols == 1) && 
	 (pglobl.rows == 6)  && (pglobl.cols == 1) &&
	 (Jacob.rows == 2*n) && (Jacob.cols == 6));
  double rx[3][3] = {{0,0,0},{0,0,-1},{0,1,0}}; cv::Mat Rx(3,3,CV_64F,rx);
  double ry[3][3] = {{0,0,1},{0,0,0},{-1,0,0}}; cv::Mat Ry(3,3,CV_64F,ry);
  double rz[3][3] = {{0,-1,0},{1,0,0},{0,0,0}}; cv::Mat Rz(3,3,CV_64F,rz);
  double s = pglobl.db(0,0);
  this->CalcShape3D(S_,plocal); Euler2Rot(R_,pglobl); 
  P_ = s*R_(cv::Rect(0,0,3,2)); Px_ = P_*Rx; Py_ = P_*Ry; Pz_ = P_*Rz;
  assert(R_.isContinuous() && Px_.isContinuous() && 
	 Py_.isContinuous() && Pz_.isContinuous());
  const double* px = Px_.ptr<double>(0);
  const double* py = Py_.ptr<double>(0);
  const double* pz = Pz_.ptr<double>(0);
  const double* r  =  R_.ptr<double>(0);
  cv::MatIterator_<double> Jx = Jacob.begin<double>();
  cv::MatIterator_<double> Jy = Jx + n*6;
  for(i = 0; i < n; i++){
    X=S_.db(i,0); Y=S_.db(i+n,0); Z=S_.db(i+n*2,0);    
    *Jx++ =  r[0]*X +  r[1]*Y +  r[2]*Z;
    *Jy++ =  r[3]*X +  r[4]*Y +  r[5]*Z;
    *Jx++ = px[0]*X + px[1]*Y + px[2]*Z;
    *Jy++ = px[3]*X + px[4]*Y + px[5]*Z;
    *Jx++ = py[0]*X + py[1]*Y + py[2]*Z;
    *Jy++ = py[3]*X + py[4]*Y + py[5]*Z;
    *Jx++ = pz[0]*X + pz[1]*Y + pz[2]*Z;
    *Jy++ = pz[3]*X + pz[4]*Y + pz[5]*Z;
    *Jx++ = 1.0; *Jy++ = 0.0; *Jx++ = 0.0; *Jy++ = 1.0;
  }return;
}
//===========================================================================
void PDM3D::CalcReferenceUpdate(cv::Mat &dp,cv::Mat &plocal,cv::Mat &pglobl)
{
  assert((dp.rows == 6+_V.cols) && (dp.cols == 1));
  plocal += dp(cv::Rect(0,6,1,_V.cols));
  pglobl.db(0,0) += dp.db(0,0);
  pglobl.db(4,0) += dp.db(4,0);
  pglobl.db(5,0) += dp.db(5,0);
  Euler2Rot(R1_,pglobl); R2_ = cv::Mat::eye(3,3,CV_64F);
  R2_.db(1,2) = -1.0*(R2_.db(2,1) = dp.db(1,0));
  R2_.db(2,1) = -1.0*(R2_.db(0,2) = dp.db(2,0));
  R2_.db(0,1) = -1.0*(R2_.db(1,0) = dp.db(3,0));
  MetricUpgrade(R2_); R3_ = R1_*R2_; Rot2Euler(R3_,pglobl); return;
}
//===========================================================================
void PDM3D::ApplySimT(double a,double b,double tx,double ty,cv::Mat &pglobl)
{
  assert((pglobl.rows == 6) && (pglobl.cols == 1) && (pglobl.type()==CV_64F));
  double angle = atan2(b,a);
  double scale = a/cos(angle);
  double ca = cos(angle);
  double sa = sin(angle);
  double xc = pglobl.db(4,0);
  double yc = pglobl.db(5,0);
  R1_ = cv::Scalar(0);
  R1_.db(2,2) = 1.0;
  R1_.db(0,0) =  ca;
  R1_.db(0,1) = -sa;
  R1_.db(1,0) =  sa;
  R1_.db(1,1) =  ca;
  Euler2Rot(R2_,pglobl);
  R3_ = R1_*R2_; 
  pglobl.db(0,0) *= scale;
  Rot2Euler(R3_,pglobl);
  pglobl.db(4,0) = a*xc - b*yc + tx;
  pglobl.db(5,0) = b*xc + a*yc + ty;
  return;
}
//===========================================================================
const cv::Mat 
PDM3D::currentShape3D() const
{
  return S_;
}
//===========================================================================
