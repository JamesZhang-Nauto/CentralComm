#include <iostream> // for standard I/O
#include <string>   // for strings

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

using namespace std;
using namespace cv;






int main(int argc, char *argv[], char *window_name)
{


    VideoCapture inputVideo(-1);        // Open input
			inputVideo.set(CV_CAP_PROP_FRAME_WIDTH , 640);
	inputVideo.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	//inputVideo.set(CV_CAP_PROP_FPS, 20);
    if ( !inputVideo.isOpened())
    {
        cout  << "Could not open the input video." /*<< source*/ << endl;
        return -1;
    }





    Size S = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    //Acquire input size
                  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    VideoWriter outputVideo;                                        // Open the output

            outputVideo.open("video.avi"  , CV_FOURCC('M','J','P','G') /* CV_FOURCC('P','I','M','1') */, 30/*inputVideo.get(CV_CAP_PROP_FPS)*/,S, true);


    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " /*<< source*/ << endl;
        return -1;
    }




    Mat src,res;


    while( true) //Show the image captured in the window and repeat
    {
			//double t = (double)getTickCount();  
        inputVideo >> src;              // read
        if( src.empty()) break;         // check if at end
				imshow("Camera", src);
				if (waitKey(1)==27) break;   //pounch "ESC"

				outputVideo << src;
				//t = ((double)getTickCount() - t)/getTickFrequency();  
				//t = 1/t;
				//cout << t <<endl;
    }

    cout << "Finished writing" << endl;
    return 0;
}