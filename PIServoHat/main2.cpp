#include <unistd.h> // required for I2C device access and usleep()
#include <fcntl.h>  // required for I2C device configuration
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions
#include <stdio.h>  // required for printf statements
// kbhit
#include <stdio.h>
#include <sys/ioctl.h> // For FIONREAD
#include <termios.h>
#include <stdbool.h>
#include <stdlib.h>
#include "MPU6050.h"

int file_i2c;

MPU6050 device(0x68);

unsigned char StartAddresses[16] = { 
0x06,	0x0A,	0x0E,	0x12,	
0x16,	0x1A,	0x1E,	0x22,	
0x26,	0x2A,	0x2E,	0x32,	
0x36,	0x3A,	0x3E,	0x42 };	

unsigned char StopAddresses[16] = { 
0x08,	0x0C,	0x10,	0x14,
0x18,	0x1C,	0x20,	0x24,
0x28,	0x2C,	0x30,	0x34,
0x38,	0x3C,	0x40,	0x44
};	

char buffer[5];   // Create a buffer for transferring data to the I2C device

void initialize_RPI_servo_hat()
{
  /*char *filename = (char*)"/dev/i2c-1"; // Define the filename
  file_i2c = open(filename, O_RDWR); // open file for R/W
  if (file_i2c < 0)
  {
    printf("Failed to open file!");
    return ;
  }*/

  file_i2c = device.f_dev;

  int addr = 0x40;    				// PCA9685 address
//  ioctl(file_i2c, I2C_SLAVE, addr); // Set the I2C address for upcoming
  ioctl(file_i2c, I2C_SLAVE, addr); // Set the I2C address for upcoming
                                    //  transactions

  // First we need to enable the chip. We do this by writing 0x20 to register
  //  0. buffer[0] is always the register address, and subsequent bytes are
  //  written out in order.
  buffer[0] = 0;    // target register
  buffer[1] = 0x20; // desired value
  int length = 2;       // number of bytes, including address
  write(file_i2c, buffer, length); // initiate write

  // Enable multi-byte writing.
  buffer[0] = 0xfe;  
  buffer[1] = 0x1e;
  write(file_i2c, buffer, length);

}


void write_servo_stuff()
{
  buffer[0] = 0x06;  // "start time" reg for channel 0
  buffer[1] = 0;     // We want the pulse to start at time t=0
  buffer[2] = 0;
  int length = 3;        // 3 bytes total written
  write(file_i2c, buffer, length); // initiate the write

  buffer[0] = 0x08;  // "start time" reg for channel 0
  buffer[1] = 0;     // low byte
  buffer[2] = 4;		// high byte
  length = 3;        // 3 bytes total written
  write(file_i2c, buffer, length); // initiate the write

}
void write_servo_stuff2()
{
  buffer[0] = 0x06;  // "start time" reg for channel 0
  buffer[1] = 0;     // We want the pulse to start at time t=0
  buffer[2] = 0;
  int length = 3;        // 3 bytes total written
  write(file_i2c, buffer, length); // initiate the write

  buffer[0] = 0x08;  // "start time" reg for channel 0
  buffer[1] = 0;     // low byte
  buffer[2] = 2;		// high byte
  length = 3;        // 3 bytes total written
  write(file_i2c, buffer, length); // initiate the write

}




float ax, ay, az, gr, gp, gy; //Variables to store the accel, gyro and angle values

void timeslice_MPU6050()
{
	device.calc_yaw = true;

	//for (int i = 0; i < 400; i++) 
	{
		device.getAngle(0, &gr);
		device.getAngle(1, &gp);
		device.getAngle(2, &gy);
		std::cout << "Current angle around the roll,pitch,yaw: " << gr << "\t";
		std::cout << gp << "\t";
		std::cout << gy << "\n";
		usleep(250000); //0.25sec
	}

	//Get the current accelerometer values
	//device.getAccel(&ax, &ay, &az);
	//std::cout << "Accelerometer Readings: X: " << ax << ", Y: " << ay << ", Z: " << az << "\n";

	//Get the current gyroscope values
	//device.getGyro(&gr, &gp, &gy);
	//std::cout << "Gyroscope Readings: X: " << gr << ", Y: " << gp << ", Z: " << gy << "\n";	
}

void switch_to_servo_hat()
{
  int addr = 0x40;    				// PCA9685 address
  int status = ioctl(file_i2c, I2C_SLAVE, addr); // Set the I2C address for upcoming
                                    //  transactions
  if (status < 0) {
      std::cout << "ERR (main2.cpp:ServoHat()): Could not get I2C bus with 0x40 " << errno << " address. Please confirm that this address is correct\n"; //Print error message
      perror("Error switching.");
  }
                                    
  buffer[0] = 0;    // target register
  buffer[1] = 0x20; // desired value
  int length = 2;       // number of bytes, including address
  //write(file_i2c, buffer, length); // initiate write

  // Enable multi-byte writing.
  buffer[0] = 0xfe;  
  buffer[1] = 0x1e;
  //write(file_i2c, buffer, length);
}

void switch_to_MPU()
{
	int status = ioctl(file_i2c, I2C_SLAVE, 0x68); //Set the I2C bus to use the correct address
	if (status < 0) {
		std::cout << "ERR (main2.cpp:MPU6050()): Could not get I2C bus with 0x68" << errno << " address. Please confirm that this address is correct\n"; //Print error message
	    perror("Error switching.");		
	}
}
enum robotStances
{ RB_STANDING, RB_FACEDOWN, RB_BELLY_UP, RB_SIDE_RIGHT, RB_SIDE_LEFT };

#define MIN_FACE_DOWN_THRESHOLD -80
#define MAX_FACE_DOWN_THRESHOLD -125

#define MIN_FACE_UP_THRESHOLD 77
#define MAX_FACE_UP_THRESHOLD -270

#define MIN_SIDE_LEFT_THRESHOLD 77
#define MAX_SIDE_LEFT_THRESHOLD -270

#define MIN_SIDE_RIGHT_THRESHOLD 77
#define MAX_SIDE_RIGHT_THRESHOLD -270


void catagorize_MPU()
{
	if ((gp>MIN_SIDE_LEFT_THRESHOLD) && (gp<MAX_SIDE_LEFT_THRESHOLD) )
	{
		RobotPosition = RB_SIDE_RIGHT;
	}
	else if ((gp>MIN_SIDE_RIGHT_THRESHOLD) && (gp<MAX_SIDE_RIGHT_THRESHOLD) )
	{
		RobotPosition = RB_SIDE_LEFT;
	}


	if ((gp>MIN_FACE_DOWN_THRESHOLD) && (gp<MAX_FACE_DOWN_THRESHOLD) )
	{
		RobotPosition = RB_FACEDOWN;
	}
	else if ((gp>MIN_FACE_UP_THRESHOLD) && (gp<MAX_FACE_UP_THRESHOLD) )
	{
		RobotPosition = RB_BELLY_UP;
	}
	else	
	{
		RobotPosition = RB_STANDING;
	}

}

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

int main(void)
{	
	initialize_RPI_servo_hat();
	
	while (1) 
	{			
		timeslice_MPU6050();		
		//usleep(2000000); // sleep for 2s.

		char input=0;
		if (kbhit() != 0) {	input = getchar();	}

		if (input == ',')
		{
			printf("Previous...\n\n");
			switch_to_servo_hat();
			write_servo_stuff();
			switch_to_MPU();				
		} 
		else if (input == '.')
		{
			printf("Next...\n\n");		
			switch_to_servo_hat();
			write_servo_stuff2();
			switch_to_MPU();
		}
		else if ((input >= '1') && (input <= '9'))
		{
			int mnum = input-'1';
			printf("Select move #%d.\n\n", mnum );		

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
		else if (input == 'q')
		{
			printf("Quitting...\n");				
			exit(0);
		}
		
		usleep(200000); 

/*
For 20Kg Red Servo:
	350    -90deg
	524
	1024
	2048   +90deg
*/						    
	}

  return 0;
}

