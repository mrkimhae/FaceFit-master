#include "clm.h"
#include "qtcvutility.h"

CFaceTracker::CFaceTracker()
{
	clm_parameters = CLMTracker::CLMParameters();
	clm_model = CLMTracker::CLM(clm_parameters.model_location);	
	fx = 20; fy = 20; cx = 0; cy = 0;
}

CFaceTracker::~CFaceTracker()
{
}


void CFaceTracker::DetectCurrentImage(QImage& inImage, QImage& resultImage)
{
	//std::cout << "Starting detection....";
	
	//current_image = cv::Mat(inImage.height(), inImage.width() ,CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine());
	QImageToCvMat(inImage,current_image);
	cvtColor(current_image, grayscale_image, CV_BGR2GRAY);		
	
	//imshow( "Display window1", current_image );  
	//imshow( "Display window2", grayscale_image );  
	fx = inImage.width()/2.0f;
	fy = inImage.height()/2.0f;
	//std::cout <<"fx: " << fx << " fy: "<< fy << std::endl;

	bool detection_success = CLMTracker::DetectLandmarksInVideo(grayscale_image, depth_image, clm_model, clm_parameters);
	pose_estimate_CLM = CLMTracker::GetCorrectedPoseCamera(clm_model, fx, fy, cx, cy, clm_parameters);
	double detection_certainty = clm_model.detection_certainty;
	double visualisation_boundary = 0.2;

	//std::cout << "detection_certainty: "<< detection_certainty << std::endl;
	if(detection_certainty < visualisation_boundary)
	{
		CLMTracker::Draw(current_image, clm_model);
		
		//std::cout << "detection_certainty: "<< current_image.cols << std::endl;

		if(detection_certainty > 1)
			detection_certainty = 1;
		if(detection_certainty < -1)
			detection_certainty = -1;

		double vis_certainty = (detection_certainty + 1)/(visualisation_boundary +1);

		// A rough heuristic for box around the face width
		int thickness = (int)std::ceil(2.0* ((double)current_image.cols) / 640.0);
				
		//Vec6d pose_estimate_to_draw = CLMTracker::GetCorrectedPoseCameraPlane(clm_model, fx, fy, cx, cy, clm_parameters);
		Vec6d pose_estimate_to_draw = CLMTracker::GetCorrectedPoseCamera(clm_model, fx, fy, cx, cy, clm_parameters);
		// Draw it in reddish if uncertain, blueish if certain
		//CLMTracker::DrawBox(current_image, pose_estimate_to_draw, Scalar((1-vis_certainty)*255.0,0, vis_certainty*255), thickness, fx, fy, cx, cy);
		
		//imshow( "Display window2", current_image );
		//imshow( "Display window2", current_image );  
		//std::cout << "image type: "<< current_image.type() << std::endl;

		CvMatToQImage(current_image, resultImage);
		//resultImage = QImage((uchar*)current_image.data, current_image.cols, current_image.rows, QImage::Format_ARGB32);
		//resultImage = resultImage.rgbSwapped();
		//resultImage.bits();
		//imshow( "Display window1", imMat );  
	   
		//std::cout << "Done";
	}

	//std::cout << std::endl;
}

std::vector<cv::Point> CFaceTracker::GetFeaturePoints()
{
	std::vector<cv::Point> featurePoints;
	Mat_<double> shape2D = clm_model.detected_landmarks;
	int n = shape2D.rows/2;

	for( int i = 0; i < n; ++i)
	{	
		Point featurePoint((int)shape2D.at<double>(i), (int)shape2D.at<double>(i +n));
		featurePoints.push_back(featurePoint);
	}
	return featurePoints;
}