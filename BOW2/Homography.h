
#ifndef HOMOGRAPHY_HH
#define HOMOGRAPHY_HH

#include <vector>
#include "QueryResults.h"

#include "DBoW2.h" // defines Surf64Vocabulary and Surf64Database
#include "DUtils.h"
#include "DVision.h"


#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>

using namespace DBoW2;
using namespace DUtils;
using namespace std;
using namespace cv;
int checkHomography( const stringstream &cap_src, QueryResults &ret);



#endif