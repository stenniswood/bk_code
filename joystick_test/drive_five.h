#ifndef _DriveFive_h
#define _DriveFive_h

#include <stdarg.h>
#include <inttypes.h>
#include <cstddef>
#include <poll.h>


#define TX_BUFFER_SIZE 100
#define RX_BUFFER_SIZE 100


/******************************************************************************
* Definitions
******************************************************************************/
char* get_error_string( uint32_t mStatus );

#define _SS_VERSION 16

class DriveFive 
{
	uint16_t crc;
	uint32_t timeout;

public:
	// public methods
	DriveFive( );
	DriveFive( const char* mDeviceName );
	DriveFive( const char* mDeviceName, uint32_t time_out );
	~DriveFive();

	void	set_baud		( int speed );
	
	void	set_device_name	( const char* mDeviceName );
	void	open			( const char* mDeviceName=NULL );	

	int 	available		( );
	char	serialGetchar	( );
	bool 	send_command	( char* mFiveCommand  );
	bool 	read_response	(  );
	void 	clear			( );

	bool 	contains_NAK( );
	
	char	 m_response[2048];
		
private:
//	FILE*  			fd;
	int  			fd;	

	int				rx_bytes;		 // rx data bytes in buffer
 	char			rx_buffer[RX_BUFFER_SIZE];
 	char			tx_buffer[TX_BUFFER_SIZE];
	int 			tx_bytes;
	
	char	 m_port_name[256];
	bool	 connected;
	struct 	pollfd 	serial_poll;	
	
	bool 	 write_n	(uint8_t mbyte,...);
	bool 	 read_n		(uint8_t mbyte,uint8_t address,uint8_t cmd,...);
	
};

#endif
