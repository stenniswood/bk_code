
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
#include "depth_plane_detect.hpp"


using namespace cv;
using namespace std;


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

vector<Point2f> corners;
vector<Point2f> good_corners;

void inside_oval()
{
	double radius;
	double minRadius = min ( faces[0].height/2, faces[0].width/2 );
	minRadius *= minRadius;
	for( int i = 0; i < corners.size(); i++ )
	{
		float deltax = corners[i].x-faces[0].width/2;
		float deltay = corners[i].y-faces[0].height/2;
		radius = (deltax*deltax) + (deltay*deltay);
		if (radius < minRadius)
			good_corners.push_back( corners[i] );
	}
	cout<<"** Faces h,w = "<<faces[0].height<<", "<<faces[0].width<<endl;
	cout<<"** Radius= "<<minRadius;
	cout<<"** Number of Good corners detected: "<<corners.size()<<endl;
}

void extract_good_features()
{
	Mat src = faceROIc;
	Mat src_gray;
	cvtColor( src, src_gray, CV_BGR2GRAY );
	int maxCorners = 10;

	double qualityLevel = 0.01;
	double minDistance  = 8;
	goodFeaturesToTrack(src_gray, corners, maxCorners, 
				qualityLevel, minDistance );

  // Draw corners detected:
  cout<<"** Number of corners detected: "<<corners.size()<<endl;
  inside_oval();
  
  RNG rng(12345);
  int r = 4;
  for( int i = 0; i < good_corners.size(); i++ )
     { circle( src, good_corners[i], r, Scalar(rng.uniform(0,255), rng.uniform(0,255),
              rng.uniform(0,255)), -1, 8, 0 ); }

  // Show what you got
  namedWindow( "source_window", CV_WINDOW_AUTOSIZE );
  imshow( "source_window", src );
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
		extract_good_features();
		
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
	
//	cv::Mat edge_frame;	
//	Canny( original, edge_frame, 30, 150 );
//	imshow("edge", edge_frame);	

}