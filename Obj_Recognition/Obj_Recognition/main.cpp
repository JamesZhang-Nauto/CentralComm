#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ObjectRecognition.h"
#include "iostream"

using namespace std;
using namespace cv;

int main()
{
	int gl_result = 255;
	Mat gl_img, gl_img_OR;
		Mat gl_img_bk;
	Mat glres_image;				//display result image
	

  VideoCapture gl_capture(3); 
	gl_capture.set(CV_CAP_PROP_FRAME_WIDTH , 640);
	gl_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

  if(!gl_capture.isOpened())
	{
		cout << "Cannot open glasses video !" << endl;
	}

	


	ObjectRecognition gl_or("glasses_3_db.yml.gz");


	namedWindow("Video Live");
	moveWindow("Video Live", 645, 0);
	namedWindow("Glasses_result",CV_WINDOW_NORMAL);
	moveWindow("Glasses_result",1000,600);


	int img_id=0;
	vector<int> params;
	params.push_back(CV_IMWRITE_JPEG_QUALITY);
	params.push_back(100);
	int queryIndex = 0;
	//while(1)
	for(; queryIndex<15; queryIndex++)
	{

		gl_capture >> gl_img;


		stringstream query_src; 
		ObjectRecognition::loadImage(query_src, queryIndex, true);
		gl_img = imread(query_src.str());


		//gl_img = imread("c:/image_000.jpeg");


		cvtColor(gl_img,gl_img_bk,CV_RGB2GRAY);
		imshow("Video Live",gl_img_bk);
		waitKey(1);

			gl_result=255;
			gl_result = gl_or.find(gl_img_bk);

			

			if(gl_result !=255)
			{
				//-------------------------Display the result ------------------------
			stringstream saveImgIndex;
			saveImgIndex << "image_"; 
			if(img_id<10) 			saveImgIndex<< "00"<<img_id<< ".jpeg";
			if(img_id>=10&&img_id<100) 		saveImgIndex<< "0"<<img_id<< ".jpeg";
			if(img_id>=100	)		saveImgIndex<< img_id << ".jpeg";
			imwrite(saveImgIndex.str(), gl_img, params );

				stringstream ret_src1;  //result src
				ObjectRecognition::loadImage(ret_src1, gl_result, true);
				glres_image = imread(ret_src1.str());
				imshow("Glasses_result", glres_image);
				waitKey(1);

			}

		}
   
}