extern uint8_t	 proc_image[640*480*3];


void clear_processing	( );
uint8_t* process_depth_changes(uint16_t* mdepth);
void calc_deltas		( uint16_t *v_depth, int16_t* v_delta, bool mUseGamma );
void calc_h_deltas		( uint16_t *v_depth, int16_t* h_delta, bool mUseGamma );
void find_peaks			( int16_t* _delta );
void shortest_length	( );
void filter_peaks		( );
int find_peak_close_by	( int mLine, int mColumn, int mThresholdAllowed );
void correlate_peaks	( );
int traverse_assign_group(int ml, int mp, int mGroup);
void group_peaks		( );
void extract_seeds		( );
int traverse_group_extract_coords(int mSeed, float* mX, float* mY);
void linefit_groups		( );
void find_door_pieces	( );
void calc_averages_raw	( int16_t *m_delta, float* m_average );
void calc_averages_1seg	( int Seg, int16_t *m_delta );
void calc_stddevs_1seg	( int Seg, int16_t *m_delta );
void calc_averages_seg	( int16_t *m_delta );
void calc_stddevs_seg	( int16_t *m_delta );
void calc_stddevs		( int16_t *m_delta, float* m_average, float* m_stddevs );
void calc_thresholds	( int16_t *m_delta, float* m_average, float* m_stddevs );
void plot_array			( uint16_t* mArray, uint16_t max_value, uint8_t *mImage );
void colorize_seg_ends2	( uint8_t *image, bool mOnlyGroups );
void colorize_seg_ends	( uint8_t *m_draw_image_line );
int line_regression		( int n, const float x[], const float y[], float* m, float* b, float* r);

void print_segs	 ( 											);
void print_darray( char* mname, int16_t* mfay, int mLength 	);
void print_xarray( char* mname, uint16_t* mfay, int mLength );
void print_farray( char* mname, float* mfay, int mLength 	);



///////////////////////////////////////////////////////////

//extern vector<int>  m_peaks;
//extern vector<int>  m_confidence;



