void init_Segments(int mImageWidth)
{
	int i;
	int start    = 0;
	int seg_size = ((float)mImageWidth / 64.);
	//printf("seg_size=%d\n", seg_size);
	num_segs = 64;
	for (i=0; i<num_segs; i++)
	{
		segs[i].col_start = start;
		segs[i].col_end   = start + seg_size;
		start += (seg_size+1); 
	}	
}

void delete_segment(int mIndex)
{
	int i;
	for (i=mIndex; i<num_segs-1; i++)
	{
		segs[i] = segs[i+1];		
	}
	segs[num_segs].col_start = 0;
	segs[num_segs].col_end   = 0;	
	num_segs--;
}

void insert_segment(int mIndex)
{
	int i;
	for (i=num_segs; i>mIndex; i--)
	{
		segs[i] = segs[i-1];		
	}
	segs[mIndex].col_start = 0;
	segs[mIndex].col_end   = 0;	
	num_segs++;
}

/* Combine two adjacent segments */
// NEED TO RECALC STDDEV AFTER:
void combine_segments(int mIndex)
{
	int size1 = (segs[mIndex].col_end - segs[mIndex].col_start);
	int size2 = (segs[mIndex+1].col_end - segs[mIndex+1].col_start);
	
	float avg = (size1*segs[mIndex].slope_avg + size2*segs[mIndex+1].slope_avg)/(size1+size2);
	segs[mIndex].slope_avg = avg;
	segs[mIndex].col_end   = segs[mIndex+1].col_end;
	
	// NEED TO RECALC STDDEV AFTER:
	//float stddev;
	//calc_stddevs( *m_delta, avg, &stddev );
	
	// delete the next index segment:	
	printf("deleting %d\t", mIndex+1 );
	delete_segment( mIndex+1 );
}

float max_slope()
{
	float max=segs[0].slope_avg;
	int i;
	for (i=0; i<num_segs; i++)
	{	
		if (max < segs[i].slope_avg) 
			max = segs[i].slope_avg;
	}
	return max;
}

/* 
Return:	0 	Slopes are significantly different.
		1	Slopes are statistically equal */
int segment_slopes_equivalent(int mIndex)
{
	//sqrt( pow(segs[mIndex].slope_stddev, 2) + pow(segs[mIndex+1].slope_stddev, 2) );
	// Use the minimum b/c the large slope also has a large stddev, b/c the discontinuity happened there.
	// So it's stddev is not reliable.  In fact we'll refine that bin.
		
	float minimum        = ((segs[mIndex].slope_avg > segs[mIndex+1].slope_avg) && (segs[mIndex+1].slope_avg>0.0)) ? segs[mIndex+1].slope_avg : segs[mIndex].slope_avg;
	float combined_error = fabs(6.*minimum);
	float discrepancy    = fabs( segs[mIndex].slope_avg - segs[mIndex+1].slope_avg );
	printf(" %4.4f - %4.4f \t", segs[mIndex].slope_avg, segs[mIndex+1].slope_avg );
	printf("Index=%d; disc=%4.4f; err=%4.4f\t", mIndex, discrepancy, combined_error);
	//float maximum = max_slope();
	//float maximum = (segs[mIndex].slope_avg > segs[mIndex+1].slope_avg ) ?  segs[mIndex].slope_avg : segs[mIndex+1].slope_avg ;
	//float percent_discrep = fabs( discrepancy / maximum )*100.;

	if (discrepancy < combined_error)
	{	
		printf("equal\n");
		return 1;
	}
	else {
		printf("Different!\n");
		return 0;	
	}
}

void scan_combine_segments()
{
	int seg_index = 0;
	while (seg_index<num_segs)
	{
		int result = segment_slopes_equivalent( seg_index );
		if (result) { 
			//printf ("combine %d\n", seg_index );
			combine_segments( seg_index );
		}
		else 
			seg_index++;
	}
}

// Return: 1		Equal
//		   0  		Different
int value_expected( float slope, float stddev, float value )
{
	float discrepancy = (slope - value);
	if (discrepancy < (6.*stddev))
		return 1;
	return 0;
}

void split_segment( int mIndex, int16_t *m_delta )
{
	/* if the stddev is large for a group split, and recompute averages & stddev in both
	   sections.  Where to place the split?  Project slopes from previous segments, and
	   where 3 samples deviation exceed in a row.
	*/
	if (mIndex==0) return;  // figure out later.

	int outliers=0;
	float prev_slope  = segs[mIndex-1].slope_avg;
	float prev_stddev = segs[mIndex-1].slope_stddev;

	// Scan the segment.  And notice when deviates from previous slope.	
	int index = segs[mIndex].col_start;
	int cut_off = 0;
	while ((index<segs[mIndex].col_end) || (cut_off>0))
	{
		int result = value_expected( prev_slope, prev_stddev, m_delta[index] );
		if (result==0) { 
			outliers++;
			if (outliers>3)
			{
				  cut_off = index;
				  break;
			}
		}
		else 
		{
			outliers=0;	// has to be in a row (sequentially or restart). 
		}
	}
	
	// CREATE NEW SEGMENT:
	insert_segment(mIndex);
	segs[mIndex+1].col_start = cut_off-3;
	segs[mIndex+1].col_end   = segs[mIndex].col_end; // leaves off where the segment originally did before the split.

	// Update end of the 1st portion segment.
	segs[mIndex].col_end = segs[mIndex+1].col_start-1;

	// RECOMPUTE AVGs and STDDEV 
	calc_averages_1seg( mIndex, m_delta   );
	calc_stddevs_1seg  ( mIndex, m_delta   );
	
	calc_averages_1seg( mIndex+1, m_delta );
	calc_stddevs_1seg  ( mIndex+1, m_delta );
}

void scan_split_segment( int16_t *m_delta )
{
	int i;
	for (i=0; i<num_segs; i++)
	{
		if (segs[i].slope_stddev >  50. )
			split_segment( i, m_delta );
	}
}
