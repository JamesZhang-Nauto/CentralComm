#ifndef OBJECTRECOGNITION_H_H
#define OBJECTRECOGNITION_H_H

#include <iostream>
#include <vector>
#include <math.h>

// OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// DBoW2
#include "OR/DBoW2.h" // defines Surf64Vocabulary and Surf64Database

#include "OR/DUtils/DUtils.h"
#include "OR/DVision/DVision.h"

#include "OR/Homography.h"

using namespace DBoW2;
using namespace DUtils;
using namespace std;
using namespace cv;

//#define dbname "d120_11_3_db.yml.gz"
//#define dbname "object_3_db.yml.gz"

class ObjectRecognition
{
public:
  ObjectRecognition(char *dbname);
  ObjectRecognition(VideoCapture& _capture, int _ret_num, char *dbname);
  ObjectRecognition(Mat _mat, int _ret_num, char *dbname);
	~ObjectRecognition(){};
  //int find();
  int find(Mat _mat);
  static void loadImage(stringstream &ret_src, int &result_id, bool _display=0)
  {
    ret_src << /*"Home/image_";*/ "largeModules//image_"; 
    if(result_id<10) 			ret_src<< "00"<<result_id<< ".jpeg";
    if(result_id>=10&&result_id<100) 		ret_src<< "0"<<result_id<< ".jpeg";
    if(result_id>=100	)		ret_src<< result_id << ".jpeg";
    if(_display)
    cout << ret_src.str()<<endl;
  }

  
private:
  vector<vector<float> > features;
  vector<cv::KeyPoint> keypoints;
  QueryResults ret;
  Surf64Database db;
  VideoCapture capture;
  int ret_num;
  bool input_type; //0: using camera as input, 1: using image as input
  
  Mat image;
  Mat descriptors;

  void loadFeatures(Mat &image, vector<KeyPoint> &keypoints, vector<vector<float> > &features, Mat &descriptors);
  void queryDatabase(const vector<vector<float> >  &features,Surf64Database &db, QueryResults &ret, int ret_num);
  
};

// int Obj_Rec();


#endif