/**
* File: ObjectRecognition.cpp
* Date: Sept 2012
* Author: Jingzhe Zhang
* Description: using voc tree and homography recognizes objects.
*/

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "ObjectRecognition.h"



//-----------------------------------------------------------------

// --------------------variable declarations-----------------------------------------

#define TRAINING_OR_QUERY 0	//training is 1, query is 0
#define FEATURE_EXTRATION 1	//sift is 1, surf is 0
#define OBJ_IMG 9 //*******************************************

// number of training images
#if TRAINING_OR_QUERY
const int MAX_IMAGES = 110;
const int STEP_IMAGES = 110;
int B_IMAGE = 0;
#else
const unsigned int MAX_IMAGES = 110;
const int STEP_IMAGES = 1;
int B_IMAGE = 0;
unsigned int img_index = B_IMAGE;
#endif


float AP=0;
// extended surf gives 128-dimensional vectors
const bool EXTENDED_SURF = true;

//-----------------------------------------------------------------------------


void cal_pre_recall(QueryResults &ret, vector<float> &precision, vector<float> &recall)
{
	precision.clear();
	recall.clear();

	int object = ret[0].Id/(OBJ_IMG+1);
	float score=0.0;
	for (unsigned int i=1;i<ret.size(); i++) //get rid of top 1
	{
		if ( (ret[i].Id/(OBJ_IMG+1)) == object)
			score+=1.0;

		precision.push_back(score/(float)i);
		recall.push_back(score/OBJ_IMG);
		if (score==OBJ_IMG)
			break;
	}
	for(unsigned int i=0; i<precision.size(); i++)
		cout<< "Presicion " << i<<" is: " << precision[i] 
	<<"; recall: " <<recall[i]<< endl;

	cout<< endl;
}


void testAP (QueryResults &ret)
{
	vector<float> precision, recall;

	cal_pre_recall(ret, precision, recall);
	AP += precision[0]*recall[0];
	for(unsigned int i=1; i<precision.size(); i++)
	{
		AP += precision[i]*(recall[i]-recall[i-1]);
		cout<< "i="<<i<<" "<<AP << "   ";

	}
	cout<< endl;
	cout<< "the Average Precision is: "<< AP << endl;

}
// ----------------------------------------------------------------------------

void wait()
{
	cout<< endl << "Press press any keyboard to continue..." << endl;
	getchar();
}



// ----------------------------------------------------------------------------

void ObjectRecognition::loadFeatures(Mat& inImage, vector< KeyPoint >& keypoints, vector< std::vector< float > >& features, Mat& descriptors)
{
	features.clear();
	keypoints.clear();
	if(input_type==0) //input_type = 0 camera
		cvtColor(inImage, inImage, CV_RGB2GRAY);


#if FEATURE_EXTRATION
	//---------------using SIFT to get features descriptors-------------------------
	//cout<< "...Extracting SIFT features..." << endl;

	initModule_nonfree();
	SIFT sift(1, 3, 0.04, 10, 1.0);
	sift(inImage, noArray(), keypoints, descriptors);


	//     vector<vector<float> > vdesc;
	//     vdesc.reserve(descriptors.rows);
	for (int i=0; i<descriptors.rows; i++)
	{
		features.push_back(descriptors.row(i));
	}
	//     cout<< "descriptors: " << vdesc.size() << "  " << vdesc[0].size() << endl;

#else
	//-----------using SURF to get features descriptors------------------------
	vector<float> descriptors;
	cv::Mat mask;
	cv::SURF surf(400, 4, 2, EXTENDED_SURF);
	surf(inimage, mask, keypoints, descriptors);
	features.push_back(vector<vector<float> >());
	changeStructure(descriptors, features.back(), surf.descriptorSize());
#endif

	//----------------------------display keypoints--------------------------  
	//     drawKeypoints(image, keypoints, image, Scalar(255,0,0));
	//     imshow("clusters", image);
	//     waitKey();

}



// ----------------------------------------------------------------------------

void changeStructure(const vector<float> &plain, vector<vector<float> > &out, int L)
{
	out.resize(plain.size() / L);

	int j = 0;
	for(int i = 0; i < plain.size(); i += L, ++j)
	{
		out[j].resize(L);
		std::copy(plain.begin() + i, plain.begin() + i + L, out[j].begin());
	}
}



// ----------------------------------------------------------------------------
void ObjectRecognition::queryDatabase(const vector< vector< float > >& features, Surf64Database& db, QueryResults& ret, int ret_num)
{
	db.query(features, ret, ret_num);
}
// ----------------------------------------------------------------------------


// --------------------------------Constructors--------------------------------
ObjectRecognition::ObjectRecognition(char *dbname)
{
	input_type = 1; //use image as input
	db = Surf64Database(dbname);
	ret_num = 4;

}
ObjectRecognition::ObjectRecognition(VideoCapture& _capture, int _ret_num, char *dbname)
{
	input_type = 0; //use camera as input
	capture = _capture;
	ret_num = _ret_num;
	db = Surf64Database(dbname);

}
ObjectRecognition::ObjectRecognition(Mat _mat, int _ret_num, char *dbname)
{
	input_type = 1; //use image as input
	image = _mat;
	ret_num = _ret_num;
	db = Surf64Database(dbname);

}



//
//int ObjectRecognition::find()
//{
//	//-----------------------------------------------------------
//	//     image = imread( "../../../New_modules1/image_014.jpeg");
//	//    image = imread( "../../../TestDATA/snap-unknown-20120901-203157-1.jpeg" );
//	//   imshow( "mywindow", image );
//	//   waitKey(0);
//	double t=0;
//	if(input_type==0)
//		capture.read(image);
//
//	//      imshow("Capure_Image", image);
//	//      waitKey();
//
//	//-----------------Extract the features----------------------------
//	//cout<< ss.str() << endl;
//	t = (double)cvGetTickCount();
//	loadFeatures(image, keypoints, features, descriptors);
//
//	//-----------------Query the Database----------------------------
//	//cout <<endl<<"-----------------Start to QUERY-------------------" <<endl;
//	queryDatabase(features, db, ret, ret_num);
//	//testAP(ret); 
//	//       cout << ret <<endl;
//	t = (double)cvGetTickCount()-t;
//	//       cout << "voc tree time = " <<  t/((double)cvGetTickFrequency()*1000)<< endl;
//	if (ret[0].Score<0.6 &&ret[0].Score>0.2)
//	{
//		t = (double)cvGetTickCount();
//		checkHomography( image, keypoints, descriptors, ret);
//		t = (double)cvGetTickCount()-t;
//		cout << "Homography time = " <<  t/((double)cvGetTickFrequency()*1000)<< endl;
//		sort(ret.begin(), ret.end());
//		//       if ((ranking/(OBJ_IMG+1))==(ret[0].Id/(OBJ_IMG+1)))
//		// 	AP+=1;
//
//		// // 	cout<<ret<<endl<<endl;
//	}
//
//	int dis_id=0;
//	bool ret_correct_flag = false;
//	int ret_end = ret.size()-1;
//	if (ret[ret_end].Score>ret[0].Score) 
//	{
//		if(ret[ret_end].Id/10==ret[ret_end-1].Id/10)
//		{
//			dis_id = ret_end;
//			ret_correct_flag = true;
//		}
//	}
//	else 
//	{
//		if (ret[1].Id/10==ret[0].Id/10)
//		{
//			dis_id = 0;
//			ret_correct_flag = true;
//		}
//	}
//
//	if ((ret[dis_id].Score>0.6) && ret_correct_flag)
//	{
//
//		cout<< "-----Object "<< ret[dis_id].Id << " is detected!!!---------------"<<endl;
//		return ret[dis_id].Id;
//
//		//------------------------display result------------------------------------
//		// 	stringstream ss;
//		// 	ss<<"../../../New_modules1/image_";
//		// 	if(ret[dis_id].Id<10) 			ss<< "00"<<ret[dis_id].Id<< ".jpeg";
//		// 	if(ret[dis_id].Id>=10&&ret[dis_id].Id<100) 	ss<< "0"<<ret[dis_id].Id<<".jpeg";
//		// 	if(ret[dis_id].Id>=100	)		ss<< ret[dis_id].Id<<".jpeg";
//		// 	cout<< ss.str()<< endl;
//		// 	Mat r=imread(ss.str());
//		// // 	namedWindow(ss.str());
//		// // 	imshow(ss.str(), r);
//		// // 	moveWindow(ss.str(), 650, 0);
//		// 	imshow("result", r);
//		//          waitKey(0);
//		//---------------------------------------------------------------------------
//	}
//	else
//	{
//		// 	cout<< "No object!!!!!"<<endl;
//		return 254;
//
//	}
//	//cout<<"AP is : " << AP <<endl<<endl<<endl;
//	//testAP(ret);
//
//	//------------------------------------------------------------------
//
//	//        waitKey(0);
//	//       if ( (waitKey(1) & 255) == 27 ) break;
//
//	return 254;
//}

int ObjectRecognition::find(Mat _image)
{
	//-----------------------------------------------------------
	//  image = imread( "../../../New_modules1/image_014.jpeg");
	//  image = imread( "../../../TestDATA/snap-unknown-20120901-203157-1.jpeg" );
	//  imshow( "mywindow", image );
	//  waitKey(0);


	//      imshow("Capure_Image", image);
	//      waitKey();

	//-----------------Extract the features----------------------------
	//cout<< ss.str() << endl;
	double t = (double)cvGetTickCount(); 
	loadFeatures(_image, keypoints, features, descriptors);

	//-----------------Query the Database----------------------------
	//cout <<endl<<"-----------------Start to QUERY-------------------" <<endl;
	queryDatabase(features, db, ret, ret_num);
	//testAP(ret); 
	//cout << ret <<endl;
	//t = (double)cvGetTickCount()-t;
	//       cout << "voc tree time = " <<  t/((double)cvGetTickFrequency()*1000)<< endl;
	//if (ret[0].Score<0.6 && ret[0].Score>0.24)
	{
		cout << "        Start to Homography   " <<endl;
		initModule_nonfree();
		double HomographyT = (double)cvGetTickCount();
		checkHomography( _image, keypoints, descriptors, ret);
		HomographyT = (double)cvGetTickCount()-HomographyT;
		cout << "Homography time (ms) = " <<  HomographyT/((double)cvGetTickFrequency()*1000)<< endl;
		sort(ret.begin(), ret.end());
		//       if ((ranking/(OBJ_IMG+1))==(ret[0].Id/(OBJ_IMG+1)))
		// 	AP+=1;

		 	//cout<<ret<<endl<<endl;
	}


	int dis_id=0;
	bool ret_correct_flag = false;
	int ret_end = ret.size()-1;
	if (ret[ret_end].Score>ret[0].Score) 
	{
		if(ret[ret_end].Id/10==ret[ret_end-1].Id/10)
		{
			dis_id = ret_end;
			ret_correct_flag = true;
		}
	}
	else 
	{
		if (ret[1].Id/10==ret[0].Id/10)
		{
			dis_id = 0;
			ret_correct_flag = true;
		}
	}


	if ((ret[dis_id].Score>=0.6) && ret_correct_flag)
	{

		cout<< "-----Object "<< ret[dis_id].Id << " is detected!!!---------------"<<endl;
		return ret[dis_id].Id;
		// 	stringstream ss;
		// 	ss<<"../../../New_modules1/image_";
		// 	if(ret[dis_id].Id<10) 			ss<< "00"<<ret[dis_id].Id<< ".jpeg";
		// 	if(ret[dis_id].Id>=10&&ret[dis_id].Id<100) 	ss<< "0"<<ret[dis_id].Id<<".jpeg";
		// 	if(ret[dis_id].Id>=100	)		ss<< ret[dis_id].Id<<".jpeg";
		// 	cout<< ss.str()<< endl;
		// 	Mat r=imread(ss.str());
		// // 	namedWindow(ss.str());
		// // 	imshow(ss.str(), r);
		// // 	moveWindow(ss.str(), 650, 0);
		// 	imshow("result", r);
		//          waitKey(0);
	}
	else
	{
		cout<< "No object!!!!!"<<endl;
		return 255;

	}
	//cout<<"AP is : " << AP <<endl<<endl<<endl;
	//testAP(ret);

	//------------------------------------------------------------------

	//        waitKey(0);
	//       if ( (waitKey(1) & 255) == 27 ) break;

	return 255;
}
