
#include "depth_plane_detect.hpp"






/* Seems easier than floor detect, so we'll begin work here!
	Returns:  change in pixel intensity as we go up the wall.
*/
double compute_slope( int mPixAbove, int mPixBelow, int mSeparationPixs  )
{
	double delta = (mPixAbove - mPixBelow);
	double retval = delta / (2*mSeparationPixs);
	return retval;
}

int scan_row = 240;
vector<int>	    intercepts;
vector<double>	slopes;

vector<double>	wall_boundary_top;		// for each column.
vector<double>	wall_boundary_bottom;	// for each column.

void gather_wall_slope_intercept_data(cv::Mat& mRawDepth)
{
	intercepts.clear();
	slopes.clear();
	
	// Scan a row in center of image.
	// get the slope 3 pixels above and 3 pixels below.
	for (int col=0; col<640; col++) 
	{
		int pix = mDepth.row(scan_row)[col];
		int pix_above = mDepth.row(scan_row-3)[col];
		int pix_below = mDepth.row(scan_row+3)[col];
		double slope = compute_slope( pix_above, pix_below, 3 );
		slopes.push_back( slope );
		intercepts.push_back( pix );
	}
}

void wall_boundary_scan( cv::Mat& mRawDepth, int mTolerance )
{
	for (int col=0; col<640; col++) 
	{
		for (int row=scan_row; row>0; row--) 
		{
			int row_delta = (row - scan_row);
			int expected_intensity = slopes[col] * row_delta;
			int low  = expected_intensity - mTolerance;
			int high = expected_intensity + mTolerance;
			if ( (mRawDepth[row][col] < low) && (mRawDepth[row][col] > high) )
			{
				wall_boundary_top.push_back( row );
				break;
			}
		}
		
		for (int row=scan_row; row<480; row++) 
		{
			int row_delta = (row - scan_row);
			int expected_intensity = slopes[col] * row_delta;
			int low  = expected_intensity - mTolerance;
			int high = expected_intensity + mTolerance;
			if ( (mRawDepth[row][col] < low) && (mRawDepth[row][col] > high) )
			{
				wall_boundary_bottom.push_back( row );
				break;
			}
		}
		
	}
}


void wall_detect(cv::Mat& mRawDepth, cv::Mat& mDepth)
{
	
}

void floor_detect(cv::Mat& mDepth)
{

}
