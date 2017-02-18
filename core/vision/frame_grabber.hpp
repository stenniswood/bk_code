#ifndef _FRAME_GRABBER_HPP_
#define _FRAME_GRABBER_HPP_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>


/* THIS WILL GRAB A VIDEO FRAME FROM EITHER 1 of 2 sources:
	Kinect_Sensor 
	WebCam
	StructureIO		TBD
	
	The OUTPUTS ARE:
		frame, 		gray_frame, 
		prev_frame, prev_gray_frame, 
		flow, 
		original

*/

extern cv::VideoCapture  capture;
extern cv::Mat frame, gray_frame, prev_frame, prev_gray_frame, flow, original, raw_depth, cv_depth;

void *freenect_threadfunc(void *arg);

void frame_grab_init ( int mDeviceNumber );
void frame_grabs	 ( );
void frame_grab_close( );



#endif
