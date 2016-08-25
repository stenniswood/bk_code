#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <queue>
#include <stdio.h>
#include <math.h>

#include "constants.h"
#include "misc_detect.hpp"

extern struct timeval GetTimeStamp();
#define DEBUG 1


std::vector<cv::Rect> cats;
std::vector<cv::Rect> licenses;
std::vector<cv::Rect> profilefaces;
std::vector<cv::Rect> fullbodys;
std::vector<cv::Rect> upperbodys;
std::vector<cv::Rect> lowerbodys;

cv::String   cat_cascade_name 		    = "/home/pi/opencv-3.1.0/data/haarcascades/haarcascade_frontalcatface.xml";
cv::String   license_plate_cascade_name = "/home/pi/opencv-3.1.0/data/haarcascades/haarcascade_licence_plate_rus_16stages.xml";
cv::String   profileface_cascade_name   = "/home/pi/opencv-3.1.0/data/haarcascades/haarcascade_profileface.xml";
cv::String   fullbody_cascade_name  = "/home/pi/opencv-3.1.0/data/haarcascades/haarcascade_fullbody.xml";
cv::String   upperbody_cascade_name = "/home/pi/opencv-3.1.0/data/haarcascades/haarcascade_upperbody.xml";
cv::String   lowerbody_cascade_name = "/home/pi/opencv-3.1.0/data/haarcascades/haarcascade_lowerbody.xml";

cv::CascadeClassifier cat_cascade;
cv::CascadeClassifier license_plate_cascade;
cv::CascadeClassifier profileface_cascade;
cv::CascadeClassifier fullbody_cascade;
cv::CascadeClassifier upperbody_cascade;
cv::CascadeClassifier lowerbody_cascade;


int misc_detect_init( )
{
  if( !cat_cascade.load( cat_cascade_name ) )
	{ printf("--(!)Error loading cat cascade.\n"); return 0; };	
  if( !license_plate_cascade.load( license_plate_cascade_name ) )
	{ printf("--(!)Error loading license plate cascade.\n"); return 0; };
  if( !profileface_cascade.load( profileface_cascade_name ) )
	{ printf("--(!)Error loading profileface cascade.\n"); return 0; };
	
  if( !fullbody_cascade.load( fullbody_cascade_name ) )
	{ printf("--(!)Error loading fullbody cascade.\n"); return 0; };
  if( !upperbody_cascade.load( upperbody_cascade_name ) )
	{ printf("--(!)Error loading upperbody cascade.\n"); return 0; };
  if( !lowerbody_cascade.load( lowerbody_cascade_name ) )
	{ printf("--(!)Error loading lowerbody cascade.\n"); return 0; };
  return 1;
}

void print_results()
{
	printf("MISC DETECTIONS: \n");
	printf("cat\t\t: %d\n", 		cats.size() 		);
	printf("licenses\t: %d\n", 		licenses.size() 	);
	printf("profilefaces\t: %d\n", 	profilefaces.size() );
	printf("fullbodys\t: %d\n", 	fullbodys.size()  	);
	printf("upperbodys\t: %d\n", 	upperbodys.size() 	);
	printf("lowerbodys\t: %d\n", 	lowerbodys.size() 	);
}

void misc_detect_timeslice( cv::Mat& mOriginal, cv::Mat mGray_frame )
{
	struct timeval start1,end1;
	start1 = GetTimeStamp();
	
	cat_cascade.detectMultiScale		  ( mGray_frame, cats, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150) );
    license_plate_cascade.detectMultiScale( mGray_frame, licenses, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150) );
    profileface_cascade.detectMultiScale  ( mGray_frame, profilefaces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150) );
    
    fullbody_cascade.detectMultiScale 	  ( mGray_frame, fullbodys, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150) );
    upperbody_cascade.detectMultiScale	  ( mGray_frame, upperbodys, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150) );
    lowerbody_cascade.detectMultiScale	  ( mGray_frame, lowerbodys, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150) );                  
    
	end1 = GetTimeStamp();
	long int sec_delta = (end1.tv_sec  - start1.tv_sec);
	long int usec_delta= (end1.tv_usec - start1.tv_usec);
	float delta = ( sec_delta*1000 - usec_delta );
	if (DEBUG)  {
		print_results();
		printf("misc_detect_timeslice() Duration = sd=%ld; usd=%ld;   %8.3f\n",  sec_delta, usec_delta, delta/1000 );
	}
	misc_annotate( mOriginal );
}

void misc_annotate( cv::Mat& mOriginal )
{
  for( int i = 0; i < cats.size(); i++ )
    rectangle(mOriginal, cats[i], 1234);
  for( int i = 0; i < licenses.size(); i++ )
    rectangle(mOriginal, licenses[i], 1234);
  for( int i = 0; i < profilefaces.size(); i++ )
    rectangle(mOriginal, profilefaces[i], 1234);

  for( int i = 0; i < fullbodys.size(); i++ )
    rectangle(mOriginal, fullbodys[i], 1234);
  for( int i = 0; i < upperbodys.size(); i++ )
    rectangle(mOriginal, upperbodys[i], 1234);
  for( int i = 0; i < lowerbodys.size(); i++ )
    rectangle(mOriginal, lowerbodys[i], 1234);
}

