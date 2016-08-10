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



const int ScreenWidth  = 1280;
const int ScreenHeight = 1080;
pthread_t serial_thread_id   = 0;
pthread_t roboclaw_thread_id = 0;
#define   MOVE_TO_MOUSE 1


void create_thread()
{
	// PWM Board:
	int iret1 = pthread_create( &serial_thread_id, NULL, serial_setup, (void*) NULL);
	if (iret1)
	{
		fprintf(stderr,"Error - Could not create Servo PWM Interface thread. return code: %d\n",iret1);
		//exit(EXIT_FAILURE);
	}

	// RoboClaw board:
	int iret2 = pthread_create( &roboclaw_thread_id, NULL, neck_thread, (void*) NULL);
	if (iret2)
	{
		fprintf(stderr,"Error - Could not create Servo PWM Interface thread. return code: %d\n",iret2);
	}
}

void find_center( int& x, int &y, cv::Rect mrect)
{
	x = mrect.x + (mrect.width/2);
	y = mrect.y + (mrect.height/2);
}

const char* base_path = "/home/pi/Pictures/robot/detected_faces/";

char* get_new_frame_captured_filename()
{
	static char frame_name[128];
	char timestamp_str    [128];

	time_t t;
	time(&t);
	char* time_str = ctime(&t);
	for (int c=0; c<strlen(time_str); c++)
		if (time_str[c]==' ')
			time_str[c] = '_';
		else if (time_str[c]=='\n')
			time_str[c] = '_';

	strcpy( frame_name, base_path );
	strcat( frame_name, "frame_" );
	strcat( frame_name, time_str );
	strcat( frame_name, ".png"   );
	printf("FRAME NAME: %s\n", frame_name );
	return frame_name;
}

const int Offset_Increment = 10;

void print_key_controls()
{
	printf("Press the following keys :\n");
	printf("v - Turn neck left :\n");
	printf("b - Turn neck Right :\n\n");
	
	printf("r - Calibrate Left eye up.\n");
	printf("f - Calibrate Left eye up.\n\n");
	
	printf("t - Calibrate Left eye up.\n");
	printf("g - Calibrate Left eye up.\n\n");
		
}
void handle_key_controls(char c)
{
	int tmp=0;
	// Key controls override all : 
	switch(c)
	{
		case 'h' : print_key_controls();	break;
		case ' ' : neck_duty = 0; 			break;
		case 'v' : neck_duty = -25; 		break;
		case 'b' : neck_duty = +25; 		break;

		// Left Eye Up/Down Calibration : 
		case 'r' : PW_Left_up_center_offset += Offset_Increment;	
					tmp = trunc(PW_Left_up_center_offset);
//					if ((tmp % (Offset_Increment*5))==0)
//						printf("LEFT CAL: %d\n", PW_Left_up_center_offset );
					break;
		case 'f' : PW_Left_up_center_offset -= Offset_Increment;	
//					if ((tmp % (Offset_Increment*5))==0)
//						printf("LEFT CAL: %d\n", PW_Left_up_center_offset );
					break;

		case 't' : PW_Right_up_center_offset += 5;	break;
		case 'g' : PW_Right_up_center_offset -= 5;	break;

		default : break;
	}
}

void init()
{
	// Initialize Vision Shared Memory (to abkInstant)
	int ipc_mem = eyes_connect_shared_memory( 1 );

	// FACE RECOGNITION INIT (load model or train):
    bool needs_training = (does_model_exist()==false);
    if (needs_training)
    {
	    string fn_csv = "train.csv";
	    printf("Training Face Recognition model...\n");
        train_model( fn_csv );
    } else {
	    printf("Loading Face Recognition model...\n");
        //open_model ( FaceRecogModelName );
    }

	create_thread();
	
	int   face_okay   = fd_init();
	int   r 		  = mouse_init( ScreenWidth, ScreenHeight );	
}

/* Components of this app:
		Opencv - Face detector webcam
		Serial comms to PWM board.
		Serial comms to roboClaw board.
		IPC Vision Shared Memory.		
*/             
int main(int argc, char ** argv)
{
	int mover_x=0;
	int mover_y=0;

	char* ptr;
	std::string  message = "no event";
	bool  unrecognized	 = false;
	bool  capture_frame	 = false;
	int   face_x, face_y;
	init();
		
    while (1)
    {
		mouse_timeslice( );
		fd_timeslice   ( );
		int c = cv::waitKey(10);
		if (num_faces_present)
		{
			//message = face_recongition_tasks(capture_frame);

			// Inform Client of Presence : 
			/*message = "face detected";
			if (!MOVE_TO_MOUSE) {
				find_center( mover_x, mover_y, faces[0] );
				update_neck_angle( mover_x, frame.cols );
			}
			eyes_compose_coordinate_xy( mover_x, mover_y );*/
		}
		else {
			// No Faces detected:
			message = "face gone";
			if (!MOVE_TO_MOUSE)
				neck_duty = 0;
		}

		if (MOVE_TO_MOUSE) {
			mover_x = trunc( mouse.x );
			mover_y = trunc( mouse.y );
			update_neck_angle( mover_x, ScreenWidth );
		}
		
		printf("mover x,y= %d,%d\r", mover_x, mover_y );
		update_eye_positions( mover_x, (ScreenHeight-mover_y), ScreenWidth, ScreenHeight );
		
		handle_key_controls(c);

		/* Several Events can trigger an image write (unknown face_detect 
		   for several frames, or a motion detect, etc)  		   */
		if (capture_frame)
		{
			char* fn = get_new_frame_captured_filename();
			imwrite(fn, frame /* see face_detect.cpp */ );
		}

		// Inform the client :
		eyes_write_server_event( message );
    }
	mouse_close( );
	fd_close   ( );
    return 1;
}
