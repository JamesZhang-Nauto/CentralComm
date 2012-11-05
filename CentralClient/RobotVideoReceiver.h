
#ifndef ROBOTVIDEORECEIVER_H_H
#define ROBOTVIDEORECEIVER_H_H 1



#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
// #include <boost/concept_check.hpp>
//#include <pthread.h>

//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <iostream>

#include <Aria.h>

using namespace std;
using namespace cv;


extern char* server_ip;

class RobotVideo : public ArASyncTask
{
public:

  void* runThread(void*) ;
};



#endif