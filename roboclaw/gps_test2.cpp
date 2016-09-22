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
#include <sys/time.h>
#include "roboclaw.h"

struct tm time_stamp;
//char	  dev[25] = "/dev/ttyUSB0";
char	  dev[25] = "/dev/ttyACM0";
char	  dev2[25] = "/dev/ttyACM1";

RoboClaw  claw(dev,1000);
RoboClaw  claw2(dev2,1000);

uint64_t GetTimeStamp2() 
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    uint64_t t = tv.tv_sec*(uint64_t)1000000 + tv.tv_usec;
    //printf("GetTimeStamp()  %ld  %ld  = %ld\n", tv.tv_sec, tv.tv_usec, t );
    return t;
}

void print_args(int argc, char *argv[])
{
	printf("ARGS %d: ",argc);
	for (int i=0; i< argc; i++)
	{
		printf(" %s ", argv[i] );
	}
	printf("\n");
}

uint8_t address = 0x80;

int main( int argc, char *argv[] )
{
	print_args(argc,argv);
	
	bool stop=false;
	if (argc>1) {

		if (strcmp(argv[1], "stop")==0) {
			stop = true;
			printf("Stop!");
		}
	}
	claw.open			(dev);
	claw.set_baud		( B38400 );	
	claw.SetMainVoltages( address, 100, 250 );	
	claw2.open			(dev2);
	claw2.set_baud		( B38400 );	
	claw2.SetMainVoltages( address, 100, 250 );
	
	time_t t;
	time(&t);
	time_stamp = *(localtime(&t));
	printf("====================== GPS =======================HELLO==\n");
	printf("==== WARNING: This sets the baud rate.\n");
	//printf("==== USE: stty -F %s 9600 \n", dev );

	char text[80];
	bool result = claw.ReadVersion( address, text );
	printf("\ntext=%s\n", text); 
	result = claw2.ReadVersion( address, text );	
	printf("\ntext=%s\n", text); 
//	exit(1);
	 
	if (stop) {
		claw.ForwardM1(address, 0);
		claw.ForwardM2(address, 0);		
		claw2.ForwardM1(address, 0);
		claw2.ForwardM2(address, 0);		

		exit(1);
	}
	while (1)
	{				
		printf("\nForward:\n");
		claw.ForwardM1(address, 127);
		claw.ForwardM2(address, 127);		

		claw2.ForwardM1(address, 127);
		claw2.ForwardM2(address, 127);		
		usleep(2000000);	// 0.1 sec
		usleep(1000000);	// 0.1 sec
		
		printf("\nBackward:\n");		
		claw.BackwardM1(address, 127);
		claw.BackwardM2(address, 127);

		claw2.BackwardM1(address, 127);
		claw2.BackwardM2(address, 127);
		usleep(2000000);	// 0.1 sec		
		usleep(1000000);	// 0.1 sec		
	
	}	
	claw.close();
}
