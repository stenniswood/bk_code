#ifndef _I2C_SWITCHING_HPP_
#define _I2C_SWITCHING_HPP_

#include "MPU6050.h"

extern MPU6050 device;

void initialize_RPI_servo_hat( );
void switch_to_servo_hat	 ( );
void switch_to_MPU			 ( );
void timeslice_MPU6050		 ( );




#endif
