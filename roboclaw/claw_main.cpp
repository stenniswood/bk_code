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
#include "serial_loadcell.hpp"


uint8_t address = 0x80;
struct tm time_stamp;
//char	  dev[25] = "/dev/ttyUSB0";
char	  dev1[25] = "/dev/ttyACM0";
char	  dev2[25] = "/dev/ttyACM1";
char	  dev3[25] = "/dev/ttyACM2";
char	  dev4[25] = "/dev/ttyACM3";

pthread_t serial_leftfoot_thread_id;
pthread_t serial_rightfoot_thread_id;

//LoadCell_SerialInterface left_foot;
//LoadCell_SerialInterface right_foot;

RoboClaw  claw (dev1,10000);

/*RoboClaw  claw2(dev2,1000);
RoboClaw  claw3(dev3,1000);
RoboClaw  claw4(dev4,1000); */

uint64_t GetTimeStamp2() 
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    uint64_t t = tv.tv_sec*(uint64_t)1000000 + tv.tv_usec;
    //printf("GetTimeStamp()  %ld  %ld  = %ld\n", tv.tv_sec, tv.tv_usec, t );
    return t;
}

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
	claw.SetMainVoltages ( address, 100, 250 );	
	claw.SetLogicVoltages( address, 100, 250 );

	uint16_t volts1 = claw.ReadMainBatteryVoltage( 0x80 );
	printf("\nMain Battery board #1 = %6.1f volts\n", (float)volts1/10. );
	volts1 = claw.ReadLogicBatteryVoltage( 0x80 );
	printf("\nLogic Battery board #1 = %6.1f volts\n", (float)volts1/10. );

	//claw.SetM1MaxCurrent(0x80, 20.0);
	//claw.SetM2MaxCurrent(0x80, 20.0);	
}

void setup_encoders()
{
	uint8_t M1mode = 0x81;
	uint8_t M2mode = 0x81;
	uint8_t address= 0x80;
	bool result = claw.SetM1EncoderMode( address, M1mode );
	result = claw.SetM2EncoderMode( address, M2mode );

	result = claw.ReadEncoderModes( address, M1mode, M2mode );
	printf("Read EncoderModes m1,m2 = %x, %x\n", M1mode, M2mode );
}
void read_encoders()
{
	uint32_t enc1;
	uint32_t enc2;
	
	uint8_t status;
	bool valid= false;
	enc1 = claw.ReadEncM1(address, &status, &valid);
	enc2 = claw.ReadEncM2(address, &status, &valid);
	printf("Encoders m1,m2= %6.2f, %6.2f \n", 2.0*enc1/2047., 2.0*enc2/2047. );
	
	bool result = claw.ReadEncoders( address, enc1, enc2);
	printf("Encoders m1,m2= %6.2f, %6.2f \n", 2.0*enc1/2047., 2.0*enc2/2047. );
	//result = claw.ReadISpeeds( address,      uint32_t &ispeed1,uint32_t &ispeed2);	
}
void init_claws()
{
	int maxCurrent = 20;
	claw.open			( dev1   );
	claw.set_baud		( B38400 );	
	read_main_battery   ( );

	uint16_t  config = 0x0067;
	claw.SetConfig      ( address, config );
	usleep(1000000);	// 0.1 sec

	config = 0;
	bool result = claw.GetConfig( address, config );
	printf("Config=%4x\n", config);	

	bool valid=false;
	uint16_t error = claw.ReadError(address, &valid);
	printf("hip ErrorStatus = %4x  %d\n\n", error, valid);
	
	setup_encoders();
	read_encoders();

/*	claw2.open			(dev2);
	claw2.set_baud		( B38400 );	
	claw2.SetMainVoltages( address, 100, 250 );
	claw.SetM1MaxCurrent(0x80, maxCurrent*100);
	
	claw3.open			(dev3);
	claw3.set_baud		( B38400 );	
	claw3.SetMainVoltages( address, 100, 250 );

	claw4.open			(dev4);
	claw4.set_baud		( B38400 );	
	claw4.SetMainVoltages( address, 100, 250 ); */
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
	if (argc>1) {

		if (strcmp(argv[1], "stop")==0) {
			stop = true;
			printf("Stop!");
		}
	}
	init_claws();
	
	time_t t;
	time(&t);
	time_stamp = *(localtime(&t));
	printf("====================== CLAW =======================HELLO==\n");
	printf("==== WARNING: This sets the baud rate.\n");

	char text[80];
	bool result = claw.ReadVersion( address, text );
	printf("\ntext=%s\n", text); 
	exit(1);
	 
	if (stop) {
		claw.ForwardM1 (address, 0);
		claw.ForwardM2 (address, 0);		
/*		claw2.ForwardM1(address, 0);
		claw2.ForwardM2(address, 0);		
		claw3.ForwardM1(address, 0);
		claw3.ForwardM2(address, 0);
		claw4.ForwardM1(address, 0);
		claw4.ForwardM2(address, 0); */
		exit(1);
	}
	while (1)
	{				
		printf("\nForward:\n");
		claw.ForwardM1(address, 64);
		claw.ForwardM2(address, 64);		
/*		claw2.ForwardM1(address, 64);
		claw2.ForwardM2(address, 64);		*/
		usleep(1000000);	// 0.1 sec
		usleep(1000000);	// 0.1 sec
		//read_main_battery();
		
		printf("\nBackward:\n");		
		claw.BackwardM1(address, 64);
		claw.BackwardM2(address, 64);
/*		claw2.BackwardM1(address, 64);
		claw2.BackwardM2(address, 64); */
		usleep(1000000);	// 0.1 sec		
		usleep(1000000);	// 0.1 sec		
	
	}	
	claw.close();
}
