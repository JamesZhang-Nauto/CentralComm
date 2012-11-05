//============================================================================
// Name        : MotionEstimation.cpp
// Author      : zhuang lishuo
// Version     : 4.0
// Copyright   : zhlshuo
// Description : Estimate the motion of camera in C++
//============================================================================
#ifndef GLASSMOTION_H_H
#define GLASSMOTION_H_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>
#include <fstream>
#include <string.h>
#include "GlassesMotion/CircularBuffer.h"
#include "GlassesMotion/svm.h"

using namespace cv;
using namespace std;



//void getPatternsWithInputImage(Mat preFrame, Mat curFrame, int& pattern);

//new
void modeSwitch(Mat preFrame, Mat curFrame);
#endif