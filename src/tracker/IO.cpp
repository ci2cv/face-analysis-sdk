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

#include <tracker/IO.hpp>
#include <stdio.h>
using namespace FACETRACKER;
using namespace std;
//===========================================================================
vector<string> IO::GetList(const char* fname)
{
  vector<string> names(0); string str;
  fstream file(fname,fstream::in);
  if(!file.is_open()){
    printf("ERROR(%s,%d) : Failed opening file %s for reading\n", 
	   __FILE__,__LINE__,fname); abort();
  }
  while(!file.eof()){
    getline(file,str); if(str.length() < 2)continue; names.push_back(str);
  }
  file.close(); return names;
}
//===========================================================================
void IO::ReadMat(ifstream& s,cv::Mat &M)
{
  int r,c,t; s >> r >> c >> t;
  M = cv::Mat(r,c,t);
  switch(M.type()){
  case CV_64FC1: 
    { 
      cv::MatIterator_<double> i1 = M.begin<double>(),i2 = M.end<double>();
      while(i1 != i2)s >> *i1++;
    }break;
  case CV_32FC1:
    {
      cv::MatIterator_<float> i1 = M.begin<float>(),i2 = M.end<float>();
      while(i1 != i2)s >> *i1++;
    }break;
  case CV_32SC1:
    {
      cv::MatIterator_<int> i1 = M.begin<int>(),i2 = M.end<int>();
      while(i1 != i2)s >> *i1++;
    }break;
  case CV_8UC1:
    {
      cv::MatIterator_<uchar> i1 = M.begin<uchar>(),i2 = M.end<uchar>(); 
      while(i1 != i2)s >> *i1++;
    }break;
  default:
    printf("ERROR(%s,%d) : Unsupported Matrix type %d!\n", 
	   __FILE__,__LINE__,M.type()); abort();
  }return;
}
//===========================================================================
void IO::WriteMat(ofstream& s,cv::Mat &M)
{
  s << M.rows << " " << M.cols << " " << M.type() << " ";
  switch(M.type()){
  case CV_64FC1: 
    {
      s.precision(10); s << std::scientific;
      cv::MatIterator_<double> i1 = M.begin<double>(),i2 = M.end<double>();
      while(i1 != i2)s << *i1++ << " ";
    }break;
  case CV_32FC1:
    {
      cv::MatIterator_<float> i1 = M.begin<float>(),i2 = M.end<float>();
      while(i1 != i2)s << *i1++ << " ";
    }break;
  case CV_32SC1:
    {
      cv::MatIterator_<int> i1 = M.begin<int>(),i2 = M.end<int>();
      while(i1 != i2)s << *i1++ << " ";
    }break;
  case CV_8UC1:
    {
      cv::MatIterator_<uchar> i1 = M.begin<uchar>(),i2 = M.end<uchar>();
      while(i1 != i2)s << *i1++ << " ";
    }break;
  default:
    printf("ERROR(%s,%d) : Unsupported Matrix type %d!\n", 
	   __FILE__,__LINE__,M.type()); abort();
  }return;
}
//===========================================================================
cv::Mat IO::LoadCon(const char* fname)
{
  int i,n; char str[256]; char c; fstream file(fname,fstream::in);
  if(!file.is_open()){
    printf("ERROR(%s,%d) : Failed opening file %s for reading\n", 
	   __FILE__,__LINE__,fname); abort();
  }
  while(1){file >> str; if(strncmp(str,"n_connections:",14) == 0)break;}
  file >> n; cv::Mat con(2,n,CV_32S);
  while(1){file >> c; if(c == '{')break;}
  for(i = 0; i < n; i++)file >> con.at<int>(0,i) >> con.at<int>(1,i);
  file.close(); return con;
}
//=============================================================================
cv::Mat IO::LoadTri(const char* fname)
{
  int i,n; char str[256]; char c; fstream file(fname,fstream::in);
  if(!file.is_open()){
    printf("ERROR(%s,%d) : Failed opening file %s for reading\n", 
	   __FILE__,__LINE__,fname); abort();
  }
  while(1){file >> str; if(strncmp(str,"n_tri:",6) == 0)break;}
  file >> n; cv::Mat tri(n,3,CV_32S);
  while(1){file >> c; if(c == '{')break;}
  for(i = 0; i < n; i++)
    file >> tri.at<int>(i,0) >> tri.at<int>(i,1) >> tri.at<int>(i,2);
  file.close(); return tri;
}
//=============================================================================
cv::Mat IO::LoadVis(const char* fname)
{
  int i,n; char str[256]; char c;
  fstream file(fname,fstream::in);
  if(!file.is_open()){
    printf("ERROR(%s,%d) : Failed opening shape file %s for reading!\n", 
	   __FILE__,__LINE__,fname); abort();
  }
  while(!file.eof()){file >> str; if(strncmp(str,"n_points:",9) == 0)break;}
  file >> n; cv::Mat shape(n,1,CV_32S);
  while(!file.eof()){file >> c; if(c == '{')break;}
  for(i = 0; i<n; i++)file >> shape.at<int>(i,0);
  file.close(); return shape;
}
//=============================================================================
cv::Mat IO::LoadPts(const char* fname)
{
  int i,n; char str[256]; char c;
  fstream file(fname,fstream::in);
  if(!file.is_open()){
    printf("ERROR(%s,%d) : Failed opening shape file %s for reading!\n", 
	   __FILE__,__LINE__,fname); abort();
  }
  while(!file.eof()){file >> str; if(strncmp(str,"n_points:",9) == 0)break;}
  file >> n; cv::Mat shape(2*n,1,CV_64F);
  while(!file.eof()){file >> c; if(c == '{')break;}
  for(i = 0; i<n; i++)file >> shape.at<double>(i,0) >> shape.at<double>(i+n,0);
  file.close(); return shape;
}
//=============================================================================
cv::Mat IO::LoadPts3D(const char* fname)
{
  int i,n; char str[256]; char c;
  fstream file(fname,fstream::in);
  if(!file.is_open()){
    printf("ERROR(%s,%d) : Failed opening shape file %s for reading!\n", 
	   __FILE__,__LINE__,fname); abort();
  }
  while(!file.eof()){file >> str; if(strncmp(str,"n_points:",9) == 0)break;}
  file >> n; cv::Mat shape(3*n,1,CV_64F);
  while(!file.eof()){file >> c; if(c == '{')break;}
  for(i = 0; i<n; i++)
    file >> shape.at<double>(i,0) 
	 >> shape.at<double>(i+n,0)
	 >> shape.at<double>(i+n*2,0);
  file.close(); return shape;
}
//=============================================================================
void IO::SavePts(const char* fname,cv::Mat &pts)
{
 int i,n = pts.rows/2; fstream file(fname,fstream::out);
  if(!file.is_open()){
    printf("ERROR(%s,%d) : Failed opening pts file %s for writing!\n", 
	   __FILE__,__LINE__,fname); abort();
  }
  file << "n_points: " << n << "\n{\n";
  for(i = 0; i < n; i++){
    file << pts.at<double>(i,0) << "\t" << pts.at<double>(i+n,0) << "\n";
  }
  file << "}\n"; file.close(); return;
}
//=============================================================================
void
IO::SavePts(const char* fname, std::vector<cv::Point_<double> > &pts)
{
  cv::Mat_<double> mat(pts.size()*2, 1);
  const size_t n = pts.size();
  for (size_t i = 0; i < n; i++) {
    mat(i + 0, 0) = pts[i].x;
    mat(i + n, 0) = pts[i].y;
  }
  
  SavePts(fname, mat);
}
//=============================================================================
void IO::SavePts3D(const char* fname,cv::Mat &pts)
{
 int i,n = pts.rows/3; fstream file(fname,fstream::out);
  if(!file.is_open()){
    printf("ERROR(%s,%d) : Failed opening pts file %s for writing!\n", 
	   __FILE__,__LINE__,fname); abort();
  }
  file << "n_points: " << n << "\n{\n";
  for(i = 0; i < n; i++){
    file << pts.at<double>(i,0) << "\t" 
	 << pts.at<double>(i+n,0) << "\t"
	 << pts.at<double>(i+n*2,0) << "\n";
  }
  file << "}\n"; file.close(); return;
}
//===========================================================================
std::vector<cv::Mat> IO::LoadMatList(const char* fname)
{
  int n; ifstream file(fname); assert(file.is_open());
  file >> n; std::vector<cv::Mat> L(n);
  for(int i = 0; i < n; i++)IO::ReadMat(file,L[i]);
  file.close(); return L;
}
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
void IOBinary::ReadMat(std::ifstream &s, cv::Mat &M)
{
	int r,c,t;
	//s >> r >> c >> t;
	s.read((char*)&r, sizeof(int));
	s.read((char*)&c, sizeof(int));
	s.read((char*)&t, sizeof(int));
	M = cv::Mat(r,c,t);
	s.read(reinterpret_cast<char*>(M.datastart), M.total()*M.elemSize());
	
	if(!s.good()){
	  std::cout << "Error reading matrix" << std::endl;
	}

	//	std::cout << "Mat read: "<< r << "x"<<c << ", type " << t << std::endl; 
}
//===========================================================================
void IOBinary::WriteMat(std::ofstream &s, cv::Mat &M)
{
	assert(M.isContinuous() && !M.isSubmatrix());
	int t = M.type();
	s.write(reinterpret_cast<char*>(&M.rows), sizeof(int));
	s.write(reinterpret_cast<char*>(&M.cols), sizeof(int));
	s.write(reinterpret_cast<char*>(&t), sizeof(int));
	//	s << M.rows << " " << M.cols << " " << M.type();
	s.write(reinterpret_cast<char*>(M.datastart), M.total()*M.elemSize());

	//	std::cout << "Mat written: "<< M.rows << "x"<< M.cols << ", type " << M.type() << std::endl; 

}
////===========================================================================
//cv::Mat IOBinary::LoadCon(const char *fname)
//{
//	int n; char str[256]; char c;
//	fstream file(fname,fstream::in);
//	if(!file.is_open()){
//		printf("ERROR(%s,%d) : Failed opening file %s for reading\n",
//			   __FILE__,__LINE__,fname);
//		abort();
//	}
//	while(1){
//		file >> str;
//		if(strncmp(str,"n_connections:",14) == 0)
//			break;
//	}
//	file >> n;
//	cv::Mat con(2,n,CV_32S);
//	while(1){
//		file >> c;
//		if(c == '{')break;
//	}
//	file.read((char*)con.datastart, con.total());
//	
//	//	for(i = 0; i < n; i++)
////		file >> con.at<int>(0,i) >> con.at<int>(1,i);
//	file.close();
//	return con;
//}
//===========================================================================
std::vector<cv::Mat> IOBinary::LoadMatList(const char *fname)
{
	int n; ifstream file(fname); assert(file.is_open());
	file >> n; std::vector<cv::Mat> L(n);
	for(int i = 0; i < n; i++) 
	  IOBinary::ReadMat(file,L[i]);
	file.close();
	return L;
}
//===========================================================================

