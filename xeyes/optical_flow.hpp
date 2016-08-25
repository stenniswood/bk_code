#ifndef _OPTICAL_FLOW_HPP_
#define _OPTICAL_FLOW_HPP_


void optic_flow( cv::Mat mFrame1, cv::Mat mFrame2, cv::Mat& Output, cv::Mat& mAnnotated );

void update_face_region_flow_based( cv::Mat mFlow, cv::Rect mFaceROI );


#endif
