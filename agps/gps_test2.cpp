#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <poll.h>
#include <linux/serial.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>

int    	fd;
int    	valid_location_lock;
struct 	pollfd 	serial_poll;
struct tm time_stamp;
float 	longitude = 0.0;
float 	latitude  = 0.0;
char 	msg[256];

// Wiring PI pin number (gpio pin 15)
#define CAN_INT_PIN 3
#define BUTTON1_INT_PIN 4
#define BUTTON2_INT_PIN 5
#define BUTTON3_INT_PIN 6

#define GPS_TX 15
#define GPS_RX 16


bool serialDataAvail()
{
	int retval = poll( &serial_poll, 1, 20 );
	if (serial_poll.revents & POLLIN)
	{
		return true;
	}
	return false;
}

char serialGetchar()
{
	char buff[3];
	int c;
	do {
		c = ::read( fd, (void*)&buff, 1 );
	} while (c==0);
	return buff[0];
}

// create a GPS data connection to GPRMC sentence type
//NMEA gps(GPRMC);

void get_baud_divisor()
{
	struct serial_struct ss;
	if (ioctl(fd, TIOCGSERIAL, &ss) != 0) {
		printf("TIOCGSERIAL failed\n");
		exit(1);
	}
	printf("Flags: %x\n",   ss.flags);
	printf("Divisor: %d\n", ss.custom_divisor);
	printf("Base: %d\n",  ss.baud_base );
	//printf("Speed: %d\n", ss.baud_base / ss.custom_divisor );
}

void set_baud_divisor(int speed)
{
	// default baud was not found, so try to set a custom divisor
	struct serial_struct ss;
	if (ioctl(fd, TIOCGSERIAL, &ss) != 0) {
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

	if (ioctl(fd, TIOCSSERIAL, &ss) < 0) {
		printf("TIOCSSERIAL failed\n");
		exit(1);
	}
}

void print_args(int argc, char *argv[])
{
	printf("ARGS: ");
	for (int i=0; i< argc; i++)
	{
		printf(" %s ", argv[i] );
	}
	printf("\n");
}


void getline()
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
}

int gps_sample() 
{

    float time,hours,min,sec;
    char  ns, ew;

	char  latitude_str [40];
	char  longitude_str[40];
	char  degree_str[5];
    getline();
        
	// Check if it is a GPGGA msg (matches both locked and non-locked msg)
	if(sscanf(msg, "GPGGA,%f,%f,%c,%f,%c,%d", &time, &latitude, &ns, &longitude, &ew, &valid_location_lock) >= 1)
	{
            if(!valid_location_lock) {
                longitude = 0.0;
                latitude = 0.0;        
                return 0;
            } else {
            	hours = trunc(time / 10000.);
            	min   = trunc((time - hours*10000)/100.);
				sec   = ((time - hours*10000 - min*100.));
            	time_stamp.tm_hour = hours;
            	time_stamp.tm_min  = min;
            	time_stamp.tm_sec  = sec;
            	
                if(ns == 'S') {    latitude  *= -1.0; }
                if(ew == 'W') {    longitude *= -1.0; }
                float degrees = trunc(latitude / 100.0f);
                float minutes = latitude - (degrees * 100.0f);
                latitude = degrees + minutes / 60.0f;    
                degrees = trunc(longitude / 100.0f);
                minutes = longitude - (degrees * 100.0f);
                longitude = degrees + minutes / 60.0f;
                return 1;
            }
	}
	valid_location_lock = 0;
	char* ptr = strchr(msg, ',');
	*ptr = 0;
	//printf(" %s\n",msg);
	return 0;
}

#include "../core/misc/fuzzy.hpp"
#include "../core/misc/fuzzy_signal.hpp"

int main( int argc, char *argv[] )
{
	//test_fuzzy();
	test_fuzzy_signal();
	
	char 			write_buff[255];
	char 			read_buff [255];
	char	dev[25] = "/dev/ttyUSB0";
//	fd = open( "/dev/ttyAMA0", O_RDWR );	// | O_NONBLOCK
	fd = open( "/dev/ttyUSB0", O_RDWR ); 
	printf("%s  fd=%x\n",dev, fd);
	if (fd < 0)
	{
		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
    	return 0;
	}
	get_baud_divisor();
	printf("get baud divisor. \n");
	serial_poll.fd = fd;
	serial_poll.events |= POLLIN;
	set_baud_divisor( B38400 );
	printf("baud divisor done\n");
	time_t t;
	time(&t);
	time_stamp = *(localtime(&t));
	printf("====================== GPS =========================\n");
	printf("==== WARNING: This does not set the baud rate.\n");
	printf("==== USE: stty -F %s 9600 \n", dev );

	write_buff[0] = 0x55;
	int BytesExpected=30;
	while (1)
	{		
		int retval = poll( &serial_poll, 1, 20 );
		if (serial_poll.revents & POLLIN)
		{			
			gps_sample();			
			if (valid_location_lock) {
				printf("%s ", asctime(&time_stamp));
				printf("Lat,Long:  %f,%f\n", latitude, longitude );
			}
		}
	}	
	close(fd);
}

