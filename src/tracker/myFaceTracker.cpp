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

#include <tracker/myFaceTracker.hpp>
#define it at<int>
#define db at<double>
using namespace FACETRACKER;
using namespace std;
//==============================================================================
struct myInitData{
  bool calculate; cv::Mat mu,cov,covi; //praFacePredictor *pra;
};
struct myTrackData1{
  cv::Mat img; myFaceTracker* tracker;
};
struct myTrackData2{
  bool calculate; cv::Mat mu,cov,covi,img; myFaceTracker* tracker; double gamma;
};
//==============================================================================
void 
myInitFunc(cv::Mat &/*im*/,   //image containing object
	   cv::Mat &s,    //current shape describing object
	   cv::Mat &dxdp, //shape model jacobian at shape
	   cv::Mat &H,    //contains Hessian on return
	   cv::Mat &g,    //contains gradient on return
	   void* data)    //additional data
{
  myInitData* d = (myInitData*)data;
  // if(d->calculate){
  //   d->pra->Predict(im,s,d->mu,d->cov,d->covi);
  //   if(d->mu.rows == 0){
  //     H = cv::Mat::zeros(dxdp.cols,dxdp.cols,CV_64F);
  //     g = cv::Mat::zeros(dxdp.cols,1,CV_64F);
  //   }else{H = dxdp.t()*d->covi*dxdp; g = dxdp.t()*d->covi*(d->mu - s); }
  // }else
  {H = dxdp.t()*d->covi*dxdp; g = dxdp.t()*d->covi*(d->mu - s); }
  d->calculate = false; return;
}
//==============================================================================
void 
myTrackFunc1(cv::Mat &/*im*/,   //image containing object
	     cv::Mat &s,        //current shape describing object
	     cv::Mat &dxdp,     //shape model jacobian at shape
	     cv::Mat &H,        //contains Hessian on return
	     cv::Mat &g,        //contains gradient on return
	     void* data)        //additional data
{
  myTrackData1* d = (myTrackData1*)data;
  cv::Mat pose = d->tracker->_clm._pglobl(cv::Rect(0,1,1,3));
  d->tracker->_atm.BuildLinearSystem(d->img,s,dxdp,pose,H,g); return;
}
//==============================================================================
void 
myTrackFunc2(cv::Mat &/*im*/,   //image containing object
	     cv::Mat &s,    //current shape describing object
	     cv::Mat &dxdp, //shape model jacobian at shape
	     cv::Mat &H,    //contains Hessian on return
	     cv::Mat &g,    //contains gradient on return
	     void* data)    //additional data
{
  myTrackData2* d = (myTrackData2*)data;
  cv::Mat pose = d->tracker->_clm._pglobl(cv::Rect(0,1,1,3));
  d->tracker->_atm.BuildLinearSystem(d->img,s,dxdp,pose,H,g);   
  //  if(d->calculate){d->tracker->_ksmooth.Predict(im,s,d->mu,d->cov,d->covi);}
  if(d->mu.rows > 0){
    H *= d->gamma; g *= d->gamma;
    H += (1.0-d->gamma)*dxdp.t()*d->covi*dxdp;
    g += (1.0-d->gamma)*dxdp.t()*d->covi*(d->mu - s); 
  }
  d->calculate = false; return;
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
myFaceTrackerParams::myFaceTrackerParams()
{
  type = IO::MYFACETRACKERPARAMS;
  init_wSize.resize(3); init_wSize[0] = 11; init_wSize[1] = 9; init_wSize[2]=7; 
  track_wSize.resize(1); track_wSize[0] = 7; 
  timeDet = -1;
  itol = 10;
  ftol = 0.1;
  clamp = 3; 
  init_lambda = 0.5;
  track_lambda = 0.01;
  gamma = 0.9;
  init_type = 0; //change to 0. PRA was not used in any case
  track_type = 0;
  shape_predict = false;
  check_health = true;
  
  atm_tri = cv::Mat();
  atm_scale = 0.25;
  atm_thresh = 100;
  atm_ntemp = 2;
  
  ksmooth_size = cv::Size(32,32);
  ksmooth_sigma = 0.01;
  ksmooth_noise = 0.1;
  ksmooth_thresh = 0.01;
  ksmooth_ntemp = 10;  
  
  center.resize(9);
  center[0] = (cv::Mat_<double>(3,1) << 0, 0, 0);
  center[1] = (cv::Mat_<double>(3,1) << 0, 3.14/20, 0);
  center[2] = (cv::Mat_<double>(3,1) << 3.14/20, 3.14/20, 0);
  center[3] = (cv::Mat_<double>(3,1) << 3.14/20, 0, 0);
  center[4] = (cv::Mat_<double>(3,1) << 3.14/20, -3.14/20, 0);
  center[5] = (cv::Mat_<double>(3,1) << 0, -3.14/20, 0);
  center[6] = (cv::Mat_<double>(3,1) << -3.14/20, -3.14/20, 0);
  center[7] = (cv::Mat_<double>(3,1) << -3.14/20, 0, 0);
  center[8] = (cv::Mat_<double>(3,1) << -3.14/20, 3.14/20, 0);
  
  visi.resize(0);
}
//=============================================================================
void myFaceTrackerParams::Save(const char* fname, bool binary)
{
  ofstream file;
  if(!binary) file.open(fname); 
  else file.open(fname, std::ios::binary);
  assert(file.is_open());
  
  if(!binary){
    file << IO::MYFACETRACKERPARAMS << " "
	 << timeDet << " "
	 << itol << " "
	 << ftol << " " 
	 << clamp << " "
	 << init_lambda << " " 
	 << track_lambda << " "
	 << gamma << " "
	 << init_type << " "
	 << track_type << " "
	 << int(shape_predict) << " ";
    file << int(init_wSize.size()) << " ";
    for(int i = 0; i < int(init_wSize.size()); i++)file << init_wSize[i] << " ";
    file << int(track_wSize.size()) << " ";
    for(int i = 0; i < int(track_wSize.size()); i++)file << track_wSize[i] << " ";
    file << int(center.size()) << " ";
    for(int i = 0; i < int(center.size()); i++)IO::WriteMat(file,center[i]);
    file << int(visi.size()) << " ";
    for(int i = 0; i < int(visi.size()); i++)IO::WriteMat(file,visi[i]);
    if(atm_tri.rows > 0){file << 1 << " ";IO::WriteMat(file,atm_tri);}
    else file << 0 << " ";
    file << atm_scale << " "
	 << atm_thresh << " "
	 << atm_ntemp << " "
	 << ksmooth_size.width << " " 
	 << ksmooth_size.height << " "
	 << ksmooth_sigma << " "
	 << ksmooth_noise << " "
	 << ksmooth_thresh << " "
	 << ksmooth_ntemp << " ";
  }
  else{
    int t = IOBinary::MYFACETRACKERPARAMS;
    file.write(reinterpret_cast<char*>(&t), sizeof(t));
    file.write(reinterpret_cast<char*>(&timeDet), sizeof(timeDet));
    file.write(reinterpret_cast<char*>(&itol), sizeof(itol));
    file.write(reinterpret_cast<char*>(&ftol), sizeof(ftol));
    file.write(reinterpret_cast<char*>(&clamp), sizeof(clamp));
    file.write(reinterpret_cast<char*>(&init_lambda), sizeof(init_lambda));
    file.write(reinterpret_cast<char*>(&track_lambda), sizeof(track_lambda));
    file.write(reinterpret_cast<char*>(&gamma), sizeof(gamma));
    file.write(reinterpret_cast<char*>(&init_type), sizeof(init_type));
    file.write(reinterpret_cast<char*>(&track_type), sizeof(track_type));
    file.write(reinterpret_cast<char*>(&shape_predict), sizeof(shape_predict));
    t = init_wSize.size();
    file.write(reinterpret_cast<char*>(&t), sizeof(t));
    file.write(reinterpret_cast<char*>(&init_wSize[0]), sizeof(int)*init_wSize.size());
    
    t = track_wSize.size();
    file.write(reinterpret_cast<char*>(&t), sizeof(t));
    file.write(reinterpret_cast<char*>(&track_wSize[0]), sizeof(int)*track_wSize.size());

    t = center.size();
    file.write(reinterpret_cast<char*>(&t), sizeof(t));
    for(int i = 0; i < int(center.size()); i++)IOBinary::WriteMat(file,center[i]);
    
    t = visi.size();
    file.write(reinterpret_cast<char*>(&t), sizeof(t));
    for(int i = 0; i < int(visi.size()); i++)IOBinary::WriteMat(file,visi[i]);
    
    if(atm_tri.rows > 0){
      t = 1;
      file.write(reinterpret_cast<char*>(&t), sizeof(t));
      IOBinary::WriteMat(file,atm_tri);
    }
    else{
      t = 0;
      file.write(reinterpret_cast<char*>(&t), sizeof(t));
    }
    
    file.write(reinterpret_cast<char*>(&atm_scale), sizeof(atm_scale));
    file.write(reinterpret_cast<char*>(&atm_thresh), sizeof(atm_thresh));
    file.write(reinterpret_cast<char*>(&atm_ntemp), sizeof(atm_ntemp));
    file.write(reinterpret_cast<char*>(&ksmooth_size), sizeof(ksmooth_size));
    file.write(reinterpret_cast<char*>(&ksmooth_sigma), sizeof(ksmooth_sigma));
    file.write(reinterpret_cast<char*>(&ksmooth_noise), sizeof(ksmooth_noise));
    file.write(reinterpret_cast<char*>(&ksmooth_thresh), sizeof(ksmooth_thresh));
    file.write(reinterpret_cast<char*>(&ksmooth_ntemp), sizeof(ksmooth_ntemp));
  }


  file.close();
}
//=============================================================================
void myFaceTrackerParams::Load(const char* fname, bool binary)
{
  ifstream file;
  if(!binary){
    file.open(fname);  assert(file.is_open());
    int t; file >> t; assert(t == IO::MYFACETRACKERPARAMS);
 
    file >> timeDet
	 >> itol
	 >> ftol
	 >> clamp
	 >> init_lambda
	 >> track_lambda
	 >> gamma
	 >> init_type
	 >> track_type
	 >> t; shape_predict = t;
    file >> t; init_wSize.resize(t);
    for(int i = 0; i < int(init_wSize.size()); i++)file >> init_wSize[i];
    file >> t; track_wSize.resize(t);
    for(int i = 0; i < int(track_wSize.size()); i++)file >> track_wSize[i];
    file >> t; center.resize(t);
    for(int i = 0; i < int(center.size()); i++)IO::ReadMat(file,center[i]);
    file >> t; visi.resize(t);
    for(int i = 0; i < int(visi.size()); i++)IO::ReadMat(file,visi[i]);
    file >> t; if(t == 1)IO::ReadMat(file,atm_tri); else atm_tri = cv::Mat();
    file >> atm_scale
	 >> atm_thresh
	 >> atm_ntemp
	 >> ksmooth_size.width
	 >> ksmooth_size.height
	 >> ksmooth_sigma
	 >> ksmooth_noise
	 >> ksmooth_thresh
	 >> ksmooth_ntemp;
  }
  else{
    file.open(fname, std::ios::binary);
    {int type; 
      file.read(reinterpret_cast<char*>(&type), sizeof(type));
      assert(type == IOBinary::MYFACETRACKERPARAMS);
    }
    file.read(reinterpret_cast<char*>(&timeDet), sizeof(timeDet));
    file.read(reinterpret_cast<char*>(&itol), sizeof(itol));
    file.read(reinterpret_cast<char*>(&ftol), sizeof(ftol));
    file.read(reinterpret_cast<char*>(&clamp), sizeof(clamp));
    file.read(reinterpret_cast<char*>(&init_lambda), sizeof(init_lambda));
    file.read(reinterpret_cast<char*>(&track_lambda), sizeof(track_lambda));
    file.read(reinterpret_cast<char*>(&gamma), sizeof(gamma));
    file.read(reinterpret_cast<char*>(&init_type), sizeof(init_type));
    file.read(reinterpret_cast<char*>(&track_type), sizeof(track_type));
    file.read(reinterpret_cast<char*>(&shape_predict), sizeof(shape_predict));
    int t;
    file.read(reinterpret_cast<char*>(&t), sizeof(t)); init_wSize.resize(t);
    file.read(reinterpret_cast<char*>(&init_wSize[0]), sizeof(int)*init_wSize.size());
    
 
    file.read(reinterpret_cast<char*>(&t), sizeof(t));  track_wSize.resize(t);
    file.read(reinterpret_cast<char*>(&track_wSize[0]), sizeof(int)*track_wSize.size());

 
    file.read(reinterpret_cast<char*>(&t), sizeof(t));   center.resize(t);
    for(int i = 0; i < int(center.size()); i++)IOBinary::ReadMat(file,center[i]);
    
 
    file.read(reinterpret_cast<char*>(&t), sizeof(t)); visi.resize(t);
    for(int i = 0; i < int(visi.size()); i++)IOBinary::ReadMat(file,visi[i]);
    
    file.read(reinterpret_cast<char*>(&t), sizeof(t));
 
    if(t > 0)
      IOBinary::ReadMat(file,atm_tri);
    else{
      atm_tri = cv::Mat();
    }
    
    file.read(reinterpret_cast<char*>(&atm_scale), sizeof(atm_scale));
    file.read(reinterpret_cast<char*>(&atm_thresh), sizeof(atm_thresh));
    file.read(reinterpret_cast<char*>(&atm_ntemp), sizeof(atm_ntemp));
    file.read(reinterpret_cast<char*>(&ksmooth_size), sizeof(ksmooth_size));
    file.read(reinterpret_cast<char*>(&ksmooth_sigma), sizeof(ksmooth_sigma));
    file.read(reinterpret_cast<char*>(&ksmooth_noise), sizeof(ksmooth_noise));
    file.read(reinterpret_cast<char*>(&ksmooth_thresh), sizeof(ksmooth_thresh));
    file.read(reinterpret_cast<char*>(&ksmooth_ntemp), sizeof(ksmooth_ntemp));
  }


  file.close();
  check_health = true;

  if(init_type!=0){
    // std::cout << "init type changed to 0: " << init_type << std::endl;
    init_type = 0;
  }
  
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
myFaceTracker::myFaceTracker(const char* clmFile,
			     const char* sInitFile,
			     const char* FcheckFile,
			     //		     const char* praFile,
			     const char* predFile,
			     bool binary)
{
  _clm.Load(clmFile, binary);
  _sinit.Load(sInitFile, binary);
  _fcheck.Load(FcheckFile, binary);
  _spred.Load(predFile, binary);
  _time = -1;
}
//=============================================================================
void 
myFaceTracker::Reset()
{
  _time = -1; _atm._init = false;
}
//=============================================================================
std::vector<cv::Point_<double> >
myFaceTracker::getShape() const
{
  const int n = _shape.rows / 2;
  std::vector<cv::Point_<double> > rv(n);
  
  if ((_shape.rows > 0) && ((_shape.rows % 2) == 0) && (_shape.cols == 1)) {
    for (size_t i = 0; i < rv.size(); i++) {
      rv[i] = cv::Point_<double>(_shape.db(i + 0, 0),
				 _shape.db(i + n, 0));
    }
  }
  
  return rv;
}

std::vector<cv::Point3_<double> >
myFaceTracker::get3DShape() const
{
  const int n = _shape.rows / 2;
  std::vector<cv::Point3_<double> > rv(n);

  cv::Mat shape_3d = _clm._pdm.currentShape3D();  

  if ((_shape.rows > 0) && ((_shape.rows % 2) == 0) && (_shape.cols == 1)) {
    assert((shape_3d.rows % 3) == 0);
    assert(shape_3d.type() == cv::DataType<double>::type);
    for (size_t i = 0; i < rv.size(); i++) {
      rv[i] = cv::Point3_<double>(shape_3d.db(i + 0*n, 0),
				  shape_3d.db(i + 1*n, 0),
				  shape_3d.db(i + 2*n, 0));
    }
  }

  return rv;
}
//=============================================================================
Pose
myFaceTracker::getPose() const
{
  Pose rv;
  rv.pitch = _clm._pglobl.db(1,0);
  rv.yaw = _clm._pglobl.db(2,0);
  rv.roll = _clm._pglobl.db(3,0);
  return rv;
}
//=============================================================================
int
myFaceTracker::NewFrame(cv::Mat &im,
			FaceTrackerParams * params)
{
  //set parameters
  myFaceTrackerParams* p = 0;
  bool release=false;
  if (params != NULL){
    p = dynamic_cast<myFaceTrackerParams *>(params);
  }
  
  if (!p) {
    p = new myFaceTrackerParams();
    release=true;
  }
  
  //convert image to greyscale
  if(im.channels() == 1)gray_ = im;
  else{
    if((gray_.rows != im.rows) || (gray_.cols != im.cols))
      gray_.create(im.rows,im.cols,CV_8U);
    cv::cvtColor(im,gray_,CV_BGR2GRAY);
  }
  
  //re-initialise and fit
  bool gen=false; 
  bool rsize=true;
  cv::Rect R;  
  if (_time < 0) {
    R = _sinit.Detect(gray_); 
    if ((R.width <= 0) || (R.height <= 0)) {
      _time = -1;
      if(release)
	delete p;
      return FaceTracker::TRACKER_FAILED;
    }
    _time = cvGetTickCount();
    gen = true;
  } else {
    R = _sinit.ReDetect(gray_);
    gen = false;
  }
  if(gen){
    _sinit.InitShape(gray_,_shape,R);
    _clm._pdm.CalcParams(_shape,_clm._plocal,_clm._pglobl);     
    if(p->init_type == 0)
      _clm.Fit(gray_,p->init_wSize,p->itol,p->clamp,p->ftol);
    else{
      std::cerr << "Bad init_type "<<p->init_type << ". init_type should be 0" << std::endl;
      abort();
    }
    //   myInitData data; data.calculate = true; 
    //   data.pra = &_pra; data.mu = mu_; data.cov = cov_; data.covi = covi_;
    //   _clm.FitPrior(gray_,p->init_wSize,p->itol,p->clamp,p->ftol,p->init_lambda,
    // 		    myInitFunc,&data);
    // }
  }else{
    double tx = R.x - rect_.x,ty = R.y - rect_.y;
    _clm._pglobl.db(4,0) += tx; _clm._pglobl.db(5,0) += ty; 
    int n = _shape.rows/2; 
    cv::Mat sx = _shape(cv::Rect(0,0,1,n)),sy = _shape(cv::Rect(0,n,1,n));
    sx += tx; sy += ty; rsize = false;
    if(p->track_type == 0)
      _clm.Fit(gray_,p->track_wSize,p->itol,p->clamp,p->ftol);
    else{
      if(_atm._scale == 1)smooth_ = gray_;
      else{
	cv::Size ksize((1.0/_atm._scale)*3+1,(1.0/_atm._scale)*3+1);
	cv::GaussianBlur(gray_,smooth_,ksize,0,0);
      }
      vector<cv::Mat> visi;
      if(p->visi.size() == _clm._visi.size()){
	visi.resize(_clm._visi.size());
	for(int i = 0; i < int(visi.size()); i++){
	  visi[i] = _clm._visi[i].clone();
	  p->visi[i].copyTo(_clm._visi[i]);
	}
      }
      if(p->track_type == 1){
	myTrackData1 data; data.img = smooth_; data.tracker = this;
	_clm.FitPrior(gray_,p->track_wSize,p->itol,p->clamp,p->ftol,
		      p->track_lambda,myTrackFunc1,&data);
      }else{
	myTrackData2 data; data.img = smooth_; data.tracker = this;
	data.cov = cov_; data.mu = mu_; data.covi = covi_; 
	data.gamma = p->gamma; data.calculate = true; 
	_clm.FitPrior(gray_,p->track_wSize,p->itol,p->clamp,p->ftol,
		      p->track_lambda,myTrackFunc2,&data);
      }
      if(p->visi.size() == _clm._visi.size()){
	for(int i = 0; i < int(visi.size()); i++)visi[i].copyTo(_clm._visi[i]);
      }
    }
  }
  _clm._pdm.CalcShape2D(_shape,_clm._plocal,_clm._pglobl);
  if(p->shape_predict){
    _spred.Predict(_shape,gray_);
    _clm._pdm.CalcParams(_shape,_clm._plocal,_clm._pglobl);
  }
  
  int health;
  if (p->check_health) {
    int n = _shape.rows/2,i;
    for (i = 0; i < n; i++) {
      if ((_shape.db(i  ,0) < 0) || (_shape.db(i  ,0) >= gray_.cols) ||
	  (_shape.db(i+n,0) < 0) || (_shape.db(i+n,0) >= gray_.rows))
	break;
    }
    
    //report when not all points are within the frame
    if(i < n)
      health = FaceTracker::TRACKER_FACE_OUT_OF_FRAME;         
    else
      health = _fcheck.Check(gray_,_shape,_clm.GetViewIdx());
  } else {
    health = 10;
  }

  if (health < 0) {
    _time = -1;
    if (release) 
      delete p;
    return health;
  }

  //update models
  rect_ = _sinit.Update(gray_,_shape,rsize);
  if ((rect_.width == 0) || (rect_.height == 0)) {
    _time = -1;
    if(release)
      delete p;
    return FaceTracker::TRACKER_FAILED;
  }

  if (p->track_type > 0) {
    if ((dxdp_.rows != 2*_clm._pdm.nPoints()) || 
	(dxdp_.cols != 6+_clm._pdm.nModes())) {
      dxdp_.create(2*_clm._pdm.nPoints(),6+_clm._pdm.nModes(),CV_64F); 
    }

    _clm._pdm.CalcJacob(_clm._plocal,_clm._pglobl,dxdp_);

    cv::Mat pose = _clm._pglobl(cv::Rect(0,1,1,3));
    if (!_atm._init) {
      if (_atm._scale == 1) {
	smooth_ = gray_;
      } else {
	cv::Size ksize((1.0/p->atm_scale)*3+1,(1.0/p->atm_scale)*3+1);
	cv::GaussianBlur(gray_,smooth_,ksize,0,0);
      }
      _atm.Init(p->center,pose,_shape,smooth_,p->atm_tri,p->atm_scale);
      _atm.Update(smooth_,_shape,dxdp_,pose,p->atm_ntemp,-1); 
    } else {
      _atm.Update(smooth_,_shape,dxdp_,pose,p->atm_ntemp,p->atm_thresh);
    }

    // if(_pra._pra.size() > 0){
    //   if(p->track_type > 1){
    // 	if(!_ksmooth._init)
    // 	  _ksmooth.Init(_shape,_clm._pdm._M,p->center,p->ksmooth_size,
    // 			p->ksmooth_sigma,p->ksmooth_noise,p->ksmooth_thresh,
    // 			p->ksmooth_ntemp);
    // 	_ksmooth.Update(gray_,_shape);
    //   }
    // }
  }

  // update the 3D shape
  _clm._pdm.CalcParams(_shape,_clm._plocal,_clm._pglobl);  

  if (release) 
    delete p;

  return health;
}
//=============================================================================
void 
myFaceTracker::Read(ifstream &s,
		    bool readType)
{
  int type; 
  if(readType){s >> type; assert(type == IO::MYFACETRACKER);}

  _clm.Read(s); _sinit.Read(s); _fcheck.Read(s); 
  //_pra.Read(s);
  _spred.Read(s);
  _time = -1; 
  int n = _clm._pdm.nPoints();
  mu_.create(2*n,1,CV_64F); cov_.create(2*n,2*n,CV_64F); 
  covi_.create(2*n,2*n,CV_64F); return;
}
//=============================================================================
void 
myFaceTracker::ReadBinary(ifstream &s,
			  bool readType)
{
  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::MYFACETRACKER);
  }

  _clm.ReadBinary(s); 
  _sinit.ReadBinary(s); 
  _fcheck.ReadBinary(s); 
  //_pra.ReadBinary(s); 
  _spred.ReadBinary(s);
  _time = -1; 
  int n = _clm._pdm.nPoints();
  mu_.create(2*n,1,CV_64F); cov_.create(2*n,2*n,CV_64F); 
  covi_.create(2*n,2*n,CV_64F); return;
}
//=============================================================================
void 
myFaceTracker::Write(ofstream &s, bool binary)
{
  if(!binary)
    s << IO::MYFACETRACKER<< " ";
  else{
    int t = IOBinary::MYFACETRACKER;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
  }
  // int k=0; 
  _clm.Write(s, binary);  //s.write((char*)&k, sizeof(k)); 
  _sinit.Write(s, binary); 
  _fcheck.Write(s, binary);  
  //s.write((char*)&k, sizeof(k));
  //  _pra.Write(s, binary);
  _spred.Write(s, binary); return;
}
//=============================================================================
