#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "CommReceiver.h"
#include "RobotVideoReceiver.h"
#include "ObjectRecognition.h"
#define uchar unsigned char
#define ROBOT_VIDEO_PORT 11000

ArMutex mutex_robotVideo =  ArMutex::STATUS_FAILED_INIT;
ArCondition  cond_robotVideo;

Mat robot_img(480,640,CV_8UC1); 

int G_Search_Step = 0;

//extern int GlassesMode;





void* RobotVideo::runThread(void* arg)
	{
		cvNamedWindow("Robot", CV_WINDOW_AUTOSIZE);
		moveWindow("Robot", 0, 0);
	  // The buffer in which to recieve the hello from the server
	  //unsigned char buff[12];
	  unsigned char imgBuff[480*640];
  
	  //Mat imgRecieve(480,640,CV_8UC1);
	  // The size of the string the server sent
	  size_t strSize;
  
	  // The socket object
	  ArSocket sock;

	  // Initialize Aria.  It is especially important to do
	  // this on Windows, because it will initialize Window's
	  // sockets system. 
	  // Aria::init();

	  // Connect to the server
	  //ArLog::log(ArLog::Normal, "socketClientExample: Connecting to localhost TCP port 7777...");

	  if (sock.connect(server_ip, ROBOT_VIDEO_PORT, ArSocket::TCP))
		ArLog::log(ArLog::Normal, "socketClientExample: Connected to server at %s TCP port 11000.", server_ip);
	  else
	  {
		ArLog::log(ArLog::Terse, "socketClientExample: Error connecting to server at  %s  TCP port 11000: %s",server_ip, sock.getErrorStr().c_str());
		//return(-1);
	  }

	  unsigned char *ptr;

	  while(1)
	  {
  
		ptr = &imgBuff[0];
		strSize = 0;
		while (strSize < 640 * 480)
		{
			//ArLog::log(ArLog::Normal, "socketClientExample: String Size: \"%d\"", strSize);
			strSize += sock.read(ptr, 2);
			ptr += 2;
		}

		ptr=robot_img.datastart;
		//ArLog::log(ArLog::Normal, "socketClientExample: String Size: \"%d\"", strSize);
		
		mutex_robotVideo.lock();
		for (int i = 0,k = 0; i < robot_img.rows*robot_img.cols; i++) 
		{
			*(ptr++) = imgBuff[k++];
		}
		
		imshow("Robot", robot_img);
		mutex_robotVideo.unlock();
		waitKey(1);
	  }
	  sock.close();
  
	  // Now close the connection to the server
  
	  ArLog::log(ArLog::Normal, "socketClientExample: Socket to server closed.");
		destroyWindow("Robot");
	  // Uninitialize Aria and exit
	  Aria::exit(0);
	  //return(3);
	}



