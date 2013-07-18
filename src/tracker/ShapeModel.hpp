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

#ifndef _TRACKER_ShapeModel_h_
#define _TRACKER_ShapeModel_h_
#include <tracker/IO.hpp>
namespace FACETRACKER
{
  //===========================================================================
  /** Related Functions */
  void GramSchmidt(cv::Mat &m);
  void AddOrthRow(cv::Mat &R);
  void MetricUpgrade(cv::Mat &R);
  void Euler2Rot(cv::Mat &R,const double pitch,const double yaw,
		 const double roll,bool full = true);
  void Euler2Rot(cv::Mat &R,cv::Mat &p,bool full = true);
  void Rot2Euler(cv::Mat &R,double& pitch,double& yaw,double& roll);
  void Rot2Euler(cv::Mat &R,cv::Mat &p);
  void Align3Dto2DShapes(double& scale,double& pitch,double& yaw,double& roll,
			 double& x,double& y,cv::Mat &s2D,cv::Mat &s3D);  
  void Align3DShapes(cv::Mat &src,cv::Mat &dst,
		     double &scale, double &pitch, double& yaw, 
		     double& roll,double &tx, double &ty, double &tz);
  //===========================================================================
  /**
     Virtual base class for shape model
  */
  class ShapeModel{
  public:
    int _type;
    void Load(const char* fname, bool binary = false){
      std::ifstream s;
      if(!binary){s.open(fname); assert(s.is_open()); this->Read(s);}
      else{s.open(fname); assert(s.is_open()); this->ReadBinary(s);}
      s.close(); return;
    }
    void Save(const char* fname, bool binary = false){
      std::ofstream s(fname); assert(s.is_open()); this->Write(s, binary);
      s.close(); return;
    }
    virtual void Write(std::ofstream &s, bool binary = false) = 0;
    virtual void Read(std::ifstream &s,bool readType = true) = 0;
    virtual void ReadBinary(std::ifstream &s,bool readType = true) = 0;
    virtual void CalcShape(cv::Mat &s,cv::Mat &params) = 0;
    virtual void CalcParams(cv::Mat &s,cv::Mat &params) = 0;
    virtual void CalcJacob(cv::Mat &params,cv::Mat &Jacob) = 0;    
  };
  //===========================================================================
  /**
     Virtual base class for linear shape model
  */
  class LinearShapeModel : public ShapeModel{
  public:        
    int     _n; /**< Number of points                   */
    cv::Mat _V; /**< basis of variation                 */
    cv::Mat _E; /**< vector of eigenvalues (row vector) */
    cv::Mat _M; /**< mean 3D shape vector               */
    
    inline int nPoints(){return _n;}
    inline int nModes(){return _V.cols;}
    inline double Var(int i){assert(i<_E.cols); return _E.at<double>(0,i);}

    void Clamp(cv::Mat &p,double c);
    void CalcShape(cv::Mat &s,cv::Mat &params);
    void CalcParams(cv::Mat &s,cv::Mat &params);
    void CalcJacob(cv::Mat &params,cv::Mat &Jacob);

    virtual void CalcShape2D(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl) = 0;
    virtual void CalcParams(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl) = 0;
    virtual void Init(cv::Mat &M,cv::Mat &V,cv::Mat &E) = 0;
    virtual void Identity(cv::Mat &plocal,cv::Mat &pglobl) = 0;
    virtual void CalcJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob) = 0;
    virtual void CalcRigidJacob(cv::Mat &plocal,cv::Mat &pglobl,
				cv::Mat &Jacob) = 0;
    virtual void CalcReferenceUpdate(cv::Mat &dp,cv::Mat &plocal,
				     cv::Mat &pglobl) = 0;
    virtual void ApplySimT(double a,double b,double tx,double ty,
			   cv::Mat &pglobl) = 0;
  };
  //===========================================================================
  /**
     A 2D linear shape model
  */
  class PDM2D : public LinearShapeModel{
  public:
    cv::Mat _T; /** Similarity basis projection */

    PDM2D(){_type = IO::PDM2D;}
    PDM2D(const char* fname, bool binary = false){_type = IO::PDM2D; this->Load(fname, binary);}
    PDM2D(cv::Mat &M,cv::Mat &V,cv::Mat &E){_type=IO::PDM2D; this->Init(M,V,E);}
    PDM2D& operator=(PDM2D const&rhs);

    void Write(std::ofstream &s, bool binary = false);
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s,bool readType = true);
    void CalcShape2D(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl);
    void CalcParams(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl);
    void Init(cv::Mat &M,cv::Mat &V,cv::Mat &E);
    void Identity(cv::Mat &plocal,cv::Mat &pglobl);
    void CalcJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob);
    void CalcRigidJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob);
    void CalcReferenceUpdate(cv::Mat &dp,cv::Mat &plocal,cv::Mat &pglobl);
    void ApplySimT(double a,double b,double tx,double ty,cv::Mat &pglobl);
  private:
    cv::Mat s_;

    void CalcSimil();
  };
  //===========================================================================
  /**
     A 3D linear shape model
  */
  class PDM3D : public LinearShapeModel{
  public:
    PDM3D(){_type = IO::PDM3D;}
    PDM3D(const char* fname, bool binary = false){_type = IO::PDM3D; this->Load(fname, binary);}
    PDM3D(cv::Mat &M,cv::Mat &V,cv::Mat &E){_type=IO::PDM3D; this->Init(M,V,E);}
    PDM3D& operator=(PDM3D const&rhs);

    void Write(std::ofstream &s, bool binary = false);
    void Read(std::ifstream &s,bool readType = true);
    void ReadBinary(std::ifstream &s,bool readType = true);
    void CalcShape2D(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl);
    void CalcShape3D(cv::Mat &s,cv::Mat &plocal);
    void CalcParams(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl);
    void CalcParams3D(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl);
    void Init(cv::Mat &M,cv::Mat &V,cv::Mat &E);
    void Identity(cv::Mat &plocal,cv::Mat &pglobl);
    void CalcJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob);
    void CalcRigidJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob);
    void CalcReferenceUpdate(cv::Mat &dp,cv::Mat &plocal,cv::Mat &pglobl);
    void ApplySimT(double a,double b,double tx,double ty,cv::Mat &pglobl);
    void Project2D(cv::Mat &s,cv::Mat &S,cv::Mat &pglobl);
    const cv::Mat currentShape3D() const; 
  private:
    cv::Mat S_,R_,s_,P_,Px_,Py_,Pz_,R1_,R2_,R3_;
  };
  //===========================================================================
}
#endif
