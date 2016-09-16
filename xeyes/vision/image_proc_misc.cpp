

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>


void filter_color( cv::Mat& mImage, Color mMin, Color mMax )
{
	cv::cvtColor ( mImage, mImage, cv::COLOR_RGB2HSV );
	cv::inRange	 ( mImage, cv::Scalar(0, 100, 100),   cv::Scalar(10, 255, 255), lower_red_hue_range );
	cv::inRange	 ( mImage, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), lower_red_hue_range);	
}