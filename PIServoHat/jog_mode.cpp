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


void help()
{
printf("Available Commands:\
q,Q : serv0 up;		a,A : serv0 down;	1 : mark s0 min; ! : mark s0 max;\n	\
w,W : serv0 up;		s,S : serv0 down;	2 : mark s0 min; @ : mark s0 max;\n	\
e,E : serv0 up;		d,D : serv0 down;	3 : mark s0 min; # : mark s0 max;\n	\
\n\
r,R : serv0 up;		f,F : serv0 down;	4 : mark s0 min; $ : mark s0 max;\n	\
t,T : serv0 up;		g,G : serv0 down;	5 : mark s0 min; % : mark s0 max;\n	\
y,Y : serv0 up;		h,H : serv0 down;	6 : mark s0 min; ^ : mark s0 max;\n	\
\n\
z,x,c,v,b,n : Mark servo[0..5] center positions respectively.\n	\
Z,X,C,V,B,N : Toggle servo[0..5] min,max,center positions respectively.\n	\
\n \
/ : show calibrations\n \
> : save calibrations\n \
");
	

}

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
	tmpL1.m_counts.clear();
	Limb* limb = walker.get_limb(0);
	int num_servs = limb->get_num_servos();
	for (int i=0; i<num_servs; i++)	
		tmpL1.m_counts.push_back( 0 );		
	retrieve_current_counts( tmpL1, limb );
	printf("Num Servos on Limb0 : %d", num_servs);

	// LIMB #1
	tmpL2.m_counts.clear();
	limb = walker.get_limb(1);
	num_servs = limb->get_num_servos();
	for (int i=0; i<num_servs; i++)	
		tmpL2.m_counts.push_back( 0 );
	retrieve_current_counts( tmpL2, limb );
	printf("Num Servos on Limb1 : %d", num_servs);
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

		if (input2 == '7')	{  serv = walker.get_servo_handle(1, 3);  mark_cal_min( serv );  }
		if (input2 == '&')	{  serv = walker.get_servo_handle(1, 3);  mark_cal_max( serv );  }
		if (input2 == '8')	{  serv = walker.get_servo_handle(1, 3);  mark_cal_min( serv );  }
		if (input2 == '*')	{  serv = walker.get_servo_handle(1, 3);  mark_cal_max( serv );  }
		
		// KEYS TO MARK CALIBRATION ZERO: (servos: 0..2)
		if (input2 == 'z')	{  serv = walker.get_servo_handle(0, 0);  mark_cal_center( serv );  }
		if (input2 == 'x')	{  serv = walker.get_servo_handle(0, 1);  mark_cal_center( serv );  }		// same angle will be the max.
		if (input2 == 'c')	{  serv = walker.get_servo_handle(0, 2);  mark_cal_center( serv );  }
		if (input2 == 'v')	{  serv = walker.get_servo_handle(1, 0);  mark_cal_center( serv );  }
		if (input2 == 'b')	{  serv = walker.get_servo_handle(1, 1);  mark_cal_center( serv );  }
		if (input2 == 'n')	{  serv = walker.get_servo_handle(1, 2);  mark_cal_center( serv );  }
		if (input2 == 'm')	{  serv = walker.get_servo_handle(0, 3);  mark_cal_center( serv );  }
		if (input2 == ',')	{  serv = walker.get_servo_handle(1, 3);  mark_cal_center( serv );  }

#define LARGE_INC 15
#define SMALL_INC 1

		// KEYS TO ADJUST SERVO POSITIONS (JOG style):	(servo #0..2)			
		if (input2 == 'q')	{ tmpL1.m_counts[0] -= SMALL_INC;	 }
		if (input2 == 'a')	{ tmpL1.m_counts[0] += SMALL_INC;	 }
		if (input2 == 'w')	{ tmpL1.m_counts[1] -= SMALL_INC;	 }
		if (input2 == 's')	{ tmpL1.m_counts[1] += SMALL_INC;	 }
		if (input2 == 'e')	{ tmpL1.m_counts[2] -= SMALL_INC;	 }
		if (input2 == 'd')	{ tmpL1.m_counts[2] += SMALL_INC;	 }
		if (input2 == 'Q')	{ tmpL1.m_counts[0] -= LARGE_INC;	 }
		if (input2 == 'A')	{ tmpL1.m_counts[0] += LARGE_INC;	printf("L1[0] counts=%d", tmpL1.m_counts[0]); }
		if (input2 == 'W')	{ tmpL1.m_counts[1] -= LARGE_INC;	 }
		if (input2 == 'S')	{ tmpL1.m_counts[1] += LARGE_INC;	 }
		if (input2 == 'E')	{ tmpL1.m_counts[2] -= LARGE_INC;	 }
 		if (input2 == 'D')	{ tmpL1.m_counts[2] += LARGE_INC;	 }


		// KEYS TO ADJUST SERVO POSITIONS (JOG style):	(servo #4..6)			
		if (input2 == 'r')	{ tmpL2.m_counts[0] -= SMALL_INC;	 }
		if (input2 == 'f')	{ tmpL2.m_counts[0] += SMALL_INC;	 }
		if (input2 == 't')	{ tmpL2.m_counts[1] -= SMALL_INC;	 }
		if (input2 == 'g')	{ tmpL2.m_counts[1] += SMALL_INC;	 }
		if (input2 == 'y')	{ tmpL2.m_counts[2] -= SMALL_INC;	 }
		if (input2 == 'h')	{ tmpL2.m_counts[2] += SMALL_INC;	 }
		if (input2 == 'u')	{ tmpL1.m_counts[3] += SMALL_INC;	 }		
		if (input2 == 'j')	{ tmpL1.m_counts[3] -= SMALL_INC;	 }
		if (input2 == 'i')	{ tmpL2.m_counts[3] += SMALL_INC;	 }		
		if (input2 == 'k')	{ tmpL2.m_counts[3] -= SMALL_INC;	 }

		if (input2 == 'R')	{ tmpL2.m_counts[0] -= LARGE_INC;	 }
		if (input2 == 'F')	{ tmpL2.m_counts[0] += LARGE_INC;	 }
		if (input2 == 'T')	{ tmpL2.m_counts[1] -= LARGE_INC;	 }
		if (input2 == 'G')	{ tmpL2.m_counts[1] += LARGE_INC;	 }
		if (input2 == 'Y')	{ tmpL2.m_counts[2] -= LARGE_INC;	 }
		if (input2 == 'H')	{ tmpL2.m_counts[2] += LARGE_INC;	 }
		if (input2 == 'U')	{ tmpL1.m_counts[3] += LARGE_INC;	 }
		if (input2 == 'J')	{ tmpL1.m_counts[3] -= LARGE_INC;	 }		
		if (input2 == 'I')	{ tmpL2.m_counts[3] += LARGE_INC;	 }
		if (input2 == 'K')	{ tmpL2.m_counts[3] -= LARGE_INC;	 }

		// KEYS TO TOGGLE LIVE POSITION : 
		if (which_point>=3) which_point = 0;
		
		unsigned short cnts = 0;
		if (input2 == 'Z')	{ serv = walker.get_servo_handle(0, 0); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL1.m_counts[0]=cnts; }	else // zero offset
		if (input2 == 'X')	{ serv = walker.get_servo_handle(0, 1); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL1.m_counts[1]=cnts; }	else // zero offset
		if (input2 == 'C')	{ serv = walker.get_servo_handle(0, 2); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL1.m_counts[2]=cnts; }	else // zero offset
		if (input2 == 'V')	{ serv = walker.get_servo_handle(1, 0); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL2.m_counts[0]=cnts; }	else // zero offset
		if (input2 == 'B')	{ serv = walker.get_servo_handle(1, 1); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL2.m_counts[1]=cnts; }	else // zero offset
		if (input2 == 'N')	{ serv = walker.get_servo_handle(1, 2); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL2.m_counts[2]=cnts; }	else // zero offset
		if (input2 == 'M')	{ serv = walker.get_servo_handle(0, 3); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL1.m_counts[3]=cnts; }	else // zero offset
		if (input2 == ',')	{ serv = walker.get_servo_handle(1, 3); cnts = serv->get_stop_counts(which_point++); serv->count_to(cnts);	tmpL2.m_counts[3]=cnts; };

		if (input2 == '?')
			help();
		if (input2 == '/')	{ 
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

		walker.actuate_vector( tmpL1 );							
		walker.actuate_vector( tmpL2 );		
		printf("VectorL1: <"   );  tmpL1.print();
		printf(">  VectorL2: <");  tmpL2.print();		printf(">\n");
	}
	
	walker.save_servo_positions("LastCounts.csv");
	
}

