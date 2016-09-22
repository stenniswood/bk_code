#ifndef _SERIAL_HPP_
#define _SERIAL_HPP_

#include <sys/types.h>

const int WORKING_LENGTH=80;
void print_args(int argc, char *argv[]);

/* This class can be used 2 ways:
		Can create a thread and call the serial_main() function
		this will continually read & write.  To send data, just put
		it into the buffer by calling my_write();
		
	The other way is like a file and a thread is not necessary.
	
*/
class SerialInterface 
{
public:
	SerialInterface ();
	~SerialInterface();

	void	Initialize();

	unsigned char read();
	int 	my_write	  (char mByte);
	int 	my_write	  (char* mBuffer, int size);	
	bool	available 	  ();

	int	 	peek ()		{ return accum_buff[0];  };
	void 	begin();
	void 	flush()		{ /* Not sure here */    };
	
	void 	parse_ascii_data	( );	
	void 	dump_data			( unsigned char * b, int count);
	void	dump_data_ascii		( unsigned char * b, int count);
	void 	set_baud_divisor	( int speed);
	int	 	get_baud			( int baud );
	void 	process_options		( int argc, char ** argv);
	void 	dump_serial_port_stats();
	void 	process_read_data	( );
	void 	process_write_data	( );
	bool 	setup_serial_port	( int baud);
	int  	diff_ms				( const struct timespec *t1, const struct timespec *t2);	
	int  	serial_main			(  );

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
	int 		  _fd;
	
	unsigned char * _write_data;
	ssize_t 		_write_size;			// buffer size
	ssize_t			current_write_size;

	// keep our own counts for cases where the driver stats don't work
	int _write_count;
	int _read_count ;
	int _error_count;
};


extern class SerialInterface si;

#endif
