#ifndef _SERIAL_LOADCELL_H_
#define _SERIAL_LOADCELL_H_

const int WORKING_LENGTH=80;
//int serial_loadcell_main(int argc, char * argv[]);

class LoadCell_SerialInterface 
{
public:
	LoadCell_SerialInterface(  );
	~LoadCell_SerialInterface();
	
	void	Initialize();
	
	void	parse_ascii_data();	
	void 	dump_data			( unsigned char * b, int count);
	void 	dump_data_ascii	( unsigned char * b, int count);
	void 	set_baud_divisor	( int speed);
	int	 	get_baud			( int baud);
	void 	process_options	( int argc, char * argv[]);
	void 	dump_serial_port_stats();
	void 	process_read_data	( );
	void 	process_write_data	( );
	void 	setup_serial_port	( int baud);
	int  	diff_ms			( const struct timespec *t1, const struct timespec *t2);
	int  	serial_loadcell_main(int argc, char * argv[]);	
	
	char 	accum_buff  [WORKING_LENGTH];
	char 	working_buff[WORKING_LENGTH];	
	char* 	last_pos;

	// command line args
	int _cl_baud;
	char *_cl_port;
	int _cl_divisor;
	int _cl_rx_dump;
	int _cl_rx_dump_ascii;
	int _cl_tx_detailed;
	int _cl_stats;
	int _cl_stop_on_error;
	int _cl_single_byte;
	int _cl_another_byte;
	int _cl_rts_cts;
	int _cl_dump_err;
	int _cl_no_rx ;
	int _cl_no_tx ;
	int _cl_rx_delay;
	int _cl_tx_delay;
	int _cl_tx_bytes;
	int _cl_rs485_delay;
	int _cl_tx_time;
	int _cl_rx_time;

	// Module variables
	unsigned char _write_count_value;
	unsigned char _read_count_value;
	int _fd;
	unsigned char *_write_data;
	ssize_t _write_size;

	// keep our own counts for cases where the driver stats don't work
	int _write_count;
	int _read_count ;
	int _error_count;

	bool	left_foot;
};

#endif
