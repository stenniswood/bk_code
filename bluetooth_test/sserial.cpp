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
	new_full_extraction = false;
	//contains_newline = false;
	fd = 0;
	memset(rx_buffer, 0, RX_BUFFER_SIZE);
	memset(tx_buffer, 0, TX_BUFFER_SIZE);	
	excess_data_found = false;
	
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
	restart_buffer();
	strcpy(devType, "unknown");
	strcpy(devName, "unknown");
	
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

void SSerial::restart_buffer()
{	
	strcpy(rx_line, " ");
	strcpy(rx_buffer, " ");
	
	rx_buff_index = 0;
	new_full_extraction = false;
	rx_bytes = 0;
}

void SSerial::acknowledge_rxd ()		// will set rx_bytes back to zero.
{
	new_full_extraction = false;
	rx_bytes = 0;
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

char compute_xor_xsum( const char* mStr )
{
	char xsum = 0;
	int  len = strlen(mStr);
	for (int i=0; i<len; i++)
	{
		xsum ^= mStr[i];
	}
	return xsum;
}

/* Return:   1 => success
			 0 => failure
*/
int SSerial::extract_line( char* rl ) 
{
	// Extract XSUM:
	char xsum = *(rl-1);
	*(rl-1) = 0;			// Make the XSUM the new end of string marker.
	*rl     = 0;
	*(rl+1) = 0;
	char computed_xsum = compute_xor_xsum( rx_buffer );
	if (computed_xsum != xsum) {
		printf("Bad Checksum! Expected %2x %c received %2x %c;  ", computed_xsum, computed_xsum, xsum, xsum  );
		//printf("rx_buffer=%p;  rl at %p; ", rx_buffer, rl );
		printf("size=%d; ", (rl-rx_buffer) );
		printf("Received Data[%s]\n", rx_buffer );
		// restart_buffer();

		// SHIFT ANY REMAINING DATA : 
		int extracted_length = ((rl+2) - rx_buffer);
		int len = sizeof(rx_buffer) - ( extracted_length );
		memcpy ( rx_buffer, (rl+2), len );	
		rx_buff_index = ((rl-1) - rx_buffer);
		new_full_extraction = false;			
		return 0;
	}


	strcpy (rx_line, rx_buffer);
	rx_line_bytes = strlen(rx_line);

printf("Checksum Validated! Line: %s\n", rx_line);

	// Shift buffer over extracted region:
	int extracted_length = ((rl+2) - rx_buffer);
	int len = sizeof(rx_buffer) - ( extracted_length );
	memcpy ( rx_buffer, (rl+2), len );	
	rx_buff_index -= extracted_length;	
	new_full_extraction = true;	
	return 1;
}

void dump_buffer(char* mBuff, int mLen)
{
	printf("Buffer len=%d\n", mLen);
	for (int i=0; i<mLen; i++)
	{
		printf("%2x ", mBuff[i]);
	}
	printf("\n");
	for (int i=0; i<mLen; i++)
	{
		if ((mBuff[i] != '\r') && (mBuff[i] != '\n'))
			printf(" %c ", mBuff[i] );
	}
	printf("\r\n");
}

/* This will read any data that is readily available.  
   It won't wait on more data.
*/
int SSerial::readLine1(  )
{
	if (rx_buff_index >= RX_BUFFER_SIZE)
	{ 
		printf("ERROR: rx_buff_index out of Bounds!!\n");  
		restart_buffer();
		return 0;
	}
	int   no_data_count = 0;
	char* ptr = rx_buffer+rx_buff_index;
	char* nl  = NULL;
	do {
		rx_bytes = ::read(fd, ptr, 80);	// we use NON BLOCKING!
		//dump_buffer(ptr, rx_bytes+1);
		if (rx_bytes<0)  {
			no_data_count++;
			if (no_data_count>10)  { 
				perror("::read() ");
				break;
			} else {
				//printf("sleep/retry\n"); 
				usleep(100000); continue;
			}
		}
		
		rx_buff_index += rx_bytes;
		ptr += rx_bytes;
		if (rx_bytes==0) {
			no_data_count++;
			usleep(1000);
			continue;		
		}
		char* rl = 	strchr( rx_buffer, '\r');
		//printf("rl=%p\n", rl);
		
		int rl_bytes = (rl - rx_buffer);
		if ((rl) && (rl_bytes <= rx_buff_index))
		{
			//printf("calling extract_line...\n");
			int success = extract_line( rl );
			return success;		// valid line obtained in rx_line[]
		}
	} while (no_data_count < 10);
	return 0;				// No complete line obtained!
}

int SSerial::readLine( )
{
	char* ptr = rx_buffer+rx_buff_index;
	char* nl = NULL;
	bool contains_newline = false;

	// Clear Buffer:
	memset(rx_buffer, 0, sizeof(rx_buffer) );
	int total_bytes_rxd = 0;
	
	do {		
		rx_bytes = ::read(fd, ptr, 80);	// if this blocks, we are okay.	
		rx_buff_index += rx_bytes;
		total_bytes_rxd += rx_bytes;
		//printf("%d, ", total_bytes_rxd);
			
		if (rx_bytes==0) {
			usleep(1000);
			continue;		
		}
		if (rx_bytes==-1) {
			//perror("Error - read_response() ");
			usleep(1000);
			continue;
		}
		ptr += rx_bytes;

		char* rl = strchr( rx_buffer, '\r');		
		char* nl = strchr( rx_buffer, '\n');
		contains_newline = (nl != NULL);
		if (rl)  {
			*rl = 0;
			strcpy (rx_line, rx_buffer );		// Extract Line.

			// Shift remaining buffer up:
//			memcpy (rx_buffer, rl+2, (sizeof(rx_buffer) - rl) );
		}

		
/*		if (contains_newline)
		{						
			size_t len = strlen(rx_buffer);
			if (rx_bytes>len)
			{
				excess_data_found = true;
				memcpy( carry_over, nl+1, (rx_bytes-len) );
				rx_buffer[len] = 0;	// Mark null
			}
		} */
		if (nl)  *nl = '|';
		
		//printf( "readLine: %d  %s\n", contains_newline, rx_buffer );
		usleep(1000);
	} while ( (contains_newline==0) );	// b/c it may not all come at once
	
	return total_bytes_rxd;
	//printf("exiting readLine\n");
}

// does it in place
static void remove_leading_whitespace(char* mStr )
{
	bool white_space = ((mStr[0] == ' ') || (mStr[0] == '\t'));
	while (white_space)
	{
		strcpy( &(mStr[0]),  &(mStr[1]) );				
		white_space = ((mStr[0] == ' ') || (mStr[0] == '\t'));
	}
}

char*	SSerial::get_device_type ()
{
	writeStr("device type\r\n");
	while (available()==false) {};
	int full_line = readLine1();
	//printf("get_dev_types: response= %s \n", rx_line);

	char* delim = strchr(rx_line, ':');
	if (delim) strcpy( devType, delim+1 );
	remove_leading_whitespace( devType );
	return devType;
}

char*	SSerial::get_device_name ()
{
	writeStr("get name\r\n");
	while (available()==false) {};
	int full_line = readLine1();
	//printf("get_dev_name: response= %s \n", rx_line);

	char* delim = strchr(rx_line, ':');
	if (delim) strcpy( devName, delim+1 );
	remove_leading_whitespace( devName );
	return devName;
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
		printf(" %2x ", mBuffer[i] );
	}
	printf("\n");
}

//#ifdef USES_DRIVEFIVE

// Not used for roboclaw - b/c it needs an ack after 1st byte. 
size_t	SSerial::writeStr(char* mBuffer )
{
	int Length = strlen (mBuffer);
	
	if (!connected) return 0;
	//if (_cl_tx_bytes)
	//	printf("Buffer already has data! overwriting...\n" );

	//printf("writeStr() %s ", mBuffer );
	//print_buffer( mBuffer, Length);
	
	memcpy(tx_buffer, mBuffer, Length);
	_cl_tx_bytes = Length;

	size_t retval = ::write(fd, tx_buffer, _cl_tx_bytes);	// if this blocks, we are okay.
	fsync(fd);
	return retval;
}
//#endif


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