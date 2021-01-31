#include <unistd.h> // required for I2C device access and usleep()
#include <fcntl.h>  // required for I2C device configuration
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions
#include <stdio.h>  // required for printf statements

#include <pthread.h> 
#include <thread>

#include <math.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <string.h>


//#include "drive_five.h"
#include "servo.hpp"
//#include "dfServo.hpp"
#include "rcServo.hpp"
//#include "arm.hpp"
#include "limb.hpp"
#include "robot.hpp"
#include "vector.hpp"
#include "i2c_switching.hpp"
#include "jog_mode.hpp"
#include "graph.hpp"
#include "read_pendant.h"

//#include "interpolator.hpp"


// THREAD LOCKING STUFF:
//pthread_t tid[2]; 
pthread_mutex_t lock; 


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


Robot walker("robot_setup.ini");


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
		//int  presult = read_pendant();
		//printf("main_processing() top of loop\n");

		if (running)
		{
			walker.play_active_vector();
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
			walker.play_active_vector();			
		} 
		else if (input == '.')
		{
			printf("Next...\n\n");
			//walker.next_sequence();
			walker.play_group();
			//walker.play_active_vector();
		}
		else if (input == ';')
		{
			printf("Next Two...\n\n");		
			walker.play_n(2);
		}		
		else if ((input >= '1') && (input <= '9'))
		{
			int mnum = input-'1';
			printf("Select move #%d.\n\n", mnum );		

			walker.select_move( mnum );	
			walker.play_active_vector();					
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
		else if (input == '?')
		{
			printf("MPU 0 Info: \n");	
			//walker.device.print_history_item();
			//walker.device.print_raw_accel   ();
		}
		else if (input == 'q')
		{
			printf("Quitting...\n");				
			exit(0);
		}		
		usleep(200000); 
	}
}

void measure_MPU_offsets()
{
	float ax_off;
	float ay_off;
	float az_off;
	
	float gr_off;
	float gp_off;
	float gy_off;
	//walker.device.getOffsets( &ax_off, &ay_off,  &az_off, 
	//						  &gr_off, &gp_off,  &gy_off);
	printf("OFFSETS:\n");
	printf("ax,ay,az: %6.3f %6.3f %6.3f \n", ax_off, ay_off, az_off );
	printf("gr,gp,gy: %6.3f %6.3f %6.3f \n", gr_off, gp_off, gy_off );
}

int main(int argc, char **argv)
{	
/*	OneLimbInterpolator ol(10,4);
	
	struct stFloatVector End;
	End.m_angles.push_back(90.0);
	End.m_angles.push_back(45.0);
	End.m_angles.push_back(20.0);
	End.m_angles.push_back(0.0);
	End.limb_num = 0;

	ol.set_new_vector( End );
	ol.print_sequence();
	
	End.m_angles[0] = (-30.0);
	End.m_angles[1] = (0.0);
	End.m_angles[2] = (0.0);
	End.m_angles[3] = (0.0);
	ol.set_new_vector( End );
	printf("\n");
	ol.print_sequence();
*/	
	std::string SeqFN = "walk.txt";
	if (argc>1)
	{
		int result = strcmp(argv[1], "gui");
		if (result==0)
			ShowGraph = true;
		else 
			SeqFN = argv[1];
	}
	
//	int result = on_exit(  exit_save_positions, NULL);
//	if (result!=0) printf("Warning cannot set the on_exit function!\n");

	walker.read_vector_file( SeqFN );
	walker.print_robot_setup();
	
	initialize_RPI_servo_hat(walker);
	switch_to_servo_hat();	

/* Now Test the interpolator: 1st the old way -- just move the servos. 	
	walker.actuate_vector( End );	

	End.m_angles[0] = (-30.0);
	End.m_angles[1] = (90.0);
	End.m_angles[2] = (45.0);
	End.m_angles[3] = (0.0);
	End.print();
	printf("Sleeping for 3\n");
	sleep(3);
	
	walker.actuate_vector( End );		
	return 0;
*/

	//measure_MPU_offsets();
	

#ifdef SERVO_TEST 	
	while(1)	direct_servo_write_test();	
#endif
	
	std::thread(&main_processing).detach(); //Create a separate thread, for the update routine to run in the background, and detach it, allowing the program to continue	
	
	if (ShowGraph) {
		printf("\n\n*** SHOWING GRAPH...\n\n");
	    gtk_init (&argc, &argv);
		mpu_graph.create_window( 960, 480 );

		DataSeries ds;
		ds.set_name("accel x");
		mpu_graph.add_data_series( ds );
		
		ds.set_name("accel y");
		mpu_graph.add_data_series( ds );

		ds.set_name("accel z");
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
