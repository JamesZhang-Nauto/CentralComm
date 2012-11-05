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


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

// number of training images
const int STEP_IMAGES = 100;
 int B_IMAGE = 100;

// extended surf gives 128-dimensional vectors
const bool EXTENDED_SURF = true;

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

  for(;B_IMAGE<1400;B_IMAGE+=100)
  {
    loadFeatures(features);
    testVocCreation(features);
  }

  



  return 0;
}
// ----------------------------------------------------------------------------

void loadFeatures(vector<vector<vector<float> > > &features)
{
  features.clear();
  features.reserve(STEP_IMAGES);

  cv::SURF surf(400, 4, 2, EXTENDED_SURF);

  cout << "...Extracting SURF features..." << endl;

  for(int i = B_IMAGE; i < B_IMAGE+STEP_IMAGES; ++i)
  {
    stringstream ss;
    ss << "../../VOC_TREE_DATA/ukbench";
    if(i<10) 			ss<< "0000"	<<i<< ".jpg";
    if(i>=10&&i<100) 		ss<< "000"	<<i<< ".jpg";
    if(i>=100&&i<1000) 		ss<< "00"	<<i<< ".jpg";
    if(i>=1000&&i<10000) 	ss<< "0"	<<i<< ".jpg";	
    if(i>=10000)		ss		<<i<< ".jpg";	
    //*/
		
    //ss << "../modules/image_" << i << ".jpeg"	;
    
    cout << ss.str() << endl;

    cv::Mat image = cv::imread(ss.str(), 0);

    cv::Mat mask;
    vector<cv::KeyPoint> keypoints;
    vector<float> descriptors;

    surf(image, mask, keypoints, descriptors);

    features.push_back(vector<vector<float> >());
    changeStructure(descriptors, features.back(), surf.descriptorSize());
    
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


  //Surf64Database db(voc, false);
  Surf64Database db("d1400_db.yml.gz");

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


