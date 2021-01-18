#ifndef _PanTilt_h
#define _PanTilt_h

#include <stdarg.h>
#include <inttypes.h>
#include <cstddef>
#include <poll.h>


#define TX_BUFFER_SIZE 100
#define RX_BUFFER_SIZE 100


/******************************************************************************
* Definitions
******************************************************************************/

#define _SS_VERSION 16

class PanTilt
{
	uint16_t crc;
	uint32_t timeout;

public:
	// public methods
	PanTilt ( );
	PanTilt ( const char* mDeviceName );
	PanTilt ( const char* mDeviceName, uint32_t time_out );
	~PanTilt( );

	void	set_baud		( int speed );	
	void	set_device_name	( const char* mDeviceName );
	void	open			( const char* mDeviceName=NULL );	

	int 	available		( );
	char	serialGetchar	( );
	bool 	send_command	( char* mFiveCommand  );
	bool 	replace_ctrl_characters( char* mStr, char mReplacement );
	bool 	read_response	(  );
	void 	print_response  ( );
	
	void 	clear			( );
	bool 	contains_NAK	( );
	
	char	 m_response[2048];

private : 
	int  			fd;

	int				rx_bytes;		 // rx data bytes in buffer
 	char			rx_buffer[RX_BUFFER_SIZE];
 	char			tx_buffer[TX_BUFFER_SIZE];
	int 			tx_bytes;
	
	char	 		m_port_name[256];
	bool	 		connected;
	struct 	pollfd 	serial_poll;	

	bool 	 		write_n		(uint8_t mbyte,...);
	bool 	 		read_n		(uint8_t mbyte,uint8_t address,uint8_t cmd,...);
};

#endif
