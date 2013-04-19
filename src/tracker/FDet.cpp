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

#include <tracker/FDet.hpp>
using namespace FACETRACKER;
using namespace std;
#define TSCALE 0.3
//===========================================================================
FDet::~FDet()
{  
  if(storage_ != NULL)cvReleaseMemStorage(&storage_);
  if(_cascade != NULL)cvReleaseHaarClassifierCascade(&_cascade);
}
//===========================================================================
FDet& FDet::operator= (FDet const& rhs)
{
  this->_min_neighbours = rhs._min_neighbours;
  this->_min_size = rhs._min_size;
  this->_img_scale = rhs._img_scale;
  this->_scale_factor = rhs._scale_factor;
  if(storage_ != NULL)cvReleaseMemStorage(&storage_);
  storage_ = cvCreateMemStorage(0);
  this->_cascade = rhs._cascade;
  this->small_img_ = rhs.small_img_.clone(); return *this;
}
//===========================================================================
void FDet::Init(const char* fname,
		const double img_scale,
		const double scale_factor,
		const int    min_neighbours,
		const int    min_size)
{
  if(!(_cascade = (CvHaarClassifierCascade*)cvLoad(fname,0,0,0))){
    printf("ERROR(%s,%d) : Failed loading classifier cascade!\n",
	   __FILE__,__LINE__); abort();
  }
  storage_        = cvCreateMemStorage(0);
  _img_scale      = img_scale;
  _scale_factor   = scale_factor;
  _min_neighbours = min_neighbours;
  _min_size       = min_size; return;
}
//===========================================================================
cv::Rect FDet::Detect(cv::Mat im)
{
  assert(im.type() == CV_8U);
  cv::Mat gray; int i,maxv; cv::Rect R;
  int w = cvRound(im.cols/_img_scale);
  int h = cvRound(im.rows/_img_scale);
  if((small_img_.rows!=h) || (small_img_.cols!=w))small_img_.create(h,w,CV_8U);
  if(im.channels() == 1)gray = im;
  else{gray=cv::Mat(im.rows,im.cols,CV_8U);cv::cvtColor(im,gray,CV_BGR2GRAY);}
  cv::resize(gray,small_img_,cv::Size(w,h),0,0,CV_INTER_LINEAR);
  cv::equalizeHist(small_img_,small_img_);
  cvClearMemStorage(storage_); IplImage simg = small_img_;
  CvSeq* obj = cvHaarDetectObjects(&simg,_cascade,storage_,
				   _scale_factor,_min_neighbours,0,
				   cv::Size(_min_size,_min_size));
  if(obj->total == 0)return cv::Rect(0,0,0,0);
  for(i = 0,maxv = 0; i < obj->total; i++){
    CvRect* r = (CvRect*)cvGetSeqElem(obj,i);
    if(i == 0 || maxv < r->width*r->height){
      maxv = r->width*r->height; R.x = r->x*_img_scale; R.y = r->y*_img_scale;
      R.width  = r->width*_img_scale; R.height = r->height*_img_scale;
    }
  }
  cvRelease((void**)(&obj)); return R;
}
//===========================================================================
void FDet::Load(const char* fname, bool binary)
{
  ifstream s;
  if(!binary){s.open(fname); assert(s.is_open()); this->Read(s);}
  else { s.open(fname); assert(s.is_open()); this->ReadBinary(s);}
  s.close(); return;
}
//===========================================================================
void FDet::Save(const char* fname, bool binary)
{
  ofstream s(fname); assert(s.is_open()); this->Write(s, binary);s.close(); return;
}
//===========================================================================
void FDet::Write(ofstream &s, bool binary)
{
  int i,j,k,l;
  if(!binary){
  s << IO::FDET                          << " "
    << _min_neighbours                   << " " 
    << _min_size                         << " "
    << _img_scale                        << " "
    << _scale_factor                     << " "
    << _cascade->count                   << " "
    << _cascade->orig_window_size.width  << " " 
    << _cascade->orig_window_size.height << " ";
  for(i = 0; i < _cascade->count; i++){
    s << _cascade->stage_classifier[i].parent    << " "
      << _cascade->stage_classifier[i].next      << " "
      << _cascade->stage_classifier[i].child     << " "
      << _cascade->stage_classifier[i].threshold << " "
      << _cascade->stage_classifier[i].count     << " "; 
    for(j = 0; j < _cascade->stage_classifier[i].count; j++){
      CvHaarClassifier* classifier = 
	&_cascade->stage_classifier[i].classifier[j];
      s << classifier->count << " ";
      for(k = 0; k < classifier->count; k++){
	s << classifier->threshold[k]           << " "
	  << classifier->left[k]                << " "
	  << classifier->right[k]               << " "
	  << classifier->alpha[k]               << " "
	  << classifier->haar_feature[k].tilted << " ";
	for(l = 0; l < CV_HAAR_FEATURE_MAX; l++){
	  s << classifier->haar_feature[k].rect[l].weight   << " "
	    << classifier->haar_feature[k].rect[l].r.x      << " "
	    << classifier->haar_feature[k].rect[l].r.y      << " "
	    << classifier->haar_feature[k].rect[l].r.width  << " "
	    << classifier->haar_feature[k].rect[l].r.height << " ";
	}
      }
      s << classifier->alpha[classifier->count] << " ";
    }
  }
  }
  else{
    int t = IOBinary::FDET;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    s.write(reinterpret_cast<char*>(&_min_neighbours), sizeof(_min_neighbours));
    s.write(reinterpret_cast<char*>(&_min_size), sizeof(_min_size));
    s.write(reinterpret_cast<char*>(&_img_scale), sizeof(_img_scale));
    s.write(reinterpret_cast<char*>(&_scale_factor), sizeof(_scale_factor));
    s.write(reinterpret_cast<char*>(&_cascade->count), sizeof(_cascade->count));
    s.write(reinterpret_cast<char*>(&_cascade->orig_window_size), sizeof(_cascade->orig_window_size));
    for(i = 0; i < _cascade->count; i++){
      s.write(reinterpret_cast<char*>(&_cascade->stage_classifier[i].parent), sizeof(int));
      s.write(reinterpret_cast<char*>(&_cascade->stage_classifier[i].next), sizeof(int));
      s.write(reinterpret_cast<char*>(&_cascade->stage_classifier[i].child), sizeof(int));
      s.write(reinterpret_cast<char*>(&_cascade->stage_classifier[i].threshold), sizeof(float));
      s.write(reinterpret_cast<char*>(&_cascade->stage_classifier[i].count), sizeof(int));
      for(j = 0; j < _cascade->stage_classifier[i].count; j++){
	CvHaarClassifier* classifier = 
	  &_cascade->stage_classifier[i].classifier[j];

	s.write(reinterpret_cast<char*>(&classifier->count), sizeof(classifier->count));
	for(k = 0; k < classifier->count; k++){
	  s.write(reinterpret_cast<char*>(&classifier->threshold[k]), sizeof(classifier->threshold[k]));
	  s.write(reinterpret_cast<char*>(&classifier->left[k]), sizeof(classifier->left[k]));
	  s.write(reinterpret_cast<char*>(&classifier->right[k]), sizeof(classifier->right[k]));
	  s.write(reinterpret_cast<char*>(&classifier->alpha[k]), sizeof(classifier->alpha[k]));
	  s.write(reinterpret_cast<char*>(&classifier->haar_feature[k].tilted), sizeof(int));
	  for(l = 0; l < CV_HAAR_FEATURE_MAX; l++){
	    s.write(reinterpret_cast<char*>(&classifier->haar_feature[k].rect[l].weight), sizeof(float));
	    s.write(reinterpret_cast<char*>(&classifier->haar_feature[k].rect[l].r), sizeof(CvRect));
	  }
	}
	s.write(reinterpret_cast<char*>(&classifier->alpha[classifier->count]), sizeof(classifier->alpha[classifier->count]));
      }
      //  s.write(reinterpret_cast<char*>(&), sizeof());
    }
  }
  
  return;
}
//===========================================================================
void FDet::Read(ifstream &s,bool readType)
{  
  int i,j,k,l,n,m;
  if(readType){int type; s >> type; assert(type == IO::FDET);}
  s >> _min_neighbours >> _min_size >> _img_scale >> _scale_factor >> n;
  m = sizeof(CvHaarClassifierCascade)+n*sizeof(CvHaarStageClassifier);
  storage_ = cvCreateMemStorage(0);
  _cascade = (CvHaarClassifierCascade*)cvAlloc(m);
  memset(_cascade,0,m);
  _cascade->stage_classifier = (CvHaarStageClassifier*)(_cascade + 1);
  _cascade->flags = CV_HAAR_MAGIC_VAL;
  _cascade->count = n;
  s >> _cascade->orig_window_size.width >> _cascade->orig_window_size.height;
  for(i = 0; i < n; i++){
    s >> _cascade->stage_classifier[i].parent
      >> _cascade->stage_classifier[i].next
      >> _cascade->stage_classifier[i].child
      >> _cascade->stage_classifier[i].threshold
      >> _cascade->stage_classifier[i].count;
    _cascade->stage_classifier[i].classifier =
      (CvHaarClassifier*)cvAlloc(_cascade->stage_classifier[i].count*
				 sizeof(CvHaarClassifier));
    for(j = 0; j < _cascade->stage_classifier[i].count; j++){
      CvHaarClassifier* classifier = 
	&_cascade->stage_classifier[i].classifier[j];
      s >> classifier->count;
      classifier->haar_feature = (CvHaarFeature*) 
	cvAlloc(classifier->count*(sizeof(CvHaarFeature) +
				   sizeof(float) + sizeof(int) + sizeof(int))+ 
		(classifier->count+1)*sizeof(float));
      classifier->threshold = 
	(float*)(classifier->haar_feature+classifier->count);
      classifier->left = (int*)(classifier->threshold + classifier->count);
      classifier->right = (int*)(classifier->left + classifier->count);
      classifier->alpha = (float*)(classifier->right + classifier->count);
      for(k = 0; k < classifier->count; k++){
	s >> classifier->threshold[k]
	  >> classifier->left[k]
	  >> classifier->right[k]
	  >> classifier->alpha[k]
	  >> classifier->haar_feature[k].tilted;
	for(l = 0; l < CV_HAAR_FEATURE_MAX; l++){
	  s >> classifier->haar_feature[k].rect[l].weight
	    >> classifier->haar_feature[k].rect[l].r.x
	    >> classifier->haar_feature[k].rect[l].r.y
	    >> classifier->haar_feature[k].rect[l].r.width
	    >> classifier->haar_feature[k].rect[l].r.height;
	}
      }
      s >> classifier->alpha[classifier->count];
    }
  }return;
}
//===========================================================================
void  FDet::ReadBinary(ifstream &s,bool readType)
{  

  int i,j,k,l, n,m;

  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::FDET);
  }

  s.read(reinterpret_cast<char*>(&_min_neighbours), sizeof(_min_neighbours));
  s.read(reinterpret_cast<char*>(&_min_size), sizeof(_min_size));
  s.read(reinterpret_cast<char*>(&_img_scale), sizeof(_img_scale));
  s.read(reinterpret_cast<char*>(&_scale_factor), sizeof(_scale_factor));
  s.read(reinterpret_cast<char*>(&n), sizeof(n));
  m = sizeof(CvHaarClassifierCascade)+n*sizeof(CvHaarStageClassifier);
  storage_ = cvCreateMemStorage(0);
  _cascade = (CvHaarClassifierCascade*)cvAlloc(m);
  memset(_cascade,0,m);
  _cascade->stage_classifier = (CvHaarStageClassifier*)(_cascade + 1);
  _cascade->flags = CV_HAAR_MAGIC_VAL;
  _cascade->count = n;

  s.read(reinterpret_cast<char*>(&_cascade->orig_window_size), sizeof(_cascade->orig_window_size));
  for(i = 0; i < _cascade->count; i++){
    s.read(reinterpret_cast<char*>(&_cascade->stage_classifier[i].parent), sizeof(int));
    s.read(reinterpret_cast<char*>(&_cascade->stage_classifier[i].next), sizeof(int));
    s.read(reinterpret_cast<char*>(&_cascade->stage_classifier[i].child), sizeof(int));
    s.read(reinterpret_cast<char*>(&_cascade->stage_classifier[i].threshold), sizeof(float));
    s.read(reinterpret_cast<char*>(&_cascade->stage_classifier[i].count), sizeof(int));
    _cascade->stage_classifier[i].classifier =
      (CvHaarClassifier*)cvAlloc(_cascade->stage_classifier[i].count*
				 sizeof(CvHaarClassifier));
    for(j = 0; j < _cascade->stage_classifier[i].count; j++){
      CvHaarClassifier* classifier = 
	&_cascade->stage_classifier[i].classifier[j];
      s.read(reinterpret_cast<char*>(&classifier->count), sizeof(classifier->count));
      classifier->haar_feature = (CvHaarFeature*) 
	cvAlloc(classifier->count*(sizeof(CvHaarFeature) +
				   sizeof(float) + sizeof(int) + sizeof(int))+ 
		(classifier->count+1)*sizeof(float));
      classifier->threshold = 
	(float*)(classifier->haar_feature+classifier->count);
      classifier->left = (int*)(classifier->threshold + classifier->count);
      classifier->right = (int*)(classifier->left + classifier->count);
      classifier->alpha = (float*)(classifier->right + classifier->count);

      for(k = 0; k < classifier->count; k++){
	s.read(reinterpret_cast<char*>(&classifier->threshold[k]), sizeof(classifier->threshold[k]));
	s.read(reinterpret_cast<char*>(&classifier->left[k]), sizeof(classifier->left[k]));
	s.read(reinterpret_cast<char*>(&classifier->right[k]), sizeof(classifier->right[k]));
	s.read(reinterpret_cast<char*>(&classifier->alpha[k]), sizeof(classifier->alpha[k]));
	s.read(reinterpret_cast<char*>(&classifier->haar_feature[k].tilted), sizeof(int));
	for(l = 0; l < CV_HAAR_FEATURE_MAX; l++){
	  s.read(reinterpret_cast<char*>(&classifier->haar_feature[k].rect[l].weight), sizeof(float));
	  s.read(reinterpret_cast<char*>(&classifier->haar_feature[k].rect[l].r), sizeof(CvRect));
	}
      }
      s.read(reinterpret_cast<char*>(&classifier->alpha[classifier->count]), sizeof(classifier->alpha[classifier->count]));
    }
    //  s.write(reinterpret_cast<char*>(&), sizeof());
  }
  
  
}
//===========================================================================
void SInit::Load(const char* fname, bool binary)
{
  ifstream s;
  if(!binary){
    s.open(fname); assert(s.is_open()); this->Read(s);}
  else {
    s.open(fname, std::ios::binary); assert(s.is_open()); this->ReadBinary(s);}

  s.close(); return;
}
//===========================================================================
void SInit::Save(const char* fname, bool binary)
{
  ofstream s;
  if(!binary) s.open(fname);
  else s.open(fname, std::ios::binary);
  assert(s.is_open()); this->Write(s, binary);s.close(); return;
}
//===========================================================================
void SInit::Write(ofstream &s, bool binary)
{
  if(!binary){
  s << IO::SINIT << " "; _fdet.Write(s); IO::WriteMat(s,_rshape); 
  s << _simil[0] << " " << _simil[1] << " " 
    << _simil[2] << " " << _simil[3] << " ";
  }
  else{
    int t = IOBinary::SINIT;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    // t = 0; 
    // s.write(reinterpret_cast<char*>(&t), sizeof(t));
    _fdet.Write(s, binary);
    IOBinary::WriteMat(s, _rshape);
    s.write(reinterpret_cast<char*>(&_simil), sizeof(_simil));
  }
 return;
}
//===========================================================================
void SInit::Read(ifstream &s,bool readType)
{
  int type = -1;
  if(readType){s >> type; assert(type == IO::SINIT);}
 
  _fdet.Read(s); IO::ReadMat(s,_rshape); 
  s >> _simil[0] >> _simil[1] >> _simil[2] >> _simil[3];
 
  return;
}
//===========================================================================
void SInit::ReadBinary(ifstream &s,bool readType)
{

  if(readType){int type; 
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == IOBinary::SINIT);
  }
  _fdet.ReadBinary(s); IOBinary::ReadMat(s, _rshape);
  s.read(reinterpret_cast<char*>(&_simil), sizeof(_simil));
  
 return;
}
//===========================================================================
int SInit::InitShape(cv::Mat &im,cv::Mat &shape, cv::Rect r)
{
  int i,n = _rshape.rows/2; double a,b,tx,ty;
  if(r.width<=0) r = _fdet.Detect(im); if((r.width==0)||(r.height==0))return -1;
  if(!((shape.rows == _rshape.rows) && (shape.cols == _rshape.cols) &&
       (shape.type() == CV_64F))){
    shape.create(_rshape.rows,_rshape.cols,CV_64F);
  }
  a = r.width*cos(_simil[1])*_simil[0] + 1;
  b = r.width*sin(_simil[1])*_simil[0];
  tx = r.x + r.width/2  + r.width *_simil[2];
  ty = r.y + r.height/2 + r.height*_simil[3];
  cv::MatIterator_<double> sx = _rshape.begin<double>();
  cv::MatIterator_<double> sy = _rshape.begin<double>()+n;
  cv::MatIterator_<double> dx =   shape.begin<double>();
  cv::MatIterator_<double> dy =   shape.begin<double>()+n;
  for(i = 0; i < n; i++,++sx,++sy,++dx,++dy){
    *dx = a*(*sx) - b*(*sy) + tx; *dy = b*(*sx) + a*(*sy) + ty;
  }return 0;
}
//===========================================================================
cv::Rect SInit::ReDetect(cv::Mat &im)
{
  if(temp_.rows == 0)return cv::Rect();
  int x,y; float v,vb=-2;
  int ww = im.cols,hh = im.rows;
  int w = TSCALE*ww-temp_.cols+1,h = TSCALE*hh-temp_.rows+1;
  if((small_.rows != TSCALE*hh) || (small_.cols != TSCALE*ww))
    small_.create(TSCALE*hh,TSCALE*ww,CV_8U);
  cv::resize(im,small_,cv::Size(TSCALE*ww,TSCALE*hh),0,0,CV_INTER_LINEAR);
  if((ncc_.rows != h) || (ncc_.cols != w))ncc_.create(h,w,CV_32F);
  IplImage im_o = small_,temp_o = temp_,ncc_o = ncc_;
  cvMatchTemplate(&im_o,&temp_o,&ncc_o,CV_TM_CCOEFF_NORMED);
  cv::MatIterator_<float> p = ncc_.begin<float>(); cv::Rect R;  
  R.width = temp_.cols; R.height = temp_.rows;
  for(y = 0; y < h; y++){
    for(x = 0; x < w; x++){
      v = *p++; if(v > vb){vb = v; R.x = x; R.y = y;}
    }
  }
  R.x *= 1.0/TSCALE; R.y *= 1.0/TSCALE; 
  R.width *= 1.0/TSCALE; R.height *= 1.0/TSCALE; return R;
}
//===========================================================================
cv::Rect SInit::Update(cv::Mat &im,cv::Mat &s,bool rsize)
{
  int i,n = s.rows/2; double vx,vy;
  cv::MatIterator_<double> x = s.begin<double>(),y = s.begin<double>()+n;
  double xmax=*x,ymax=*y,xmin=*x,ymin=*y;
  for(i = 0; i < n; i++){
    vx = *x++; vy = *y++;
    xmax = std::max(xmax,vx); ymax = std::max(ymax,vy);
    xmin = std::min(xmin,vx); ymin = std::min(ymin,vy);
  }
  if((xmin < 0) || (ymin < 0) || (xmax >= im.cols) || (ymax >= im.rows) ||
     cvIsNaN(xmin) || cvIsInf(xmin) || cvIsNaN(xmax) || cvIsInf(xmax) ||
     cvIsNaN(ymin) || cvIsInf(ymin) || cvIsNaN(ymax) || cvIsInf(ymax))
    return cv::Rect(0,0,0,0);
  else{
    xmin *= TSCALE; ymin *= TSCALE; xmax *= TSCALE; ymax *= TSCALE;
    cv::Rect R = cv::Rect(std::floor(xmin),std::floor(ymin),
			  std::ceil(xmax-xmin),std::ceil(ymax-ymin));
    int ww = im.cols,hh = im.rows;
    if(rsize){
      if((small_.rows != TSCALE*hh) || (small_.cols != TSCALE*ww))
	small_.create(TSCALE*hh,TSCALE*ww,CV_8U);
      cv::resize(im,small_,cv::Size(TSCALE*ww,TSCALE*hh),0,0,CV_INTER_LINEAR);
    }
    cv::resize(im,small_,cv::Size(TSCALE*ww,TSCALE*hh),0,0,CV_INTER_LINEAR);
    temp_ = small_(R).clone(); 
    R.x *= 1.0/TSCALE; R.y *= 1.0/TSCALE; 
    R.width *= 1.0/TSCALE; R.height *= 1.0/TSCALE; return R;
  }
}
//===========================================================================
