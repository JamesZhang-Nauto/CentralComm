#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	Mat image;
	VideoCapture capture(3);
	capture.set(CV_CAP_PROP_FRAME_WIDTH , 640);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	vector<int> params;
	params.push_back(CV_IMWRITE_JPEG_QUALITY);
	params.push_back(100);
	
	int img_id=0;

	while(capture.read(image))
	{
		imshow("Camera", image);
		if (waitKey(50)>=0)
		{
			stringstream saveImgIndex;
			saveImgIndex << "image_"; 
			if(img_id<10) 			saveImgIndex<< "00"<<img_id<< ".jpeg";
			if(img_id>=10&&img_id<100) 		saveImgIndex<< "0"<<img_id<< ".jpeg";
			if(img_id>=100	)		saveImgIndex<< img_id << ".jpeg";
			imwrite(saveImgIndex.str(), image, params );

			cout << "image has been token ..." << endl;
			img_id++;
			
		}
			
	}

}