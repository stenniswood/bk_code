
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#include <iostream>
#include <queue>
#include <stdio.h>
#include <math.h>
#include "constants.h"
#include "optical_flow.hpp"

//	hsv = np.zeros_like(frame1)
//	hsv[...,1] = 255
extern struct timeval GetTimeStamp();


void optic_flow( cv::Mat mGray1, cv::Mat mGray2, cv::Mat& flow, cv::Mat& mAnnotated )
{
	cv::Mat frame1_gray, frame2_gray;
	cv::UMat  flowUmat;
	struct timeval start1,end1;	
	start1 = GetTimeStamp();
	calcOpticalFlowFarneback(mGray1, mGray2, 
							flowUmat, 0.5, 3, 15, 3, 5, 1.2, 0);
	flowUmat.copyTo(flow);    

    for (int y = 0; y < mAnnotated.rows; y += 5) 
     	for (int x = 0; x < mAnnotated.cols; x += 5)
     	{
			// get the flow from y, x position * 10 for better visibility
			const cv::Point2f flowatxy = flow.at<cv::Point2f>(y, x) * 10;

            // draw line at flow direction
       		line(mAnnotated, cv::Point(x, y), cv::Point( cvRound(x + flowatxy.x), 
       			 cvRound(y + flowatxy.y)), cv::Scalar(255,0,0));

            // draw initial point
       		circle(mAnnotated, cv::Point(x, y), 1, cv::Scalar(0, 0, 0), -1);
      	}
	end1 = GetTimeStamp();
	float delta = ((end1.tv_sec-start1.tv_sec)*1000 - (end1.tv_usec - start1.tv_usec));
	printf("OpticalFlowFarneback() Duration =  %8.3f\n",  delta/1000 );

}

/* 
    Based on the optical flow output,
	update the Face region to new expected area.
*/
void update_face_region_flow_based( cv::Mat mFlow, cv::Rect mFaceROI )
{
	int end_x = mFaceROI.x + mFaceROI.width;
	int end_y = mFaceROI.y + mFaceROI.height;
	float average_x_shift = 0;
	float average_y_shift = 0;
	int points = 0;
		
    for (int y = mFaceROI.y;     y<end_y;  y += 5)
     	for (int x = mFaceROI.x; x<end_x;  x += 5)
     	{
			// get the flow from y, x position * 10 for better visibility
			const cv::Point2f flowatxy = mFlow.at<cv::Point2f>(y, x) * 10;
            average_x_shift += flowatxy.x;
            average_y_shift += flowatxy.y;
            points += 1;
		}
	average_x_shift /= points;
	average_y_shift /= points;

	mFaceROI.x += average_x_shift;
	mFaceROI.y += average_y_shift;
}

