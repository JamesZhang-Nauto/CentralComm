/**
 * File: Demo.cpp
 * Date: November 2011
 * Author: Dorian Galvez-Lopez
 * Description: demo application of DBoW2
 */

#include <iostream>
#include <vector>

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


#define uint unsigned int

// --------------------variable declarations-----------------------------------------

#define TRAINING_OR_QUERY 0	//training is 1, query is 0
#define FEATURE_EXTRATION 1	//sift is 1, surf is 0
// number of training images
#if TRAINING_OR_QUERY
const int MAX_IMAGES = 15;
const int STEP_IMAGES = 15;
int B_IMAGE = 0;
#else
const uint MAX_IMAGES = 10;
const int STEP_IMAGES = 1;
int B_IMAGE = 0;
  uint img_index = B_IMAGE;
#endif
const string dbname = "home_3_db.yml.gz"; //"newmodules_db.yml.gz";
int obj_img = 4; //*******************************************
float AP=0;
// extended surf gives 128-dimensional vectors
const bool EXTENDED_SURF = true;

//-----------------------------------------------------------------------------

    
void cal_pre_recall(QueryResults &ret, vector<float> &precision, vector<float> &recall)
{
  precision.clear();
  recall.clear();

  int object = ret[0].Id/(obj_img+1);
  float score=0.0;
  for (uint i=1;i<ret.size(); i++) //get rid of top 1
  {
    if ( (ret[i].Id/(obj_img+1)) == object)
      score+=1.0;

    precision.push_back(score/(float)i);
    recall.push_back(score/obj_img);
    if (score==obj_img)
      break;
  }
  for(uint i=0; i<precision.size(); i++)
    cout << "Presicion " << i<<" is: " << precision[i] 
    <<"; recall: " <<recall[i]<< endl;
  
  cout << endl;
}


void testAP (QueryResults &ret)
{
  vector<float> precision, recall;
  
  cal_pre_recall(ret, precision, recall);
  AP += precision[0]*recall[0];
  for(uint i=1; i<precision.size(); i++)
  {
    AP += precision[i]*(recall[i]-recall[i-1]);
    cout << "i="<<i<<" "<<AP << "   ";

  }
  cout << endl;
  cout << "the Average Precision is: "<< AP << endl;

}
// ----------------------------------------------------------------------------

void wait()
{
  cout << endl << "Press enter to continue" << endl;
  getchar();
}



// ----------------------------------------------------------------------------

void loadFeatures(vector<cv::KeyPoint> &keypoints, vector<vector<vector<float> > > &features, stringstream &ss)
{
  features.clear();
  features.reserve(STEP_IMAGES);
#if TRAINING_OR_QUERY 
  for(int i = B_IMAGE; i < B_IMAGE+STEP_IMAGES; ++i)
  {

#endif

    cv::Mat image = cv::imread(ss.str(), 0);

    cv::Mat mask;
    
    
#if FEATURE_EXTRATION
    //---------------using SIFT to get features descriptors-------------------------
    //cout << "...Extracting SIFT features..." << endl;
    cv::Mat descriptors;
    initModule_nonfree();

		SIFT sift(1, 3, 0.04, 10, 1.0);
    sift(image, noArray(), keypoints, descriptors);

    //Ptr<Feature2D> sift1 = Algorithm::create<Feature2D>("Feature2D.SIFT");
    //sift1->set("contrastThreshold", 0.01f);
    //(*sift1)(image, noArray(), keypoints, descriptors);

    
    vector<vector<float> > vdesc;
    for (int i=0; i<descriptors.rows; i++)
    {
      vdesc.push_back(descriptors.row(i));

    }
    cout << "descriptors: " <<vdesc.size() << "  " << vdesc[0].size() << endl;
    features.push_back(vdesc);
#else
    //-----------using SURF to get features descriptors------------------------
    vector<float> descriptors;
    cv::SURF surf(400, 4, 2, EXTENDED_SURF);
    surf(image, mask, keypoints, descriptors);
    features.push_back(vector<vector<float> >());
    changeStructure(descriptors, features.back(), surf.descriptorSize());
#endif

//-------------------------------------------------------  
//     drawKeypoints(image, keypoints, image, Scalar(255,0,0));
//     imshow("clusters", image);
//     waitKey();
#if TRAINING_OR_QUERY    
  }
#endif 
}



// ----------------------------------------------------------------------------

void changeStructure(const vector<float> &plain, vector<vector<float> > &out,
  int L)
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

  cout << "Creating a small " << k << "^" << L << " vocabulary..." << endl;
  voc.create(features);
  cout << "... done!" << endl;

  cout << "Vocabulary information: " << endl
  << voc << endl << endl; 


  Surf64Database db(voc, false);
  //Surf64Database db(dbname);
  cout << "... done! This is: " << endl << db << endl;

  // add images to the database
  for(int i = 0; i < STEP_IMAGES; i++)
  {
    db.add(features[i]);
  }
  cout << "Saving database..." << endl;
  db.save(dbname);
  cout << "... done!" << endl;


  cout << "Open database!" << endl;

  // once saved, we can load it again  
  cout << "Retrieving database once again..." << endl;
  Surf64Database db2(dbname);
  cout << "... done! This is: " << endl << db2 << endl;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void queryDatabase(const vector<vector<vector<float> > > &features,Surf64Database &db, QueryResults &ret)
{

    // and query the database
    cout << "Querying the database: " << endl;

    
    for(int i = 0; i < STEP_IMAGES; i++)
    {
      db.query(features[i], ret, 20);

      // ret[0] is always the same image in this case, because we added it to the 
      // database. ret[1] is the second best match.

      cout << "Searching for Image " << i+B_IMAGE << ". " << ret << endl;
    }

    cout << endl;
    

}
// ----------------------------------------------------------------------------


int main()
{
  vector<vector<vector<float> > > features;
  vector<cv::KeyPoint> keypoints;
  QueryResults ret;
#if TRAINING_OR_QUERY  
  cout << "*********Start to traning********" << endl;
  int temp=0;
    loadFeatures(features,temp);
    testVocCreation(features);

#else
  cout << "*********Start to query an image********" << endl;
  
  cout << endl<<"Open database " << dbname << " !" << endl;      
  Surf64Database db(dbname);
  cout << "Database information: " << endl << db << endl;
  

  for (; img_index<MAX_IMAGES; img_index++)
  {

    stringstream ss;

    ss << "C:/Users/James/Dropbox/projects/CentralComm/Obj_Recognition/Obj_Recognition/Home/image_";
    if(img_index<10) 			ss<< "00"<<img_index<< ".jpeg";
    if(img_index>=10&&img_index<100) 		ss<< "0"<<img_index<< ".jpeg";
    if(img_index>=100	)		ss<< img_index << ".jpeg";
    
//     ss << "../../VOC_TREE_DATA/ukbench";    
//     if(i<10) 			ss<< "0000" <<i<< ".jpg";
//     if(i>=10&&i<100) 		ss<< "000"  <<i<< ".jpg";
//     if(i>=100&&i<1000) 		ss<< "00"   <<i<< ".jpg";
//     if(i>=1000&&i<10000) 	ss<< "0"    <<i<< ".jpg";
//     if(i>=10000) 		ss	    <<i<< ".jpg";//*/


    
    //ss << "../snap-unknown-20120825-200118-1.jpeg"	;
    //ss << "../../VOC_TREE_DATA/ukbench00005.jpg"	;
    //ss << "../ukbench000004.jpg"	;
//#endif
    cout << ss.str() << endl;
    loadFeatures(keypoints, features, ss);
    queryDatabase(features, db, ret);
    //testAP(ret);
    if ( ((ret[1].Id/(obj_img+1))==(ret[2].Id/(obj_img+1)))
//       || ((ret[1].Id/(obj_img+1))==(ret[3].Id/(obj_img+1))) 
//       || ((ret[2].Id/(obj_img+1))==(ret[3].Id/(obj_img+1))) 
    )
    {
      cout<< "the top 3 are the same!" <<endl;
      if ((ret[1].Id/(obj_img+1))==(ret[0].Id/(obj_img+1)))
				AP+=1;
    }
    else
    {
      stringstream ss;
          ss << "C:/Users/James/Dropbox/projects/CentralComm/Obj_Recognition/Obj_Recognition/Home/image_";
    if(img_index<10) 			ss<< "00"<<img_index<< ".jpeg";
    if(img_index>=10&&img_index<100) 		ss<< "0"<<img_index<< ".jpeg";
    if(img_index>=100	)		ss<< img_index << ".jpeg";
    
      int ranking = checkHomography( ss, ret);
      if ((ranking/(obj_img+1))==(ret[0].Id/(obj_img+1)))
	AP+=1;
    }
    cout<<"AP is : " << AP <<endl<<endl<<endl;
    //testAP(ret);
  }
#endif
  //wait();
#if TRAINING_OR_QUERY==0
  AP= AP/(MAX_IMAGES-B_IMAGE);
  cout << "final AP: " << AP;
	getchar();
#endif;
  return 0;
}

