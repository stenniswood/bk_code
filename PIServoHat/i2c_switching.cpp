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



extern int file_i2c;

MPU6050 device(0x68);

static char buffer[5];   // Create a buffer for transferring data to the I2C device

void initialize_RPI_servo_hat()
{
  /*  We're not going to open the i2c file here.  
  	  That was done for us in the MPU6050 constructor.  
  	  Just point the file to that one...  */
  file_i2c = device.f_dev;

  int addr = 0x40;    				// PCA9685 address
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

float ax, ay, az, gr, gp, gy; //Variables to store the accel, gyro and angle values

void timeslice_MPU6050()
{
	device.calc_yaw = true;

		int result = device.getAngle(0, &gr);
		result = device.getAngle(1, &gp);
		result = device.getAngle(2, &gy);
		
		
		std::cout << "Current angle around the roll,pitch,yaw: " << gr << "\t";
		std::cout << gp << "\t";
		std::cout << gy << "\n";
//		usleep(250000); //0.25sec
}

