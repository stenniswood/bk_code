#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <poll.h>
#include <linux/serial.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>
#include <sys/time.h>
#include <pthread.h>
#include "roboclaw.h"
//#include "serial_loadcell.hpp"


const uint8_t address = 0x80;
struct tm time_stamp;
char	  dev1[25] = "/dev/ttyACM0";
char	  dev2[25] = "/dev/ttyACM1";
char	  dev3[25] = "/dev/ttyACM2";
char	  dev4[25] = "/dev/ttyACM3";

pthread_t serial_leftfoot_thread_id;
pthread_t serial_rightfoot_thread_id;

//LoadCell_SerialInterface left_foot;
//LoadCell_SerialInterface right_foot;

RoboClaw mot ("/dev/ttyACM0", 1000);
RoboClaw mot2("/dev/ttyACM1", 1000);
RoboClaw mot3("/dev/ttyACM2", 1000);		// RIGHT LEG
RoboClaw mot4("/dev/ttyACM3", 1000);		// RIGHT LEG

void print_args(int argc, char *argv[])
{
	printf("ARGS %d: ",argc);
	for (int i=0; i< argc; i++)
	{
		printf(" %s ", argv[i] );
	}
	printf("\n");
}

/*
void* serial_interface(void* mParam)				// serial port used for arduino connections & GPS.
{
	LoadCell_SerialInterface* mFoot = (LoadCell_SerialInterface*) mParam;
	static char  app_name[12];
	static char  read_r[4];
	static char  no_write_w[4];
	static char  device_p[4];

	static char  read_[20];
	static char  no_write_[20];
	static char  device_[20];

	sprintf(app_name,   "./avisual");
	sprintf(read_r,     "-R");
	sprintf(no_write_w, "-w");
	sprintf(device_p,   "-p");

	sprintf(read_,     "ascii");
	sprintf(no_write_, "0");

	//printf("mFoot=%p;  left_foot=%p\n", mFoot, &left_foot);
	if (mFoot == &left_foot)	{
		sprintf(device_,   "/dev/ttyACM0");
		mFoot->_cl_rx_dump = 1;
		mFoot->_cl_rx_dump_ascii = 1;		
		mFoot->_cl_port = strdup("/dev/ttyACM0");
		mFoot->_cl_tx_bytes = 0;
		mFoot->left_foot = false;
	} else {
		mFoot->_cl_port = strdup("/dev/ttyACM1");	
		mFoot->_cl_rx_dump = 1;
		mFoot->_cl_rx_dump_ascii = 1;		
		mFoot->_cl_tx_bytes = 0;
		mFoot->left_foot = true;		
		//sprintf(device_,   "/dev/ttyACM1");
	}
	char * argv[] = { app_name, read_r, read_, no_write_w, no_write_, device_p, device_ }; 
	
	mFoot->serial_loadcell_main( 7, argv );	
	return NULL;
}
*/
void read_main_battery()
{
//	mot.SetMainVoltages ( address, 100, 250 );	
//	mot.SetLogicVoltages( address, 100, 250 );

	uint16_t volts1 = mot.ReadMainBatteryVoltage( 0x80 );
	float volt = ((float)volts1)/10.;
	printf("\nMain Battery board #1 = %6.1f volts\n", volt );
	volts1 = mot.ReadLogicBatteryVoltage( 0x80 );
	volt = ((float)volts1)/10.;
	printf("\nLogic Battery board #1 = %6.1f volts\n", volts1 );

	//mot.SetM1MaxCurrent(0x80, 20.0);
	//mot.SetM2MaxCurrent(0x80, 20.0);	
}

void setup_encoders()
{
	uint8_t M1mode = 0x81;
	uint8_t M2mode = 0x81;
	uint8_t address= 0x80;
	bool result = mot.SetM1EncoderMode( address, M1mode );
	result = mot.SetM2EncoderMode( address, M2mode );

	result = mot.ReadEncoderModes( address, M1mode, M2mode );
	printf("Read EncoderModes m1,m2 = %x, %x\n", M1mode, M2mode );
}
void read_encoders()
{
	uint32_t enc1;
	uint32_t enc2;
	
	uint8_t status;
	bool valid= false;
	enc1 = mot.ReadEncM1(address, &status, &valid);
	enc2 = mot.ReadEncM2(address, &status, &valid);
	printf("Encoders m1,m2= %6.2f, %6.2f \n", 2.0*enc1/2047., 2.0*enc2/2047. );
	
	bool result = mot.ReadEncoders( address, enc1, enc2);
	printf("Encoders m1,m2= %6.2f, %6.2f \n", 2.0*enc1/2047., 2.0*enc2/2047. );
	//result = mot.ReadISpeeds( address,      uint32_t &ispeed1,uint32_t &ispeed2);	
}
void init_claws()
{
	if (mot.connected == true)		
		 printf("Roboclaw #1 - Available\n"    );
	else printf("Roboclaw #1 - not available\n");
	if (mot2.connected == true)	
		 printf("Roboclaw #2 - Available\n"    );
	else printf("Roboclaw #2 - not available\n");
	if (mot3.connected == true)	
		 printf("Roboclaw #3 - Available\n"    );
	else printf("Roboclaw #3 - not available\n");
	if (mot4.connected == true)	
		 printf("Roboclaw #4 - Available\n"	   );
	else printf("Roboclaw #4 - not available\n");

	int maxCurrent = 20;
	read_main_battery( );

	//uint16_t  config = 0x0067;
	//mot.SetConfig      ( address, config );
	usleep(1000000);	// 0.1 sec

	//config = 0;
	//bool result = mot.GetConfig( address, config );
	//printf("Config=%4x\n", config);	

	bool valid=false;
	uint16_t error = mot.ReadError(address, &valid);
	printf("hip ErrorStatus = %4x  %d\n\n", error, valid);
	
	char text[80];
	bool result = mot.ReadVersion( address, text );
	printf("\ntext=%s\n", text); 
	
	//setup_encoders();
	//read_encoders ();

/*	mot.SetM1MaxCurrent(0x80, maxCurrent*100);
	mot2.SetMainVoltages( address, 100, 250 );
	mot3.SetMainVoltages( address, 100, 250 );
	mot4.SetMainVoltages( address, 100, 250 ); */
}

void create_threads()
{
/*	int iret1 = pthread_create( &serial_leftfoot_thread_id, NULL, serial_interface, (void*) &left_foot);
	if (iret1)
	{
		fprintf(stderr,"Error - Could not create right_foot thread. return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	} */
}


int main( int argc, char *argv[] )
{
	print_args(argc,argv);
	
	bool stop=false;
	init_claws();
	
	time_t t;
	time(&t);
	time_stamp = *(localtime(&t));
	printf("====================== CLAW =========================\n");

	if (argc>1) {

		if (strcmp(argv[1], "stop")==0) {
			stop = true;
			printf("Stop!");
			int count = 10;
			while (count--) {
				mot.ForwardM1 (address, 0);
				mot.ForwardM2 (address, 0);		
				mot2.ForwardM1(address, 0);
				mot2.ForwardM2(address, 0);	
		/*		mot3.ForwardM1(address, 0);
				mot3.ForwardM2(address, 0);
				mot4.ForwardM1(address, 0);
				mot4.ForwardM2(address, 0); */
				usleep(1000000);	// 0.1 sec
				usleep(1000000);	// 0.1 sec
			}
			exit(1);
		}
	}
	 
	while (1)
	{				
		printf("\nForward:\n");
		mot.ForwardM1(address, 64);
		mot.ForwardM2(address, 64);
/*		mot2.ForwardM1(address, 64);
		mot2.ForwardM2(address, 64);		*/
		usleep(1000000);	// 0.1 sec
		//usleep(1000000);	// 0.1 sec
		//read_main_battery();
		
		printf("\nBackward:\n");		
		mot.BackwardM1(address, 64);
		mot.BackwardM2(address, 64);
/*		mot2.BackwardM1(address, 64);
		mot2.BackwardM2(address, 64); */
		usleep(1000000);	// 0.1 sec		
		//usleep(1000000);	// 0.1 sec
	}
}
