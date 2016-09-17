#ifndef _IMAGE_PROC_MISC_H_
#define _IMAGE_PROC_MISC_H_

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


void filter_color( cv::Mat& mImage, Color mMin, Color mMax );


#endif
