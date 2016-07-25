#include <cstdio>
#include <string.h>
#include <pthread.h>
#include "x_eyes.hpp"
#include "serial.hpp"
#include "mouse.h"


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
	si.serial_main( 7, argv );
	return NULL;
}

pthread_t serial_thread_id=0;

void create_thread()
{
	int iret1 = pthread_create( &serial_thread_id, NULL, serial_setup, (void*) NULL);
	if (iret1)
	{
		fprintf(stderr,"Error - Could not create right_foot thread. return code: %d\n",iret1);
		//exit(EXIT_FAILURE);
	}
}

int main()
{
    float left_angle;
    float right_angle;
	int r = mouse_init( 1080, 720 );	
	create_thread();
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
    }
	mouse_close( );
    return 1;
}
