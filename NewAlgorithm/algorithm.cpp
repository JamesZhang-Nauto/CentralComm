//  demo.cpp
//
//	Here is an example on how to use the descriptor presented in the following paper:
//	A. Alahi, R. Ortiz, and P. Vandergheynst. FREAK: Fast Retina Keypoint. In IEEE Conference on Computer Vision and Pattern Recognition, 2012.
//  CVPR 2012 Open Source Award winner
//


#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>

using namespace cv;

static void help( char** argv )
{
    std::cout << "\nUsage: " << argv[0] << " [path/to/image1] [path/to/image2] \n"
              << "This is an example on how to use the keypoint descriptor presented in the following paper: \n"
              << "A. Alahi, R. Ortiz, and P. Vandergheynst. FREAK: Fast Retina Keypoint. \n"
              << "In IEEE Conference on Computer Vision and Pattern Recognition, 2012. CVPR 2012 Open Source Award winner \n"
              << std::endl;
}

int main( int argc, char** argv ) {
    // check http://opencv.itseez.com/doc/tutorials/features2d/table_of_content_features2d/table_of_content_features2d.html
    // for OpenCV general detection/matching framework details

    //if( argc != 3 ) {
    //    help(argv);
    //    return -1;
    //}

    // Load images
    Mat imgA = imread("./image_016.jpeg", CV_LOAD_IMAGE_GRAYSCALE );
    if( !imgA.data ) {
        std::cout<< " --(!) Error reading image " << argv[1] << std::endl;
        return -1;
    }

    Mat imgB = imread("./image_017.jpeg", CV_LOAD_IMAGE_GRAYSCALE );
    if( !imgA.data ) {
        std::cout << " --(!) Error reading image " << argv[2] << std::endl;
        return -1;
    }

    std::vector<KeyPoint> keypointsA, keypointsB, goodKPA, goodKPB;
    Mat descriptorsA, descriptorsB;
    std::vector<DMatch> matches;

    // DETECTION
    // Any openCV detector such as

		//SiftFeatureDetector  detector(2000,4);
		//SurfFeatureDetector  detector(2000,4);
    //MserFeatureDetector  detector;
		BRISK brisk;
		
    // DESCRIPTOR
    // Our proposed FREAK descriptor
    // (roation invariance, scale invariance, pattern radius corresponding to SMALLEST_KP_SIZE,
    // number of octaves, optional vector containing the selected pairs)
    // FREAK extractor(true, true, 22, 4, std::vector<int>());
    FREAK extractor;

    // MATCHER
    // The standard Hamming distance can be used such as
    // BruteForceMatcher<Hamming> matcher;
    // or the proposed cascade of hamming distance using SSSE3
    BruteForceMatcher<Hamming> matcher;

    // detect
		double  t; 
		//for(int i=0;i<20;i++)
		{
    t = (double)getTickCount();
    //detector.detect( imgA, keypointsA );
		brisk(imgA, noArray(),keypointsA, descriptorsA);
    t = ((double)getTickCount() - t)/getTickFrequency();
		std::cout << "keypoints A # = " << keypointsA.size() <<std::endl;
		std::cout << "detection time [ms]: " << t*1000.0 << std::endl;
		t = (double)getTickCount();
		//detector.detect( imgB, keypointsB );
		brisk(imgB, noArray(),keypointsB, descriptorsB);
		t = ((double)getTickCount() - t)/getTickFrequency();
    std::cout << "detection time [ms]: " << t*1000.0 << std::endl;
		}

    // extract
  //  t = (double)getTickCount();
  //  extractor.compute( imgA, keypointsA, descriptorsA );
  //  t = ((double)getTickCount() - t)/getTickFrequency();
		//extractor.compute( imgB, keypointsB, descriptorsB );
  //  std::cout << "extraction time [ms]: " << t*1000.0 << std::endl;

    // match
    t = (double)getTickCount();
    //matcher.match(descriptorsA, descriptorsB, matches);
		vector<vector<DMatch> > tmatches;
		BFMatcher desc_matcher(NORM_L2, true);
		//     vector< DMatch > matches;
		//Mat mask = windowedMatchingMask(test_kpts, train_kpts, 25, 25);
		//desc_matcher.match(res_desc, cap_desc , matches);
		desc_matcher.knnMatch(descriptorsA, descriptorsB , tmatches, 1);
		//     cout << matches.size() <<" " <<endl;
		for(unsigned int k=0;k<tmatches.size();k++)
		{
			if(tmatches[k].size())
			{
				matches.push_back(tmatches[k][0]);
			}
		}

    t = ((double)getTickCount() - t)/getTickFrequency();
    std::cout << "matching time [ms]: " << t*1000.0 << std::endl;


		//if(matches.size()>10)
		//{
			vector<Point2f> db_obj;
			vector<Point2f> scene;
			for( unsigned int i = 0; i < matches.size(); i++ )
			{
				//-- Get the keypoints from the good matches
				db_obj.push_back( keypointsA[ matches[i].queryIdx ].pt );
				goodKPA.push_back( keypointsA[ matches[i].queryIdx ]);
				scene.push_back( keypointsB[ matches[i].trainIdx ].pt ); 
				goodKPB.push_back( keypointsB[ matches[i].trainIdx ]);
			}
			vector<unsigned char> match_mask;
			t = (double)cvGetTickCount();

			Mat H = findHomography(db_obj, scene, CV_RANSAC, 50, match_mask);

			t = (double)cvGetTickCount()-t;



    // Draw matches
    Mat imgMatch;
    drawMatches(imgA, goodKPA, imgB, goodKPB, matches, imgMatch);

    namedWindow("matches", CV_WINDOW_KEEPRATIO);
    imshow("matches", imgMatch);
    waitKey(0);
}
