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

#include "display.h"
#include "load_cell_view.hpp"
#include "bk_system_defs.h"
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
