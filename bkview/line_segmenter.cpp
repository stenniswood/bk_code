/**********************************************************************************
The algorithm is:
	
	Initial 64 segments spaced evenly across the line.
	Computer stats on each segment.
	Combine all segments which are similar.
	Scan within to find threshold violations.  The new edges.
		Might be no significant errors.
		Might be an outlier.
		Might be an edge.	Split and re-evaluate stats.
				Compare - both stddev should be smaller than the unsplit stddev.
*********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include "libfreenect.h"
#include <pthread.h>
#include <math.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif


int line_regression(int n, const float x[], const float y[], float* m, float* b, float* r);

using namespace std;
int16_t  v_delta[640];
int16_t  h_delta[480];	// for scans from top down.

struct segment 
{
	int col_start;		// start column
	int col_end;	
	float slope_avg;
	float slope_stddev;	
	int	tag_for_splitting;	// 1 means split; 0 means leave as is.
};

const int PEAK_TYPE_SLOPE_CHANGE = 0;
const int PEAK_TYPE_FALLING = 1;
const int PEAK_TYPE_RISING = 2;

struct peak_info 
{
	int peak_column;
	int confidence;
	int peak_type;
	int line_above;			// index to a peak in the line above within a threshold 
	int line_below;
	int group;		// the raw peaks get grouped for line fit etc.
};
vector<struct peak_info>  			m_peaks;				// for 1 line
int 								m_lines_start_row;		// first row (line) the m_all_lines starts at.
vector<vector<struct peak_info> >	m_lines;
vector<struct segment> 				m_segs;

struct segment 			segs[64];
int    					num_segs = 0;
//float  					v_avg   [64];
//float  					v_stddev[64];

void clear_processing()
{
	m_peaks.clear();
	m_lines.clear();
}

/* Computes derivative over 1 row.  */
void calc_deltas( uint16_t *v_depth, int16_t* v_delta, bool mUseGamma )
{
	int i;
	unsigned char v1,v2;
	
	for (i=0; i<640-1; i++)
	{
		if (mUseGamma) {		
			v1 = 255-(t_gamma[v_depth[i]  ]&0xff);
			v2 = 255-(t_gamma[v_depth[i+1]]&0xff);
		} else {
			v1 = v_depth[i  ];
			v2 = v_depth[i+1];
		}
		v_delta[i] = (v1 - v2);				
	}
}

/* Computes derivative over 1 row.  */
void calc_h_deltas( uint16_t *v_depth, int16_t* h_delta, bool mUseGamma )
{
	int index1,index2;
	unsigned char h1,h2;
	
	for (int r=0; r<480-1; r++)
	{
		index1 = r * 640;
		index2 = (r+1) * 640;
		if (mUseGamma) {		
			h1 = 255-(t_gamma[v_depth[index1]]&0xff);
			h2 = 255-(t_gamma[v_depth[index2]]&0xff);
		} else {
			h1 = v_depth[index1];
			h2 = v_depth[index2];
		}
		h_delta[r] = (h1-h2);
	}
}

const int PIXEL_DIFF_ALLOWED = 5;		// +- 5 

/* This should operate on the horizontal deltas just as easily as
on the v_delta array.  Will verify this soon. */
void find_peaks( int16_t* _delta )
{
	bool	pos_slope 	   = (_delta[0]>0)? true:false;
	bool	pos_slope_prev = (_delta[0]>0)? true:false;
	int 	slope_counter=0;
	struct peak_info tmp;
	int 	i,delta;
	m_peaks.clear();
	tmp.line_above = -1;
	tmp.line_below = -1;
	tmp.group      = -1;
	int prev_column = -1;
		
	// Create a list of peaks:
	for (i=1; i<640; i++)
	{
		prev_column = -1;
		// Inflection point (change of slope) :
		if (_delta[i]==0) { /*skip*/ }
		else {
			// Check for inflection:
			pos_slope = (_delta[i]>0) ? true:false;	
			if (pos_slope == pos_slope_prev)
			{
				slope_counter++; 
				if (slope_counter>=2) slope_counter=2;
			} else 
			{
				slope_counter--; 
				if (slope_counter==0) {
					tmp.peak_column = i;
					tmp.confidence = 10.;		// could scan ahead - several samples in a row on both sides is more confident.
					tmp.peak_type = PEAK_TYPE_SLOPE_CHANGE;
					
					delta = (i-prev_column);
					delta = (delta>0)?delta:-delta;
					if ((delta>PIXEL_DIFF_ALLOWED) || (prev_column==-1))
					{
						m_peaks.push_back( tmp );
						prev_column = i;
					}
					pos_slope_prev = pos_slope;
				}
			}
		}

		// Abrupt Change in Distance:
		delta = fabs(_delta[i]);
		if (delta > 4)
		{
			tmp.peak_column = i;
			tmp.confidence  = _delta[i];	
			if (_delta[i]>0)
				tmp.peak_type = PEAK_TYPE_RISING;
			else
				tmp.peak_type = PEAK_TYPE_FALLING;
			m_peaks.push_back( tmp );
		}		
	}
	// Now add to list of all lines:
	m_lines.push_back( m_peaks );
}

void shortest_length()
{
	int tmp = 2000;
	int shortest = 2000;
	for (int i=0; i<m_lines.size(); i++) 
	{
		tmp = m_lines[i].size();
		if (tmp < shortest)	{
			shortest = tmp;
		}
	}
}


void filter_peaks()
{
	bool repeat = true;
	int ref,comp;
	int lsize = 0;
	int delta=0;
	int c1,c2;
		
	// peaks very close to each other on the same line.  And medium to low confidence.
	for (int i=0; i<m_lines.size(); i++)
	{
		repeat = true;
		//while (repeat==true)
		{
			repeat = false;
			lsize  = m_lines[i].size();
			for (int pi=0; pi<lsize-1; pi++)
			{	
				ref  = m_lines[i][pi].peak_column;
				comp = m_lines[i][pi+1].peak_column;
				delta = ref-comp;
				delta = (delta>0) ? delta:-delta;
				if (delta < PIXEL_DIFF_ALLOWED) 
				{
					// Enhancement: Find which one is closest to lines above and below.
					// not implemented yet.					

					repeat = true;
					// Remove one which is farthest out and lowest confidence:
					c1 = m_lines[i][pi].confidence;
					c2 = m_lines[i][pi+1].confidence;
					if (c1>c2) {
					//	m_lines[i].erase( m_lines[i].begin() + pi );						
					} else {
					//	m_lines[i].erase( m_lines[i].begin() + pi+1 );
					}
				}
			}
		}
	}
}


/* 
Return:  index  into the peak vector. 
One peak.
*/
int find_peak_close_by( int mLine, int mColumn, int mThresholdAllowed )
{
	int peak_comp;
	int delta;
	// scan all the peaks on the next line for one within range:
	for (int p=0; p<m_lines[mLine].size(); p++)
	{
		peak_comp = m_lines[mLine][p].peak_column;
		delta = (peak_comp-mColumn);
		delta = (delta>0) ? delta:-delta;	// absolute value.
		if (delta< mThresholdAllowed)
		{
			return p;		
		}
	}
	return -1;
}

// Each row may have the peaks in different indices.
void correlate_peaks( )
{
	//const int PIXEL_DIFF_ALLOWED = 3;		// +- 5 
	int num_lines = m_lines.size();
	int peak_ref;
	int next_line;
	int peak_comp, delta;
	int column,close_peak;
	int lsize = 0;

	// Scan thru all lines:
	for (int l=0; l<m_lines.size()-1; l++)
	{		
		lsize = m_lines[l].size();
		// Scan thru all peaks on the line:
		for (int p=0; p<lsize; p++)
		{
			// For each peak on the line. Try to find a match on line above.			
			//m_lines[l][p].line_above = -1;
			// Get current column:
			column     = m_lines[l][p].peak_column;
			// 
			close_peak = find_peak_close_by( l+1, column, 4 );  // +-4						
			m_lines[l][p].line_above      = close_peak;
			if (close_peak != -1)
				m_lines[l+1][close_peak].line_below = p;
		}
	}
	/* Now every peak which has one close by on the line above is noted */
}

/*
	This will go along a connected line peaks and assign the group number to all of them.
*/
int traverse_assign_group(int ml, int mp, int mGroup)
{
	int p = mp;
	int l = ml;
	int count = 0;
	
	// Scan thru all lines:
	while ((l<(480-1)) && (p != -1))
	{	
		if (m_lines[l][p].group == -1) {
			m_lines[l][p].group = mGroup;

		}
		count++;
		
		// Now it's line above :
		p = m_lines[l][p].line_above;		
		l++;
	}
	return count;
}
vector<int> m_group_counts;
/* 
   Assign group numbers to each vertical line which is of the same type.
*/
void group_peaks()
{
	int group_counter=0;
	int line_above=0;
	int lsize = 0;
	int c=0;
	m_group_counts.clear();
	
	// Scan thru all lines:
	for (int l=0; l<m_lines.size()-1; l++)
	{
		lsize = m_lines[l].size();
		// Scan thru all peaks on the line : 
		for (int p=0; p<lsize; p++)
		{
			if (m_lines[l][p].group == -1)	// if unassigned
			{
				c = traverse_assign_group( l, p, group_counter );
				m_group_counts.push_back(c);
				group_counter++;
			}
		}
	}
}

struct Seed
{
	int line;
	int peak_index;
	int group_count;
	float slope;
	float intercept;
};
vector<Seed> m_seeds;

/*
	This will pull out 1 lin/peak index for each group.
*/
void extract_seeds()
{
	struct Seed tmp;
	int group_counter=0; 
	tmp.slope 	  = 0;
	tmp.intercept = 0;
	int lsize = 0;
	m_seeds.clear();
	
	// Scan thru all lines:
	for (int l=0; l<m_lines.size()-1; l++)
	{
		lsize = m_lines[l].size();
		// Scan thru all peaks on the line:
		for (int p=0; p<lsize; p++)
		{
			// The first time we encounter the group add it to the seed list.
			// Hopefully this gets all of them because the groups were assigned
			// in this order (scan lines, scan peaks within) method.
			if (m_lines[l][p].group == group_counter)
			{
				tmp.line = l;
				tmp.peak_index = p;
				tmp.group_count = m_group_counts[group_counter];
				m_seeds.push_back( tmp );
				group_counter++;
			}
		}
	}
	printf("extracted %d seeds\n", m_seeds.size() );
}

float x_lf[3024];
float y_lf[3024];

int traverse_group_extract_coords(int mSeed, float* mX, float* mY)
{
	int ml = m_seeds[mSeed].line;
	int mp = m_seeds[mSeed].peak_index;

	int l = ml;
	int p = mp;
	int coord_count = 0;
	int lsize = 0;
	//printf("traverse_group_extract_coords()\n" );

	// Scan thru all lines:
	while ((l<(480-1)) && (p != -1))
	{			
		// Check bounds:
		lsize = m_lines[l].size();
		if (p>=lsize)
		{
			printf("Error: line %d : lsize=%d; p=%d\n", l, lsize, p );
			break;
		}
		
		// Extract the coordinate:			
		mX[coord_count] = m_lines[l][p].peak_column;
		mY[coord_count] = l;
		coord_count++;

		// Update indices:		
		p = m_lines[l][p].line_above;
		l++;
	}
	return coord_count;
}

void linefit_groups()
{
	int   num_coords=0;
	float slope		=1.;
	float intercept	=0.; 
	for (int s=0; s<m_seeds.size(); s++)
	{
		num_coords = traverse_group_extract_coords(s, x_lf, y_lf);		
		if (num_coords>10) {
			int g_count = m_seeds[s].group_count;
			printf ("seed=%d; group_count=%d;  num_coords=%d\n", s, g_count, num_coords);
			//line_regression(num_coords, x_lf, y_lf, &slope, &intercept, NULL);
		}
		m_seeds[s].slope = slope;
		m_seeds[s].intercept = intercept;		
	}
}

/* This groups the edges according to our best guess for a door.
	ie. a door consists of [Falling Edge, Rising Edge]
	and a top.

Keep thinking simple.

*/	
void find_door_pieces()
{
	

}

// Cut line into segments of 10 pixels.
// compute the average delta in each 10 pixels.
void calc_averages_raw( int16_t *m_delta, float* m_average )
{
	int segment_size = 10;
	int i=0,j=0;
	m_average[j]     = 0.0;
	for (i=1; i<640; i++)
	{
		m_average[j] += m_delta[i];
		if ((i % segment_size)==0)
		{
			m_average[j] /= segment_size;
			j++;
			m_average[j] = 0;
		}
	}
}

void calc_averages_1seg( int Seg, int16_t *m_delta )
{
	int i=0;
	int segment_size;

	segment_size = (segs[Seg].col_end - segs[Seg].col_start);
	segs[Seg].slope_avg = 0;
	for (i=segs[Seg].col_start; i<segs[Seg].col_end; i++)
		segs[Seg].slope_avg += m_delta[i];
	segs[Seg].slope_avg /= segment_size;
}

// compute the stddev for each average.
void calc_stddevs_1seg( int Seg, int16_t *m_delta )
{
	int i=0;
	int segment_size;
	
	segment_size = (segs[Seg].col_end - segs[Seg].col_start);
	segs[Seg].slope_stddev = 0;
	for (i=segs[Seg].col_start; i<segs[Seg].col_end; i++)		
		segs[Seg].slope_stddev += pow( m_delta[i] - segs[Seg].slope_avg, 2 );
	segs[Seg].slope_stddev /= segment_size;
}

void calc_averages_seg( int16_t *m_delta )
{
	int i=0,j=0;
	int segment_size;
	
	for (j=0; j<num_segs; j++)
	{
		segment_size = (segs[j].col_end - segs[j].col_start);
		segs[j].slope_avg = 0;
		for (i=segs[j].col_start; i<segs[j].col_end; i++)		
			segs[j].slope_avg += m_delta[i];
		segs[j].slope_avg /= segment_size;
	}
}

// compute the stddev for each average.
void calc_stddevs_seg( int16_t *m_delta )
{
	int i=0,j=0;
	int segment_size;
	
	for (j=0; j<num_segs; j++)
	{
		segment_size = (segs[j].col_end - segs[j].col_start);
		segs[j].slope_stddev = 0;
		for (i=segs[j].col_start; i<segs[j].col_end; i++)		
			segs[j].slope_stddev += pow( m_delta[i] - segs[j].slope_avg, 2 );
		segs[j].slope_stddev /= segment_size;
	}
}


// compute the stddev for each average.
void calc_stddevs( int16_t *m_delta, float* m_average, float* m_stddevs )
{
	int segment_size = 10;
	int i,j=0;
	for (i=1; i<640; i++)
	{
		m_stddevs[j] += pow( m_delta[i] - m_average[j], 2 );

		if ((i%segment_size)==0)
		{
			m_stddevs[j] /= segment_size;
			j++;
			m_stddevs[j] = 0;			
		}
	}
}

void calc_thresholds( int16_t *m_delta, float* m_average, float* m_stddevs )
{
	int   segment_size = 10;
	int i=0;
	int j=0;
	num_segs = 0;
	float plus_6sigma  = m_average[j] + 6. * m_stddevs[j];
	float minus_6sigma = m_average[j] - 6. * m_stddevs[j];

	segs[num_segs].col_start = 0;
	for (i=1; i<640; i++)
	{
		if ((m_delta[i] > plus_6sigma) || (m_delta[i] < minus_6sigma))
		{
			printf(" %d;  H:%6.2f, L:%6.2f \n", m_delta[i], plus_6sigma, minus_6sigma);
			segs[num_segs].col_end   = i;
			num_segs++;
			segs[num_segs].col_start = i+1;
		}			

		if ((i % segment_size)==0)
		{		
			//printf("next_segment...\n");	
			j++;
			plus_6sigma  = m_average[j] + 6. * m_stddevs[j];
			minus_6sigma = m_average[j] - 6. * m_stddevs[j];
		}
	}
}

/* This draws a graph on top of the image for debugging. */
void plot_array( uint16_t* mArray, uint16_t max_value, uint8_t *mImage )
{
	int index;
	float scaled;
	for (int i=0; i<640; i++)
	{
		//scaled = ((float)mArray[i] / (float)max_value) * 480.;
		//scaled = mArray[i];
		// gamma function stretches(increases) the contrast (especially at low end).
		scaled = 400*(255-(t_gamma[mArray[i]]&0xff))/max_value;
		scaled = (scaled > 470.) ? 470.:scaled;

		index = scaled * 640*3;
		mImage[index + 3*i +0] = 255.;
		mImage[index + 3*i +1] = 0.;
		mImage[index + 3*i +2] = 0.;
	}
}

void colorize_seg_ends2(uint8_t *image, bool mOnlyGroups )
{
	int rl = 640*3;			// row length
	int column=0;
	int line=0;				// [m_lines_start_row... + m_lines.size()]
	int line_index = m_lines_start_row;	// 
	int index;
	// SCAN ALL LINES (that we have) : 
	//printf("start_row=%d; + rows=%d\n", m_lines_start_row, m_lines.size() );
	for (line=m_lines_start_row; line<(m_lines_start_row+m_lines.size()); line++)
	{
		index = line * rl;
		line_index = line - m_lines_start_row;
		// SCAN ALL PEAKS (that we have)
		//printf( "%d peaks=%d\n", line, m_lines[line_index].size() );
		for (int p=0; p<m_lines[line_index].size(); p++)
		{
			column    = m_lines[line_index][p].peak_column;
			int conf  = m_lines[line_index][p].confidence;
			int group = m_lines[line_index][p].group;
			if (mOnlyGroups && (group==-1))
				continue;	// Don't colorize it.
				
			if (conf==10)
			{
				image[ index + 3*column + 0 ] = 255;
				image[ index + 3*column + 1 ] = 255;
				image[ index + 3*column + 2 ] = 0;
			}
			else if (conf > 0)
			{
				image[ index + 3*column + 0 ] = 255;
				image[ index + 3*column + 1 ] = 0;
				image[ index + 3*column + 2 ] = 255;
			} else {
				image[ index + 3*column + 0 ] =   0;
				image[ index + 3*column + 1 ] = 255;
				image[ index + 3*column + 2 ] = 255;			
			}
			image[ index + 3*column + 0-3 ] = 0;	// for added contrast!
			image[ index + 3*column + 1-3 ] = 0;
			image[ index + 3*column + 2-3 ] = 0;
		}
	}	
}

void colorize_seg_ends(uint8_t *m_draw_image_line )
{
	int rl = 640*3;	// row length
	int i=0;
	for (; i<num_segs; i++)
	{
		m_draw_image_line[3*segs[i].col_start+0] = 0;
		m_draw_image_line[3*segs[i].col_start+1] = 255;
		m_draw_image_line[3*segs[i].col_start+2] = 0;

		m_draw_image_line[3*segs[i].col_end+0] = 255;
		m_draw_image_line[3*segs[i].col_end+1] = 0;
		m_draw_image_line[3*segs[i].col_end+2] = 0;		

		// DRAW TRIPLICATE:
		m_draw_image_line[3*segs[i].col_start+rl+0] = 0;
		m_draw_image_line[3*segs[i].col_start+rl+1] = 255;
		m_draw_image_line[3*segs[i].col_start+rl+2] = 0;

		m_draw_image_line[3*segs[i].col_end+rl+0] = 255;
		m_draw_image_line[3*segs[i].col_end+rl+1] = 0;
		m_draw_image_line[3*segs[i].col_end+rl+2] = 0;		

		// DRAW TRIPLICATE:
		m_draw_image_line[3*segs[i].col_start+2*rl+0] = 0;
		m_draw_image_line[3*segs[i].col_start+2*rl+1] = 255;
		m_draw_image_line[3*segs[i].col_start+2*rl+2] = 0;

		m_draw_image_line[3*segs[i].col_end+2*rl+0] = 255;
		m_draw_image_line[3*segs[i].col_end+2*rl+1] = 0;
		m_draw_image_line[3*segs[i].col_end+2*rl+2] = 0;		

		// DRAW TRIPLICATE:
		m_draw_image_line[3*segs[i].col_start+3*rl+0] = 0;
		m_draw_image_line[3*segs[i].col_start+3*rl+1] = 255;
		m_draw_image_line[3*segs[i].col_start+3*rl+2] = 0;

		m_draw_image_line[3*segs[i].col_end+3*rl+0] = 255;
		m_draw_image_line[3*segs[i].col_end+3*rl+1] = 0;
		m_draw_image_line[3*segs[i].col_end+3*rl+2] = 0;

	}
}

#define sqr(h) h*h

int line_regression(int n, const float x[], const float y[], float* m, float* b, float* r)
{
	float   sumx = 0.0;                        /* sum of x                      */
	float   sumx2 = 0.0;                       /* sum of x**2                   */
	float   sumxy = 0.0;                       /* sum of x * y                  */
	float   sumy = 0.0;                        /* sum of y                      */
	float   sumy2 = 0.0;                       /* sum of y**2                   */

   for (int i=0;i<n;i++)   
	  { 
	  sumx  += x[i];       
	  sumx2 += sqr(x[i]);  
	  sumxy += x[i] * y[i];
	  sumy  += y[i];      
	  sumy2 += sqr(y[i]); 
	  } 

   float denom = (n * sumx2 - sqr(sumx));
   if (denom == 0) {
	   // singular matrix. can't solve the problem.
	   *m = 0;
	   *b = 0;
	   *r = 0;
	   return 1;
   }

   *m = (n * sumxy  -  sumx * sumy) / denom;
   *b = (sumy * sumx2  -  sumx * sumxy) / denom;
   if (r!=NULL) {
	  *r = (sumxy - sumx * sumy / n) /          /* compute correlation coeff     */
			sqrt((sumx2 - sqr(sumx)/n) *
			(sumy2 - sqr(sumy)/n));
   }

   return 0; 
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////// PRINT ROUTINES //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void print_segs( )
{
	int i=0;
	printf("Segments:\n");
	for (; i<num_segs; i++)
	{
		printf ("%d:  <%d, %d> \n", i, segs[i].col_start, segs[i].col_end );		
	}
	printf("\n");
}

void print_seg_stats( )
{
	int i=0;
	printf("Segment Stats %d:\n", num_segs);
	for (i=0; i<num_segs; i++)
		printf("%d\t", i );

	for (i=0; i<num_segs; i++)
		printf("%d\t", segs[i].col_start );

	for (i=0; i<num_segs; i++)
		printf("%d\t", segs[i].col_end );

	for (i=0; i<num_segs; i++)
		printf("%4.4f\t", segs[i].slope_avg );

	for (i=0; i<num_segs; i++)
		printf("%4.4f\t", segs[i].slope_stddev );

	printf("\n");
}

void print_darray( char* mname, int16_t* mfay, int mLength )
{
	int i;
	printf("|%s|==", mname );
	for (i=0; i<mLength; i++)
	{
		printf("%d, ", mfay[i] );
	}
	printf("==|\n");
}
void print_xarray( char* mname, uint16_t* mfay, int mLength )
{
	int i;
	printf("|%s|==\n", mname );
	for (i=0; i<mLength; i++)
	{
		printf("%x, ", (-mfay[i])&0xFF );
	}
	printf("==|\n");
}

void print_farray( char* mname, float* mfay, int mLength )
{
	int i;
	printf("|%s|==", mname );
	for (i=0; i<mLength; i++)
	{
		printf("%6.2f, ", mfay[i] );
	}
	printf("==|\n");
}

/* A corner:

	run => -
	rise => darker => -
	
	run => +
	rise => darker => -
==== in other words ===
run always +
	rise => brighter => +
then
	rise => darker => -
So slope changes sign. on a convex corner.		
void find_inflection( )
{
	bool	pos_slope 	   = (v_delta[0]>0);
	bool	pos_slope_prev = (v_delta[0]>0);	
	struct peak_info tmp;
	int i;
	int slope_counter;		// number of counts
	// Create a list of peaks:
	for (i=1; i<640; i++)
	{
		pos_slope = (v_delta[i]>0);
		if (v_delta[i]==0) {  }	//skip
		else {
			// Check for inflection:
			pos_slope = (v_delta[i]>0) ? true:false;	
			if (pos_slope == pos_slope_prev)
			{
				slope_counter++; 
				if (slope_counter>=2) slope_counter=2;
			} else 
			{
				slope_counter--; 
				if (slope_counter==0) {
					
					tmp.peak_column = i;
					tmp.confidence = 10.;		// could scan ahead - several samples in a row on both sides is more confident.
					m_peaks.push_back( tmp );
					pos_slope_prev = pos_slope;
				}
			}
	}
	// Now add to list of all lines:
	m_lines.push_back( m_peaks );
}*/
