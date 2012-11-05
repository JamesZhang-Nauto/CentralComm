#ifndef VIDEOSERVER_H_H
#define VIDEOSERVER_H_H


#include "Aria.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define VIDEO_PORT 11000



using namespace cv;
using namespace std;



class VideoServerBase : public ArASyncTask
{
  void* runThread(void*) ;
};

#endif