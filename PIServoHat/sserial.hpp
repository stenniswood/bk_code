#ifndef _SSERIAL_HPP_
#define _SSERIAL_HPP_

#include <poll.h>
#include <linux/serial.h>
#include <time.h>

#define TX_BUFFER_SIZE 80
#define RX_BUFFER_SIZE 80


class SSerial
{
public:
	SSerial();
	SSerial(const char* mDeviceName);
	~SSerial();
	void	initialize		( );
	
	void	open			( const char* mDeviceName=NULL );
	void	close			( );
	int		available		( );
	char	serialGetchar	( );
	int		readLine		( );
	void	print_buffer	(char* mBuffer, int mLength );
	
	size_t	writeStr		( char* mBuffer );
	size_t	write			( char mBuffer );	
	
	void	set_baud		(int speed);	
	void	get_baud_divisor(  );
	void	set_baud_divisor(int speed);

	void 	flush_inbuff	();	
	void	acknowledge_rxd ();		// will set rx_bytes back to zero.

	//bool 			contains_newline;
	int    			fd;
	struct 	pollfd 	serial_poll;
	struct tm 		time_stamp;

	bool			excess_data_found;
	char			carry_over[RX_BUFFER_SIZE];
	int				rx_bytes;		 // rx data bytes in buffer
 	char			rx_buffer[RX_BUFFER_SIZE];
 	char			tx_buffer[TX_BUFFER_SIZE];
	int 			_cl_tx_bytes;
	
	char*			_cl_port;	
	int				_cl_baud;
	bool    		connected;
};


#endif
