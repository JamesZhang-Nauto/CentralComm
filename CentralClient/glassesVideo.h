#ifndef GLASSESVIDEO_H_H
#define GLASSESVIDEO_H_H

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "Aria.h"
using namespace cv;
using namespace std;

enum GLASSESMODE{idle=1, glassesOR, robotSearch, glassesControl, targetApproach, glassesConform };
enum RSGLASSESMODE{nod=1, shake, others};

// void *glassesVideo(void*); 
class GlassesVideo : public ArASyncTask
{
  void* runThread(void*) ;
};

#endif