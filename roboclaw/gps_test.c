#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "gpio.h"
#include "mcp2515.h"
#include "pican_defines.h"
#include "gpio.h"
#include "motor_vector.h"
#include <wiringSerial.h>

// Wiring PI pin number (gpio pin 15)
#define CAN_INT_PIN 3
#define BUTTON1_INT_PIN 4
#define BUTTON2_INT_PIN 5
#define BUTTON3_INT_PIN 6

#define GPS_TX 15
#define GPS_RX 16

void Button1_isr()
{
	printf("Button 1 Pressed Interrupt - reg_dump()\n");
} 
void Button2_isr()
{
	printf("Button 2 Pressed Interrupt\n");	
	printf("--------------->>>>>>>\n");
} 
void Button3_isr()
{
	printf("Button 3 Pressed Interrupt\n");
}

// create a GPS data connection to GPRMC sentence type
int fd;

void init()
{
		
	// Ready-to-send, Receive buffer full
}

short MsgCounter   = 0;
byte  MsgIDCounter = 0;
int counter = 0;
char user_input[255];
char cmd[4];
byte instance;
byte mask;
byte value;

void print_args(int argc, char *argv[])
{
	printf("ARGS: ");
	for (int i=0; i< argc; i++)
	{
		printf(" %s ", argv[i] );
	}
	printf("\n");
}

float longitude = 0.0;
float latitude  = 0.0;
char msg[256];

void getline()
{
	char c=serialGetchar(fd); 
	printf ("%c", c);
    while(c != '$') {
    	if (serialDataAvail(fd)>0) {
    	c=serialGetchar(fd); printf ("%c", c);  }
    };    // wait for the start of a line
	
    for(int i=0; i<256; i++) 
    {
        msg[i] = serialGetchar(fd);
        if(msg[i] == '\r') {
            msg[i] = 0;
            return;
        }
    }
    printf("Overflowed message limit");
}

int gps_sample() 
{
    float time;
    char ns, ew;
    int lock;

    while(1) {        
        getline();
        
        // Check if it is a GPGGA msg (matches both locked and non-locked msg)
        if(sscanf(msg, "GPGGA,%f,%f,%c,%f,%c,%d", &time, &latitude, &ns, &longitude, &ew, &lock) >= 1) { 
            if(!lock) {
                longitude = 0.0;
                latitude = 0.0;        
                return 0;
            } else {
                if(ns == 'S') {    latitude  *= -1.0; }
                if(ew == 'W') {    longitude *= -1.0; }
                float degrees = trunc(latitude / 100.0f);
                float minutes = latitude - (degrees * 100.0f);
                latitude = degrees + minutes / 60.0f;    
                degrees = trunc(longitude / 100.0f * 0.01f);
                minutes = longitude - (degrees * 100.0f);
                longitude = degrees + minutes / 60.0f;
                return 1;
            }
        }
    }
}


/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.
*/
int main( int argc, char *argv[] )
{
	fd = serialOpen ("/dev/ttyUSB0", 38400);
	printf("fd=%x\n",fd);
	if (fd < 0)
	{
		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
	    	return 1;
	}
	int result = wiringPiSetup();
	printf("====================== GPS =========================\n");
	
	while (1)
	{
		while (serialDataAvail(fd))
		{
		printf (" -> %3d", serialGetchar(fd));
		fflush (stdout) ;
		}
	}	
	serialClose(fd);
}
