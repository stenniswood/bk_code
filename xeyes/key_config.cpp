#include <cstdio>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <math.h>

#include "neck_thread.hpp"
#include "roboclaw.h"
#include "cal_screen.hpp"
#include "x_eyes.hpp"
#include "vision_memory.h"


const int Offset_Increment = 5;

void shutdown()
{
	eyes_detach_memory();
	eyes_deallocate_memory();
	exit(EXIT_SUCCESS);
}

void print_key_controls()
{
	printf("Press the following keys :\n");
	printf("v - Turn neck left :\n");
	printf("b - Turn neck Right :\n\n");

	printf("s - Show calibrate screen.\n");	
	printf("r - Calibrate Left eye up.\n");
	printf("f - Calibrate Left eye up.\n\n");
	
	printf("t - Calibrate Left eye up.\n");
	printf("g - Calibrate Left eye up.\n\n");
	printf("z - Capture Face for later trainning.\n\n");
	
}


void handle_key_controls(char c)
{
	int tmp=0;
	if ((c==-1) || (c==255)) return;		/* no key */ 	
	//printf("waitKey=%c\n", c);

	// Key controls override all : 
	switch(c)
	{
		case 'h' : print_key_controls();	break;
		case ' ' :  neck_duty = 0; 			
					close_cal_screen();
					break;
		case 'v' : neck_duty = -25; 		break;
		case 'b' : neck_duty = +25; 		break;

		// Left Eye Up/Down Calibration : 
		case 'r' : adjust_eye_center( LEFT_UD, 	 Offset_Increment );	break;
		case 'f' : adjust_eye_center( LEFT_UD,  -Offset_Increment );	break;
		case 't' : adjust_eye_center( RIGHT_UD,  Offset_Increment);		break;
		case 'g' : adjust_eye_center( RIGHT_UD, -Offset_Increment);		break;
		
		case 's' : draw_cal_screen();				break;
		case 'q' : shutdown();						break;		
//		case 'z' : capture_face();					break;		
		default : break;
	}
}

