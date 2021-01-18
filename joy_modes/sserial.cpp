#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <math.h>
#include <poll.h>
#include <linux/serial.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>
#include "global.h"
#include "sserial.hpp"

SSerial::SSerial()
{
	initialize();
}

SSerial::SSerial(const char* mDeviceName)
{
	initialize();
	_cl_port = strdup( mDeviceName );
	open();
}

SSerial::~SSerial()
{
}

void SSerial::initialize()
{
	fd = 0;
	memset(rx_buffer, 0, RX_BUFFER_SIZE);
	memset(tx_buffer, 0, TX_BUFFER_SIZE);	
	
	rx_bytes	=0;		 // rx data bytes in buffer
	_cl_baud	= B9600;
	_cl_tx_bytes=0;
	connected	= false;
	_cl_port 	= NULL;
	
	serial_poll.fd 		= 0;
	serial_poll.revents = POLLIN;
	serial_poll.events  = POLLIN;

	memset(&time_stamp, 0, sizeof(struct tm) );
	printf("SSerial::initialize() \n");	
}

void SSerial::open(const char* mDeviceName)
{
	if (mDeviceName)
		_cl_port = strdup( mDeviceName );
		
	fd = ::open( _cl_port, O_RDWR | O_NONBLOCK  ); 		// | O_NONBLOCK
	if (fd < 0)
	{
		printf ("Unable to open serial device: %s - %s\n", _cl_port, strerror (errno));
		//fprintf (stderr, "Unable to open serial device: %s - %s\n", _cl_port, strerror (errno));
    	return ;
	}	

	connected = true;
	serial_poll.fd = fd;
	serial_poll.events  |= POLLIN;
	serial_poll.revents |= POLLIN;	
	//fsync(fd);
	printf("SSerial : opened port=%s\n", _cl_port );	
}

void SSerial::close()
{
	::close(fd);
}

int SSerial::available()
{
	int retval = poll( &serial_poll, 1, 5 );	// 20 ms timeout
	if (serial_poll.revents & POLLIN)
		return TRUE;
	return FALSE;
}

char SSerial::serialGetchar()
{
	if (!connected) {
		printf( "Getchar()  not connected\n");		//return -1;
		return 0;
	}
	
	//printf( "Getchar() "); fflush(stdout);
	char      buff[3];
	const long int LOOPS = 600000;
	int c;
	long int count=0;
	if (available()) {
//	do {	
			c = ::read( fd, (void*)&buff, 1 );
			if (c==0) printf("0");
			else if (c==-1) printf("?");
			else printf("%c", buff[0]);		
			return buff[0];
		}
		else {
			printf("no data\n");
			return 0;
		}
		
//			count++;
//	} while ((c==0) || ((c==-1) && (count<LOOPS)));

	//printf( "c=%d; count=%d; ", c, count); 	fflush(stdout);
	if (count>=LOOPS) {
		buff[0] = -1;
		printf("timeout!\n");
		return 0;
	}
//	else
//		printf(" %x_%c ", buff[0], buff[0]);
		

}


void SSerial::set_baud(int speed)
{
	_cl_baud = speed;
	printf("Baudrate = %d\n", speed);
	struct termios options;
	tcgetattr  ( fd, 	   		&options);
	cfsetispeed( &options, 		speed );
	cfsetospeed( &options, 		speed );
	tcsetattr  ( fd, TCSANOW, 	&options);
}

void SSerial::print_buffer(char* mBuffer, int mLength )
{
	printf("print_buffer:\n");
	for (int i=0; i<mLength; i++)
	{
		//printf(" %2x ", mBuffer[i] );
	}
	printf("\n");
}

#ifdef USES_DRIVEFIVE

// Not used for roboclaw - b/c it needs an ack after 1st byte. 
size_t	SSerial::write(char* mBuffer, int mLength )
{
	if (!connected) return 0;
	if (_cl_tx_bytes)
		printf("Buffer already has data! overwriting...\n" );

	print_buffer( mBuffer, mLength);
	
	memcpy(tx_buffer, mBuffer, mLength);
	_cl_tx_bytes = mLength;

//	for (int i=0; i<mLength; i++)
//		printf(" %2x ", mBuffer[i] );
		
	size_t retval = ::write(fd, tx_buffer, _cl_tx_bytes);	// if this blocks, we are okay.
	fsync(fd);
	return retval;
}
#endif


size_t	SSerial::write( char mBuffer )
{	
	//printf(" %2x ", mBuffer );
	// since all writes block, we are free to send immediately.	
	size_t retval = ::write(fd, &mBuffer, 1);
//	fsync(fd);	
	return retval;
}

void SSerial::flush_inbuff()
{
	int retval=0;
	while ( available() )
		retval = ::read(fd, rx_buffer, RX_BUFFER_SIZE);
}

struct serial_struct ss;
void SSerial::get_baud_divisor()
{	
	if (ioctl(fd, TIOCGSERIAL, &ss) != 0) {
		printf("TIOCGSERIAL failed\n");
		exit(1);
	}
	printf("Flags: %x\n",   ss.flags         );
	printf("Divisor: %d\n", ss.custom_divisor);
	printf("Base: %d\n",    ss.baud_base     );
	//printf("Speed: %d\n", ss.baud_base / ss.custom_divisor );
}

void SSerial::set_baud_divisor(int speed)
{
	// default baud was not found, so try to set a custom divisor
	ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
	ss.custom_divisor = (ss.baud_base + (speed/2)) / speed;
	int closest_speed = ss.baud_base / ss.custom_divisor;

	if (closest_speed < speed * 98 / 100 || closest_speed > speed * 102 / 100) {
		printf("Cannot set speed to %d, closest is %d\n", speed, closest_speed);
		exit(1);
	}
	printf("closest baud=%i, divisor= %i, Base= %i \n", closest_speed, 
			ss.custom_divisor, ss.baud_base );

	if (ioctl(fd, TIOCSSERIAL, &ss) < 0) {
		printf("TIOCSSERIAL failed\n");
		exit(1);
	}
}

/*void getline()
{
	char c=serialGetchar();
	// wait for the start of a line
    while(c != '$') {
    	if (serialDataAvail()) 
    		c=serialGetchar(); 
    }

    for(int i=0; i<256; i++) 
    {
        msg[i] = serialGetchar();
        if(msg[i] == '\r') {
            msg[i] = 0;
            return;
        }
    }
    //printf("Overflowed message limit"); 
} */