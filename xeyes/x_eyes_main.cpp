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

const int MIN_SERVO_PW = 553;
const int MAX_SERVO_PW = 2300;

const int ScreenWidth  = 1280;
const int ScreenHeight = 1080;

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

RoboClaw 	claw1( 1000 );		// 1ms timeout
	
const unsigned char address = 0x80;
char opt1[] = "./xeyes";
char opt2[] = "-Rascii";
char opt3[] = "-w50";
char opt4[] = "-p/dev/ttyACM0";
char opt5[] = "-b9600";
char opt6[] = "-T";		// show TX details
//char opt7[] = "-r";	// means NO Rx
char* options[5];

void setup_roboclaw_comms(void)
{	
	options[0] = opt1;
	options[1] = opt3;
	options[2] = opt4;
	options[3] = opt5;
	options[4] = opt6;

	claw1.process_options( 5, (char**)options );
	//claw1.serial_main  (  );
	printf("setup_roboclaw_comms()done\n");
}

void roboclaw()
{
	for (int i=0; i<5; i++)  
		usleep( 1000000 );

	while (1) { 
		printf("roboclaw1:");
		claw1.ForwardM1 ( address, 64 );	
		for (int i=0; i<5; i++)  
			usleep( 1000000 );
		printf("\n");
		//claw1.BackwardM1( address, 64 );	
		//for (int i=0; i<5; i++)  
		//	usleep( 1000000 );
		printf("\n");			
	}
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

/*	int iret2 = pthread_create( &roboclaw_thread_id, NULL, setup_roboclaw_comms, (void*) NULL);
	if (iret2)
	{
		fprintf(stderr,"Error - Could not create Servo PWM Interface thread. return code: %d\n",iret2);
	} */
}

void find_center( int& x, int &y, cv::Rect mrect)
{
	x = mrect.x + (mrect.width/2);
	y = mrect.y + (mrect.height/2);
}

void update_eye_positions(int x, int y)
{
    float left_angle;
    float right_angle;
    float up_down_angle;

	// Send Left/Right : 
//	left_angle    = calc_left_eye_angle   ( x, y );
//	right_angle   = calc_right_eye_angle  ( x, y );

	right_angle   = left_angle = calc_left_right_angle( x, MIN_SERVO_PW, MAX_SERVO_PW, 0, ScreenWidth );
	up_down_angle = calc_eye_up_down_angle            ( ScreenHeight-y, MIN_SERVO_PW, MAX_SERVO_PW, 0, ScreenHeight );
	set_LeftEye_angle ( txt, left_angle ); 
	char* ptr     = txt + strlen( txt ); 
	set_RightEye_angle( ptr, right_angle ); 

	// Now Send Up/Down :
	ptr += strlen( ptr );
	set_left_eye_up_angle ( ptr, up_down_angle );
	ptr += strlen( ptr );
	set_right_eye_up_angle( ptr, up_down_angle );

	strcat(txt, "\r");
	si.my_write( txt, strlen(txt)   );
	printf("%d,%d : %s\n", x,y, txt );
}

float neck_angle = 0.0;			// Straight ahead [-90, +90]

void update_neck_angle( int x )
{
	float neck_duty;

	if (x < ScreenWidth/4)
	{
		neck_duty = 0.25;
	}
	else if (x > 3*ScreenWidth/4)
	{
		neck_duty = -0.25;		
	}
	else 
		neck_duty = 0.0;	
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
	//setup_roboclaw_comms();
	//roboclaw(); 

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

	int face_okay = fd_init();
	int r 		  = mouse_init( ScreenWidth, ScreenHeight );
	char* ptr;
	std::string  client_msg = "no event";
	bool unrecognized=false;
	bool capture_frame=true;
    while (1)
    {
		mouse_timeslice( );
		fd_timeslice   ( );
		if (num_faces_present)
		{
			// Recognize face :
			// scale image to fixed size (convert to gray?)
			//cv::Mat face = gray(ROI);
			//model->predict( face );
			if (unrecognized)
			{
				
			}
			/* Several Events can trigger an image write (unknown face_detect
			   for several frames, or a motion detect, etc)  		   */
			if (capture_frame)
			{
				struct timeval tv;
				struct timezone tz;
				int tod = gettimeofday(&tv, &tz);
				char frame_name   [128];
				char timestamp_str[128];

				time_t t;
				time(&t);
				char* time_str = ctime(&t);
//				size_t strftime( frame_name, size_t max, "",
//				                 const struct tm *tm );
				strcpy( frame_name, "frame_" );
				strcat( frame_name, time_str );
				strcat( frame_name, ".png"   );
				//imwrite(frame_name, frame /* see face_detect.cpp */ );
			}

			// Inform Client of Presence : 
			int x,y;
			client_msg = "face detected ";
			find_center( x, y, faces[0] );
			eyes_compose_coordinate_xy( x,y );
		}
		else
			client_msg = "face gone";

		update_eye_positions( mouse.x, (ScreenHeight-mouse.y) );
		update_neck_angle   ( mouse.x  );

		// Inform the client : 
		eyes_write_server_event( client_msg );
    }
	mouse_close( );
	fd_close();
    return 1;
}
