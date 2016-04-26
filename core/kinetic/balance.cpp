/****************************************************************
	This balances based on Load cells.  (and gyro?)
	
	Algorithm is : 
			a) Too much pressure on toes, bend ankle backward (push on toes)
				i) Do this independantly for each leg.
			b) If pressure on heels for both legs gets near zero,
				and pressure is > 80% on toes then...
				Take a step forward (move the left hip forward for now always left hip)
			c) If pressure on toes decreases near zero 
				[and Gyro indicates tilt backwards], then...
				Rotate ankles forward.
****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <math.h>
#include <vector>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"

#include "display.h"
#include "load_cell_view.hpp"
#include "bk_system_defs.h"
#include "fuse_accel.h"
#include "vector_base.h"
#include "test_layouts.hpp"

#define Debug 0

struct sLearnedThreshold
{
	float value;
	float std_dev;
};
struct sLearnedThreshold LeftThreshold;
struct sLearnedThreshold RightThreshold;

//extern LoadCellView* loadcell_left_foot;
//extern LoadCellView* loadcell_right_foot;
/*float compute_toe_pressure_fraction( LoadCellView* loadcell_foot )
*/

void upper_threshold_action()
{

}
void lower_threshold_action()
{

}

#define BALANCED_VALUE  0
#define ABOVE_THRESHOLD 1
#define BELOW_THRESHOLD 2

int compare_threshold( float mMeasuredValue, struct sLearnedThreshold* mThresh )
{
	float upper_limit = mThresh->value + 3 * mThresh->std_dev;
	float lower_limit = mThresh->value - 3 * mThresh->std_dev;
	
	if (mMeasuredValue > upper_limit)
	{
		return ABOVE_THRESHOLD;
	} 
	else if (mMeasuredValue < lower_limit)
	{
		return BELOW_THRESHOLD;
	}
	return BALANCED_VALUE;
}

void  standing_balance()
{
	float left_foot_toe  = loadcell_left_foot->compute_toe_pressure_fraction ( );
	float right_foot_toe = loadcell_right_foot->compute_toe_pressure_fraction( );

	int comparison = compare_threshold( left_foot_toe, &LeftThreshold );
	switch ( comparison )
	{
		case ABOVE_THRESHOLD :	upper_threshold_action();	break;
		case BELOW_THRESHOLD :	lower_threshold_action();	break;
		default:	break;
	}

	comparison = compare_threshold( right_foot_toe, &RightThreshold );
	switch ( comparison )
	{
		case ABOVE_THRESHOLD :	upper_threshold_action();	break;
		case BELOW_THRESHOLD :	lower_threshold_action();	break;
		default:	break;
	}
}
/************************************************************************/
bool fuzzy_equal( float mValue, float mAverage, float mStdDev )
{
	float upper = mAverage + mStdDev;
	float lower = mAverage - mStdDev;	
	if ((mValue > lower) && (mValue<upper))
		return true;
	return false;
}
bool fuzzy_not_to_exceed( float mValue, float mAverage, float mStdDev )
{
	float upper = mAverage + mStdDev;
	float lower = mAverage - mStdDev;	
	if ((mValue > lower))
		return true;
	return false;
}
/************************************************************************/

/************************************************************************
float Tilt = 10.0;
void bow( float mDesiredTiltAngleDegrees )
{
	// float duty = 40;		// depends on weight of robot above the hip.
	//robot.rotate_left_hip( duty );
	//robot.rotate_right_hip( duty );		
}
bool bow_end_reached()		// based on Tilt Sensor
{
	// Possibility if moving fast, that it skips over the 45deg.  ie. 39, 51degs.	
	if (fuzzy_not_to_exceed(AccelAngularPosition.rx, radians(45), radians(5) ))
		return true;
	return false;
}
void reverse_bow( )
{
	// float duty = -40;		// depends on weight of robot above the hip.
	//robot.rotate_left_hip( duty );
	//robot.rotate_right_hip( duty );
}
************************************************************************
void squat( )
{
}
bool squat_end_reached()
{
	return false;
}
void reverse_squat()
{
}
*/