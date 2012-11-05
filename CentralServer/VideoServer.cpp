#include "VideoServer.h"


ArMutex mutex_video;
Mat cap_img;
Mat gray_img;
int is_data_ready = 0;


class streamServer : public ArASyncTask
{
  void* runThread(void*) 
  {
    size_t strSize;
		

    // The socket objects: one for accepting new client connections,
    // and another for communicating with a client after it connects.
    ArSocket serverSock, clientSock;

    // Open the server socket
    if (serverSock.open(VIDEO_PORT, ArSocket::TCP))
      ArLog::log(ArLog::Normal, " Opened the server port %d.", VIDEO_PORT);
    else
      ArLog::log(ArLog::Normal, " Failed to open the server port: %s.", serverSock.getErrorStr().c_str());
    

    if (serverSock.accept(&clientSock))
      ArLog::log(ArLog::Normal, " Client has connected.");
    else
      ArLog::log(ArLog::Terse, " Error in accepting a connection from the client: %s.", serverSock.getErrorStr().c_str());

    while(1)
    {
      /* send the grayscaled frame, thread safe */
      mutex_video.lock();
      if (is_data_ready) 
      {
				int grayImgSize=gray_img.rows*gray_img.cols;
				//cout <<" Sending image to the client.grayImgSize= " << grayImgSize;
				if ((strSize = clientSock.write(gray_img.data, grayImgSize))==grayImgSize )
					;//ArLog::log(ArLog::Normal, " Sent image to the client.grayImgSize= %d", grayImgSize);
// 				else
// 					ArLog::log(ArLog::Normal, " Error in sending hello string to the client.");

				//ArLog::log(ArLog::Normal, " String Size: \"%d\"", strSize);
	
				is_data_ready = 0;
      }
      mutex_video.unlock();
      
      if(!clientSock.isOpen())
      {
	if (serverSock.accept(&clientSock))
	  ArLog::log(ArLog::Normal, " Client has connected.");
	else
	  ArLog::log(ArLog::Terse, " Error in accepting a connection from the client: %s.", serverSock.getErrorStr().c_str());
      }
    }
	// Now lets close the connection to the client
    clientSock.close();
    ArLog::log(ArLog::Normal, " Socket to client closed.");
    // And lets close the server port
    serverSock.close();
    ArLog::log(ArLog::Normal, " Server socket closed.");

    // Uninitialize Aria and exit the program
    //Aria::exit(0);

  } //end of runThread
};


void* VideoServerBase::runThread(void*) 
{
  VideoCapture capture(-1);

  //int key=0;

  capture.set( CV_CAP_PROP_FRAME_WIDTH, 640);
  capture.set( CV_CAP_PROP_FRAME_HEIGHT, 480);

  capture.read(cap_img);

  /* run the stream server as a separate thread */
  streamServer ss;
  ss.runAsync();

  while(1/*key != 'q'*/) //display routine
  {

		capture.read(cap_img);

		mutex_video.lock();
		cvtColor(cap_img, gray_img, CV_BGR2GRAY);

		//imshow(" ",gray_img);
		waitKey(1);

		is_data_ready = 1;
		mutex_video.unlock();

  }

}
