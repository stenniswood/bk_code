#ifndef _FRAME_GRABBER_HPP_
#define _FRAME_GRABBER_HPP_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>


extern cv::VideoCapture  capture;
extern cv::Mat frame, gray_frame, prev_frame, prev_gray_frame, flow, original;


void frame_grab_init ( int mDeviceNumber );
void frame_grabs	 ( );
void frame_grab_close( );



#endif
