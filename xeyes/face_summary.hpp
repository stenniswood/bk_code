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
#include <iostream>
#include <vector>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

class FaceSummary
{
public:
	FaceSummary();
	~FaceSummary();

	bool		is_a_match( Rect& mRect, int mFace_label=-1 );

    MathVector	velocity_vector; // Accumulative (frame to frame) faces
    Rect    	boundary1;       // Accumulative (frame to frame) faces
    Rect    	boundary2;       // Accumulative (frame to frame) faces

    float   confidence;
    Rect    left_eye;
    Rect    right_eye;
    Rect    mouth;
    int		recognized_index;    
};


extern vector<stFace>  people_present;     // high Confidence Faces

void    process_detected_faces ( vector<Rect>& faces );
void    annotate_people_present( Mat& img, double mscale );

float   distance         ( Rect& m1, Rect& m2 );
int     most_likely_match( Rect& mFace        );


#endif 
