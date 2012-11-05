

#include "Homography.h"
#include "QueryResults.h"
// ----------------------------------------------------------------------------

void drawMatchesRelative(const vector<KeyPoint>& train, const vector<KeyPoint>& query,
        std::vector<cv::DMatch>& matches, Mat& img, const vector<unsigned char>& mask = vector<
        unsigned char> ())
    {
        for (int i = 0; i < (int)matches.size(); i++)
        {
            if (mask.empty() || mask[i])
            {
                Point2f pt_new = query[matches[i].queryIdx].pt;
                Point2f pt_old = train[matches[i].trainIdx].pt;

                cv::line(img, pt_new, pt_old, Scalar(125, 255, 125), 1);
                cv::circle(img, pt_new, 2, Scalar(255, 0, 125), 1);

            }
        }
    }

    //Takes a descriptor and turns it into an xy point
    void keypoints2points(const vector<KeyPoint>& in, vector<Point2f>& out)
    {
        out.clear();
        out.reserve(in.size());
        for (size_t i = 0; i < in.size(); ++i)
        {
            out.push_back(in[i].pt);
        }
    }

    //Takes an xy point and appends that to a keypoint structure
    void points2keypoints(const vector<Point2f>& in, vector<KeyPoint>& out)
    {
        out.clear();
        out.reserve(in.size());
        for (size_t i = 0; i < in.size(); ++i)
        {
            out.push_back(KeyPoint(in[i], 1));
        }
    }

    //Uses computed homography H to warp original input points to new planar position
    void warpKeypoints( const vector<KeyPoint>& in, vector<KeyPoint>& out,const Mat& H)
    {
        vector<Point2f> pts;
        keypoints2points(in, pts);
        vector<Point2f> pts_w(pts.size());
        Mat m_pts_w(pts_w);
        perspectiveTransform(Mat(pts), m_pts_w, H);
        points2keypoints(pts_w, out);
    }

    //Converts matching indices to xy points
    void matches2points(const vector<KeyPoint>& train, const vector<KeyPoint>& query,
        const std::vector<cv::DMatch>& matches, std::vector<cv::Point2f>& pts_train,
        std::vector<Point2f>& pts_query)
    {

        pts_train.clear();
        pts_query.clear();
        pts_train.reserve(matches.size());
        pts_query.reserve(matches.size());

        size_t i = 0;

        for (; i < matches.size(); i++)
        {

            const DMatch & dmatch = matches[i];

            pts_query.push_back(query[dmatch.queryIdx].pt);
            pts_train.push_back(train[dmatch.trainIdx].pt);

        }

    }
int checkHomography( const stringstream &cap_src, QueryResults &ret )
{
  

  vector<vector<vector<float> > >  result;
  std::vector<KeyPoint> keypoints_object, res_keypoints;
//   Mat capImgDesp(features[0].size(),features[0][0].size(),CV_32F);
//   cout <<capImgDesp.rows << " " << capImgDesp.cols << endl;
//   for(uint i=0; i<features[0].size(); i++)
//   {
//     Mat temp(features[0][i]);
//     cv::transpose(temp,temp);  
//     capImgDesp.row(i)=temp.row(0);
//     //cout <<temp.rows << " " << temp.cols << endl;
//   }

  vector<cv::KeyPoint> cap_keypoints;
  Mat cap_Desp,  res_image;  
  

  
  
  //cap_src << "../snap-unknown-20120825-200118-1.jpeg"	;
  Mat cap_image = imread(cap_src.str(),CV_LOAD_IMAGE_GRAYSCALE);
  initModule_nonfree();

  Ptr<Feature2D> sift1 = Algorithm::create<Feature2D>("Feature2D.SIFT");
  sift1->set("contrastThreshold", 0.01f);
  (*sift1)(cap_image, noArray(), cap_keypoints, cap_Desp);
    
  for (int i=1;i<ret.size();i++)
  {
    unsigned int id = ret[i].Id;
    stringstream ret_src;
    ret_src << "C:/Users/James/Dropbox/projects/CentralComm/Obj_Recognition/Obj_Recognition/Home//image_";
    if(id<10) 			ret_src<< "00"<<id<< ".jpeg";
    if(id>=10&&id<100) 		ret_src<< "0"<<id<< ".jpeg";
    if(id>=100	)		ret_src<< id << ".jpeg";
    cout << ret_src.str()<<endl;
     res_image = imread(ret_src.str(),CV_LOAD_IMAGE_GRAYSCALE);
    vector<cv::KeyPoint> res_keypoints;
    cv::Mat res_Desp;

    vector< DMatch > matches;
    
  vector<Point2f> train_pts, query_pts;
  vector<KeyPoint> train_kpts, query_kpts;
  vector<unsigned char> match_mask;
  Mat original_desc, query_desc;
   Mat train_desc;
   const int DESIRED_FTRS = 500;
   GridAdaptedFeatureDetector detector1(new FastFeatureDetector(10, true), DESIRED_FTRS, 4, 4);
   detector1.detect(cap_image, query_kpts); //Find interest points
  BriefDescriptorExtractor brief(32);
  brief.compute(cap_image, query_kpts, query_desc);
  
  vector<KeyPoint> test_kpts;
   detector1.detect(res_image, train_kpts); //Find interest points
  brief.compute(res_image, train_kpts, train_desc);
  

  BFMatcher desc_matcher(NORM_HAMMING);
  //Mat mask = windowedMatchingMask(test_kpts, train_kpts, 25, 25);
  desc_matcher.match(query_desc, train_desc, matches);
  cout <<matches.size();


  matches2points(train_kpts, query_kpts, matches, train_pts, query_pts);

  if (matches.size() > 4)
  {
      Mat H = findHomography(train_pts, query_pts, RANSAC, 4, match_mask);
      if (countNonZero(Mat(match_mask)) > 10)
      {
	  cout << "*****************************"<<endl
	  <<"the result is : image" << id << endl
	  <<"reranking from " <<i <<endl;
	  return ret[i].Id;
      }
      else
	  cout<< "Homography is not match"<<endl;
      //drawMatchesRelative(train_kpts, query_kpts, matches, frame, match_mask);
  }
  else
     cout<< "matches is too small. not match"<<endl;
  }  
//   cvtColor(cap_image,cap_image,CV_GRAY2RGB);
//   cvtColor(res_image,res_image,CV_GRAY2RGB);
//      Mat cap_image1, res_image1;
//      cap_image.copyTo(cap_image1);
//      res_image.copyTo(res_image1);
//   for(int i=0; i<query_kpts.size();i++)
//   {
// //     circle( cap_image1, query_kpts[i].pt, 2, Scalar(0, 255, 0), CV_FILLED, CV_AA );
// //     circle( res_image1, test_kpts[i].pt, 2, Scalar(0, 0, 255), CV_FILLED, CV_AA );
//   }
// 
   
//   imshow("cap_image1", cap_image1);
//   imshow("res_image1", res_image1);
//   waitKey(0);

  return 0;
}