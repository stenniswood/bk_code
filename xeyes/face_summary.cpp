//
//  confidence_booster.cpp
//  opencv_test
//
//  Created by Stephen Tenniswood on 7/22/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#include "face_summary.hpp"
#include "facedetect.hpp"

vector<stFace>  people_present;     // Accumulative (frame to frame) faces.
vector<float>   confidence;

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
    for (int hcf=0; hcf < people_present.size(); hcf++)
    {
        dist = distance( mFace, people_present[hcf].boundary );
        if (dist<min_distance) {
            min_distance = dist;
            min_index = hcf;
        }
    }
    return min_index;
}

void process_detected_faces(vector<Rect>& faces )
{
    
    // We can confirm an existing face,
    
    //  reject as false detect,
    
    //  or add new.
    
}

#define face people_present[p].boundary
void annotate_people_present(Mat& img, double mscale)
{
    for ( size_t p = 0; p < people_present.size(); p++ )
    {
        Scalar color = colors[p%8];
        
        // Draw Face :
        draw_rectangle( img, people_present[p].boundary, mscale, color );
        
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
