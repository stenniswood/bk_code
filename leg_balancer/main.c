#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>


#include "drive_five.h"
#include "motor_mapping.h"
#include "vector_file.hpp"
#include "cal_file.hpp"
#include "scan_usb_devs.hpp"
#include "load_cell.hpp"


SSerial pendant;

#define FALSE 0
#define TRUE  1

//DriveFive df("/dev/tty.usbmodem-DN03KN53");
//DriveFive df2("/dev/tty.usbmodem-DN03KN53");
//DriveFive df("/dev/ttyUSB0");
char input_argument[128];
	
	
void setup()
{
	pendant.open("/dev/ttyACM0");
	
	
	// ESTABLISH MAP OF ALL MOTORS:
	init_map();

	scan_devices();		// USB devices.
	
	for (int i=0; i<devices.size(); i++)  // For each device found,
		fill_in_device_name( devices[i].board_name, devices[i].device, devices[i].df );	// update Map file.


 	make_motors();
	read_cal_file( "cal_data.txt" );

	//setup_load_cell("/dev/ttyACM0","/dev/ttyACM1");  	
	
	
	//read_vector_file();		
	//print_sequence();
}

void help()
{
	printf("Beyond Kinetics - Leg Balancer App \n");
	printf("Author: Stephen Tenniswood  2019 \n");
	printf(" \n");	
	printf("This App uses the load cells in the feet (USB serial) to balance.\n");	
	printf("The lower leg.  The two legs will be joined right above the knees. \n");
	
	printf("\n");
	printf("Therefore any movement of one ankle should be copied by the other.\n");
	printf("\n");
	printf("\n");		
}

int main(int argc, char *argv[])
{
	int stop_cmp=0;
	// PROCESS ARGUMENTS : 
	if (argc > 1) {
		int help_cmp = strcmp(argv[1],"help")==0;	
		stop_cmp = strcmp(argv[1],"stop")==0;	
		int show_cmp = strcmp(argv[1],"show")==0;		
		if (help_cmp)
			{
			 	help();
			 	exit(1);
			}
		if (show_cmp)
			{
				setup();	
				print_map();
			 	print_motors();					
			  	exit(1);
			}
	
		// chain all arguments to get requested Pose
		strcpy(input_argument, argv[1] );
		for (int a=2; a<argc; a++)
		{
			strcat (input_argument, " ");
			strcat (input_argument, argv[a] );
		}
	}

	printf("INPUT: %s\n", input_argument );
	setup();	
		if (stop_cmp)
		{
			load_cell_stop();
			exit(1);
		}

	printf("==============================\n");
	int count = 0;
	int l_rx_bytes = 0;
	int r_rx_bytes = 0;
	int p_rx_bytes = 0;

	while (1) {
			p_rx_bytes = pendant.readLine   ( );
			l_rx_bytes = read_data_left		( );
			r_rx_bytes = read_data_right	( );

			if (l_rx_bytes) printf("PENDANT: %s\t\t", pendant.rx_buffer);
			
			if (l_rx_bytes) printf("LEFT: %s\t\t", loadcell_left.rx_buffer  );
			if (r_rx_bytes) printf("RIGHT: %s\t", loadcell_right.rx_buffer  );
			if ((l_rx_bytes) || (r_rx_bytes) || (p_rx_bytes)) printf("\n");
			
			
			/*int key = getchar();
			printf("Keypressed = %c\n", key);			
			if (keypressed()) {
				load_cell_stop();
				exit(1);  }*/
			count++;
	};
}

