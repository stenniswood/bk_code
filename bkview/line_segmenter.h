

extern vector<int>  m_peaks;
extern vector<int>  m_confidence;


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


void print_segs	 ( 											);
void print_darray( char* mname, int16_t* mfay, int mLength 	);
void print_xarray( char* mname, uint16_t* mfay, int mLength );
void print_farray( char* mname, float* mfay, int mLength 	);
