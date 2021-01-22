#include <unistd.h> // required for I2C device access and usleep()
#include <fcntl.h>  // required for I2C device configuration
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions
#include <stdio.h>  // required for printf statements


#include "servo.hpp"
#include "rcServo.hpp"
#include "limb.hpp"
#include "robot.hpp"
#include "vector.hpp"
#include "jog_mode.hpp"



// kbhit
#include <stdio.h>
#include <sys/ioctl.h> // For FIONREAD
#include <termios.h>
#include <stdbool.h>

int kbhit(void) 
{
    static bool initflag = false;
    static const int STDIN = 0;

    if (!initflag) {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initflag = true;
    }

    int nbbytes;
    ioctl(STDIN, FIONREAD, &nbbytes);  // 0 is STDIN
    return nbbytes;
}

bool running = false;

struct stOneCounts tmpL1;// = walker.get_active_vector_package();
struct stOneCounts tmpL2;// = walker.get_active_vector_package();




void mark_cal_min ( Servo* serv )
{
	unsigned short cnts = serv->get_counts();
	 printf("\tMarking Cal_min = %d \n", cnts ) ;	
	serv->set_cal_min( cnts );	
}
void mark_cal_max ( Servo* serv )
{
	unsigned short cnts = serv->get_counts();
	 printf("\tMarking Cal_max = %d \n", cnts ) ;	
	serv->set_cal_max( cnts );	
}
void mark_cal_center ( Servo* serv )
{
	unsigned short cnts = serv->get_counts();
	 printf("\tMarking Cal_center = %d \n", cnts ) ;
	serv->set_cal_center( cnts );	
}


/* Retrieve from the servo objects held in the Limb object. */
void retrieve_current_counts( struct stOneCounts& tmpL, Limb* mlimb )
{
	unsigned short cnts = 0;

	Servo* serv = NULL;
	int numservs = mlimb->get_num_servos();
	for (int i=0; i<numservs; i++)
	{
		serv = mlimb->get_servo_handle(i);
		cnts = serv->get_counts();

		tmpL.m_counts[i] = cnts;
	}
}

/*
	
*/
void init_vects( Robot& walker )
{
	tmpL1.limb_num = 0;
	tmpL2.limb_num = 1;

	// LIMB #0
	Limb* limb = walker.get_limb(0);
	int num_servs = limb->get_num_servos();
	for (int i=0; i<num_servs; i++)	
		tmpL1.m_counts.push_back( 0 );		
	retrieve_current_counts( tmpL1, limb );
	
	// LIMB #1
	limb = walker.get_limb(1);
	num_servs = limb->get_num_servos();
	for (int i=0; i<num_servs; i++)	
		tmpL2.m_counts.push_back( 0 );
	retrieve_current_counts( tmpL2, limb );
}

void jog_mode( Robot& walker )
{
	printf("Jog mode...\n");				
	init_vects( walker );
	int which_point = 0;
	
	running = false;
	char input2 = 0;
	while (input2 != ';')
	{
		input2 = 0;
		if (kbhit() != 0) {	input2 = getchar();	}
		if (input2 == 0) continue;
		Servo* serv = NULL;		

		// KEYS TO MARK CALIBRATION MIN/MAX: (servos: 0..2)
		if (input2 == '1')	{  serv = walker.get_servo_handle(0, 0);  mark_cal_min( serv );  }
		if (input2 == '!')	{  serv = walker.get_servo_handle(0, 0);  mark_cal_max( serv );  }		// same angle will be the max.
		if (input2 == '2')	{  serv = walker.get_servo_handle(0, 1);  mark_cal_min( serv );  }
		if (input2 == '@')	{  serv = walker.get_servo_handle(0, 1);  mark_cal_max( serv );  }
		if (input2 == '3')	{  serv = walker.get_servo_handle(0, 2);  mark_cal_min( serv );  }
		if (input2 == '#')	{  serv = walker.get_servo_handle(0, 2);  mark_cal_max( serv );  }

		if (input2 == '4')	{  serv = walker.get_servo_handle(1, 0);  mark_cal_min( serv );  }
		if (input2 == '$')	{  serv = walker.get_servo_handle(1, 0);  mark_cal_max( serv );  }		// same angle will be the max.
		if (input2 == '5')	{  serv = walker.get_servo_handle(1, 1);  mark_cal_min( serv );  }
		if (input2 == '%')	{  serv = walker.get_servo_handle(1, 1);  mark_cal_max( serv );  }
		if (input2 == '6')	{  serv = walker.get_servo_handle(1, 2);  mark_cal_min( serv );  }
		if (input2 == '^')	{  serv = walker.get_servo_handle(1, 2);  mark_cal_max( serv );  }

		
		// KEYS TO MARK CALIBRATION ZERO: (servos: 0..2)
		if (input2 == 'z')	{  serv = walker.get_servo_handle(0, 0);  mark_cal_center( serv );  }
		if (input2 == 'x')	{  serv = walker.get_servo_handle(0, 1);  mark_cal_center( serv );  }		// same angle will be the max.
		if (input2 == 'c')	{  serv = walker.get_servo_handle(0, 2);  mark_cal_center( serv );  }
		if (input2 == 'v')	{  serv = walker.get_servo_handle(1, 0);  mark_cal_center( serv );  }
		if (input2 == 'b')	{  serv = walker.get_servo_handle(1, 1);  mark_cal_center( serv );  }
		if (input2 == 'n')	{  serv = walker.get_servo_handle(1, 2);  mark_cal_center( serv );  }

		// KEYS TO ADJUST SERVO POSITIONS (JOG style):	(servo #0..2)			
		if (input2 == 'q')	{ tmpL1.m_counts[0] -= 1;	 }
		if (input2 == 'a')	{ tmpL1.m_counts[0] += 1;	 }
		if (input2 == 'w')	{ tmpL1.m_counts[1] -= 1;	 }
		if (input2 == 's')	{ tmpL1.m_counts[1] += 1;	 }
		if (input2 == 'e')	{ tmpL1.m_counts[2] -= 1;	 }
		if (input2 == 'd')	{ tmpL1.m_counts[2] += 1;	 }
		if (input2 == 'Q')	{ tmpL1.m_counts[0] -= 5;	 }
		if (input2 == 'A')	{ tmpL1.m_counts[0] += 5;	printf("L1[0] counts=%d", tmpL1.m_counts[0]); }
		if (input2 == 'W')	{ tmpL1.m_counts[1] -= 5;	 }
		if (input2 == 'S')	{ tmpL1.m_counts[1] += 5;	 }
		if (input2 == 'E')	{ tmpL1.m_counts[2] -= 5;	 }
		if (input2 == 'D')	{ tmpL1.m_counts[2] += 5;	 }

		// KEYS TO ADJUST SERVO POSITIONS (JOG style):	(servo #4..6)			
		if (input2 == 'r')	{ tmpL2.m_counts[0] -= 1;	 }
		if (input2 == 'f')	{ tmpL2.m_counts[0] += 1;	 }
		if (input2 == 't')	{ tmpL2.m_counts[1] -= 1;	 }
		if (input2 == 'g')	{ tmpL2.m_counts[1] += 1;	 }
		if (input2 == 'y')	{ tmpL2.m_counts[2] -= 1;	 }
		if (input2 == 'h')	{ tmpL2.m_counts[2] += 1;	 }
		if (input2 == 'R')	{ tmpL2.m_counts[0] -= 5;	 }
		if (input2 == 'F')	{ tmpL2.m_counts[0] += 5;	 }
		if (input2 == 'T')	{ tmpL2.m_counts[1] -= 5;	 }
		if (input2 == 'G')	{ tmpL2.m_counts[1] += 5;	 }
		if (input2 == 'Y')	{ tmpL2.m_counts[2] -= 5;	 }
		if (input2 == 'H')	{ tmpL2.m_counts[2] += 5;	 }

		// KEYS TO TOGGLE LIVE POSITION : 
		if (which_point>=3) which_point = 0;
		
		unsigned short cnts = 0;
		if (input2 == 'Z')	{ serv = walker.get_servo_handle(0, 0); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL1.m_counts[0]=cnts; }	else // zero offset
		if (input2 == 'X')	{ serv = walker.get_servo_handle(0, 1); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL1.m_counts[1]=cnts; }	else // zero offset
		if (input2 == 'C')	{ serv = walker.get_servo_handle(0, 2); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL1.m_counts[2]=cnts; }	else // zero offset
		if (input2 == 'V')	{ serv = walker.get_servo_handle(1, 0); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL2.m_counts[0]=cnts; }	else // zero offset
		if (input2 == 'B')	{ serv = walker.get_servo_handle(1, 1); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL2.m_counts[1]=cnts; }	else // zero offset
		if (input2 == 'N')	{ serv = walker.get_servo_handle(1, 2); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL2.m_counts[2]=cnts; }	else // zero offset
		{
			walker.actuate_vector( tmpL1 );							
			walker.actuate_vector( tmpL2 );		
		}

		if (input2 == '?')	{ 
			// Save Calibrations
			walker.show_calibrations();
		}
		if (input2 == '>')	{ 
			// Save Calibrations
			walker.save_calibrations("calibrations_newest.csv");
		}
		if ((input2 == '0') || (input2 == '='))	
		{ 
			walker.actuate_centers();
			Limb* limb = walker.get_limb(0);
			retrieve_current_counts( tmpL1, limb );
	
			limb = walker.get_limb(1);
			retrieve_current_counts( tmpL2, limb );			
		 }

		printf("VectorL1: <%d, %d, %d>\n", tmpL1.m_counts[0], tmpL1.m_counts[1], tmpL1.m_counts[2]);
		printf("VectorL2: <%d, %d, %d>\n", tmpL2.m_counts[0], tmpL2.m_counts[1], tmpL2.m_counts[2]);
	}
	
	walker.save_servo_positions("LastCounts.csv");
	
}

