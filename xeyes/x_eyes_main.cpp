#include <cstdio>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
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
#include "vision_logger.hpp"
#include "cal_screen.hpp"
#include "optical_flow.hpp"
#include "misc_detect.hpp"
#include "key_config.hpp"
#include "frame_grabber.hpp"
#include "face_summary.hpp"
#include "nlp_thread.hpp"

#include "frame_grabber.hpp"

  
int   	  device_number;
const int ScreenWidth  		 = 1280;
const int ScreenHeight 		 = 1080;
pthread_t serial_thread_id   = 1;
pthread_t nlp_thread_id   	 = 3;
pthread_t roboclaw_thread_id = 2;

bool   MOVE_TO_MOUSE = false;

bool CameraAvailable = false;
bool EyeServoBoardAvailable = false;
bool NeckServoBoardAvailable = false;


#include <signal.h>

static volatile int keepRunning = 1;

void app_close_handler(int dummy) 
{
    keepRunning = 0;
    sql_logger.sql_add_system_deactivated();
    mouse_close( );
	fd_close   ( );
}

void create_thread()
{
	// PWM Board:
	int iret1 = pthread_create( &serial_thread_id, NULL, serial_setup, (void*) NULL);
	if (iret1)
	{
		fprintf(stderr,"Error - Could not create Servo PWM Interface thread. return code: %d\n",iret1);
		//exit(EXIT_FAILURE);
	}

	// NLP Task:
	int iret3 = pthread_create( &nlp_thread_id, NULL, nlp_handler, (void*) NULL);
	if (iret3)
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


void init()
{
	mkdir("/home/pi/Pictures/face_recog", 	S_IRWXU);
	mkdir("/home/pi/Pictures/robot", 		S_IRWXU);
	mkdir("/home/pi/Pictures/robot/detected_faces", S_IRWXU);
	
	eye_init();
	
	// Initialize Vision Shared Memory (to abkInstant)
	int ipc_mem = eyes_connect_shared_memory( 1 );

	// FACE RECOGNITION INIT (load model or train):
    bool needs_training = (does_model_exist()==false);
    if (needs_training)
    {
	    string fn_csv = "train.csv";
	    //printf("Training Face Recognition model...\n");
        //train_model( fn_csv );
    } else {
	    //printf("Loading Face Recognition model...\n");
        //open_model ( FaceRecogModelName );
    }

	//capture.open("/Users/stephentenniswood/Movies/iMovie Library.imovielibrary/My Movie/Original Media/2016-07-24 15_47_08.mov");
    //2016-07-24 15_45_46.mov

	create_thread();
	int   face_okay   = fd_init();
	int   misc_okay   = misc_detect_init();
	int   r 		  = mouse_init( ScreenWidth, ScreenHeight );	
	frame_grab_init( device_number );

	printf("init() done\n");	
	//sql_logger.connect_to_logger_db();
}

void find_center( int& x, int &y, cv::Rect mrect)
{
	x = mrect.x + (mrect.width/2);
	y = mrect.y + (mrect.height/2);
}




void handle_arguments(int argc, char ** argv)
{
	string fn_csv = "train.csv";
	string filename;
	if ((strcmp(argv[1],"train")==0)) {
		printf("Training Face Recognition model...\n");
		train_model( fn_csv );
		exit( EXIT_SUCCESS );
	} else if ((strcmp(argv[1],"recog")==0)) {
		bool needs_training = (does_model_exist()==false);
		if (needs_training)
			printf("Face Recognition model NEEDS TRAINING...\n");
		else {
			printf("Loading Face Recognition model...\n");
			open_model ( FaceRecogModelName, fn_csv );
		}
	} else if ((strcmp(argv[1],"show")==0)) {
		filename = "/home/pi/Pictures/face_recog/";
		filename += argv[2];
		filename += "/";
		show_person_faces( filename.c_str() );
		//result = show_person_faces("/home/pi/Pictures/face_recog/s10/");
	} else if ((strcmp(argv[1],"mouse")==0)) {
		MOVE_TO_MOUSE = 1;
	} else if ((strcmp(argv[1],"shrink")==0)) {
		filename = "/home/pi/Pictures/face_recog/";
		filename += argv[2];
		filename += "/";
		make_training_size( filename );
		exit( EXIT_SUCCESS );
	} else if ((strcmp(argv[1],"cam")==0)) {
		device_number = atoi( argv[2] );
	}

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
	int frame_count=0;
	int result=0;
	char* ptr;
	std::string  message = "no event";
	bool  unrecognized	 = false;
	bool  capture_frame	 = false;
	int   face_x, face_y;

	if (argc>1) {
		print_args( argc,argv );
		handle_arguments(argc,argv);		
	}	
	//init();
	signal(SIGINT, app_close_handler );

	text_to_speech_pico	   ( "Welcome, my name is ronny.");
	text_to_speech_festival( "Welcome, my name is ronny.");
		
	sql_logger.connect_to_logger_db();	
	sql_logger.create_events_table();	
	sql_logger.sql_add_system_activated( CameraAvailable, EyeServoBoardAvailable, NeckServoBoardAvailable);	
	test_face_summary();
		
/*	sql_logger.sql_add_event("Stephen", "face detected");
	sql_logger.sql_add_event("Stephen", "face left"  );
	sql_logger.sql_add_event("David",   "face detected"  );
	sleep(10);
	sql_logger.sql_add_event("David",   "face left"  );

	//sql_logger.sql_query_event("David", "face detected");	
	time_t 		rawtime,rawtime2;
	time (&rawtime);
	rawtime2 = rawtime;
	rawtime -= (3600 * 4);		// 4 hours ago.	
	struct tm start_time_bd = *(localtime( &rawtime ));
	struct tm end_time_bd   =*(localtime( &rawtime2 ));
	sql_logger.sql_query_time( "David", "face detected", start_time_bd, end_time_bd );
	sql_logger.print_results();  

	string Response;
	sql_logger.form_response__last_time_i_saw( "Stephen", Response ); 
	printf("%s\n", Response.c_str() );
	sql_logger.form_response__last_time_i_saw( "Johnathon", Response ); 
	printf("%s\n", Response.c_str() );
*/
	exit(1);

	if( capture.isOpened()==false) 
	{
		printf("Warning: Camera is not on!\n");
		exit(1);
	}

    while (1)
    {    
		mouse_timeslice( );		
		frame_grabs    ( );

		if (frame_count++) { 
			//optic_flow( prev_gray_frame, gray_frame, flow, frame );
			//update_face_region_flow_based( flow, faces[0] );
			//imshow( "optical flow", frame );
		}
 
		fd_timeslice( gray_frame, original, false );
		//misc_detect_timeslice( original, gray_frame );

		if (num_faces_present)
		{			
			std::vector<int> predictedLabels;
			if (model) {
				message = face_recongition_tasks( gray_frame, faces, predictedLabels, capture_frame );
				save_faces(frame, predictedLabels);
			}
			
			// Inform Client of Presence : 
			message = "face detected";
			if (!MOVE_TO_MOUSE) {
				find_center( mover_x, mover_y, faces[0] );
				update_neck_angle( mover_x, frame.cols  );
			}
			eyes_compose_coordinate_xy( mover_x, mover_y );
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
			//update_neck_angle( mover_x, ScreenWidth );
		}

		printf("mover x,y= %d,%d\r", mover_x, mover_y );
		update_eye_positions( mover_x, (ScreenHeight-mover_y), ScreenWidth, ScreenHeight );

      	imshow( main_window_name, original );

		/* Several Events can trigger an image write (unknown face_detect 
		   for several frames, or a motion detect, etc)  		   */
		if (capture_frame)
		{
			char* fn = get_new_frame_captured_filename();
			imwrite(fn, frame /* see face_detect.cpp */ );
		}

		int c = cv::waitKey( 10 );
		handle_key_controls( c  );

		// Inform the client :
		//eyes_write_server_event( message );
    }
	
	app_close_handler(1);
    return 1;
}
