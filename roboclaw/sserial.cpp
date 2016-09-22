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
	_cl_port = NULL;
	_cl_baud = B9600;
	_cl_tx_bytes = 0;	
	_cl_tx_detailed = 0;
	memset(rx_buffer, 0, RX_BUFFER_SIZE);
	memset(tx_buffer, 0, TX_BUFFER_SIZE);	
}

SSerial::SSerial(const char* mDeviceName)
{
	_cl_port = NULL;
	_cl_baud = B9600;
	_cl_tx_bytes = 50;
	_cl_tx_detailed = 0;
	_cl_port = strdup( mDeviceName);
}
SSerial::~SSerial()
{

}

void SSerial::open(const char* mDeviceName)
{
	if (mDeviceName)
		_cl_port = strdup( mDeviceName );
	fd = ::open( _cl_port, O_RDWR ); 		
	if (fd < 0)
	{
		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
    	return ;
	}	
	serial_poll.fd = fd;
	serial_poll.events |= POLLIN;
}
void SSerial::close()
{
	::close(fd);
}

int SSerial::available()
{
	int retval = poll( &serial_poll, 1, 20 );
	if (serial_poll.revents & POLLIN)
	{
		return TRUE;
	}
	return FALSE;
}

char SSerial::serialGetchar()
{
	char buff[3];
	int c;
	do {
		c = ::read( fd, (void*)&buff, 1 );
	} while (c==0);
	return buff[0];
}

void SSerial::set_baud(int speed)
{
	_cl_baud = speed;
	//printf("Baudrate = %d\n", speed);
	struct termios options;
	tcgetattr  ( fd, 	   		&options);
	cfsetispeed( &options, 		speed );
	cfsetospeed( &options, 		speed );
	tcsetattr  ( fd, TCSANOW, 	&options);
}

size_t	SSerial::read( )
{
//	return ::read(fd, rx_buffer, RX_BUFFER_SIZE);
	return serialGetchar();
}

size_t	SSerial::write(char* mBuffer, int mLength )
{
	memcpy(tx_buffer, mBuffer, mLength);
	_cl_tx_bytes = mLength;

	printf("\nLen=%d: ", mLength );
	for (int i=0; i<mLength; i++)
		printf(" %2x ", mBuffer[i] );
		
	return ::write(fd, tx_buffer, _cl_tx_bytes);
}

size_t	SSerial::write( char mBuffer )
{
	printf(" %2x ", mBuffer);
	return ::write(fd, &mBuffer, 1);
}


void SSerial::flush()
{

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