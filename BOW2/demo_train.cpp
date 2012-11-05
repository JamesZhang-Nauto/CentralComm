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

// DBoW2
#include "DBoW2.h" // defines Surf64Vocabulary and Surf64Database

#include "DUtils.h"
#include "DVision.h"




using namespace DBoW2;
using namespace DUtils;
using namespace std;
using namespace cv;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void loadFeatures(vector<vector<vector<float> > > &features);
void queryFeatures(vector<vector<vector<float> > > &features);
void changeStructure(const vector<float> &plain, vector<vector<float> > &out,
  int L);
void testVocCreation(const vector<vector<vector<float> > > &features);
void queryDatabase(const vector<vector<vector<float> > > &features);
void queryDatabase(const vector<vector<vector<float> > > &features);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

#define training_or_query 1
// number of training images
const int MAX_IMAGES = 1400;
const int STEP_IMAGES = 100;
 int B_IMAGE = 0;

// extended surf gives 128-dimensional vectors
const bool EXTENDED_SURF = true;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void testAccruacy ()
{
  //   cout << "Matching images against themselves (0 low, 1 high): " << endl;
//   BowVector v1, v2;
//   for(int i = 0; i < STEP_IMAGES; i++)
//   {
//     voc.transform(features[i], v1);
//     for(int j = 0; j < STEP_IMAGES; j++)
//     {
//       voc.transform(features[j], v2);
//       
//       double score = voc.score(v1, v2);
//       cout << "Image " << i << " vs Image " << j << ": " << score << endl;
//     }
//   }
// 
//   // save the vocabulary to disk
//   cout << endl << "Saving vocabulary..." << endl;
//   voc.save("lab_voc.yml.gz");
//   cout << "Done" << endl;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void wait()
{
  cout << endl << "Press enter to continue" << endl;
  getchar();
}

// ----------------------------------------------------------------------------

int main()
{
  vector<vector<vector<float> > > features;

  
  
#if training_or_query  
  cout << "*********Start to traning********" << endl;
  for(; B_IMAGE<MAX_IMAGES; B_IMAGE+=STEP_IMAGES)
  {
    loadFeatures(features);
    testVocCreation(features);
  }
#else
  cout << "*********Start to query an image********" << endl;
  loadFeatures(features);
  queryDatabase(features);
#endif
  //wait();

  return 0;
}
// ----------------------------------------------------------------------------

void loadFeatures(vector<vector<vector<float> > > &features)
{
  features.clear();
  features.reserve(STEP_IMAGES);


  cout << "...Extracting SIFT features..." << endl;

  for(int i = B_IMAGE; i < B_IMAGE+STEP_IMAGES; ++i)
  {
    stringstream ss;
    ss << "../../VOC_TREE_DATA/ukbench";
    if(i<10) 			ss<< "0000"<<i<< ".jpg";
    if(i>=10&&i<100) 		ss<< "000"<<i<< ".jpg";
    if(i>=100&&i<1000) 		ss<< "00"<<i<< ".jpg";
    if(i>=1000&&i<10000) 	ss<< "0"<<i<< ".jpg";	//*/
		
    //ss << "../modules/image_" << i << ".jpeg"	;
    //ss << "../../VOC_TREE_DATA/ukbench00417.jpg"	;
    cout << ss.str() << endl;

    cv::Mat image = cv::imread(ss.str(), 0);

    cv::Mat mask;
    vector<cv::KeyPoint> keypoints;
    //vector<float> descriptors;
    cv::Mat descriptors;

    
    //-----------using SURF to get features descriptors------------------------
  //cv::SURF surf(400, 4, 2, EXTENDED_SURF);
    //surf(image, mask, keypoints, descriptors);
//     features.push_back(vector<vector<float> >());
//     changeStructure(descriptors, features.back(), surf.descriptorSize());

//---------------using SIFT to get features descriptors-------------------------
    initModule_nonfree();

    Ptr<Feature2D> sift1 = Algorithm::create<Feature2D>("Feature2D.SIFT");
    sift1->set("contrastThreshold", 0.01f);
    (*sift1)(image, noArray(), keypoints, descriptors);

    
    vector<vector<float> > vdesc;
    for (int i=0; i<descriptors.rows; i++)
    {
      vector<float> temp;
      descriptors.row(i).copyTo(temp);
      vdesc.push_back(temp);

    }
    cout << vdesc.size() << "  " << vdesc[0].size();
    features.push_back(vdesc);
//-------------------------------------------------------  
//     drawKeypoints(image, keypoints, image, Scalar(255,0,0));
//     imshow("clusters", image);
//     waitKey();
  }
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
  //Surf64Database db("d1400_db.yml.gz");
  cout << "... done! This is: " << endl << db << endl;

  // add images to the database
  for(int i = 0; i < STEP_IMAGES; i++)
  {
    db.add(features[i]);
  }
  cout << "Saving database..." << endl;
  db.save("d1400_db.yml.gz");
  cout << "... done!" << endl;


  cout << "Open database!" << endl;


  // once saved, we can load it again  
  cout << "Retrieving database once again..." << endl;
  Surf64Database db2("d1400_db.yml.gz");
  cout << "... done! This is: " << endl << db2 << endl;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void queryDatabase(const vector<vector<vector<float> > > &features)
{
    cout << "Open database!" << endl;
    
    cout << "d1400_db.yml.gz" << endl;
    Surf64Database db("lab_db.yml.gz");
    
    
    cout << "Database information: " << endl << db << endl;

    // and query the database
    cout << "Querying the database: " << endl;

    QueryResults ret;
    for(int i = 0; i < STEP_IMAGES; i++)
    {
      db.query(features[i], ret, 4);

      // ret[0] is always the same image in this case, because we added it to the 
      // database. ret[1] is the second best match.

      cout << "Searching for Image " << i+B_IMAGE << ". " << ret << endl;
    }

    cout << endl;


}

// ----------------------------------------------------------------------------


