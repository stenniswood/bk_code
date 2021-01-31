#ifndef _MPU6050_H_
#define _MPU6050_H_

//-------------------------------MPU6050 Accelerometer and Gyroscope C++ library-----------------------------
//Copyright (c) 2019, Alex Mous
//Licensed under the CC BY-NC SA 4.0


//-----------------------MODIFY THESE PARAMETERS-----------------------

#define GYRO_RANGE 0 //Select which gyroscope range to use (see the table below) - Default is 0
//	Gyroscope Range
//	0	+/- 250 degrees/second
//	1	+/- 500 degrees/second
//	2	+/- 1000 degrees/second
//	3	+/- 2000 degrees/second
//See the MPU6000 Register Map for more information


#define ACCEL_RANGE 0 //Select which accelerometer range to use (see the table below) - Default is 0
//	Accelerometer Range
//	0	+/- 2g
//	1	+/- 4g
//	2	+/- 8g
//	3	+/- 16g
//See the MPU6000 Register Map for more information


//Offsets - supply your own here (calculate offsets with getOffsets function)
//     Accelerometer
#define A_OFF_X 411
#define A_OFF_Y 25500  
//10634
#define A_OFF_Z -13623

//    Gyroscope
#define G_OFF_X -733
#define G_OFF_Y 433
#define G_OFF_Z -75

//-----------------------END MODIFY THESE PARAMETERS-----------------------

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
extern "C" {
	#include <linux/i2c-dev.h>
	#include <i2c/smbus.h>
}
#include <cmath>
#include <thread>

#define _POSIX_C_SOURCE 200809L //Used for calculating time

#define TAU 0.05 //Complementary filter percentage
#define RAD_T_DEG 57.29577951308 //Radians to degrees (180/PI)

//Select the appropriate settings
#if GYRO_RANGE == 1
	#define GYRO_SENS 65.5
	#define GYRO_CONFIG 0b00001000
#elif GYRO_RANGE == 2
	#define GYRO_SENS 32.8
	#define GYRO_CONFIG 0b00010000
#elif GYRO_RANGE == 3
	#define GYRO_SENS 16.4
	#define GYRO_CONFIG 0b00011000
#else //Otherwise, default to 0
	#define GYRO_SENS 131.0
	#define GYRO_CONFIG 0b00000000
#endif
#undef GYRO_RANGE


#if ACCEL_RANGE == 1
	#define ACCEL_SENS 8192.0
	#define ACCEL_CONFIG 0b00001000
#elif ACCEL_RANGE == 2
	#define ACCEL_SENS 4096.0
	#define ACCEL_CONFIG 0b00010000
#elif ACCEL_RANGE == 3
	#define ACCEL_SENS 2048.0
	#define ACCEL_CONFIG 0b00011000
#else //Otherwise, default to 0
	#define ACCEL_SENS 16384.0
	#define ACCEL_CONFIG 0b00000000
#endif
#undef ACCEL_RANGE


extern int file_i2c;		// alias for f_dev in class


class MPU6050 {
public:
		int f_dev; //Device file
 		int16_t raw_ax, raw_ay, raw_az; //Temporary storage variables used in _update()
 		float ax, ay, az, gr, gp, gy; //Temporary storage variables used in _update()
		float _angle[3]; //Store all angles (accel roll, accel pitch, accel yaw, gyro roll, gyro pitch, gyro yaw, comb roll, comb pitch comb yaw)
		
		void 	_update();
		void 	print_raw_accel();
								
	private:
		float _accel_angle[3];
		float _gyro_angle[3];


		int MPU6050_addr;


		float dt; //Loop time (recalculated with each loop)

		struct timespec start,end; //Create a time structure

		bool _first_run = 1; //Variable for whether to set gyro angle to acceleration angle in compFilter
	public:
		MPU6050(int8_t addr);
		~MPU6050();
		
		void getAccelRaw(float *x, float *y, float *z);
		void getGyroRaw(float *roll, float *pitch, float *yaw);
		void getAccel(float *x, float *y, float *z);
		void getGyro(float *roll, float *pitch, float *yaw);
		void getOffsets(float *ax_off, float *ay_off, float *az_off, float *gr_off, float *gp_off, float *gy_off);
		int getAngle(int axis, float *result);
		bool calc_yaw;
};

struct stSnapShot {
	float g_y;
	float g_p;
	float g_r;

	float a_x;
	float a_y;
	float a_z;

	float angle_r;
	float angle_p;
	float angle_y;	
};
#define HISTORY_SIZE 10240
extern struct stSnapShot history[HISTORY_SIZE];
extern int hist_index;

class MPU6050_Velocity : public MPU6050 
{
public:
	MPU6050_Velocity(int8_t addr);		

	void	update();
	void 	add_to_history();
	void 	print_history_item(int mIndex = -1 );
	
private:
	float velocity_x;
	float velocity_y;
	float velocity_z;	
	bool  first_update;	

};


#endif
