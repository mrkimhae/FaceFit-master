#ifndef CLM_H
#define CLM_H

#include "CLM_core.h"

#include <fstream>
#include <sstream>

#include <opencv2/videoio/videoio.hpp>  // Video write
#include <opencv2/videoio/videoio_c.h>  // Video write

#include <qimage.h>

class CFaceTracker
{
public:
	CFaceTracker();
	~CFaceTracker();

	void DetectCurrentImage(QImage& currentImage, QImage& resultImage);
	std::vector<cv::Point> GetFeaturePoints();

	CLMTracker::CLMParameters clm_parameters;
	CLMTracker::CLM clm_model;	
	cv::Mat current_image;
	cv::Mat result_image;
	float fx,fy, cx, cy;
	Mat_<float> depth_image;
	Mat_<uchar> grayscale_image;
	Vec6d pose_estimate_CLM;

};

#endif