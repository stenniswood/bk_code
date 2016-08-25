#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <linux/serial.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#include "serial.hpp"
#include "bk_system_defs.h"


#define Debug 0 

class SerialInterface si;


SerialInterface::SerialInterface()
{
	Initialize();
}
SerialInterface::~SerialInterface()
{
	close( _fd );
}

void SerialInterface::Initialize()
{
	last_pos = accum_buff;

	// command line args
	_cl_baud 	= 0;
	_cl_port 	= NULL;
	_cl_divisor 	= 0;
	_cl_rx_dump 	= 0;
	_cl_rx_dump_ascii = 0;
	_cl_tx_detailed  = 0;
	_cl_stats 	 = 0;
	_cl_stop_on_error= 0;
	_cl_single_byte  = -1;
	_cl_another_byte = -1;
	_cl_rts_cts 	= 0;
	_cl_dump_err 	= 0;
	_cl_no_rx 	= 0;
	_cl_no_tx 	= 0;
	_cl_rx_delay 	= 0;
	_cl_tx_delay 	= 0;
	_cl_tx_bytes 	= 0;
	_cl_rs485_delay = -1;
	_cl_tx_time 	= 0;
	_cl_rx_time 	= 0;

	// Module variables
	_write_count_value = 0;
	_read_count_value = 0;
	_fd = -1;
	_write_data=0;
	_write_size=0;

	// keep our own counts for cases where the driver stats don't work
	_write_count = 0;
	_read_count  = 0;
	_error_count = 0;
}

void print_args(int argc, char *argv[])
{
	printf("ARGS: %d", argc);
	for (int i=0; i< argc; i++)
	{
		printf(" %s ", argv[i] );
	}
	printf("\n");
}

void SerialInterface::dump_data(unsigned char * b, int count) {
	printf("%i bytes: ", count);
	int i;
	for (i=0; i < count; i++) {
		printf("%02x ", b[i]);
	}
	printf("\n");
}

void SerialInterface::dump_data_ascii(unsigned char * b, int count)
{
	printf("%i bytes: ", count);
	int i;
	for (i=0; i < count; i++) {
		printf("%c ", b[i]);
	}
	printf("\n");
}

void SerialInterface::dump_serial_port_stats()
{
	struct serial_icounter_struct icount = { 0 };
	printf("%s: count for this session: rx=%i, tx=%i, rx err=%i\n", _cl_port, _read_count, _write_count, _error_count);

	int ret = ioctl(_fd, TIOCGICOUNT, &icount);
	if (ret != -1) {
		printf("%s: TIOCGICOUNT: ret=%i, rx=%i, tx=%i, frame = %i, overrun = %i, parity = %i, brk = %i, buf_overrun = %i\n",
				_cl_port, ret, icount.rx, icount.tx, icount.frame, icount.overrun, icount.parity, icount.brk,
				icount.buf_overrun);
	}
}


/* In other words set the baudrate for the serial /dev/tty* device driver.
	Raspberry Pi Linux (not the device)
  */
void SerialInterface::set_baud_divisor(int speed)
{
	// default baud was not found, so try to set a custom divisor
	struct serial_struct ss;
	if (ioctl(_fd, TIOCGSERIAL, &ss) != 0) {
		printf("TIOCGSERIAL failed\n");
		exit(1);
	}

	ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
	ss.custom_divisor = (ss.baud_base + (speed/2)) / speed;
	int closest_speed = ss.baud_base / ss.custom_divisor;

	if (closest_speed < speed * 98 / 100 || closest_speed > speed * 102 / 100) {
		printf("Cannot set speed to %d, closest is %d\n", speed, closest_speed);
		exit(1);
	}
	printf("closest baud = %i, base = %i, divisor = %i\n", closest_speed, ss.baud_base,
			ss.custom_divisor);

	if (ioctl(_fd, TIOCSSERIAL, &ss) < 0) {
		printf("TIOCSSERIAL failed\n");
		exit(1);
	}
}

// converts integer baud to Linux define
int SerialInterface::get_baud(int baud)
{
	switch (baud) {
	case 9600:
		return B9600;
	case 19200:
		return B19200;
	case 38400:
		return B38400;
	case 57600:
		return B57600;
	case 115200:
		return B115200;
	case 230400:
		return B230400;
	case 460800:
		return B460800;
	case 500000:
		return B500000;
	case 576000:
		return B576000;
	case 921600:
		return B921600;
	case 1000000:
		return B1000000;
	case 1152000:
		return B1152000;
	case 1500000:
		return B1500000;
	case 2000000:
		return B2000000;
	case 2500000:
		return B2500000;
	case 3000000:
		return B3000000;
	case 3500000:
		return B3500000;
	case 4000000:
		return B4000000;
	default: 
		return -1;
	}
}

void SerialInterface::process_options(int argc, char ** argv)
{
	print_args(argc,argv);
	for (;;) {
		int option_index = 0;
		static const char *short_options = "hb:p:d:R:TsSy:z:certq:l:a:w:o:i:";
		static const struct option long_options[] = {
			{"help", no_argument, 0, 0},
			{"baud", required_argument, 0, 'b'},
			{"port", required_argument, 0, 'p'},
			{"divisor", required_argument, 0, 'd'},
			{"rx_dump", required_argument, 0, 'R'},
			{"detailed_tx", no_argument, 0, 'T'},
			{"stats", no_argument, 0, 's'},
			{"stop-on-err", no_argument, 0, 'S'},
			{"single-byte", no_argument, 0, 'y'},
			{"second-byte", no_argument, 0, 'z'},
			{"rts-cts", no_argument, 0, 'c'},
			{"dump-err", no_argument, 0, 'e'},
			{"no-rx", no_argument, 0, 'r'},
			{"no-tx", no_argument, 0, 't'},
			{"rx-delay", required_argument, 0, 'l'},
			{"tx-delay", required_argument, 0, 'a'},
			{"tx-bytes", required_argument, 0, 'w'},
			{"rs485", required_argument, 0, 'q'},
			{"tx-time", required_argument, 0, 'o'},
			{"rx-time", required_argument, 0, 'i'},
			{0,0,0,0},
		};

		int c = getopt_long(argc, argv, short_options,
				long_options, &option_index);

		if (c == EOF) {
			break;
		}

		switch (c) {
		case 0:
			//display_help();
			break;
		case 'h':
			//display_help();
			break;
		case 'b':
			_cl_baud = atoi(optarg);
			break;
		case 'p':
			_cl_port = strdup(optarg);
			break;
		case 'd':
			_cl_divisor = atoi(optarg);
			break;
		case 'R':
			_cl_rx_dump = 1;
			//printf("Serial : (\"-r\") READ DUMP Requested\n");
			_cl_rx_dump_ascii = !strcmp(optarg, "ascii");		
			break;
		case 'T':
			_cl_tx_detailed = 1;
			break;
		case 's':
			_cl_stats = 1;
			break;
		case 'S':
			_cl_stop_on_error = 1;
			break;
		case 'y': {
			char * endptr;
			_cl_single_byte = strtol(optarg, &endptr, 0);
			break;
		}
		case 'z': {
			char * endptr;
			_cl_another_byte = strtol(optarg, &endptr, 0);
			break;
		}
		case 'c':
			_cl_rts_cts = 1;
			break;
		case 'e':
			_cl_dump_err = 1;
			break;
		case 'r':
			_cl_no_rx = 1;
			break;
		case 't':
			_cl_no_tx = 1;
			break;
		case 'l': {
			char *endptr;
			_cl_rx_delay = strtol(optarg, &endptr, 0);
			break;
		}
		case 'a': {
			char *endptr;
			_cl_tx_delay = strtol(optarg, &endptr, 0);
			break;
		}
		case 'w': {
			char *endptr;
			_cl_tx_bytes = strtol(optarg, &endptr, 0);
			break;
		}
		case 'q': {
			char *endptr;
			_cl_rs485_delay = strtol(optarg, &endptr, 0);
			break;
		}
		case 'o': {
			char *endptr;
			_cl_tx_time = strtol(optarg, &endptr, 0);
			break;
		}
		case 'i': {
			char *endptr;
			_cl_rx_time = strtol(optarg, &endptr, 0);
			break;
		}
		}
	}
}

/* Non blocking read from serial device  - accumulates data : 
*/
void SerialInterface::process_read_data()
{
	unsigned char rb[1024];
	memset(rb, 0, 1024);
	printf("process_read_data:");

	int c = ::read(_fd, (void*)&rb, sizeof(rb));		// Non blocking
	if (c > 0) {
		printf("read=%d:%2x", c, rb[0] );
		if (_cl_rx_dump) {			// User "-R" option.
			if (_cl_rx_dump_ascii)
				dump_data_ascii(rb, c);
			else
				dump_data(rb, c);
		}

		// verify read count is incrementing
		for (int i = 0; i < c; i++) {
			if (rb[i]=='\n') {
				*last_pos = 0;
				parse_ascii_data();
				last_pos = accum_buff;		// start over!
			} else {
				*last_pos = rb[i];			
				last_pos++;
			}
			_read_count_value++;		
		}
		_read_count += c;			// total bytes in accum_buff[]
	}
}

/* Incoming Received Text */
void SerialInterface::parse_ascii_data()
{	
	static int counter = 0;
	
	// Data is in accum_buff:
	int number=0;

	char* ptr = strstr( (char*)accum_buff, "left:" );
	
	// Look for Encoder readings:
	// Look for Speed reading:
	// Look for version:
}

int SerialInterface::my_write(char* mBuffer, int mSize)
{
	if ((current_write_size+mSize) > _write_size)
		mSize = _write_size-current_write_size;

	memcpy( &(_write_data[current_write_size]), mBuffer, mSize );
	current_write_size += mSize;
}

/* 
*/
unsigned char SerialInterface::read( )
{
	if (_read_count>0)
	{
		unsigned char one_byte = accum_buff[0];
		// Shift Data by one byte : 
		memcpy( &(accum_buff[0]), &(accum_buff[1]), _read_count-1 );
		_read_count--;
		return one_byte;
	}
	return 0;
}

int SerialInterface::my_write( char mByte )
{
	my_write( &mByte, 1 );
}
bool	SerialInterface::available	( )
{
	return (_read_count > 0) ? true:false;
}

/*
	
*/
void SerialInterface::process_write_data()
{
	ssize_t count = 0;
	int repeat = (_cl_tx_bytes == 0);		// User option "-w"	
	do
	{
		ssize_t i;
		if (current_write_size) {
			ssize_t c = write(_fd, _write_data, current_write_size);
/*			printf("\nprocess_write_data(%d)", current_write_size );
			for (int s=0; s<current_write_size; s++)
				printf("%2x ", _write_data[s] ); 	*/
			current_write_size=0;
//			printf("\n");
			
			if (c < 0) {
				dprintf("write failed (%d)\n", errno);
				c = 0;
			}
			count += c;

			if (c < _write_size) {
				_write_count_value -= _write_size - c;
				repeat = 0;
			}
		}
	} while (repeat);

	_write_count += count;

	if (current_write_size) {
		if (_cl_tx_detailed)
			printf("wrote %zd bytes\n", count);
	}
}

void SerialInterface::setup_serial_port( int baud )
{
	struct termios newtio;
	_fd = open(_cl_port, O_RDWR | O_NONBLOCK);

	if (_fd < 0) {
		printf("\n%s\n",_cl_port);
		perror("Error opening serial port ");
		free(_cl_port);
		//exit(1);
		_cl_port = NULL;
	}
	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

	/* man termios get more info on below settings */
	newtio.c_cflag = baud | CS8 | CLOCAL | CREAD;
	if (_cl_rts_cts) {
		newtio.c_cflag |= CRTSCTS;
	}

	newtio.c_iflag = 0;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;

	// block for up till 128 characters
	newtio.c_cc[VMIN] = 128;

	// 0.5 seconds read timeout
	newtio.c_cc[VTIME] = 5;

	/* now clean the modem line and activate the settings for the port */
	tcflush(_fd, TCIOFLUSH);
	tcsetattr(_fd,TCSANOW,&newtio);

	/* enable rs485 direction control */
	if (_cl_rs485_delay >= 0) {
		struct serial_rs485 rs485;
		if(ioctl(_fd, TIOCGRS485, &rs485) < 0) {
			printf("Error getting rs485 mode\n");
		} else {
			rs485.flags |= SER_RS485_ENABLED | SER_RS485_RTS_ON_SEND | SER_RS485_RTS_AFTER_SEND;
			rs485.delay_rts_after_send = _cl_rs485_delay;
			rs485.delay_rts_before_send = 0;
			if(ioctl(_fd, TIOCSRS485, &rs485) < 0) {
				printf("Error setting rs485 mode\n");
			}
		}
	}
}

int SerialInterface::diff_ms(const struct timespec *t1, const struct timespec *t2)
{
	struct timespec diff;

	diff.tv_sec = t1->tv_sec - t2->tv_sec;
	diff.tv_nsec = t1->tv_nsec - t2->tv_nsec;
	if (diff.tv_nsec < 0) {
		diff.tv_sec--;
		diff.tv_nsec += 1000000000;
	}
	return (diff.tv_sec * 1000 + diff.tv_nsec/1000000);
}

/*
	call process_options() before calling this to setup the serial port.
	This function will loop until someone turns off the user options:
			User option: "-r" and "-t"
	
	Incoming data is stored in accum_buff.  and processed
	To send data, 
	
	Basically it loops, calling :
		process_read_data()  	and
		process_write_data()	
*/
int SerialInterface::serial_main( )
{
	dprintf("Linux Loadcell serial app\n");
	if (!_cl_port) { 
		printf("serial_main() - ERROR: Port argument required\n");
	}

	// ESTABLISH BAUD RATE:
	int baud = B9600;
	if (_cl_baud)
		baud = get_baud(_cl_baud);
	if (baud<=0)
		baud = B38400;

	// SETUP PORT :
	setup_serial_port( baud );
	if (baud <= 0) {
		printf("NOTE: non standard baud rate, trying custom divisor\n");
		set_baud_divisor(_cl_baud);
	}

	// Specify with the "-p" option.  Should refer to "/dev/ttyUSB0" or other.
	if (_cl_port==NULL)
		return -1;

	// WRITE 1 or 2 BYTES (stored in _cl_single_byte & _cl_another_byte) :
	if (_cl_single_byte >= 0) {
		unsigned char data[2];
		data[0] = (unsigned char)_cl_single_byte;
		if (_cl_another_byte < 0) {
			write(_fd, &data, 1);
		} else {
			data[1] = _cl_another_byte;
			write(_fd, &data, 2);
		}
		return 0;
	}

	// ALLOCATE MEMORY FOR THE WRITE BUFFER SIZE :
	_write_size = (_cl_tx_bytes == 0) ? 1024 : _cl_tx_bytes;
	_write_data = (unsigned char*)malloc(_write_size);
	if (_write_data == NULL) {
		printf("ERROR: Memory allocation failed\n");
	}

	// ESTABLISH THE POLL Flags based on user, "-r" and "-t" options
	struct pollfd serial_poll;
	serial_poll.fd = _fd;
	if (!_cl_no_rx) {
		serial_poll.events |= POLLIN;
	} else {
		serial_poll.events &= ~POLLIN;
	}
	if (!_cl_no_tx) {
		serial_poll.events |= POLLOUT;
	} else {
		serial_poll.events &= ~POLLOUT;
	}

	// SETUP TIMESTAMPS:
	struct timespec start_time, last_stat;
	//struct timespec last_read  = { .tv_sec = 0, .tv_nsec = 0 };
	//struct timespec last_write = { .tv_sec = 0, .tv_nsec = 0 };
	struct timespec last_read  = { 0, 0 };
	struct timespec last_write = { 0, 0 };
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	last_stat = start_time;

	// while either User option: "-r" and "-t"
	printf("Entering serial_main while () loop\n");
	while (!(_cl_no_rx && _cl_no_tx)) 
	{	
		//printf("Inside serial_main while () loop\n");
		int retval = poll(&serial_poll, 1, 1000);
		if (retval == -1) {
			perror("poll()");
		} else if (retval) {
			// Received Data : 
			if (serial_poll.revents & POLLIN) { /* Recieve */
				printf("Poll received\n");
				// 
				if (_cl_rx_delay) { 
					// only read if it has been rx-delay ms
					// since the last read
					struct timespec current;
					clock_gettime(CLOCK_MONOTONIC, &current);
					if (diff_ms(&current, &last_read) > _cl_rx_delay) {
						process_read_data();
						last_read = current;
					}
				} else {
					process_read_data();
				}
			}

			// Transmit Buffer Empty : 
			if (serial_poll.revents & POLLOUT) {

				if (_cl_tx_delay) {
					// only write if it has been tx-delay ms
					// since the last write
					struct timespec current;
					clock_gettime(CLOCK_MONOTONIC, &current);
					if (diff_ms(&current, &last_write) > _cl_tx_delay) {
						process_write_data();
						last_write = current;
					}
				} else {
					//printf(".");
					process_write_data();
				}
			}
		} else if (!(_cl_no_tx && _write_count != 0 && _write_count == _read_count)) {
			// No data. We report this unless we are no longer
			// transmitting and the receive count equals the
			// transmit count, suggesting a loopback test that has
			// finished.
			printf("No data within one second. %s\n", _cl_port);
		}

		if (_cl_stats || _cl_tx_time || _cl_rx_time) {
			struct timespec current;
			clock_gettime(CLOCK_MONOTONIC, &current);

			if (_cl_stats) {
				if (current.tv_sec - last_stat.tv_sec > 5) {
					dump_serial_port_stats();
					last_stat = current;
				}
			}
			if (_cl_tx_time) {
				if (current.tv_sec - start_time.tv_sec >= _cl_tx_time) {
					_cl_tx_time = 0;
					_cl_no_tx = 1;
					serial_poll.events &= ~POLLOUT;
					printf("Stopped transmitting.\n");
				}
			}
			if (_cl_rx_time) {
				if (current.tv_sec - start_time.tv_sec >= _cl_rx_time) {
					_cl_rx_time = 0;
					_cl_no_rx = 1;
					serial_poll.events &= ~POLLIN;
					printf("Stopped receiving.\n");
				}
			}
		}
	}

	tcdrain(_fd);
	dump_serial_port_stats();
	tcflush(_fd, TCIOFLUSH);
	free(_cl_port);

	printf("serial_main()  Thread Terminated.\n");
	int result = abs(_write_count - _read_count) + _error_count;
	return (result > 255) ? 255 : result;
}


/*void display_help()
{
	printf("Usage: linux-serial-test [OPTION]\n"
			"\n"
			"  -h, --help\n"
			"  -b, --baud        Baud rate, 115200, etc (115200 is default)\n"
			"  -p, --port        Port (/dev/ttyS0, etc) (must be specified)\n"
			"  -d, --divisor     UART Baud rate divisor (can be used to set custom baud rates)\n"
			"  -R, --rx_dump     Dump Rx data (ascii, raw)\n"
			"  -T, --detailed_tx Detailed Tx data\n"
			"  -s, --stats       Dump serial port stats every 5s\n"
			"  -S, --stop-on-err Stop program if we encounter an error\n"
			"  -y, --single-byte Send specified byte to the serial port\n"
			"  -z, --second-byte Send another specified byte to the serial port\n"
			"  -c, --rts-cts     Enable RTS/CTS flow control\n"
			"  -e, --dump-err    Display errors\n"
			"  -r, --no-rx       Don't receive data (can be used to test flow control)\n"
			"                    when serial driver buffer is full\n"
			"  -t, --no-tx       Don't transmit data\n"
			"  -l, --rx-delay    Delay between reading data (ms) (can be used to test flow control)\n"
			"  -a, --tx-delay    Delay between writing data (ms)\n"
			"  -w, --tx-bytes    Number of bytes for each write (default is to repeatedly write 1024 bytes\n"
			"                    until no more are accepted)\n"
			"  -q, --rs485       Enable RS485 direction control on port, and set delay\n"
			"                    from when TX is finished and RS485 driver enable is\n"
			"                    de-asserted. Delay is specified in bit times.\n"
			"  -o, --tx-time     Number of seconds to transmit for (defaults to 0, meaning no limit)\n"
			"  -i, --rx-time     Number of seconds to receive for (defaults to 0, meaning no limit)\n"
			"\n"
	      );
	exit(0);
}*/
