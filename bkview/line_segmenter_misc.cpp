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
