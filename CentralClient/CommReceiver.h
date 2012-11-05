#ifndef COMMRECEIVER_H_H
#define COMMRECEIVER_H_H 1

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>

#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "ObjectRecognition.h"

#include "Aria.h"
#include "ArNetworking.h"

//#include "stdafx.h"


using namespace std;
using namespace cv;
enum { RobotMotion=1, CameraMotion, RobotTurnLeft, RobotTurnRight, TargetApproach };

void RobotCommand(int robotMoveComm);
// void RobotMotion(ArClientBase client);
//void CommReceiver(ArNetPacket * pack);
void C_RobotMotion(ArNetPacket * pack);
void C_CameraMotion(ArNetPacket * pack);
void C_TargetApproach(ArNetPacket * pack);
void C_RobotTurnLeft(ArNetPacket * pack);
void C_RobotTurnRight(ArNetPacket * pack);
int robotSpeakObjName(int index);
int robotSpeak(const int index, const char *option);


class RobotSearch : public ArASyncTask
{
	public:
  void* runThread(void*) ;
};

#endif
