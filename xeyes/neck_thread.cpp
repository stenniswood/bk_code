/************************************************************
	Neck thread - 
		neck_thread.cpp, roboclaw.cpp, serial_synchronous.cpp

	The thread looks for any change in neck_duty compared to last send.
		And sends the data over serial port to Roboclaw.
		
		Use void* neck_thread(void*) as thread function.
 ************************************************************/
#include <cstdio>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "x_eyes.hpp"
#include "serial.hpp"
#include "mouse.h"
#include "vision_memory.h"
#include "roboclaw.h"
#include "face_detect.hpp"
#include "face_recog.hpp"
#include "neck_thread.hpp"

const unsigned char address = 0x80;

#define DEV_NAME "/dev/ttyACM0"
float   	neck_duty = 0;
RoboClaw 	claw1( DEV_NAME, 1000 );		// 1ms timeout

void roboclaw_test()
{
	for (int i=0; i<5; i++)
		usleep( 1000000 );
	while (1) {
		printf("roboclaw1:");
		claw1.ForwardM1 ( address, 64 );
		for (int i=0; i<2; i++)
			usleep( 1000000 );
		printf("\n");
		claw1.BackwardM1( address, 64 );
		for (int i=0; i<2; i++)
			usleep( 1000000 );
		printf("\n");
	}
}

void setup_roboclaw_comms(void)
{
	claw1._cl_tx_bytes  = 50;
	claw1._cl_port 		= strdup("/dev/ttyACM0");
	claw1._cl_baud 		= 9600;
	claw1._cl_tx_detailed = 1;	
}

void update_neck_angle( int x, int width )
{
	//printf("Neck: x=%d;  %d < %d < %d\r", x, width/4, width, 3*width/4 );
	if (x < width/4)
	{
		printf("Neck CouterClockwise: x=%d;  %d < %d < %d\r", x, width/4, width, 3*width/4 );
		neck_duty = NECK_SPEED;
	}
	else if (x > 3*width/4)
	{
		printf("Neck Clockwise: x=%d;  %d < %d < %d\r", x, width/4, width, 3*width/4 );	
		neck_duty = -NECK_SPEED;
	}
	else 
		neck_duty = 0;	
}

void* neck_thread(void*)
{
	usleep				( 1000000 );
	printf("Neck_thread()  %s Starting \n", DEV_NAME);

	setup_roboclaw_comms( );
	claw1.general_setup ( );
	
	const int MAX_TRIES = 100; 
	bool result = false;
	int Tries = 0;
	do {
		result = claw1.open_block( );
		usleep(500000);		// 100ms
		Tries++;
	} while (( result==false ) && (Tries<MAX_TRIES));
	if (result==false) {
		printf("Warning Connection to Neck Roboclaw cannot be established.\n");
		printf("Neck Thread Terminated.\n");		
		return NULL;
	} else 
		printf("Neck_thread running\n");
	int last_speed = 0;	
	printf("roboclaw()\n");
	while (1)  
	{
		if (last_speed != neck_duty)
		{
			printf("Neck duty = %6.1f\n", neck_duty );
			if (neck_duty > 0)
				claw1.ForwardM1  ( address, neck_duty );
			else 
				claw1.BackwardM1 ( address, fabs(neck_duty) );
			last_speed = neck_duty;
		}
	}
}
