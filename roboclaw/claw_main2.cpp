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
#include <pthread.h>
#include "global.h"
#include "roboclaw.h"
#include "robot_claw.hpp"
#include "leg_claw.hpp"
#include "serial_loadcell.hpp"


struct tm time_stamp;

char	  dev1[25] = "/dev/ttyACM0";
char	  dev2[25] = "/dev/ttyACM1";
char	  dev3[25] = "/dev/ttyACM2";
char	  dev4[25] = "/dev/ttyACM3";
//Leg		LeftLeg (dev1, dev2);
//Leg		RightLeg(dev3, dev4);
RobotAct robot;

pthread_t serial_leftfoot_thread_id;
pthread_t serial_rightfoot_thread_id;

LoadCell_SerialInterface left_foot;
LoadCell_SerialInterface right_foot;

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

void* serial_interface(void* mParam)				// serial port used for arduino connections & GPS.
{
	LoadCell_SerialInterface* mFoot = (LoadCell_SerialInterface*) mParam;
	static char  app_name[12];
	static char  read_r[4];
	static char  no_write_w[4];
	static char  device_p[4];

	static char  read_[20];
	static char  no_write_[20];
	static char  device_[20];

	sprintf(app_name,   "./avisual");
	sprintf(read_r,     "-R");
	sprintf(no_write_w, "-w");
	sprintf(device_p,   "-p");

	sprintf(read_,     "ascii");
	sprintf(no_write_, "0"    );

	//printf("mFoot=%p;  left_foot=%p\n", mFoot, &left_foot);
	if (mFoot == &left_foot)	{
		sprintf(device_,   "/dev/ttyACM0");
		mFoot->_cl_rx_dump = 1;
		mFoot->_cl_rx_dump_ascii = 1;		
		mFoot->_cl_port = strdup("/dev/ttyACM0");
		mFoot->_cl_tx_bytes = 0;
		mFoot->left_foot = false;
	} else {
		mFoot->_cl_port = strdup("/dev/ttyACM1");	
		mFoot->_cl_rx_dump = 1;
		mFoot->_cl_rx_dump_ascii = 1;		
		mFoot->_cl_tx_bytes = 0;
		mFoot->left_foot = true;		
		//sprintf(device_,   "/dev/ttyACM1");
	}
	char * argv[] = { app_name, read_r, read_, no_write_w, no_write_, device_p, device_ }; 
	
	mFoot->serial_loadcell_main( 7, argv );	
	return NULL;
}

void create_threads()
{
	int iret1 = pthread_create( &serial_leftfoot_thread_id, NULL, serial_interface, (void*) &left_foot);
	if (iret1)
	{
		fprintf(stderr,"Error - Could not create right_foot thread. return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}

int main( int argc, char *argv[] )
{
	print_args(argc,argv);
	bool stop=false;
	if (argc>1) {

		if (strcmp(argv[1], "stop")==0) {
			stop = true;
			printf("Stop!");
		}
		if (strcmp(argv[1], "status")==0) {
			robot.left_leg.read_status();
			robot.right_leg.read_status();
			printf("Read Status!");
			exit(1);
		}
		if (strcmp(argv[1], "version")==0) {
			printf("Reading versions:\n");
			robot.left_leg.read_versions();
			//robot.right_leg.read_versions();
			printf("Read Status!");
			exit(1);			
		}
		if (strcmp(argv[1], "battery")==0) {
			printf("Reading Battery:\n");
			robot.left_leg.GetMainVoltage();
			robot.left_leg.GetLogicVoltage();
			//robot.right_leg.read_versions();
			printf("Read Status!");
			exit(1);			
		}
	}	
	time_t t;
	time(&t);
	time_stamp = *(localtime(&t));
	init_duties();
	printf("====================== CLAW =========================\n");
	
	if (stop) {
		robot.stop_all_motors();
		exit(1);
	}

	robot.left_leg.read_currents();	
	printf("\nLEGS - SIT:\n");
	robot.Sit();
	usleep(200000);	
	
	printf("\nLEGS - STAND:\n");
	robot.Stand_up();
	
	printf("\nStopping motors...\n");
	robot.stop_all_motors();
	usleep(100000);	
}
