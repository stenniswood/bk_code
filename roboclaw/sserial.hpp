#ifndef _SSERIAL_HPP_
#define _SSERIAL_HPP_

#include <poll.h>
#include <linux/serial.h>
#include <time.h>

#define TX_BUFFER_SIZE 50
#define RX_BUFFER_SIZE 50

class SSerial
{
public:
	SSerial();
	SSerial(const char* mDeviceName);
	~SSerial();
	
	void	open			( const char* mDeviceName=NULL );
	void	close			( );
	int		available		( );
	char	serialGetchar	( );

	char	read			( );
	size_t	write			( char* mBuffer, int mLength );
	size_t	write			( char mBuffer );
	
	void	set_baud		(int speed);	
	void	get_baud_divisor(  );
	void	set_baud_divisor(int speed);

	void 	flush();		// stub

	int    			fd;
	int    			valid_location_lock;
	struct 	pollfd 	serial_poll;
	struct tm 		time_stamp;

 	char	rx_buffer[RX_BUFFER_SIZE];
 	char	tx_buffer[TX_BUFFER_SIZE];

	int		rx_bytes;		 // rx data bytes in buffer
	int 	_cl_tx_bytes;
	int		_cl_baud;
	int		_cl_tx_detailed;
	bool    connected;
	char*	_cl_port;
};


#endif
