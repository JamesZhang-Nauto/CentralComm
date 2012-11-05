/**
 * File: Demo.cpp
 * Date: November 2011
 * Author: Dorian Galvez-Lopez
 * Description: demo application of DBoW2
 */

#include <iostream>
#include <vector>
#include <math.h>

// OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// DBoW2
#include "DBoW2.h" // defines Surf64Vocabulary and Surf64Database

#include "DUtils.h"
#include "DVision.h"

#include "Homography.h"


using namespace DBoW2;
using namespace DUtils;
using namespace std;
using namespace cv;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

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
const string dbname = "d120_11_3_db.yml.gz"; 

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
  cout<< endl << "Press enter to continue" << endl;
  getchar();
}



// ----------------------------------------------------------------------------

void loadFeatures(Mat &image, vector<KeyPoint> &keypoints, vector<vector<float> > &features, Mat &descriptors)
{
  features.clear();
  //features.reserve(STEP_IMAGES);
  keypoints.clear();
  
    cvtColor(image,image,CV_RGB2GRAY);
#if FEATURE_EXTRATION
    //---------------using SIFT to get features descriptors-------------------------
    //cout<< "...Extracting SIFT features..." << endl;

    initModule_nonfree();

    SIFT sift(1, 3, 0.04, 10, 1.0);
    sift(image, noArray(), keypoints, descriptors);

    
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
    surf(image, mask, keypoints, descriptors);
    features.push_back(vector<vector<float> >());
    changeStructure(descriptors, features.back(), surf.descriptorSize());
#endif

//-------------------------------------------------------  
//     drawKeypoints(image, keypoints, image, Scalar(255,0,0));
//     imshow("clusters", image);
//     waitKey();

}



// ----------------------------------------------------------------------------

void changeStructure(const vector<float> &plain, vector<vector<float> > &out, int L)
{
  out.resize(plain.size() / L);

  unsigned int j = 0;
  for(unsigned int i = 0; i < plain.size(); i += L, ++j)
  {
    out[j].resize(L);
    std::copy(plain.begin() + i, plain.begin() + i + L, out[j].begin());
  }
}

// ----------------------------------------------------------------------------

void testVocCreation(const vector<vector<vector<float> > > &features)
{
  // branching factor and depth levels 
  const int k = 9;
  const int L = 3;
  const WeightingType weight = TF_IDF;
  const ScoringType score = L1_NORM;

  Surf64Vocabulary voc(k, L, weight, score); 

  cout<< "Creating a small " << k << "^" << L << " vocabulary..." << endl;
  voc.create(features);
  cout<< "... done!" << endl;

  cout<< "Vocabulary information: " << endl
  << voc << endl << endl; 


  Surf64Database db(voc, false);
  //Surf64Database db(dbname);
  cout<< "... done! This is: " << endl << db << endl;

  // add images to the database
  for(int i = 0; i < STEP_IMAGES; i++)
  {
    db.add(features[i]);
  }
  cout<< "Saving database..." << endl;
  db.save(dbname);
  cout<< "... done!" << endl;


  cout<< "Open database!" << endl;

  // once saved, we can load it again  
  cout<< "Retrieving database once again..." << endl;
  Surf64Database db2(dbname);
  cout<< "... done! This is: " << endl << db2 << endl;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void queryDatabase(const vector<vector<float> >  &features,Surf64Database &db, QueryResults &ret, int ret_num)
{
    db.query(features, ret, ret_num);

}
// ----------------------------------------------------------------------------

int main()
{
//---------------------------Define the variables--------------------------------
  vector<vector<float> > features;
  vector<cv::KeyPoint> keypoints;
  QueryResults ret;
//-------------------------------------------------------------------------------
  
#if TRAINING_OR_QUERY  
  cout<< "*********Start to traning********" << endl;
  int temp=0;
    loadFeatures(features,temp);
    testVocCreation(features);
#else
  cout<< "*********Start to query an image********" << endl;
  
  cout<< endl<<"Open database " << dbname << " !" << endl;      
  Surf64Database db(dbname);
  cout<< "Database information: " << endl << db << endl;
 
//------------------Calling the camera----------------------
   VideoCapture capture = VideoCapture(-1);
   capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
   capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
   Mat image;
   Mat descriptors;
   cvNamedWindow( "Capure_Image", CV_WINDOW_AUTOSIZE );
//-----------------------------------------------------------
   //image = imread( "../../New_modules1/image_014.jpeg");
//    image = imread( "../../tea2.jpeg" );
//   imshow( "mywindow", image );
//   waitKey(0);
   double t=0;
   while(capture.read(image))
   {
     imshow("Capure_Image", image);
//-----------------Extract the features----------------------------
  //cout<< ss.str() << endl;
     t = (double)cvGetTickCount();
     loadFeatures(image, keypoints, features, descriptors);

//-----------------Query the Database----------------------------
//   cout <<endl<<"-----------------Start to QUERY-------------------" <<endl;
     queryDatabase(features, db, ret, 4);
    //testAP(ret); 
     cout << ret <<endl;
    t = (double)cvGetTickCount()-t;

    cout << "voc tree time = " <<  t/((double)cvGetTickFrequency()*1000)<< endl;
    if (ret[0].Score<0.6 &&ret[0].Score>0.2)
    {
	t = (double)cvGetTickCount();
	checkHomography( image, keypoints, descriptors, ret);
	t = (double)cvGetTickCount()-t;
	cout << "Homography time = " <<  t/((double)cvGetTickFrequency()*1000)<< endl;
	sort(ret.begin(), ret.end());
  //       if ((ranking/(OBJ_IMG+1))==(ret[0].Id/(OBJ_IMG+1)))
  // 	AP+=1;
    
     cout<<ret<<endl<<endl;
    int dis_id=0;
    bool ret_correct_flag = false;
    if (ret[ret.size()-1].Score>ret[0].Score) 
    {
      if ((ret[ret.size()-1].Id/10 == ret[ret.size()-2].Id/10)||(ret[20].Id+ret[19].Id>240))
      {
	dis_id=ret.size()-1;
	ret_correct_flag = true;
      }
    }
    else 
    {
      if ((ret[1].Id/10==ret[0].Id/10 ) ||(ret[1].Id+ret[0].Id>240))
      {
	dis_id=0;
	ret_correct_flag = true;
      }
    }
    cout <<dis_id<<endl;
    if ((ret[dis_id].Score>0.6) && ret_correct_flag)
    {
      
      cout<< "-----Object is detected!!!---------------"<<endl;
	stringstream ss;
	ss<<"../../New_modules1/image_";
	if(ret[dis_id].Id<10) 			ss<< "00"<<ret[dis_id].Id<< ".jpeg";
	if(ret[dis_id].Id>=10&&ret[dis_id].Id<100) 	ss<< "0"<<ret[dis_id].Id<<".jpeg";
	if(ret[dis_id].Id>=100	)		ss<< ret[dis_id].Id<<".jpeg";
	cout<< ss.str()<< endl;
	Mat r=imread(ss.str());
// 	namedWindow(ss.str());
// 	imshow(ss.str(), r);
// 	moveWindow(ss.str(), 650, 0);
	imshow("result", r);
//          waitKey(0);
    }
    else
    {
      cout<< "No object is detected!!!!!"<<endl;
    }
    }
  //     //cout<<"AP is : " << AP <<endl<<endl<<endl;
      //testAP(ret);

  //------------------------------------------------------------------
      
//        waitKey(0);
      if ( (waitKey(1) & 255) == 27 ) break;
   }
 
#endif
  //wait();
#if TRAINING_OR_QUERY==0
  AP= AP/(MAX_IMAGES-B_IMAGE);
  cout<< "final AP: " << AP;
#endif;
  return 0;
}

