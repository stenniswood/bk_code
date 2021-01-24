#ifndef _RCSERVO_HPP_
#define _RCSERVO_HPP_

#include <string>
#include "servo.hpp"

/********* DriveFive Servo Base Class.    **********
	1 Motor Abstracted as a Servo.
	

************************************************/

class rcServo : public Servo
{
public:
	rcServo( int mServoNumber );
	~rcServo();

//	void    set_counts( int mCounts );	
	float			get_angle			( )  { return m_curr_angle;  };
	unsigned short	get_counts			( )  { return m_curr_counts; };	
	void    		angle_to  			( float mDegree );	
	virtual	void    count_to  			( unsigned short mCounts );	

	virtual void    load_current_position( std::ifstream& fd );
	virtual void    save_current_position( FILE* fd );	
	virtual void	save_cal_file_line	 ( FILE* fd );	
	virtual void 	print_parameters	 ( );	

	virtual void	setup_hawrdware()	{	set_start_time(0); 	};

protected:
	void 			set_start_time( unsigned short mCounts );			
	void 			set_stop_time ( unsigned short mCounts );

	unsigned char 	get_start_address( );
	unsigned char 	get_stop_address ( );
	
	
private:
	unsigned char		ServoNum;		// Index on the PI Hat board.
	
	float				m_curr_angle;
	unsigned short		m_curr_counts;	

};



#endif



