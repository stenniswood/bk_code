//
//  confidence_booster.cpp
//  opencv_test
//
//  Created by Stephen Tenniswood on 7/22/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <iostream>
#include <vector>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "face_summary.hpp"
#include "face_detect.hpp"

const int FRAMES_DETECTED = 5;

vector<class FaceSummary>  people_present;     // Accumulative (frame to frame) faces
vector<float>   		   confidence;
const Scalar colors[] = 
{
    Scalar(255,0,0),
    Scalar(255,128,0),
    Scalar(255,255,0),
    Scalar(0,255,0),
    Scalar(0,128,255),
    Scalar(0,255,255),
    Scalar(0,0,255),
    Scalar(255,0,255)
};

void draw_circle( Mat& mimg, Rect& mr, double mscale, Scalar& mcolor )
{
    Point center;
    int   radius;
    center.x = cvRound((mr.x + mr.width *0.5)*mscale   );
    center.y = cvRound((mr.y + mr.height*0.5)*mscale   );
    radius   = cvRound((mr.width + mr.height)*0.25*mscale);
    circle( mimg, center, radius, mcolor, 3, 8, 0 );
}

void draw_rectangle( Mat& mimg, Rect& mr, double mscale, Scalar& mcolor )
{
    int x1 = cvRound(mr.x*mscale);
    int y1 = cvRound(mr.y*mscale);
    int x2 = cvRound((mr.x + mr.width -1)*mscale);
    int y2 = cvRound((mr.y + mr.height-1)*mscale);
    rectangle( mimg, cvPoint(x1, y1), cvPoint( x2, y2 ), mcolor, 3, 8, 0);
}

float distance(Rect& m1, Rect& m2 )
{
    float deltaX = fabs(m1.x - m2.x);
    float deltaY = fabs(m1.y - m2.y);
    float distance =  deltaX + deltaY;      // leave out sqrt porportional to distance only.
    return distance;
}

int most_likely_match( Rect& mFace  )
{
    float min_distance = 1000000.0;
    float dist=1000000;
    int min_index = -1;
    //for (int hcf=0; hcf < people_present.size(); hcf++)
    {
//        dist = distance( mFace, people_present[hcf].boundary2 );
        if (dist<min_distance) {
            min_distance = dist;
            //min_index = hcf;
        }
    }
    return min_index;
}

/*********************************************************/
/********************* Face Summary **********************/
/*********************************************************/
FaceSummary::FaceSummary()
{

}
FaceSummary::~FaceSummary()
{

}

const int X_THRESHOLD = 10;
const int Y_THRESHOLD = 10;
bool FaceSummary::is_a_match( Rect& mRect, int mFace_label )
{
	int deltax = (boundary2.x - mRect.x);
	int deltay = (boundary2.y - mRect.y);
	
	// Ideally we compare the deltas to the results of optic flow.
	// For a nearly stationary face (quite often) we'll give it a marginal amount 
	// to move
	if ((deltax < X_THRESHOLD) && (deltay < Y_THRESHOLD))
	{
		if (mFace_label==recognized_label)
			return true;
	}
	return false;
}

// matching detection occurred	
bool FaceSummary::register_detected( Rect& mRect, int mFace_label )
{
	m_detected_count++;
	if (m_detected_count > FRAMES_DETECTED)
	{
		m_undetected_count = 0;
		m_is_present = true;
		boundary2 = mRect;
	}
}

bool FaceSummary::register_nondetect( Rect& mRect, int mFace_label )
{
	m_undetected_count++;
	if (m_undetected_count > FRAMES_DETECTED)
	{
		m_detected_count = 0;
		m_is_present = false;
	}
}


/************ GROUP DETECT FUNCTIONS *****************/
void process_detected_faces( vector<Rect>& faces, vector<int> mIDs )
{    
    // We can confirm an existing face,
    
    //  reject as false detect,
    
    //  or add new.
    
}

#define face people_present[p].boundary2
void annotate_people_present(Mat& img, double mscale)
{
    for ( size_t p = 0; p < people_present.size(); p++ )
    {
        Scalar color = colors[p%8];
        
        // Draw Face :
        draw_rectangle( img, people_present[p].boundary2, mscale, color );
        
        // Left eye:
        Rect nr = people_present[p].left_eye;
        nr.x += face.x;
        nr.y += face.y;
        draw_circle( img, nr, mscale, color );

        // Right eye:
        nr = people_present[p].right_eye;
        nr.x += face.x;
        nr.y += face.y;
        draw_circle( img, nr, mscale, color );
        
        // Mouth :
        nr = people_present[p].mouth;
        nr.x += face.x;
        nr.y += face.y;
        draw_circle( img, nr, mscale, color );
        
    }
}
