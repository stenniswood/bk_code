//
//  confidence_booster.hpp
//  opencv_test             - This file composes the entire face based on individual detections.
//                          It also filters out glitchy detects and
//
//  Created by Stephen Tenniswood on 7/22/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef _FACE_SUMMARY_HPP_
#define _FACE_SUMMARY_HPP_

#include <stdio.h>
#include <unistd.h>

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "vector.hpp"

using namespace std;
using namespace cv;

// Main External Function:
void    process_detected_faces( vector<Rect>& faces, vector<int>& mIDs );
void 	test_face_summary();


#define DETECTED_THRESHOLD 5
#define UNDETECTED_THRESHOLD 5
#define UNDETECTED_AFTER_GLOW 5


/* Keep track of one individual */
class FaceSummary
{
public:
	FaceSummary ();
	~FaceSummary();
	void		initialize();
	
	bool		is_a_match		  ( Rect& mRect, int mFace_label=-1 );
	bool		register_detected ( Rect& mRect, int mFace_label=-1 );	// matching detection occurred.
	void		penalize_nondetect( );

	// VARIABLES : 
    int			recognized_label;    
	string		m_name;
    MathVector	velocity_vector;
    Rect    	boundary1;       // prev frame detection mRect
    Rect    	boundary2;       // current detection mRect

	bool	m_just_left;
	bool	m_is_present;
	bool	m_is_present_in_current_frame;
	int 	m_detected_count;
	int 	m_undetected_count;

    float   confidence;
    Rect    left_eye;
    Rect    right_eye;
    Rect    mouth;
};

extern  vector<class FaceSummary>  people_present;		// high Confidence Faces

void    annotate_people_present ( Mat& img, double mscale );
float   distance         		( Rect& m1, Rect& m2 	  );
int     most_likely_match		( Rect& mFace        	  );

#endif 
