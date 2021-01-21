#include <unistd.h> // required for I2C device access and usleep()
#include <fcntl.h>  // required for I2C device configuration
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions
#include <stdio.h>  // required for printf statements

#include <math.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <string.h>


#include "drive_five.h"
#include "servo.hpp"
#include "dfServo.hpp"
#include "rcServo.hpp"
//#include "arm.hpp"
#include "limb.hpp"
#include "robot.hpp"
#include "vector.hpp"
#include "i2c_switching.hpp"
#include "jog_mode.hpp"
#include "graph.hpp"
#include "read_pendant.h"

//int file_i2c;

//DriveFive df_lArm( "/dev/ttyUSB1");
//DriveFive df_rArm( "/dev/ttyUSB0");

Graph mpu_graph("MPU6050 -vs- time", "Time", "Data");


//#define SERVO_TEST 1
#ifdef SERVO_TEST 
rcServo serv0(0);		// Left arm (4 servos)
rcServo serv1(1);		// Left arm (4 servos)
rcServo serv4(2);		// Right arm

void direct_servo_write_test()
{
	serv0.angle_to(-45);		//serv0.set_stop_time( 525 );
	serv4.count_to( 736 );		
    usleep(2000000); // sleep for 2s.

  //  836 corresponds to 1ms time, which is 90 degrees offset from neutral.

	serv0.angle_to     ( 70.0);
	serv4.count_to( 1064 );	
	usleep(2000000);

	serv0.angle_to     ( 180.0 );
	serv4.count_to( 1925 );	
	usleep(2000000); 
}
#endif


Robot walker("robot_setup.csv");


void exit_save_positions(int, void*)
{
	printf("EXITING APP!\n");
	walker.save_servo_positions( "LastCounts.csv" );
	// Save calibrations?  no. done when exiting jogging mode.	
}

bool ShowGraph = false;

void main_processing()
{
	while(1) {
		int  presult = read_pendant();
		
		walker.play_active_vector();
		if (running)
		{
			usleep(1000000); 
			walker.next_sequence();
			continue;
		}
		// else...
		char input=0;
		if (kbhit() != 0) {	input = getchar();	}

		if (input == ',')
		{
			printf("Previous...\n\n");
			walker.prev_sequence();
		} 
		else if (input == '.')
		{
			printf("Next...\n\n");		
			walker.next_sequence();
		}
		else if ((input >= '1') && (input <= '9'))
		{
			int mnum = input-'1';
			printf("Select move #%d.\n\n", mnum );		

			walker.select_move( mnum );			
		}
		else if (input == 'r')
		{
			printf("Running...\n");				
			running = true;
		}		
		else if (input == 's')
		{
			printf("Stopping...\n");				
			running = false;
		}			
		else if (input == 'j')
		{
			printf("Jog mode...\n");	
			jog_mode( walker );			
		}
		
		else if (input == 'q')
		{
			printf("Quitting...\n");				
			exit(0);
		}		
		usleep(200000); 
	}
}

int main(int argc, char **argv)
{	
	if (argc>1)
	{
		int result = strcmp(argv[1], "gui");
		if (result==0)
			ShowGraph = true;
	}
	
//	int result = on_exit(  exit_save_positions, NULL);
//	if (result!=0) printf("Warning cannot set the on_exit function!\n");

	walker.print_robot_setup();
	
	initialize_RPI_servo_hat(walker);
	switch_to_servo_hat();	

#ifdef SERVO_TEST 	
	while(1)	direct_servo_write_test();	
#endif
	
	std::thread(&main_processing).detach(); //Create a seperate thread, for the update routine to run in the background, and detach it, allowing the program to continue	
		
	if (ShowGraph) {
		printf("\n\n*** SHOWING GRAPH...\n\n");
	    gtk_init (&argc, &argv);
		mpu_graph.create_window( 960, 480 );

		DataSeries ds;
		ds.set_name("accel x");
		mpu_graph.add_data_series( ds );
		
		ds.set_name("accel y");
		mpu_graph.add_data_series( ds );

		ds.set_name("pitch");
		mpu_graph.add_data_series( ds );
		
	    gtk_main ();	
	}
	
	while (1) 
	{			
		usleep(200000); 
	}

  return 0;
}



/*	df_lArm.send_command("pwm v 0.2");
	df_rArm.send_command("pwm v 0.2");
	usleep(1000000); 
	df_lArm.send_command("stop");
	df_rArm.send_command("stop"); */
