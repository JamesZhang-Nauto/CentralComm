/**
* File: Homography.cpp
* Date: Sept 2012
* Author: Jingzhe Zhang
* Description: 
*/

#include "Homography.h"
#include <omp.h>
#include "../ObjectRecognition.h"

#define HOMO_DISPLAY 1
// ----------------------------------------------------------------------------
BRISK brisk;

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

vector<Result> calMinDistance(Point2f &test_pts, vector<Point2f> &cap_pts)
{
	double x,y;
	vector<Result> result;
	//make_heap<double>();

	for (unsigned int i=0; i<cap_pts.size(); i++)
	{
		x = abs(test_pts.x - cap_pts[i].x);
		x = x*x;
		y = abs(test_pts.y - cap_pts[i].y);
		y = y*y;
		x = x+y;
		if ( x < 900 ) // distance is less than 30
		{
			result.push_back(Result(i, x));
		}
	}
	//   cout << "BEGINBEGINBEGINBEGINBEGINBEGINBEGINBEGINBEGINBEGIN*************_______________" <<endl;
	//   for (unsigned int i=0; i<result.size(); i++)
	//   {
	//     
	//     cout << result[i].Id << ": " << result[i].Score <<endl;
	//   }
	sort(result.begin(),result.end());
	//   cout << "__END__*************__END______" <<endl;
	//   for (unsigned int i=0; i<result.size(); i++)
	//   {
	//     
	//     cout << result[i].Id << ": " << result[i].Score <<endl;
	//   }
	return result;
}
bool calDistrubution(vector<Point2f> &test_pts, vector<Point2f> &cap_pts)
{
	//   ulong disx = 0, disy = 0;
	//   for(unsigned int i=0; i<test_pts.size(); i++)
	//   {
	//     int x = abs(test_pts[i].x - test_pts[i+1].x);
	//     int y = abs(test_pts[i].y - test_pts[i+1].y);
	//     disx += x*x;
	//     disy += y*y;    
	//   }
	//   disx /= (test_pts.size());
	//   disy /= (test_pts.size());
	//   cout <<"Distrubution: " << disx << " " << disy <<endl;
	//   if (disx>500&& disy>350)
	//     return true;
	//   else 
	//     return false;


	float xleft=0, xright=0, ytop=0, ybottom=0;
	unsigned int testkps_count=0, capkps_count=0;

	for (unsigned int i=0; i<test_pts.size(); i++)
	{
		if (test_pts[i].x>640 || test_pts[i].y>480)
			continue;
		xleft=test_pts[i].x, xright=test_pts[i].x, 
			ytop=test_pts[i].y, ybottom=test_pts[i].y;
		for (unsigned int j=i+1; j<test_pts.size()-1; j++)
		{

			if(test_pts[j].x<0) continue;
			if (test_pts[j].x>640 || test_pts[j].y>480)	continue;

			//cout << "test_pts[j].y = " <<test_pts[j].y << " ---- "<<endl ;
			if ( (xleft-test_pts[j].x<=10) && (xleft-test_pts[j].x>=0) )
			{

				xleft = test_pts[j].x;

			}
			if( (test_pts[j].x - xright <=10) && (test_pts[j].x-xright>=0) )
			{

				xright =  test_pts[j].x;
			}

			if (ybottom - test_pts[j].y <= 10)
			{
				ybottom = test_pts[j].y;
				//cout << "ybottom " << ybottom <<endl;
			}
			if (test_pts[j].y - ytop < 10)
			{
				ytop = test_pts[j].y;
				//cout << "ytop " << ytop <<endl;
			}




		}
		if ((xleft != xright) || (ytop != ybottom))	break;
	}
	for(unsigned int i=0; i<min(cap_pts.size(),test_pts.size()); i++)
	{

		if( (xleft<=cap_pts[i].x) && (cap_pts[i].x<=xright) && (ytop<=cap_pts[i].y) && (cap_pts[i].y<=ybottom))
		{ 
			//cout <<cap_pts[i].x << " ...x...y..."<<cap_pts[i].y<<endl;
			capkps_count++;
		}
		if( (xleft<=test_pts[i].x) && (test_pts[i].x<=xright) && (ytop<=test_pts[i].y) && (test_pts[i].y<=ybottom))
		{ 
			//cout <<test_pts[i].x << " ...x...y..."<<test_pts[i].y<<endl;
			testkps_count++ ;
		}

	}
	
	/*display the conformed section size! *****************************/
	   //cout << "cap=" << capkps_count << " " << "test" << testkps_count<< " " <<xleft<<" " <<xright<< " "<<ytop<<" "<<ybottom<< endl;
	if (capkps_count*2 >= testkps_count)
		return true;
	else 
		return false;
}

void conformPts(vector<Point2f> &test_pts, vector<Point2f> &cap_pts, Mat &res_desc, Mat &cap_desc, float &score)
{
	//cout << "Calculate NORM" <<endl;
	if(calDistrubution(test_pts, cap_pts))
	{
		double nres=0;	
		//cout << "nres=" <<nres<<endl;
		for(unsigned int i=0;i<test_pts.size();i++)
		{
			//     cout << calMinDistance(test_pts[i],cap_pts) << endl;

			vector<Result> res = calMinDistance(test_pts[i],cap_pts);

			for(unsigned int j=0;j<res.size(); j++)
			{
				nres = norm(res_desc.row(i), cap_desc.row(res[j].Id), NORM_L2);
				//cout << "nres=" <<nres<<endl;
				if ( nres< 480)    //discriptors distance
				{
					score++;
					break;
				}
			}
		}
		//cout << "score: "<< score<<endl;
		score = score/test_pts.size();
	}
	//cout << "score: "<< score<<endl;
}
// void testnearneighbor()
// {
//       double ori_xd, ori_yd, des_xd, des_yd;
//     for(unsigned int j=0; j<(tmatches.size()-1); j++)
//     {
//       if((tmatches[j].size()>0) && (tmatches[j+1].size()>0))
//       {
// 	for(unsigned int k=0; k<tmatches[j].size(); k++)   
// 	{
// 	  cout << "j="<<j<<endl;
// 	  
// 	  ori_xd = abs(res_kpts[tmatches[j][k].queryIdx].pt.x - res_kpts[tmatches[j+1][k].queryIdx].pt.x); 
// 	  ori_xd = ori_xd*ori_xd;
//  	  ori_yd = abs(res_kpts[tmatches[j][k].queryIdx].pt.y - res_kpts[tmatches[j+1][k].queryIdx].pt.y); 
// 	  ori_yd = ori_yd*ori_yd;
// 	  des_xd = abs(cap_kpts[tmatches[j][k].trainIdx].pt.x - cap_kpts[tmatches[j+1][k].trainIdx].pt.x);
// 	  des_xd = des_xd*des_xd;
// 	  des_yd = abs(cap_kpts[tmatches[j][k].trainIdx].pt.y - cap_kpts[tmatches[j+1][k].trainIdx].pt.y); 
// 	  des_yd = des_yd*des_yd;
// 	  ori_xd += ori_yd;
// 	  des_xd += des_yd;
// 	  
// 	  if (ori_xd > des_xd) 
// 	  {
// // 	    cout<<"inter  " << ori_xd <<  " "<< goodmatches.size()<< " "<< endl;
// 	    
// 	    goodmatches.push_back(tmatches[j+1][k]);
// 	    
// 	    goto endofloop;
// 	  }
// 	}
//     } 
//     endofloop: ;
//     }
// }

void checkHomography(Mat &cap_image, vector<KeyPoint> &cap_kpts, Mat &cap_desc, QueryResults &ret )
{

//--------------------modify keyponits extractor into Brisk by ZJZ---------------------------
	cap_kpts.clear();
	
	double timer1 = (double)cvGetTickCount();
	
	
	brisk(cap_image, noArray(), cap_kpts, cap_desc);


	//sift(cap_image, mask1, cap_kpts, cap_desc);


	//   cout <<"cap keypoints: "<<cap_kpts.size()<<endl;


	
				timer1 = (double)cvGetTickCount()-timer1;
		cout << "timer1 time = " <<  timer1/((double)cvGetTickFrequency()*1000)<< endl;
#pragma omp parallel for

	for (int curResImg=0;curResImg<(int)ret.size();curResImg++)
	{
	
	double timerPerImg = (double)cvGetTickCount();
		vector<vector<vector<float> > >  result;
		vector<Point2f> cap_pts;
		vector<KeyPoint> res_kpts;
		keypoints2points(cap_kpts,cap_pts);

#if HOMO_DISPLAY
		Mat cap_display;
		drawKeypoints(cap_image, cap_kpts, cap_display, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		imshow("cap",cap_display);
		waitKey(); 
		destroyWindow("cap");
#endif

		int id = ret[curResImg].Id;
		stringstream ret_src;

		ObjectRecognition::loadImage(ret_src, id);

		//     cout << ret_src.str()<<endl;
		Mat res_image = imread(ret_src.str(), CV_LOAD_IMAGE_GRAYSCALE);

		vector<Point2f> res_pts;

		Mat res_desc;



		//---------------------------------------------------    
		//     BriefDescriptorExtractor brief(32);
		//     GridAdaptedFeatureDetector detector1(new FastFeatureDetector(20, true), 300, 4, 4);
		//     detector1.detect(cap_image, cap_kpts); //Find interest points
		//     brief.compute(cap_image, cap_kpts, cap_desc);

		//     detector1.detect(res_image, res_kpts); //Find interest points
		//     brief.compute(res_image, res_kpts, res_desc);

//--------------------modify keyponits extractor into Brisk by ZJZ---------------------------
		double t = (double)cvGetTickCount();
		//SIFT sift(1,3,0.04,10,1.0);
		//sift(res_image, noArray(), res_kpts, res_desc);
		brisk(res_image, noArray(), res_kpts, res_desc);
		t = (double)cvGetTickCount()-t;
		//cout << "Homography sift time = " <<  t/((double)cvGetTickFrequency()*1000)<< endl;


#if HOMO_DISPLAY
		
		cout <<"res keypoints: "<<res_kpts.size()<<endl;
		Mat res_display;
		namedWindow("res",CV_WINDOW_NORMAL);
		drawKeypoints(res_image,res_kpts,res_display, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		
		imshow("res", res_display);
		waitKey();
		destroyWindow("res");
#endif
		//surf(res_image, mask2, train_kpts, train_desc);
		//(*sift1)(res_image, noArray(), train_kpts, train_desc);

		//-------------------------Match--------------------------------------
		t = (double)cvGetTickCount();
		BFMatcher desc_matcher(NORM_L2, true);
		//     vector< DMatch > matches;
		vector<vector<DMatch> > matches;
		vector<DMatch> goodmatches;
		//Mat mask = windowedMatchingMask(test_kpts, train_kpts, 25, 25);
		//desc_matcher.match(res_desc, cap_desc , matches);
		desc_matcher.knnMatch(res_desc, cap_desc , matches, 1);
		//     cout << matches.size() <<" " <<endl;
		for(unsigned int k=0;k<matches.size();k++)
		{
			if(matches[k].size())
			{
				goodmatches.push_back(matches[k][0]);
			}
		}


		//     
		//     double max_dist = 0; double min_dist = 500;
		//     for( unsigned int i = 0; i < matches.size(); i++ )
		//     { 
		//       double dist = matches[i].distance;
		//       
		//       if( dist < min_dist ) min_dist = dist;
		//       if( dist > max_dist ) max_dist = dist;
		//     }
		//     cout <<endl;
		//     cout << min_dist << "  --  " << max_dist;

		//     do
		//     {
		//       for( unsigned int i = 0; i < matches.size(); i++ )
		//       { 
		// 	if( matches[i].distance < 3*min_dist )   //pick up the good matches.
		// 	{ goodmatches.push_back( matches[i]); }
		//       }  
		//       if( goodmatches.size()<4) min_dist+=10;
		//     }while(goodmatches.size()<4);

		t = (double)cvGetTickCount()-t;
		//cout << "Matching time = " <<  t/((double)cvGetTickFrequency()*1000)<< endl;

		//cout <<"good matches: "<< goodmatches.size()<< endl;

		//--------------------display image---------------------------------	
#if HOMO_DISPLAY
		Mat img_matches;
		namedWindow("image matches",CV_WINDOW_NORMAL);
		resizeWindow("image matches", 1200 , 400);
		moveWindow("image matches",0 , 0);
		drawMatches(  res_display, res_kpts, cap_display, cap_kpts,
			goodmatches, img_matches, Scalar::all(-1), Scalar::all(-1), 
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS); 
		imshow("image matches", img_matches);
		waitKey();
		destroyWindow("image matches");
#endif
		//-------------------------Generate the homography Matrix H---------------------------------	


		//matches2points(res_kpts, cap_kpts, matches, res_pts, cap_pts);

		if(goodmatches.size()>10)
		{
			vector<Point2f> db_obj;
			vector<Point2f> scene;
			for( unsigned int i = 0; i < goodmatches.size(); i++ )
			{
				//-- Get the keypoints from the good matches
				db_obj.push_back( res_kpts[ goodmatches[i].queryIdx ].pt );
				scene.push_back( cap_kpts[ goodmatches[i].trainIdx ].pt ); 
			}
			vector<unsigned char> match_mask;
			t = (double)cvGetTickCount();

			Mat H = findHomography(db_obj, scene, CV_RANSAC, 30, match_mask);

			t = (double)cvGetTickCount()-t;
			//cout << "findHomography time = " <<  t/((double)cvGetTickFrequency()*1000)<< endl;


			//--------------------display image---------------------------------	
			//---------------------combine two images into one---------------------------
#if HOMO_DISPLAY
			goodmatches.clear();
			vector<Point2f> good_obj, good_scene;

			for(unsigned int i =0; i<match_mask.size(); i++)
			{
				//cout << (int)match_mask[i] << " ";
				if (match_mask[i])
				{
					good_obj.push_back(db_obj[i]);
					good_scene.push_back(scene[i]);
				}
			}
			Mat part, img_gdmatch(Size(cap_display.cols + res_display.cols, MAX(cap_display.rows,res_display.rows)), cap_display.type());
			img_gdmatch = Scalar::all(0);
			part = img_gdmatch(Rect(0, 0, res_display.cols, res_display.rows));
			cvtColor(res_display, part, 1);
			part = img_gdmatch(Rect(res_display.cols, 0, cap_display.cols, cap_display.rows));
			cvtColor(cap_display, part, 1);	
			Scalar clrset[5] = {        
				Scalar(0, 0, 255),
				Scalar(0,255,0),
				Scalar(255,100,100),
				Scalar(255,0,255),
				Scalar(0,255,255)};
				for(unsigned int i=0; i<good_obj.size(); i++)
				{

					good_scene[i].x += cap_display.cols;
					line(img_gdmatch, good_obj[i], good_scene[i], clrset[i%5], 1, CV_AA );

				}

				namedWindow("AfterRANSAC",CV_WINDOW_NORMAL);
				resizeWindow("AfterRANSAC",1200 , 400);
				imshow("AfterRANSAC", img_gdmatch);
				waitKey();
#endif
				//--------------------------------Perspective Transform-------------------------------------	


				// 	cout<< endl<<endl;
				//  	cout << "match_mask  " << countNonZero(Mat(match_mask))<<endl;
				vector<Point2f> test_pts;

				keypoints2points(res_kpts,res_pts);

				t = (double)cvGetTickCount();

				perspectiveTransform(res_pts, test_pts, H);

				// 	for (unsigned int i=0;i<test_pts.size();i++)
				// 	{
				// 	  if (test_pts[i].x>640 || test_pts[i].y>480)
				// 	  {
				// 	    cout << test_pts[i].x << " " << test_pts[i].y<<endl;
				// 	    test_pts.erase(test_pts.begin()+i);
				// 	  }
				// 	}
				// 	cout << test_pts.size()<<endl;
				t = (double)cvGetTickCount()-t;
				//cout << "perspectiveTransform time = " <<  t/((double)cvGetTickFrequency()*1000)<< endl;

				//----------------------------------Conform keypoints-----------------------------------
				float score=0;
				// 	cout << test_pts.size() << " " << cap_pts.size() << " " << res_desc.rows << " " <<cap_desc.rows<<endl;
				t = (double)cvGetTickCount();
				conformPts(test_pts, cap_pts, res_desc, cap_desc, score);
				t = (double)cvGetTickCount()-t;
				//cout << "conformPts time = " <<  t/((double)cvGetTickFrequency()*1000)<< endl;
				ret[curResImg].Score+=score;
				//cout << ret[curResImg].Score;




				//--------------------display image---------------------------------	
#if HOMO_DISPLAY
				for(unsigned int i=0; i<test_pts.size();i++)
				{
				 	circle( cap_display, test_pts[i], 3, Scalar(0, 0, 255), CV_FILLED, CV_AA );
				 	  
				}

				namedWindow("cap_image1",CV_WINDOW_NORMAL);
				namedWindow("res_image1",CV_WINDOW_NORMAL);

				imshow("cap_image1", cap_display);
				imshow("res_image1", res_display);
				resizeWindow("res_image1",400 , 300);
				moveWindow("res_image1",475  ,500);
				resizeWindow("cap_image1",400 , 300);
				moveWindow("cap_image1",1000 ,500);
				waitKey();
				destroyWindow("cap_image1");
				destroyWindow("res_image1");
				destroyWindow("AfterRANSAC");
#endif
				//-----------------------------------------------------------------	

				// 	    return ret[i].Id;
				// 	}
				// 	else
				// 	    cout<< "Homography is not match"<<endl;
				//drawMatchesRelative(train_kpts, query_kpts, matches, frame, match_mask);
				//     }
				//     else
				//       cout<< "matches is too small. not match"<<endl;*/


		}//end of if(goodmatches.size()>10)
		timerPerImg = (double)cvGetTickCount()-timerPerImg;
		cout << "Evaluate Per Img time = " <<  timerPerImg/((double)cvGetTickFrequency()*1000)<< endl;
	}//end for (int curResImg=0;curResImg<ret.size();curResImg++)
	//return 0;
			timer1 = (double)cvGetTickCount()-timer1;
		cout << "timer1 time = " <<  timer1/((double)cvGetTickFrequency()*1000)<< endl;
}
