#ifndef _DFSERVO_HPP_
#define _DFSERVO_HPP_

#include <string>
#include "servo.hpp"


/********* DriveFive Servo Base Class.    **********
	1 Motor Abstracted as a Servo.
	

************************************************/
   
class dfServo : public Servo
{
public:
	dfServo( char mMotorLetter );
	~dfServo();

	virtual float	get_angle (  						);	
	virtual	void    angle_to  ( float mDegree 		 	);
	virtual	void    count_to  ( unsigned short mCounts 	);
	
	
	virtual void	save_cal_file_line( FILE* fd );
			
	
private:
	char	m_motorLetter;
	

};



#endif
