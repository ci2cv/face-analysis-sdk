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

#include <avatar/myAvatar.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#define db at<double>
#define it at<int>
using namespace std;
//=============================================================================
int main(int argc, const char** argv)
{
  if(argc < 5){
    cout << "Usage: ./add_avatar image shape eyes avatar" << endl
	 << "Add a new avatar from data to current avatar"
	 << endl;
    return 0;
  }
  string iFile=argv[1],sFile=argv[2],eFile=argv[3],aFile=argv[4];
  cv::Mat image  = cv::imread(iFile.c_str(),1);
  cv::Mat points = FACETRACKER::IO::LoadPts(sFile.c_str());
  cv::Mat eyes   = FACETRACKER::IO::LoadPts(eFile.c_str());
  AVATAR::myAvatar ava(aFile.c_str(), true); 

  ava.Save((aFile+".old").c_str(), true);
  ava.AddAvatar(image,points,eyes);
  ava.Save(aFile.c_str(), true); return 0;
}
//=============================================================================

// //=============================================================================
// int main(int argc, const char** argv)
// {
//   if(argc < 3){
//     cout << "Usage: ./remove_avatar avatarfile outputavatarfile" << endl;
//     return 0;
//   }
//   string iFile=argv[1],oFile=argv[2];//,eFile=argv[3],aFile=argv[4];

//   AVATAR::myAvatar ava(iFile.c_str()); 
  
//   std::cout << "press x to remove the avatar, other key to keep it" << std::endl;
//   std::cout << ava._images.at(0).rows << " " << ava._images.at(1).cols << std::endl;
//   cv::namedWindow("rem avatar");

//   std::vector<int> torem;

//   for(size_t i=0; i<ava._images.size();i++){
//     cv::imshow("rem avatar", ava._images.at(i));
//     int key = cv::waitKey(0);
//     if(key == int('x')) torem.push_back(i);
//   }
  
//   for(int i= (int)torem.size()-1; i>=0; i--){
//     std::cout << "removing " << torem.at(i)<<std::endl;
//     ava._images.erase(ava._images.begin()+torem.at(i));
//     ava._shapes.erase(ava._shapes.begin()+torem.at(i));
//     ava._textr.erase(ava._textr.begin()+torem.at(i));
//     ava._scale.erase(ava._scale.begin()+torem.at(i));
//     ava._lpupil.erase(ava._lpupil.begin()+torem.at(i));
//     ava._rpupil.erase(ava._rpupil.begin()+torem.at(i));
//   }


//   if(iFile == oFile){
//     oFile += "output";
//   }

//   ava.Save(oFile.c_str(), true); 

//   return 0;
// }
// //=============================================================================
