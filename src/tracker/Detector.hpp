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

#ifndef DETECTOR_CLASS_HPP_
#define DETECTOR_CLASS_HPP_

#include <opencv2/core/core.hpp>
#include <tracker/Patch.hpp>

#define DETECTOR_BAD_RESPONSE -.001

class Detector{
public:
  Detector(){};
  virtual ~Detector(){};

  virtual void ReadBinary(std::ifstream& s, bool readType)=0;
  virtual void Read(std::ifstream& s, bool readType)=0;
  virtual void Write(std::ofstream& s, bool binary = true)=0;
  virtual void Load(std::string fname, bool binary = true)=0;

  virtual bool response(cv::Mat& img, cv::Mat &shape,
       		cv::Size wSize, 
		cv::Mat& visibility) =0;	

  //resize the responses, and crop or pad so the returned shapes have
  //size resSize
  virtual std::vector<cv::Mat>
  getResponsesForRefShape(cv::Size resSize, 
			  cv::Mat r = cv::Mat());
  //resize the responses, and return the used scale
  virtual std::vector<cv::Mat>
  getResponsesForRefShape( cv::Mat r, double *sc = NULL);
  //resize the responses with the given scale
  virtual std::vector<cv::Mat>
  getResponsesForRefShape(double scale = 1.);

  virtual void setReferenceShape(cv::Mat&m);

  cv::Mat _refs; //reference frame of the detector.
  cv::Mat _refs_zm; // reference shape with mean removed

  std::vector<cv::Mat> prob_;

};


class DetectorNCC : public Detector{

  std::vector<cv::Mat> pmem_, wmem_;

public:
  DetectorNCC(){};
  DetectorNCC(std::string file, bool binary);
  ~DetectorNCC(){};
  
  void ReadBinary(std::ifstream& s, bool readType = true);
  void Write(std::ofstream &s, bool binary = true);
  void Read(std::ifstream &s, bool readType = true);
  void Load(std::string fname, bool binary = true);
  void Save(std::string fname, bool binary = true);
  
  bool response(cv::Mat& img, cv::Mat &shape,
		cv::Size wSize, 
		cv::Mat& visibility);

  // void setPatchExperts(std::vector<FACETRACKER::MPatch>& p);

  std::vector<FACETRACKER::MPatch> _patch;

};


#endif
