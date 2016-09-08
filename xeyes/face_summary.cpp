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
#include "vision_logger.hpp"
#include "face_recog.hpp"


#define LEFT_TEXT "face left"
const int X_THRESHOLD = 10;
const int Y_THRESHOLD = 10;

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
	velocity_vector.dimension(3);
}

FaceSummary::~FaceSummary()
{

}

void FaceSummary::initialize()
{
	m_name = "unknown";

	m_just_left  				 = false;
	m_is_present				 = false;
	m_is_present_in_current_frame= false;
	m_detected_count	=0;
	m_undetected_count	=0;
    confidence 			=0.0;
    recognized_label 	=-1;
 
 	velocity_vector[0] = 0;
 	velocity_vector[1] = 0;
 	velocity_vector[2] = 0;

    boundary1.x=0;		boundary1.y=0;	boundary1.width=0;	boundary1.height=0;
    boundary2.x=0;		boundary2.y=0;	boundary2.width=0;	boundary2.height=0;        

    left_eye.x=0;		left_eye.y=0;	left_eye.width=0;	left_eye.height=0;
    right_eye.x=0;		right_eye.y=0;	right_eye.width=0;	right_eye.height=0;
    mouth.x=0;			mouth.y=0;		mouth.width=0;		mouth.height=0;
}

bool FaceSummary::is_a_match( Rect& mRect, int mFace_label )
{
	int deltax = abs(boundary2.x - mRect.x);
	int deltay = abs(boundary2.y - mRect.y);
	
	//printf("is_a_match()  delta x,y = %d, %d\n", deltax, deltay );

	// Ideally we compare the deltas to the results of optic flow.
	// For a nearly stationary face (quite often) we'll give it a marginal amount 
	// to move
	if ((deltax < X_THRESHOLD) && (deltay < Y_THRESHOLD))
	{
		if (mFace_label==recognized_label)
			return true;
		return true;
	}
	return false;
}

// Matching detection occurred 
bool FaceSummary::register_detected( Rect& mRect, int mFace_label )
{
	if ( m_name.compare("unknown") )
	{		
		m_name = known_people[mFace_label];		
	}
	m_detected_count++;
	m_is_present_in_current_frame = true;
	boundary1 			= boundary2;
	boundary2 			= mRect;

	if (m_detected_count == DETECTED_THRESHOLD)
	{
		m_undetected_count 	= 0;
		m_is_present 	   	= true;
		return true;
	}
	return false;
}

void FaceSummary::penalize_nondetect( )
{
	if (m_is_present_in_current_frame==false)
	{
		m_undetected_count++;
	}
	if (m_undetected_count == UNDETECTED_THRESHOLD)
	{
		m_detected_count = 0;
		m_is_present = false;
		m_just_left  =  true;
	}
	if ((m_undetected_count > UNDETECTED_THRESHOLD) && 
	    (m_undetected_count < (UNDETECTED_THRESHOLD+UNDETECTED_AFTER_GLOW)))
	{
		m_detected_count = 0;
		m_is_present = false;
		m_just_left  = false;
	}
}

/************ GROUP DETECT FUNCTIONS *****************/
void reset_current_frame()
{
	// Clear all : 
	for (int p=0; p < people_present.size(); p++) 
		people_present[p].m_is_present_in_current_frame = false; 
}

void process_detected_faces( vector<Rect>& faces, vector<int>& mIDs )
{
	int 		 id;
	vector<bool> NewFaces;
	bool match   = false;
	bool use_ids = true;
	//printf("process_detected_faces()\n");

	if (faces.size() != mIDs.size())
		use_ids = false;

	reset_current_frame();

	// For each detected face : 
	for (int f=0; f<faces.size(); f++) 
	{
		// Get Recognized Face Index : 
		if (use_ids) id = mIDs[f];
		else 		 id = -1;

		// Scan all known people present :
		for (int p=0; p<people_present.size(); p++)  
		{ 
			match = people_present[p].is_a_match( faces[f], id );
			if (match)  {
				//printf("face %d - Scan compared to person # %d = %d\n", 
				//		f, p, match );
				bool log = people_present[p].register_detected( faces[f], id );
				if (log)
					sql_logger.sql_add_event( people_present[p].m_name, "Face Detected" );
				break; 	// exit loop
			}
		}
		NewFaces.push_back(match);
	}
	
	// Now penalize all nondetected faces .
	// Can call on all people because internally stored if match was made up above.
	for (int p=0; p<people_present.size(); p++)
	{
		people_present[p].penalize_nondetect( );

		if (people_present[p].m_just_left==true)
			sql_logger.sql_add_event( people_present[p].m_name, LEFT_TEXT );

		// Remove from list:		
		if (people_present[p].m_undetected_count==UNDETECTED_AFTER_GLOW) {
			people_present.erase(people_present.begin() + p );
			printf("Removing person index %d\n", p );
		}
	}

	// Now add any new faces:
	FaceSummary new_face;
	//printf("%d NewFaces %d\n", NewFaces.size(), people_present.size() );
	for (int nf=0; nf<NewFaces.size(); nf++)
	{	
		if (NewFaces[nf]==false)		// was not matched with any person.
		{			
			new_face.initialize();
			new_face.register_detected( faces[nf] );
			people_present.push_back( new_face );
			printf("Added new Person \n");
		}
	}

    //  reject as false detect ( these will in the people_present vector 
    // 		temporarily, but will never get m_is_present set, because the
	// 		face was spuriously detected and disappeared.
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

void 	test_face_summary()
{
	Rect 		 rect;
	vector<Rect> faces;
	vector<int>  IDs;
	// Add 1 Face
	rect.x=25;		rect.y=25;		rect.width=50;	rect.height=50;	
	faces.push_back( rect );
	IDs.push_back( 10 );	
	//printf("test_face_summary() \n");
	
	process_detected_faces( faces, IDs );		// represents 1 frame.
	process_detected_faces( faces, IDs );		// 2
	process_detected_faces( faces, IDs );		// 3
	process_detected_faces( faces, IDs );		// 4
	process_detected_faces( faces, IDs );		// 5

	rect.x=200;		rect.y=200;		rect.width=50;	rect.height=50;
	faces.push_back( rect );
	IDs.push_back( 11 );	

	process_detected_faces( faces, IDs );		// represents 1 frame.
	process_detected_faces( faces, IDs );		// 2
	process_detected_faces( faces, IDs );		// 3
	process_detected_faces( faces, IDs );		// 4
	process_detected_faces( faces, IDs );		// 4	
	process_detected_faces( faces, IDs );		// 5		should store another SQL event.
	
	faces.clear();
	IDs.clear();
	process_detected_faces( faces, IDs );		// represents 1 frame.
	process_detected_faces( faces, IDs );		// 2
	process_detected_faces( faces, IDs );		// 3
	process_detected_faces( faces, IDs );		// 4
	process_detected_faces( faces, IDs );		// 5
	process_detected_faces( faces, IDs );		// 6	
		
	//printf("test_face_summary() done");	
}
