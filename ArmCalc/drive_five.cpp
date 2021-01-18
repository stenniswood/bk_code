#include <sys/time.h>
#include <stdlib.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>


#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include "global.h"
#include "drive_five.h"
#include <signal.h>


#define MAXRETRY 2
#define SetDWORDval(arg) (uint8_t)(((uint32_t)arg)>>24),(uint8_t)(((uint32_t)arg)>>16),(uint8_t)(((uint32_t)arg)>>8),(uint8_t)arg
#define SetWORDval(arg)  (uint8_t)(((uint16_t)arg)>>8),(uint8_t)arg

#define Debug 1

uint64_t GetTimeStamp2() 
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    uint64_t t = tv.tv_sec + tv.tv_usec*(uint64_t)1000000;
    //printf("GetTimeStamp()  %ld  %ld  = %ld\n", tv.tv_sec, tv.tv_usec, t );
    return t;
}
extern uint64_t 	   GetTimeStamp2();

/*	Constructor:
*/
DriveFive::DriveFive( )
{
}

// 	mPortName : for instance "/dev/ttyUSB0"	
DriveFive::DriveFive( const char* mDeviceName)
{
	strcpy (m_port_name, mDeviceName );	
}

DriveFive::DriveFive( const char* mDeviceName, uint32_t time_out )
{
	strcpy (m_port_name, mDeviceName );	
	timeout = time_out;
}

//
// Destructor
//
DriveFive::~DriveFive()
{
}

void DriveFive::set_device_name	( const char* mDeviceName )
{
	if (mDeviceName)
		strcpy (m_port_name, mDeviceName );
}


void DriveFive::set_baud(int speed)
{

	//_cl_baud = speed;
	printf("Baudrate = %d\n", speed);
	struct termios options;
	tcgetattr  ( fd, 	   		&options);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE; /* Mask the character size bits */
	options.c_cflag |= CS8;    /* Select 8 data bits */
	//options.c_cflag &= ~CNEW_RTSCTS;
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    options.c_oflag &= ~OPOST;
	
	cfsetispeed( &options, 		speed );
	cfsetospeed( &options, 		speed );
	tcsetattr  ( fd, TCSANOW, 	&options);
}

void DriveFive::open(const char* mDeviceName)
{
	if (mDeviceName)
		strcpy(m_port_name, mDeviceName );

	fd = ::open( m_port_name, O_RDWR  ); 		// | O_NONBLOCK		
//	fd = ::fopen( m_port_name, "r+"  ); 		// | O_NONBLOCK
	if (fd < 0)
	{
		printf("Unable to open serial device: %s - %s\n", m_port_name, strerror(errno) );
    	return ;
	}	

	connected = true;
	serial_poll.fd = fd;
	serial_poll.events  |= POLLIN;
	serial_poll.revents |= POLLIN;	
	printf("DriveFive : opened port=%s\n", m_port_name );	
}

char DriveFive::serialGetchar()
{
	if (!connected) {
		printf( "Getchar()  not connected\n");		//return -1;
		return 0;
	}

	int c;	
	char      buff[3];
	const long int LOOPS = 600000;
	long int count=0;
	
	if (available()) {
//			c = ::fread( fd, (void*)&buff, 1 );
			c = ::read( fd, (void*)&buff, 1 );
			printf("%c", buff[0]);		
			return buff[0];
	} else {
			printf("no data\n");
			return 0;
	}
		
	if (count>=LOOPS) {
		buff[0] = -1;
		printf("timeout!\n");
		return 0;
	}
}

// IMEDIAT3E RETURN!
int DriveFive::available()
{
	int retval = poll( &serial_poll, 1, 5 );	// 20 ms timeout
	if (serial_poll.revents & POLLIN)
		return TRUE;
	return FALSE;
}

void DriveFive::clear()
{
	while(available())
		serialGetchar();
}

/* NOTE: Instead of all this,
		Just make 1 function which sends whatever telegram you want!
		Don't need separate member functions for each command!
*/
bool DriveFive::send_command(char* mFiveCommand) 
{
	tx_bytes = strlen(mFiveCommand);	
	
	//if (Debug) printf("Sending %d bytes : %s\n", tx_bytes, mFiveCommand);
	
//	size_t retval = ::fwrite(tx_buffer, tx_bytes, 1, fd);	// if this blocks, we are okay.
	size_t retval = ::write( fd, mFiveCommand, tx_bytes );	// if this blocks, we are okay.	
	if (retval == -1)
		perror("Error - send_command() ");

	// Send the Deliminator!
	retval = ::write( fd, (char*)"\r", 1 );	// if this blocks, we are okay.	
	if (retval == -1)
		perror("Error - send_command() ");

	return retval>0;
}

bool DriveFive::contains_NAK( )
{
	bool isNAK = strstr(m_response, "NAK:") != NULL;	
	return isNAK;
}

bool DriveFive::replace_ctrl_characters( char* mStr, char mReplacement ) 
{
	size_t len = strlen(mStr);
	for (int i=0; i<mReplacement; i++)
	{
		if (
			(mStr[i] == '\r') | (mStr[i] == '\n') | (mStr[i] == '\b') | (mStr[i] == '\r') 
			)
			*mStr = mReplacement;			
	}
}

bool DriveFive::read_response() 
{
	memset(m_response, 0, sizeof(m_response) );
	
	// blah get from comm port, don't know how - interrupt driven or blocking?	
	while (!available()) { };
	char*  ptr      = m_response;
	char*  null_ptr = ptr;
	size_t timeout  = 100;	// ~10 seconds
	int    total_bytes_rxd = 0;
	do {
		rx_bytes = ::read(fd, ptr, 2048);	// if this blocks, we are okay.	
		if (rx_bytes==-1)
			perror("Error - read_response() ");

		total_bytes_rxd  += rx_bytes;
		null_ptr   = ptr + rx_bytes;
		*null_ptr  = 0;
		//printf("read_reponse (%d bytes; %d total) \n", rx_bytes, total_bytes_rxd );
		usleep(100000);
		ptr += rx_bytes;
		timeout--;
		if (timeout==0) {
			printf("read_response Timeout!");
			break;
		}
	} while (available());	// b/c it may not all come at once

	replace_ctrl_characters( m_response, ' ' );
//	printf("full response (%d bytes) = %s\n", rx_bytes, m_response);

	return rx_bytes>0;
}


void DriveFive::print_response() 
{
		printf("Drive Five Reponse: (%d bytes) = %s\n", rx_bytes, m_response);
}

char* get_error_string( uint32_t mStatus )
{
	static char Text[255];
	memset(Text, 0, 254);
	if (mStatus == 0x0000) 	{
		strcat(Text, "Normal Working");
		return Text;
	}
	if (mStatus & 0x0001) 	strcat(Text, "M1 OverCurrent Warning;");
	if (mStatus & 0x0002) 	strcat(Text, "M2 OverCurrent Warning;");
	if (mStatus & 0x0004) 	strcat(Text, "E-Stop;");
	if (mStatus & 0x0008) 	strcat(Text, "Temperature Error;");
	
	if (mStatus & 0x0010) 	strcat(Text, "Temperature2 Error;");
	if (mStatus & 0x0020) 	strcat(Text, "Main Battery High Error;");
	if (mStatus & 0x0040) 	strcat(Text, "Logic Battery High Error;");
	if (mStatus & 0x0080) 	strcat(Text, "Logic Battery Low Error;");
	
	if (mStatus & 0x0100) 	strcat(Text, "M1 Driver Fault;");
	if (mStatus & 0x0200) 	strcat(Text, "M2 Driver Fault;");
	if (mStatus & 0x0400) 	strcat(Text, "Main Battery High Warning;");
	if (mStatus & 0x0800) 	strcat(Text, "Main Battery Low Warning;");
	
	if (mStatus & 0x1000) 	strcat(Text, "Temperature Warning;");
	if (mStatus & 0x2000) 	strcat(Text, "Temperature2 Warning;");
	if (mStatus & 0x4000) 	strcat(Text, "M1 Home;");
	if (mStatus & 0x8000) 	strcat(Text, "M2 Home;");

	return Text;
}
