#include <math.h>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <vector>
#include <list>
#include "neighborhood_tricks.hpp"


cv::Mat				planeLabels;
int originals;

double average(uint16_t value[] , int count )
{
	double sum=0;
	for (int i=0; i<count; i++)
	{
		sum += value[i];
	}
	return (sum / count);
}
double stddev(uint16_t value[] , double average, int count )
{
	double sum=0;
	double dev=0;
	for (int i=0; i<count; i++)
	{
		dev = (value[i]-average);
		sum += dev*dev;
	}
	return sqrt(sum / count);
}

void get_slopes_ab( cv::Mat mImage, int mRowCenter, int mColCenter, struct stPlane& Plane )
{
	Plane.centerX = mColCenter;
	Plane.centerY = mRowCenter;
	
	// In a 3x3, there are 9 slopes up 
	uint16_t slope_ud[9];
	slope_ud[0] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-1,mRowCenter-1) - mImage.at<uint16_t>(mColCenter-1,mRowCenter-0)));
	slope_ud[1] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-0,mRowCenter-1) - mImage.at<uint16_t>(mColCenter-0,mRowCenter-0)));
	slope_ud[2] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter+1,mRowCenter-1) - mImage.at<uint16_t>(mColCenter+1,mRowCenter-0)));

	slope_ud[3] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-1,mRowCenter-0) - mImage.at<uint16_t>(mColCenter-1,mRowCenter+1)));
	slope_ud[4] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-0,mRowCenter-0) - mImage.at<uint16_t>(mColCenter-0,mRowCenter+1)));
	slope_ud[5] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter+1,mRowCenter-0) - mImage.at<uint16_t>(mColCenter+1,mRowCenter+1)));
	
	slope_ud[6] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-1,mRowCenter-1) - mImage.at<uint16_t>(mColCenter-1,mRowCenter+1)));
	slope_ud[7] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter+0,mRowCenter-1) - mImage.at<uint16_t>(mColCenter-0,mRowCenter+1)));
	slope_ud[8] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter+1,mRowCenter-1) - mImage.at<uint16_t>(mColCenter+1,mRowCenter+1)));
	
	// AND 9 slopes left/right:
	uint16_t slope_lr[9];
	slope_lr[0] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-1,mRowCenter-1) - mImage.at<uint16_t>(mColCenter-0,mRowCenter-1)));
	slope_lr[1] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-1,mRowCenter-0) - mImage.at<uint16_t>(mColCenter-0,mRowCenter-0)));
	slope_lr[2] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-1,mRowCenter+1) - mImage.at<uint16_t>(mColCenter-0,mRowCenter+1)));

	slope_lr[3] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-0,mRowCenter-1) - mImage.at<uint16_t>(mColCenter+1,mRowCenter-1)));
	slope_lr[4] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-0,mRowCenter-0) - mImage.at<uint16_t>(mColCenter+1,mRowCenter+0)));
	slope_lr[5] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-0,mRowCenter+1) - mImage.at<uint16_t>(mColCenter+1,mRowCenter+1)));
	
	slope_lr[6] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-1,mRowCenter-1) - mImage.at<uint16_t>(mColCenter+1,mRowCenter-1)));
	slope_lr[7] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-1,mRowCenter+0) - mImage.at<uint16_t>(mColCenter+1,mRowCenter+0)));
	slope_lr[8] = (0x0FFF)&&((mImage.at<uint16_t>(mColCenter-1,mRowCenter+1) - mImage.at<uint16_t>(mColCenter+1,mRowCenter+1)));

	Plane.Slope_ud 	   = average(slope_ud, 9);
	Plane.Slope_lr 	   = average(slope_lr, 9);
	Plane.Slope_ud_Dev = stddev( slope_ud,  Plane.Slope_ud,  9 );
	Plane.Slope_lr_Dev = stddev( slope_lr,  Plane.Slope_lr,  9 );

	// Compute average Intercept:
	uint16_t inter[9];	
	inter[0] = (0x0FFF)&&(mImage.at<uint16_t>(mColCenter-1,mRowCenter-1));// + Plane.Slope_ud - Plane.Slope_lr;
	inter[1] = (0x0FFF)&&(mImage.at<uint16_t>(mColCenter-0,mRowCenter-1));// + Plane.Slope_ud;
	inter[2] = (0x0FFF)&&(mImage.at<uint16_t>(mColCenter+1,mRowCenter-1));// + Plane.Slope_ud + Plane.Slope_lr;

	inter[3] = (0x0FFF)&&(mImage.at<uint16_t>(mColCenter-1,mRowCenter-0));
	inter[4] = (0x0FFF)&&(mImage.at<uint16_t>(mColCenter-0,mRowCenter-0));
	inter[5] = (0x0FFF)&&(mImage.at<uint16_t>(mColCenter+1,mRowCenter-0));
	
	inter[6] = (0x0FFF)&&(mImage.at<uint16_t>(mColCenter-1,mRowCenter+1));// - Plane.Slope_ud - Plane.Slope_lr;
	inter[7] = (0x0FFF)&&(mImage.at<uint16_t>(mColCenter+0,mRowCenter+1));// - Plane.Slope_ud;
	inter[8] = (0x0FFF)&&(mImage.at<uint16_t>(mColCenter+1,mRowCenter+1));// - Plane.Slope_ud + Plane.Slope_lr;
	Plane.Intercept	    = average( inter, 9 );
	Plane.Intercept_Dev = stddev( inter,  Plane.Intercept,  9 );
}

std::list<stPlane> voxels;
int voxel_rows = 0;
int voxel_cols = 0;

void scan_image_neighborhoods( cv::Mat& mRawDepth )
{
	struct stPlane Plane;
	double mAvg_ud;
	double mAvg_lr;
	double mIntercept;	
	voxel_rows=0;
	for (int row=1; row<480-1; row+=3)	
	{
		voxel_rows++;
		voxel_cols=0;
		for (int col=1; col<640-1; col+=3)
		{
			get_slopes_ab(  mRawDepth, row, col, Plane );							
			voxels.push_back( Plane );			
			voxel_cols++;
		}
	}
}

bool is_similar( struct stPlane Ref1, struct stPlane Ref2)
{
	double delta_slope_ud  = (Ref1.Slope_ud - Ref2.Slope_ud);
	double delta_slope_lr  = (Ref1.Slope_lr - Ref2.Slope_lr);
	double delta_intercept = (Ref1.Intercept - Ref2.Intercept);
	
	double margin_slope_ud  = (Ref1.Slope_ud_Dev + Ref2.Slope_ud_Dev);
	double margin_slope_lr  = (Ref1.Slope_lr_Dev + Ref2.Slope_lr_Dev);
	double margin_intercept = (Ref1.Intercept + Ref2.Intercept);
	
	bool retval = ((delta_slope_ud < margin_slope_ud) &&
				   (delta_slope_lr < margin_slope_lr) &&
				   (delta_intercept < margin_intercept));
	return retval;
}

int combine_voxels( struct stPlane Reference, list<struct stPlane>& mCombo )
{
	struct stPlane tmp;
	int adds=0;
	std::list<struct stPlane>::iterator iter = voxels.begin();
	while (iter != voxels.end() )
	{
		if (is_similar(Reference, *iter) )
		{
			// Add to current list
			tmp = *iter;
			mCombo.push_back(tmp);
			// Remove from main list.
			iter = voxels.erase(iter);
			adds++;
		}
		iter++;
	}
	return adds;
}

vector<list<struct stPlane>> Combos;

bool order_by_ud_slope ( struct stPlane& first, struct stPlane& second )
{
//	if (first.Slope_ud > second.Slope_ud)
	if (first.Slope_lr > second.Slope_lr)	
		return true;
	return false;
}

void print_voxel(struct stPlane& pl)
{
	printf("%3d,%3d: ud=%6.1f+-%6.1f;\t lr=%7.1f+-%6.1f;\t inter=%6.1f+-%6.1f\n", 
			pl.centerX,  		pl.centerY, 
			pl.Slope_ud, 		pl.Slope_ud_Dev,
			pl.Slope_lr, 		pl.Slope_lr_Dev,
			pl.Intercept, 		pl.Intercept_Dev );
}

void combine_all_voxels( )
{
	int counter=0;
	int adds=0;
	list<struct stPlane> tmp;

	printf("\nOriginal # Voxels = %d\n", voxels.size() );
	
	// Let's do this in 2 Passes - to reduce computational load.  (was 34000x34000 voxel scans)
	// First pass we'll check all 8 neighbors (b/c similar planes are likely to be together)
	std::list<stPlane>::iterator vi    = std::next(voxels.begin(), voxel_cols );
	std::list<stPlane>::iterator ender = std::prev(voxels.end(), voxel_cols );
	std::list<stPlane>::iterator nn[8];
	std::list<stPlane>::iterator row_above, row_below;	
	list<struct stPlane> tmp_list;

	/*while (vi != ender)
	{
		row_above = std::next(vi, voxel_cols );
		row_below = std::prev(vi, voxel_cols );

		nn[0] = std::prev(row_above, 1);
		nn[1] = row_above;
		nn[2] = std::next(row_above, 1);				
		nn[3] = std::prev(vi,1);
		nn[4] = std::next(vi,1);
		nn[5] = std::prev(row_below, 1);
		nn[6] = row_below;
		nn[7] = std::prev(row_below, 1);

		int adds=0;
		tmp_list.clear();
		tmp_list.push_back( *vi );
		for (int n=0; n<8; n++)
		{
			if ( is_similar( *nn[n], *vi) )
			{
				tmp_list.push_back( *nn[n] );
				voxels.erase( nn[n] );
				adds++;
			}
		}
		if (adds)
			Combos.push_back( tmp );
		vi++;
	}
	printf("End of 1st pass:  proposed PlaneCombos=%d;  voxels=%d\n ", Combos.size(), voxels.size() );
*/

	// Second pass, we'll compare all remaining for completeness. (hopefully more like 8000x8000)
	voxels.sort( order_by_ud_slope );
	printf("Sorted:  \n ");	
	vi = voxels.begin();
	for(int i=0; i<30; )
	{
		if (vi->Slope_ud < 50000)
		{
			print_voxel(*vi);
			i++;
		}
		vi++;
	}
	
	vi = voxels.begin();	
	while ((vi != voxels.end()) && (counter<originals))
	{
		counter++;
		if ((counter%5000)==0)
			printf("combing #%d\n", counter);
			
		tmp.clear();
		adds = combine_voxels( *vi, tmp );	// removes from main voxel list and adds to tmp!
		if (adds) {
			printf("combing #%d added  %ld voxels to plane\t", counter, tmp.size() );
			print_voxel(*vi);
		}
		Combos.push_back( tmp );
		vi++;
	}	
}


void find_three_largest_planes( int& mIndex1, int& mIndex2, int& mIndex3 )
{
	int size = Combos.size();
	int counts[60000];
	for (int c=0; c<size; c++)
		counts[c] = Combos[c].size();		
	
	int max_count1 = 0;
	int max_count2 = 0;
	int max_count3 = 0;		
	for (int c=0; c<size; c++)
	{
		if (counts[c] > max_count1)
		{	mIndex1 = c;	max_count1=counts[c];	}		
	}
	for (int c=0; c<size; c++)
	{
		if ((counts[c] > max_count2) && (counts[c]<max_count1))
		{	mIndex2 = c;	max_count2=counts[c];	}
	}
	for (int c=0; c<size; c++)
	{
		if ((counts[c] > max_count3) && (counts[c]<max_count2))
		{	mIndex3 = c;	max_count3=counts[c];	}
	}	
	printf("max counts:  %d, %d, %d \n", max_count1, max_count2, max_count3  );
	printf("Three largest planes:  %d, %d, %d \n", mIndex1, mIndex2, mIndex3 );
}

void annotate_combo(cv::Mat& mDepth, int mComboIndex, int mColor)
{
	int x,y;
	for ( auto v : Combos[mComboIndex] )
	{	
		x = v.centerX;
		y = v.centerY;		
		cv::Vec3b zero(0,0,0);
		mDepth.at<cv::Vec3b>(x,y) = zero;
		switch (mColor)
		{
		case 1 : mDepth.at<cv::Vec3b>( x,y )[0] = 255;	break;	
		case 2 : mDepth.at<cv::Vec3b>( x,y )[1] = 255;	break;
		case 3 : mDepth.at<cv::Vec3b>( x,y )[2] = 255;	break;
		default : break;
		}
	}
}


//  z(row,col) = d + a*col + b*row 
void segment_planes( cv::Mat& mRawDepth, cv::Mat& mPlaneLabels )
{
	voxels.clear();
	Combos.clear();
	
	//planeLabels.create ( 480, 640, CV_8UC3 );		in frame_grabber.cpp
	//scan_image_neighborhoods( mRawDepth );
	imshow( "raw depth", mRawDepth );

	originals = voxels.size();
/*	combine_all_voxels( );

	// Now make sure main list is zero:
	int remains= voxels.size();
	int planes=Combos.size();	
	printf("origs=%d;  remaining voxels=%d; planes=%d \n\n", originals, remains, planes );
	
	int index1,index2,index3;
	find_three_largest_planes( index1, index2, index3 );

	annotate_combo( mPlaneLabels, index1, 1 );
	annotate_combo( mPlaneLabels, index2, 2 );
	//annotate_combo( mPlaneLabels, index3, 3 );		
	imshow( "labels", mPlaneLabels );
*/	
}

