
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#include "frame_grabber.hpp"
#include "tk_colors.h"
#include "face_detect.hpp"
#include "face_recog.hpp"
#include "neck_thread.hpp"		// can we remove this?
#include "vision_memory.h"


int   mover_x=0;
int   mover_y=0;
int   mover_width=0;
int   mover_height=0;
int   frame_count=0;
int   face_x, face_y;
bool  capture_frame	 = false;

void find_center( int& x, int &y, cv::Rect mrect)
{
	x = mrect.x + (mrect.width/2);
	y = mrect.y + (mrect.height/2);
}

	
void filter_color( cv::Mat& mImage, Color mMin, Color mMax )
{
	cv::Mat lower_red_hue_range;
	cv::Mat upper_red_hue_range;
	
	cv::cvtColor ( mImage, mImage, cv::COLOR_RGB2HSV );
	cv::inRange	 ( mImage, cv::Scalar(70, 100, 100),   cv::Scalar(100, 255, 255), mImage );
//	cv::cvtColor ( mImage, mImage, cv::COLOR_HSV2RGB );
	
	//cv::inRange	 ( mImage, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);	
}


void process_frames()
{
	//filter_color   ( original, blue, coral );
	
	if (frame_count++) { 
		//optic_flow( prev_gray_frame, gray_frame, flow, frame );
		//update_face_region_flow_based( flow, faces[0] );
		//imshow( "optical flow", frame );
	}

	fd_timeslice( gray_frame, original, true );
	//misc_detect_timeslice( original, gray_frame );
	if (num_faces_present)
	{	
/*		std::vector<int> predictedLabels;
		if (model) {
			message = face_recongition_tasks( gray_frame, faces, predictedLabels, capture_frame );
			save_faces(frame, predictedLabels);
		}
*/
		// Inform Client of Presence : 
		message = "face detected";
		find_center( mover_x, mover_y, faces[0] );
		mover_width  = frame.cols;
		mover_height = frame.rows;
		eyes_compose_coordinate_xy( mover_x, mover_y ); 
	}
	else {
		// No Faces detected:
		message = "face gone";
//		if (!MOVE_TO_MOUSE)
//			neck_duty = 0;
	}

}