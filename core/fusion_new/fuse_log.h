#ifdef  __cplusplus
extern "C" {
#endif


void open_log_file				( );

void save_sample_raw			( );
void save_sample_angles 		( );
void save_sample_angular_rate	( );
void save_sample_intermediate	( );
void save_sample_result			( );
void save_timeslice_data		( );

void close_log_file				( );



#ifdef  __cplusplus
}
#endif
