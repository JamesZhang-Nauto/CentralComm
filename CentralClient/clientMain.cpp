#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "Aria.h"
#include "ArNetworking.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "glassesVideo.h"
#include "CommReceiver.h"
#include "RobotVideoReceiver.h"


using namespace cv;
using namespace std; 


//--------------------------------Global Variables-------------------------------------------

char* server_ip = "127.0.0.1";/*"127.0.0.1";*/	//107 robot    145 workstation  132 alienware 117 desktop
ArClientBase *client;

GLASSESMODE G_glassesMode;

ArMutex GlassesModeMutex;
//--------------------------------------------------------------------------------------------

/**
* This function provides a way to exit nicely from the system
*/
//void quit(char* msg, int retval)
//{
//	if (retval == 0) {
//		fprintf(stdout, (msg == NULL ? "" : msg));
//		fprintf(stdout, "\n");
//	} else {
//		fprintf(stderr, (msg == NULL ? "" : msg));
//		fprintf(stderr, "\n");
//	}
//
//	exit(retval);
//}



//------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	G_glassesMode=idle;
	Aria::init();
	client = new ArClientBase();
	if (!client->blockingConnect(server_ip, 7272))
	{
		printf("Could not connect to server, exiting\n");
		exit(1);
	} 

	//------------------Define Callback Handlers-----------------------

	//client->addHandler("turn", new ArGlobalFunctor1<ArNetPacket*>(&CommReceiver));
	client->addHandler("RobotMotion", new ArGlobalFunctor1<ArNetPacket*>(&C_RobotMotion));
	client->addHandler("CameraMotion", new ArGlobalFunctor1<ArNetPacket*>(&C_CameraMotion));
	client->addHandler("RobotTurnLeft", new ArGlobalFunctor1<ArNetPacket*>(&C_RobotTurnLeft));
	client->addHandler("RobotTurnRight", new ArGlobalFunctor1<ArNetPacket*>(&C_RobotTurnRight));
	client->addHandler("TargetApproach", new ArGlobalFunctor1<ArNetPacket*>(&C_TargetApproach));
	client->runAsync();


	//-----------------------------Open robot video thread-------------------------------
	RobotVideo rotvideo;

	rotvideo.runAsync();
	//-----------------------------------------------------------------------------------   

	//-----------------------------Open glasses video thread-----------------------------
	GlassesVideo glassesVideo;

	glassesVideo.runAsync();
	//-----------------------------------------------------------------------------------

	while(1);

	Aria::shutdown();
	return -1;

}




