#ifndef _BALANCE_HPP_
#define _BALANCE_HPP_


void upper_threshold_action();
void lower_threshold_action();

float compute_toe_pressure_fraction( LoadCellView* loadcell_foot );
void  standing_balance();


bool fuzzy_equal( float mValue, float mAverage, float mStdDev );
bool fuzzy_not_to_exceed( float mValue, float mAverage, float mStdDev );


/* This requires info from the Loadcells (serial threads)
	We're going to try balancing completely based on those.
	
	Tilt sensor will be used to control amount of bow/squating (hip to knee ratio)		
*/

/****************************************
Art of Bowing
	Rotate both hip swing motors same direction, same speed.
	
	Function below will begin the motion.
	
	Wouldn't it be nice if these could learn the proper speed (duties).
****************************************/
void bow( );
bool bow_end_reached();		// based on Tilt Sensor
void reverse_bow( );

/****************************************
Art of Squatting
(Downward)
	Rotate both hip swing motor and knee (2x speed opposite direction).
	Adjust ankle to keep level foot.
(Upward)
	Reverse rotate both hip swing motor and knee (2x speed opposite direction)
	Adjust ankle to keep level foot.
****************************************/
void squat( );
bool squat_end_reached();
void reverse_squat();


#endif
