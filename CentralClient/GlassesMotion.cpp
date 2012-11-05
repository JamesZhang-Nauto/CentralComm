//============================================================================
// Name        : MotionEstimation.cpp
// Author      : zhuang lishuo
// Version     : 4.0
// Copyright   : zhlshuo
// Description : Estimate the motion of camera in C++
//============================================================================
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
#include "Aria.h"
#include "glassesVideo.h"
#include "CommReceiver.h"
#include <time.h>


using namespace cv;
using namespace std;

const int bufferFrames = 60;
CircularBuffer CB(bufferFrames);
int FrameCount = 0;
int ControlCount =0;
time_t startTime = 0;
time_t curTime = 0;
int diff = 0;
struct svm_model *model = svm_load_model("GlassesMotion/train.txt.model");
struct svm_model *NSModel = svm_load_model("GlassesMotion/Nod&Shake.model");
struct svm_model *SModel = svm_load_model("GlassesMotion/shake.model");
extern GLASSESMODE G_glassesMode;//1 idle, 2 OR, 3 control, 4 Cancel
RSGLASSESMODE RS_glassesMode;//1 Nod, 2 Shake
extern ArMutex GlassesModeMutex;
extern bool isDoneRobot;
int MSE(cv::Mat& curImage, cv::Mat& prevImage, int nPosX, int nPosY);
void MotionEst(cv::Mat& curImage, cv::Mat& prevImage, int range, int& moveX, int& moveY);
void gridSamplingImageMat(cv::Mat& org_imageMat, cv::Mat& to_imageMat, const int spacing, const int binSizeInShift);
void getPatternsWithInputImage(Mat preFrame, Mat curFrame, int& pattern);


//new
//turn left return 1, turn right return 2, nod return 3, no move return 0
int LeftOrRightOrNod(){
	vector<pair<int, int> > temp = CB.getLatestBuffer(3);
	float cumulatedX = 0;
	float cumulatedY = 0;
	for(int i = 0; i < 3; ++i){
		cumulatedX += temp[i].first;
		cumulatedY += temp[i].second;
	}

	cout << "cumulatedY:" << cumulatedY << endl;
	cout << "cumulatedX:" << cumulatedX << endl;
	
	if(cumulatedY > 2.5 && cumulatedX < 1.5 && cumulatedX > -1.5){
		CB.clear();
		return 3;
	}

	if(cumulatedX > 3){
		CB.clear();
		return 2;
	}

	if(cumulatedX < -3){
		CB.clear();
		return 1;
	}

	return 0;
}


void modeSwitch(Mat preFrame, Mat curFrame){
	int pattern = 0;
	getPatternsWithInputImage(preFrame, curFrame, pattern);
	GlassesModeMutex.lock();
	/*if(pattern != 0)*/{

		switch(G_glassesMode)
		{
		case idle:
			if(pattern == 1) {
				robotSpeak(255, "OR_Entry");
				G_glassesMode = glassesOR;
				cout << "OR" << endl;

			}
			break;//in the mode "idle", pattern "turn around" activate mode "OR"
		case glassesOR:
			if(pattern == 1)	{
				robotSpeak(255, "idle");
				G_glassesMode = idle;
				cout << "idle" << endl;

			}
			break;//in the mode "OR", pattern "turn around" switch mode "OR" into "idle"
		case glassesControl:
			
			++ControlCount;
			curTime = time(NULL);
			diff = curTime - startTime;
			if(ControlCount > 3 && diff > 1){
				ControlCount = 0;
				switch(LeftOrRightOrNod()){
				case 0: cout << "no move" << endl;;break;
				case 1: cout << "turn left" << endl;	startTime = time(NULL);	RobotCommand(RobotTurnLeft);		break;
				case 2: cout << "turn right" << endl;	startTime = time(NULL);	RobotCommand(RobotTurnRight);		break;
				case 3: cout << "nod" << endl;	startTime = time(NULL);		G_glassesMode=targetApproach;		RobotCommand(TargetApproach);break;

				}
				
				if(pattern == 1) 
				{
					robotSpeak(255, "idle");
					cout << "idle" << endl;
					G_glassesMode = idle;
				}
				
			}
			break;//in the mode "control", pattern "turn around" switch mode "OR" into "idle"
		  case robotSearch:
				if(RS_glassesMode == shake){
					CB.clear();
					isDoneRobot = true;
					robotSpeak(255, "cancel");
					RS_glassesMode = others;
					G_glassesMode = idle;
					
				}
				break;
			case targetApproach:
				if(RS_glassesMode == shake){
					CB.clear();
					isDoneRobot = true;
					//printf("\a");
					robotSpeak(0, "cancel");
					RS_glassesMode = others;
					G_glassesMode = idle;
					
				}
				break;
		}

		GlassesModeMutex.unlock();
	}
}

void getPatternsWithInputImage(Mat preFrame, Mat curFrame, int& pattern){
		++FrameCount;
		Mat preSampleMat, curSampleMat;
		gridSamplingImageMat(preFrame, preSampleMat, 2, 5);
		gridSamplingImageMat(curFrame, curSampleMat, 2, 5);
		int moveX = 0;
		int moveY = 0;
		double *p = new double[4];

		MotionEst(curSampleMat, preSampleMat, 2, moveX, moveY);
		CB.addElement(make_pair(moveX, moveY));

		//cout << moveX << " " << moveY << endl;
		struct svm_node *sample = (struct svm_node*)malloc(57 * sizeof(struct svm_node));
		for(int i = 0; i < 56; ++i){
			sample[i].index = i + 1;
		}
		sample[56].index = -1;

		vector<float> _56vector;

		if(FrameCount == 40){
			
			CircularBuffer tempCB;
			vector<pair<int, int> > reverseTemp = CB.getLatestBuffer(30);
			for(int i = 29; i >= 0; --i){
				tempCB.addElement(reverseTemp[i]);
			}

			FrameCount = 0;

			_56vector = tempCB.creat56Vector();
			for(int i = 0; i < _56vector.size(); ++i){
				sample[i].value = _56vector[i];
			}

			if(G_glassesMode == robotSearch || G_glassesMode == targetApproach){
				if(svm_predict(NSModel, sample) == 1){
					RS_glassesMode = nod;
				}else if(svm_predict(NSModel, sample) == 2){
					int framesNeeded = 10;
					vector<pair<int, int> > temp = tempCB.getLatestBuffer(framesNeeded);
					float cumulatedY = 0;
					float cumulatedX = 0;
					for(int i = 0; i < framesNeeded; ++i){
						cumulatedX += temp[i].first;
						cumulatedY += abs(temp[i].second);
					}
					cout << cumulatedY << " " << cumulatedX << endl;
					if(cumulatedY > 4 || cumulatedX > 8 || cumulatedX < -8)
						RS_glassesMode = others;
					else
						RS_glassesMode = shake;
				}else{
					RS_glassesMode = others;
				};//1.Nod 2.Shake 3.Others
			}else{
				pattern =  svm_predict_probability(model, sample, p);//1.TurnAround 2.Nod 3.Shake 4.Others
			}
			
			CB.clear();
		}else{
			pattern = 0;
		}
	}

	int MSE(cv::Mat& curImage, cv::Mat& prevImage, int nPosX, int nPosY)
	{
		int nCount = 0;
		int nDifference = 0;

		for (int y1 = 0; y1 < curImage.rows; y1++)
		{
			for (int x1 = 0; x1 < curImage.cols; x1++)
			{
				int x2 = x1 + nPosX;
				int y2 = y1 + nPosY;
				if (x2 >= 0 && x2 < curImage.cols && y2 >= 0 && y2 < curImage.rows)
				{

					nDifference += ((int)curImage.at<uchar>(y1,x1) - (int)prevImage.at<uchar>(y2,x2)) * ((int)curImage.at<uchar>(y1,x1) - (int)prevImage.at<uchar>(y2,x2));
					nCount++;
				}
			}
		}

		nDifference /= nCount;

		return nDifference;
	}

	void MotionEst(cv::Mat& curImage, cv::Mat& prevImage, int range, int& moveX, int& moveY)
	{
		int nMinError = 999999;
		int nMV_X = 0;
		int nMV_Y = 0;
		int nMinDist = 0;

		for (int nOffsetY = -range; nOffsetY <= range; nOffsetY++)
		{
			for (int nOffsetX = -range; nOffsetX <= range; nOffsetX++)
			{
				int nDifference = 0;

				nDifference = MSE(curImage, prevImage, nOffsetX, nOffsetY);

				if ( nDifference < nMinError)
				{
					nMinError = nDifference;
					nMV_X = nOffsetX;
					nMV_Y = nOffsetY;
					nMinDist = nOffsetX*nOffsetX + nOffsetY*nOffsetY;
				}
				else if (nDifference == nMinError)
				{
					int nDistance = nOffsetX*nOffsetX + nOffsetY*nOffsetY;

					if (nDistance < nMinDist)
					{
						nMinError = nDifference;
						nMV_X = nOffsetX;
						nMV_Y = nOffsetY;
						nMinDist = nDistance;
					}
				}
			}
		}

		moveX = nMV_X;
		moveY = nMV_Y;
	}

	void gridSamplingImageMat(cv::Mat& org_imageMat, cv::Mat& to_imageMat, const int spacing, const int binSizeInShift) {

		int nWidth = org_imageMat.cols;
		int nHeight = org_imageMat.rows;

		const int nSampledWidth = (nWidth >> binSizeInShift);
		const int nSampledHeight = (nHeight >> binSizeInShift);

		to_imageMat = cv::Mat(nSampledHeight, nSampledWidth, CV_8U);

		int** sampleBufferCounts;
		sampleBufferCounts = new int*[nSampledHeight];
		for(int i = 0; i < nSampledHeight; ++i){
			sampleBufferCounts[i] = new int[nSampledWidth];
		}

		int** sampleBuffer;
		sampleBuffer = new int*[nSampledHeight];
		for(int i = 0; i < nSampledHeight; ++i){
			sampleBuffer[i] = new int[nSampledWidth];
		}

		for (int i = 0; i < nSampledHeight; i++)
		{
			for (int j = 0; j < nSampledWidth; j++)
			{
				sampleBuffer[i][j] = 0;
				sampleBufferCounts[i][j] = 0;
			}
		}

		for (int y = 0; y < nHeight; y += spacing)
		{
			int by = y >> binSizeInShift;
			for (int px = 1; px < nWidth; px += spacing)
			{
				int the_x = (px>>binSizeInShift);
				int gray = (int)org_imageMat.at<uchar>(y+spacing/2,px);
				sampleBuffer[by][the_x] += gray;
				sampleBufferCounts[by][the_x] += 1;
			}
		}

		//< Average
		for (int i = 0; i < nSampledHeight; i++)
		{
			for (int j = 0; j < nSampledWidth; j++)
			{
				to_imageMat.at<uchar>(i,j) = sampleBuffer[i][j] / sampleBufferCounts[i][j];
			}
		}

	}
