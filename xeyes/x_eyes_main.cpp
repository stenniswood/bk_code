#include <cstdio>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "x_eyes.hpp"
#include "serial.hpp"
#include "mouse.h"
#include "vision_memory.h"
#include "roboclaw.h"
#include "face_detect.hpp"


void* serial_setup(void*)				// serial port used for arduino connections & GPS.
{
	static char  app_name  [12];
	static char  read_r    [4];
	static char  no_write_w[4];
	static char  device_p  [4];
	static char  read_     [20];
	static char  no_write_ [20];
	static char  device_   [20];

	sprintf(app_name,   "./xeyes");
	sprintf(read_r,     "-R");
	sprintf(no_write_w, "-w");
	sprintf(device_p,   "-p");

	sprintf(read_,     "ascii");
	sprintf(no_write_, "0");

	sprintf(device_,   "/dev/ttyUSB0");
	si._cl_rx_dump = 1;
	si._cl_rx_dump_ascii = 1;		
	si._cl_port = strdup("/dev/ttyUSB0");
	si._cl_tx_bytes = 0;

	char * argv[] = { app_name, read_r, read_, no_write_w, no_write_, device_p, device_ }; 
	
	si.process_options( 7, argv );	
	si.serial_main    ( );
	return NULL;
}


SerialInterface claw_si;
RoboClaw claw1( &claw_si, 1000 );		// 1ms timeout
	
const unsigned char address = 0x80;
char opt1[] = "./xeyes";
char opt2[] = "-R ascii";
char opt3[] = "-w 1";
char opt4[] = "-p /dev/ttyACM0";
char opt5[] = "-t";
char opt6[] = "-r";

void* setup_roboclaw_comms(void*) 
{
	char* options[6];
	options[0] = opt1;
	options[1] = opt2;
	options[2] = opt3;
	options[3] = opt4;
	options[4] = opt5;
	options[5] = opt6;

	claw_si.process_options( 6, (char**)options );
	claw_si.serial_main    (  );
}

void roboclaw()
{
	claw1.ForwardM1( address, 64 );	
	for (int i=0; i<5; i++)  
		usleep( 1000000 );
	claw1.ForwardM1( address, 0 );	
	while (1) {};
}

pthread_t serial_thread_id   = 0;
pthread_t roboclaw_thread_id = 0;

void create_thread()
{
	int iret1 = pthread_create( &serial_thread_id, NULL, serial_setup, (void*) NULL);
	if (iret1)
	{
		fprintf(stderr,"Error - Could not create Servo PWM Interface thread. return code: %d\n",iret1);
		//exit(EXIT_FAILURE);
	}

	int iret2 = pthread_create( &roboclaw_thread_id, NULL, setup_roboclaw_comms, (void*) NULL);
	if (iret2)
	{
		fprintf(stderr,"Error - Could not create Servo PWM Interface thread. return code: %d\n",iret2);
	}
}

/* Components of this app:
		Opencv - Face detector webcam
		Serial comms to PWM board.
		Serial comms to roboClaw board.
		IPC Vision Shared Memory.		
*/
int main()
{
	create_thread();	
	//roboclaw(); 
	
    float left_angle;
    float right_angle;

	int ipc_mem = eyes_connect_shared_memory( 1 );

	int r = mouse_init( 1080, 720 );	
	char* ptr;
	
    while (1)
    {
		left_angle  = calc_left_eye_angle ( mouse.x, mouse.y );
		right_angle = calc_right_eye_angle( mouse.x, mouse.y );
		
		set_LeftEye_angle ( txt,	left_angle  );
		ptr = txt + strlen( txt );
		set_RightEye_angle( ptr, right_angle );
		si.my_write		  ( txt, strlen(txt) );
		//printf("[%4.0f,%4.0f]  l=6.2f; r=6.2f;  s;\n", mouse.x, mouse.y ); 
		//					   left_angle, right_angle, txt );
		mouse_timeslice();
		fd_timeslice   ();
    }
	mouse_close( );
    return 1;
}
