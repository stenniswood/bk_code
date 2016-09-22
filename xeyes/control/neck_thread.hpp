

#define NECK_SPEED -12

#include "roboclaw.h"


extern RoboClaw 	claw1;		// 1ms timeout
extern float   		neck_duty;


void  setup_roboclaw_comms(void);
void  roboclaw_test		  ();
void  roboclaw			  ();
void* neck_thread		  (void*);

void  update_neck_angle	  ( int x, int width, int num_faces_present );


