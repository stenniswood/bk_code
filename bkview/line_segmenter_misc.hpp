void Init_Segments(int mImageWidth);
void calc_deltas( uint16_t *v_depth, int16_t* v_delta );
void delete_segment			( int mIndex		);
void combine_segments		( int mIndex		);
int segment_slopes_equivalent(int mIndex		);
void scan_combine_segments	(					);
void calc_averages			( int16_t *m_delta, float* m_average );
void calc_stddevs			( int16_t *m_delta, float* m_average, float* m_stddevs );
void calc_thresholds		( int16_t *m_delta, float* m_average, float* m_stddevs );
void colorize_seg_ends		( uint8_t *m_draw_image_line 		);


