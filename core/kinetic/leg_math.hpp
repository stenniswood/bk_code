#ifndef _MATH_LEG_HPP_
#define _MATH_LEG_HPP_

#include "vector.hpp"


// Joint degs index:
#define HIP			0
#define HIP_SWIVEL	1
#define KNEE		2
#define ANKLE		3

const float UPLEG_LEN = 12.0;
const float LOWLEG_LEN = 12.0;
const float ANKLE_HEIGHT = 4.0;
const float ANKLE_TO_TOE_LEN = 7.0;
const float ANKLE_TO_HEEL_LEN = 3.5;


class MathLeg 
{
public:
	MathLeg();
	~MathLeg();
		
	// Forward Kinematics:
	MathVector 	get_knee_position	(bool &valid=true);
	MathVector 	get_ankle_position	(bool &valid=true);
	MathVector 	get_toe_position	(bool &valid=true);

	// Inverse Kinematics:	
	bool		place_knee_at_position	(MathVector knee );
	bool 		place_ankle_at_position	(MathVector ankle);
	bool 		place_toe_at_position	(MathVector toe  );
	bool 		place_heel_at_position	(MathVector toe  );
	
protected:
	float	joint_degs[4];

};



#endif
