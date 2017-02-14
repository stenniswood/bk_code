#include <sys/time.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <math.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "constants.h"
#include "findEyeCenter.h"
#include "findEyeCorner.h"
#include "face_detect.hpp"


using namespace std;

/** Constants **/
int num_faces_present;

/** Function Headers */
int detectAndDisplay( cv::Mat frame );

/** Global variables */
//-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
cv::String            face_cascade_name = "/home/pi/opencv-3.1.0/data/haarcascades/haarcascade_frontalface_alt.xml";
cv::CascadeClassifier face_cascade;
std::string 		  main_window_name = "Capture - Face detection";
std::string 		  face_window_name = "Capture - Face";
cv::RNG     		  rng(12345);
cv::Mat     		  debugImage;
cv::Mat     		  skinCrCbHist = cv::Mat::zeros(cv::Size(256, 256), CV_8UC1);
cv::Mat 			  faceROIc;
std::vector<cv::Rect> faces;

const int trained_width  = 92;
const int trained_height = 112;


struct timeval GetTimeStamp()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
//    uint64_t t = tv.tv_sec*(uint64_t)1000000 + tv.tv_usec;
//    printf("GetTimeStamp()  %ld  %ld  = %ld\n", tv.tv_sec, tv.tv_usec, t );
    return tv;
} 
uint64_t GetTimeStamp2() 
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    uint64_t t = tv.tv_sec*(uint64_t)1000000 + tv.tv_usec;
    //printf("GetTimeStamp()  %ld  %ld  = %ld\n", tv.tv_sec, tv.tv_usec, t );
    return t;
}

/**
 * @function main
 */
int fd_init( ) 
{
  // Load the cascades
  if( !face_cascade.load( face_cascade_name ) ){ 
	printf("--(!)Error loading face cascade, please change face_cascade_name in source code.\n"); return -1; };

  cv::namedWindow(main_window_name,CV_WINDOW_NORMAL);
  cv::moveWindow (main_window_name, 400, 100);
  cv::namedWindow(face_window_name,CV_WINDOW_NORMAL);
  cv::moveWindow (face_window_name, 10, 100);
#ifdef SHOW_EYES
  cv::namedWindow("Right Eye",CV_WINDOW_NORMAL);
  cv::moveWindow ("Right Eye", 10, 600);
  cv::namedWindow("Left Eye",CV_WINDOW_NORMAL);
  cv::moveWindow ("Left Eye", 10, 800);
#endif

  createCornerKernels();
  ellipse(skinCrCbHist, cv::Point(113, 155.6), cv::Size(23.4, 15.2),
          43.0, 0.0, 360.0, cv::Scalar(255, 255, 255), -1);
	printf("fd_init() done \n" );
}

void fd_timeslice( cv::Mat& frame_gray, cv::Mat& original, bool mFindEyes )
{
	frame_gray.copyTo(debugImage);

	//equalizeHist( frame_gray, frame_gray  );
	//cv::pow(frame_gray, CV_64F, frame_gray);
	uint64_t  start = GetTimeStamp2();
	face_cascade.detectMultiScale( frame_gray, faces, 1.015, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150) );
	//  findSkin(debugImage);
	uint64_t end = GetTimeStamp2();
	num_faces_present = faces.size();
		
	// extract Face : 
	if (num_faces_present) 
	{
		printf("FaceDetect() Duration = %6.2f;  faces_present=%d\n", (float)(end-start)/1000000., num_faces_present );
		//cv::flip(frame_gray, frame_gray, 1);
		start = GetTimeStamp2();
		printf("FaceRect: <%6.2f, %6.2f>\n", faces[0].height, faces[0].width );
		
		faceROIc = original(faces[0]);
		if (mFindEyes)
			findEyes( frame_gray, faces[0] );
		  end = GetTimeStamp2();
		printf("FindEyes() Duration = %6.2f\n", (float)(end-start)/1000000. );
		imshow(face_window_name, faceROIc);
	}

	annotate_faces( original );
}


void fd_close() 
{
	releaseCornerKernels();
}

//#define annotatedFace debugFace
#define annotatedFace faceROIc
      
void findEyes(cv::Mat frame_gray, cv::Rect face) 
{
  cv::Mat faceROI   = frame_gray(face);
  cv::Mat debugFace = faceROI;
		
  if (kSmoothFaceImage) {
    double sigma = kSmoothFaceFactor * face.width;
    GaussianBlur( faceROI, faceROI, cv::Size( 0, 0 ), sigma);
  }
  //-- Find eye regions and draw them
  int eye_region_width = face.width * (kEyePercentWidth/100.0);
  int eye_region_height = face.width * (kEyePercentHeight/100.0);
  int eye_region_top = face.height * (kEyePercentTop/100.0);
  cv::Rect leftEyeRegion(face.width*(kEyePercentSide/100.0),
                         eye_region_top,eye_region_width,eye_region_height);
  cv::Rect rightEyeRegion(face.width - eye_region_width - face.width*(kEyePercentSide/100.0),
                          eye_region_top,eye_region_width,eye_region_height);

  //-- Find Eye Centers
  cv::Point leftPupil = findEyeCenter(faceROI, faceROIc, leftEyeRegion,"Left Eye");
  cv::Point rightPupil = findEyeCenter(faceROI,faceROIc, rightEyeRegion,"Right Eye");

  // get corner regions
  cv::Rect leftRightCornerRegion(leftEyeRegion);
  leftRightCornerRegion.width -= leftPupil.x;
  leftRightCornerRegion.x += leftPupil.x;
  leftRightCornerRegion.height /= 2;
  leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
  cv::Rect leftLeftCornerRegion(leftEyeRegion);
  leftLeftCornerRegion.width = leftPupil.x;
  leftLeftCornerRegion.height /= 2;
  leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;
  cv::Rect rightLeftCornerRegion(rightEyeRegion);
  rightLeftCornerRegion.width = rightPupil.x;
  rightLeftCornerRegion.height /= 2;
  rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
  cv::Rect rightRightCornerRegion(rightEyeRegion);
  rightRightCornerRegion.width -= rightPupil.x;
  rightRightCornerRegion.x += rightPupil.x;
  rightRightCornerRegion.height /= 2;
  rightRightCornerRegion.y += rightRightCornerRegion.height / 2;

  rectangle(annotatedFace,leftRightCornerRegion,200);        // was debugFace
  rectangle(annotatedFace,leftLeftCornerRegion,200);
  rectangle(annotatedFace,rightLeftCornerRegion,200);
  rectangle(annotatedFace,rightRightCornerRegion,200);
  // change eye centers to face coordinates
  rightPupil.x += rightEyeRegion.x;
  rightPupil.y += rightEyeRegion.y;
  leftPupil.x += leftEyeRegion.x;
  leftPupil.y += leftEyeRegion.y;
  // draw eye centers
  circle(annotatedFace, rightPupil, 3, 1234);                // was debugFace
  circle(annotatedFace, leftPupil, 3, 1234);

  //-- Find Eye Corners
  if (kEnableEyeCorner) {
    cv::Point2f leftRightCorner = findEyeCorner(faceROI(leftRightCornerRegion), true, false);
    leftRightCorner.x += leftRightCornerRegion.x;
    leftRightCorner.y += leftRightCornerRegion.y;
    cv::Point2f leftLeftCorner = findEyeCorner(faceROI(leftLeftCornerRegion), true, true);
    leftLeftCorner.x += leftLeftCornerRegion.x;
    leftLeftCorner.y += leftLeftCornerRegion.y;
    cv::Point2f rightLeftCorner = findEyeCorner(faceROI(rightLeftCornerRegion), false, true);
    rightLeftCorner.x += rightLeftCornerRegion.x;
    rightLeftCorner.y += rightLeftCornerRegion.y;
    cv::Point2f rightRightCorner = findEyeCorner(faceROI(rightRightCornerRegion), false, false);
    rightRightCorner.x += rightRightCornerRegion.x;
    rightRightCorner.y += rightRightCornerRegion.y;
    circle(annotatedFace, leftRightCorner, 3, 200);
    circle(annotatedFace, leftLeftCorner, 3, 200);
    circle(annotatedFace, rightLeftCorner, 3, 200);
    circle(annotatedFace, rightRightCorner, 3, 200);
  }

//  imshow(face_window_name, faceROIc);
//  cv::Rect roi( cv::Point( 0, 0 ), faceROI.size());
//  cv::Mat destinationROI = debugImage( roi );
//  faceROI.copyTo( destinationROI );
}


cv::Mat findSkin (cv::Mat &frame) 
{
  cv::Mat input;
  cv::Mat output = cv::Mat(frame.rows,frame.cols, CV_8U);

  cvtColor(frame, input, CV_BGR2YCrCb);

  for (int y = 0; y < input.rows; ++y) {
    const cv::Vec3b *Mr = input.ptr<cv::Vec3b>(y);
//    uchar *Or = output.ptr<uchar>(y);
    cv::Vec3b *Or = frame.ptr<cv::Vec3b>(y);
    for (int x = 0; x < input.cols; ++x) {
      cv::Vec3b ycrcb = Mr[x];
//      Or[x] = (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0) ? 255 : 0;
      if(skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) == 0) {
        Or[x] = cv::Vec3b(0,0,0);
      }
    }
  }
  return output;
}

void single_channel_gray( cv::Mat& frame, cv::Mat& output_gray )
{
	//cvtColor( frame, gray_frame, CV_BGR2GRAY );		This leaves more than 1 channel however!
	std::vector<cv::Mat> rgbChannels(3);
	cv::split            (frame, rgbChannels);
	output_gray = rgbChannels[2];
}     

void annotate_faces( cv::Mat& mOriginal )
{
  if (faces.size() > 0) 
  {
	for( int i = 0; i < faces.size(); i++ )
		rectangle(mOriginal, faces[i], 1234);
  }
}

const char* base_path = "/home/pi/Pictures/robot/detected_faces/";
char* get_new_frame_captured_filename()
{
	static char frame_name[128];
	char timestamp_str    [128];

	time_t t;
	time(&t);
	char* time_str = ctime(&t);
	for (int c=0; c<strlen(time_str); c++)
		if (time_str[c]==' ')
			time_str[c] = '_';
		else if (time_str[c]=='\n')
			time_str[c] = '_';

	//strcpy( frame_name, base_path );
	strcpy( frame_name, "frame_" );
	strcat( frame_name, time_str );
	strcat( frame_name, ".png"   );
	//printf("FRAME NAME: %s\n", frame_name );
	return frame_name;
}

/* Saves all detected faces */
void save_faces( cv::Mat& mOriginal, std::vector<int>& predictedLabels )
{
	string  Path = "/home/pi/Pictures/face_recog/unknown/";
	string  filename,timestamp;	
	cv::Mat sav_image;
	char    ending[16];
	static  int count=0;
	timestamp = get_new_frame_captured_filename();
	for( int i=0; i<faces.size(); i++ )
	{
		if (predictedLabels[i] == -1) 	// If Unknown person : 
		{
			sprintf(ending, "_%d_", count++ );
			filename = Path + ending + timestamp;
			printf("FRAME NAME: %s\n", filename.c_str() );
			sav_image = mOriginal( faces[i] );
			imwrite( filename.c_str(), sav_image );
		}
	}	
}

//cv::namedWindow("aa",CV_WINDOW_NORMAL);
//cv::moveWindow("aa", 10, 800);
//cv::namedWindow("aaa",CV_WINDOW_NORMAL);
//cv::moveWindow("aaa", 10, 800);
