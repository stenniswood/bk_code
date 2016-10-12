#ifndef _IMAGE_PROC_MISC_H_
#define _IMAGE_PROC_MISC_H_

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


extern int   mover_x;
extern int   mover_y;
extern int   mover_width;
extern int   mover_height;
extern bool  capture_frame;

void find_center( int& x, int &y, cv::Rect mrect);
void filter_color( cv::Mat& mImage, Color mMin, Color mMax );
void process_frames();


#endif
