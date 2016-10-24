/**********************************************************************************	

	Starting point.  Hough Transform.
	
	Convert all lines to slope/intercept (use Hough() )
	Combine lines which have nearly the same slope and intercept.


	Rectangle detector:
		Look for a nearly vertical line.  Put all these line indexes into a vector<>
		Look for all horizontal lines.    Put these into another vector<>
		

*********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include "libfreenect.h"
#include <pthread.h>
#include <math.h>

#include <stdio.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "line_segmenter.h"

using namespace cv;
using namespace std;

/*
*/
Vec4i m_vlines;


void convert_to_slope_intercept( Vec4i lines )
{
	float rise = (lines[3] - lines[1]);
	float run  = (lines[2] - lines[0]);

	float slope = rise / run;
//	if (run == 0)	slope = nan;	
}


void extract_vlines( Vec4i lines )
{
	float rise = (lines[3] - lines[1]);
	float run  = (lines[2] - lines[0]);

	for (int l=0; l<4; l++)  {
		rise = (lines[3] - lines[1]);
		run  = (lines[2] - lines[0]);
			
		
	}
}
