#ifndef _MISC_DETECT_HPP_
#define _MISC_DETECT_HPP_


int misc_detect_init( );

void misc_detect_timeslice( cv::Mat& mOriginal, cv::Mat mGray_frame );
void misc_annotate( cv::Mat& mOriginal );


#endif
