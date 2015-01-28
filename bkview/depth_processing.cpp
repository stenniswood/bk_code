#include <stdio.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


using namespace cv;
using namespace std;


/* 								Algorithm for finding walls:

What are we going to find exactly?
A) Find a wall unbounded (ie. just a slope and distance away.  No other end points or features available)
B) A concave Corner
	Should see a derivative change. (measure how big)
	The slopes on either side should be reciprocals (maybe not if perspective)

	By scanning several lines, the wall edge should be at specific locations on all
		lines.  this will bolster confidence.
C) A convex Corner
E) Scan for ceiling
D) Later add:
		1) a convex corner with header.
		2) a doorway (even partial visible)
		3) a post
		4) a barrier (ie. wall to the floor)
		5) cabinets (ie. wall with protuding box top and bottom.
*/
void depth_cb( freenect_device *dev, void *v_depth, uint32_t timestamp )
{
	int line;
	int i=0;
	uint16_t *depth = (uint16_t*)v_depth;

	if (pause) {
		pthread_cond_signal(&gl_frame_cond);
		pthread_mutex_unlock(&gl_backbuf_mutex);
		return;
	}
		
	// GENERATE SCAN LINE:
	//print_xarray("ScanPixels", &(depth[(line*640)+320]), 320);
	clear_processing();

	// DERIVATIVES
	for (line=0; line<480-1; line++)
	{
		calc_deltas( &(depth[(line*640)]), v_delta, false );
		find_peaks( v_delta );
	}
 
	correlate_peaks();		// connect the line_above and line_belows.
	//filter_peaks();		// filter crashes when erasing a point.
	group_peaks();			// assign group numbers to each group connected above.
	extract_seeds();		// now put a starter of each group into a vector.
	linefit_groups();
	

/*	for (int column=0; column<640-1; column++)
	{
		calc_h_deltas( &(depth[(column*640)]), h_delta, false );
		find_peaks( h_delta );
	}*/
	
	// Find Wall edge:
	// histogram type anaylsis on the peaks.
	// When they're obviously underneath each other.  (ie. distance between < threshold)
	//		there could be a jump ie same peaks have different indexes on different lines. but don't handle this yet. simplicity.
	//average_peaks();
	
	pthread_mutex_lock(&gl_backbuf_mutex);
	for (i=0; i<640*480; i++) {
		int pval = t_gamma[depth[i]];
		int lb = pval & 0xff;
		lb = depth[i];
		depth_mid[3*i+0] = 255-lb;
		depth_mid[3*i+1] = 255-lb;
		depth_mid[3*i+2] = 255-lb;
	}
	colorize_seg_ends2( depth_mid, true );  
	// Now analyze corner ( )
	line = 150;
	calc_deltas( &(depth[(line*640)]), v_delta, false );	
	
	//plot_array( &(depth[line*640]), 255, depth_mid );
	//plot_array( (uint16_t *)v_delta, 100, depth_mid );	
	int index;
	for (i=0; i<640; i++) 
	{
		index = line*640*3 + 3*i;
		depth_mid[index+0] = 0;
		depth_mid[index+1] = 255;
		depth_mid[index+2] = 0;	
	}
	got_depth++;
	pthread_cond_signal(&gl_frame_cond);
	pthread_mutex_unlock(&gl_backbuf_mutex);
}
