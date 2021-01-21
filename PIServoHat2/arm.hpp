#ifndef _ARM_HPP_
#define _ARM_HPP_


#include <string>
#include <vector>
#include "drive_five.h"
#include "servo.hpp"


struct stMotId
{
	char label[3];
};


/* This is being deprecated! */


class Arm
{
public: 
	Arm();
	Arm( std::string mChain );	 
	~Arm();
	void Init();
	
	// each one 'space' deliminated:
	void define_motors ( std::string mChain );
	void load_cal_info ( );
	void actuate_vector( std::vector<float>	mAngles );
	void set_drivefive ( DriveFive* mFive )   { df = mFive; };

protected:
	void actuate_servo( int servoChannel, float  mAngle );
	void actuate_df   ( char mDriveLetter, float  mAngle );

private:
	DriveFive* df;				// File descriptor to the opened DriveFive object.
	
	
	//std::vector<std::string> composition;		// Each entry specifies a motor.
	/* df:v		==>  DriveFive board motor V
	   df:x     ==>  DriveFive board motor X
	   S:0		==>	 RPI Servo Hat connector #0
	   S:12		==>	 RPI Servo Hat connector #12	   
	*/

	std::vector<struct stMotId>  m_motor_sequence;
	
	Servo*  servs[8];		// Up to half of the Phat	

};


extern Arm  leftArm ;
extern Arm  rightArm;


#endif
