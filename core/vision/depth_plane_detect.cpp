
#include "depth_plane_detect.hpp"


#define MAX_WALLS 10
cv::Mat wall_images[MAX_WALLS];
cv::Mat LabelImage;

void create_wall_image()
{

}

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


void wall_boundary_scan( cv::Mat& mRawDepth, cv::Mat& mLabelImg, int mTolerance )
{

}

vector<double>	included_columns;

void line_regression(vector<double> mX, vector<double> mY, double& mSlope, double& mIntercept)
{
	assert( mX.size() == mY.size() );
	size_t n = mX.size();
	double sumX  = 0.0;
	double sumXX = 0.0;
	double sumY = 0.0;
	double sumYY = 0.0;
	double sumXY = 0.0;
		
	for (int i=0; i<n; i++)
	{
		sumX += mX[i];
		sumY += mY[i];
		sumXX += mX[i]*mX[i];
		sumYY += mY[i]*mY[i];
	}
	mSlope     = (n*sumXY - sumX*sumY) / (n*sumXX - sumX*sumX);
	mIntercept = (sumY - mSlope*sumX) / n;
}

void revise_wall_angle_estimate(double mWall_angle, double mWall_position)
{
	vector<double> X;
	vector<double> Y;
	for (auto c:included_columns)
	{
		X.push_back( c );
		Y.push_back( mRawDepth.row(scan_row)[c] );		
	}
	line_regression( X, Y, mWall_angle, mWall_position );
}

void find_wall_angle()
{
	int scan_row = 480/2;
	
	// Start with an initial estimate of the wall angle:
	int centerW   = mRawDepth.width / 2;
	int pix_left  = mRawDepth.row(scan_row)[col-3];
	int pix_right = mRawDepth.row(scan_row)[col+3];	
	double slope  = compute_slope( pix_left, pix_right, 3 );
	double intercept = mRawDepth.row(scan_row)[centerW];
	
	// y= slope*(column-centerW) + intercept		our origin is at centerW of image
	// Now grab all vertical lines which fit in this slope,intercept : 
	for (int col=0; col<640; col++)
	{
		double expected_intensity = slope*(column - centerW) + intercept;
		int low  = expected_intensity - tolerance;
		int high = expected_intensity + tolerance;
		if ((mRawDepth.row(scan_row)[col] >= low) && (mRawDepth.row(scan_row)[col] <= high))
			included_columns.push_back(col);
	}
	// And get a better estimate of the wall angle (all included columns)
	double wall_angle;
	double wall_position;
	revise_wall_angle_estimate( wall_angle, wall_position );
	printf("Revised Wall Angle    = %6.2f\n", wall_angle    );
	printf("Revised Wall Position = %6.2f\n", wall_position );	
}

void annotate_depth_image(cv::Mat& mDepth)
{
	// Scan 
	for (auto c:included_columns)
	{
		Point pt1(c, wall_boundary_bottom[c]);
		Point pt2(c, wall_boundary_top[c]   );
		Scalar color( 0, 0, 255);
		line(mDepth, pt1, pt2, color );
	}
}

/*	Assumptions:
		a)	Walls are nearly vertical planes.  ie. nearly constant depth as we scan up a column.
		b)	They have a constant column to column slope and this is the angle of the wall to us.
				remember how useful you thought this would be for robot locating (in the map)				
		c)  use			 

*/
void wall_detect(cv::Mat& mRawDepth, cv::Mat& mDepth)
{
	// Scan a row;  extracting vertical slope & intercept
	gather_wall_slope_intercept_data(mRawDepth);

	// Mark each point going up and down where it stops being the wall:
	wall_boundary_scan( mRawDepth, LabelImage, mTolerance );

	// with each vertical, compute the angle of the wall wrt the camera:	
	find_wall_angle();
	
	// Label
	annotate_wall_image(mDepth);	
}


/* Now you know what's wrong with this don't you?
		Yes.  
		a) if our starting point is not the wall.
				everything will be off.
		b) it won't pick up the wall again after an obstruction.
				so this boundary top to bottom is misguided.
				1) It'd be better to take another blank image and label each pixel as we scan
					as belonging to the wall or not.
				   Yes, then i) to annotate we change color of each labelled pixel.
				   		ii) to get the wall angle - 
				   			b/c some pixels at bottom may be used to estimate angle.
				   			or some at top.  
				   			or some right in a row in the middle as is common.
				   			
		a plane equation is just 2 slopes and 1 intercept.
		z(row,col) = d + a*col + b*row 
		say we know b quite well (b/c we know accelerometer and the wall is quite vertical => (b == close to 0))

		a can be anything - unknown.
		if we knew d (as a hypothesis), we could use pixels high or low to get a.
		yes.  good thinking.
			Now how do we get d?
				and this would answer problem a) above.
				start with a seed (region growing area of smooth pixels)
				
		
*/


void floor_detect(cv::Mat& mDepth)
{

}




/*void wall_boundary_scan( cv::Mat& mRawDepth, int mTolerance )
{
	wall_boundary_top.clear();
	wall_boundary_bottom.clear();
	// Scan across the scan row:
	for (int col=0; col<640; col++) 
	{
		bool added=false;
		int row;
		// For each column, start scanning upward, until a non-wall pixel is found.  
		//  add that as the topbounday.
		for (row=scan_row; row>0; row--) 
		{
			int row_delta = (row - scan_row);
			int expected_intensity = slopes[col] * row_delta;
			int low  = expected_intensity - mTolerance;
			int high = expected_intensity + mTolerance;
			if ( (mRawDepth[row][col] < low) || (mRawDepth[row][col] > high) )
			{
				wall_boundary_top.push_back( row );
				added = true;
				break;
			}
		}
		if (!added)
			wall_boundary_top.push_back( 0 );
		
		added = false;
		// Then for each column, start scanning downward, until a non-wall pixel is found.  
		//  add that as the bottom boundary.
		for (row=scan_row; row<480; row++) 
		{
			int row_delta = (row - scan_row);
			int expected_intensity = slopes[col] * row_delta;
			int low  = expected_intensity - mTolerance;
			int high = expected_intensity + mTolerance;
			if ( (mRawDepth[row][col] < low) || (mRawDepth[row][col] > high) )
			{
				wall_boundary_bottom.push_back( row );
				added = true;
				break;
			}
		}
		if (!added)
			wall_boundary_bottom.push_back( 480 );		
	}
}*/
