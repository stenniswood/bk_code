#ifndef _ROBOT_HPP_
#define _ROBOT_HPP_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

#include "limb.hpp"
#include "vector.hpp"
#include "MPU6050.h"
#include "servo.hpp"


class Robot
{
public:
	Robot( std::string mFilename );
	~Robot();
	
	void 	select_move( int mMoveNum );
	void 	do_move();
	void 	read_setup_file ( std::string mSetup_file_name );
	void 	read_vector_file( std::string mSequenceName );
	void 	setup_start_times( );

	void	actuate_vector ( struct stOneVector mVec );
	void	actuate_vector ( struct stOneCounts mVec );	
	void	actuate_centers( );		// All limbs goto 0.0 degrees for calibration.
	
	void 	play_n( int mNumVecsToPlay = 2 ); // Do n vectors (left & right limbs) at once.
	void	play_active_vector( );	
	
		
	void	goto_first_sequence();		
	void	prev_sequence();
	void	next_sequence();
	Limb*   get_limb( int mLimbIndex )   { return &(m_limbs[mLimbIndex]);  };
	

	Servo*				get_servo_handle( int mLimbIndex, int mServoIndex );

	void 				show_calibrations  ( );
	void				read_servo_positions( std::string mCountsFileName );
	void				save_servo_positions( std::string mCountsFileName );
	FILE*  				save_calibrations  ( std::string mFilename );
	void				print_robot_setup  (        );
//	struct stOneCounts	get_limb_zero_vector_package( int mLimbNum );
//	struct stOneCounts 	get_active_vector_package();		// for entire robot.

	// ACTIE ROBOT ROUTINES:  STANDING BALANCE:
	void				stand_still();

	MPU6050_Velocity	device;		// 6 axis gyro, accel, etc.	
	
private:
	std::vector<Limb>	m_limbs;
	std::string 		m_setup_file_name;

	// A Collection of Sequences:
	std::vector<VectorSequence*>	m_moves;
	int						m_selected_move;
	bool	active_vector_played;
};




#endif
