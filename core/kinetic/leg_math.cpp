
#include <stdlib>
#include <stdio>






MathLeg::MathLeg()
{

}
MathLeg::~MathLeg()
{

}


MathVector MathLeg::get_knee_position	(bool &valid=true)
{
	float radians = degrees_to_radians( joints_deg[HIP] );
	float ud   = UPLEG_LEN * cos( radians );		// up down axis
	float longitude = UPLEG_LEN * sin( radians );		// fore/aft axis
	
	float radians2 = degrees_to_radians( joints_deg[HIP_SWIVEL] );
	float io = longitude * sin(radians2);
	float fa = longitude * cos(radians2);

	/* knee position pivot point xyz location in leg coordinates */	
	MathVector knee(3);
	knee[0] = fa;
	knee[1] = io;
	knee[2] = ud;
	return knee;
}

MathVector MathLeg::get_ankle_position	(bool &valid=true)
{
	MathVector knee = get_knee_position();
	
	float radians = degrees_to_radians( joints_deg[HIP]+joints_deg[KNEE] );
	float ud   		= LOWLEG_LEN * cos( radians );		// up down axis
	float longitude = LOWLEG_LEN * sin( radians );		// fore/aft axis

	float radians2 = degrees_to_radians( joints_deg[HIP_SWIVEL] );
	float io = longitude * sin( radians );		// fore/aft axis	
	float fa = longitude * cos( radians );		// fore/aft axis	

	/* knee position pivot point xyz location in leg coordinates */	
	MathVector ankle(3);
	ankle[0] = fa;
	ankle[1] = io;
	ankle[2] = ud;
	ankle += knee;
	return ankle;
}

MathVector MathLeg::get_toe_position	(bool &valid=true)
{
	MathVector ankle = get_ankle_position();
	
	float radians   = degrees_to_radians( joints_deg[HIP]+joints_deg[KNEE]+joints_deg[ANKLE] );
	float ud   		= ANKLE_HEIGHT * cos( radians );		// up down axis
	float longitude = ANKLE_HEIGHT * sin( radians );		// fore/aft axis

	float radians2  = degrees_to_radians( joints_deg[HIP_SWIVEL] );
	float io = longitude * sin( radians );		// fore/aft axis	
	float fa = longitude * cos( radians );		// fore/aft axis	

	/* knee position pivot point xyz location in leg coordinates */	
	MathVector toe(3);
	toe[0] = fa;
	toe[1] = io;
	toe[2] = ud;
	toe += ankle;	
	return ankle;
}

// Inverse Kinematics:	
// Return true ==> able to reach.
//        false ==> out of range.
bool MathLeg::place_knee_at_position	(MathVector knee )
{
	// Only points on a sphere will be valid.  However we will 
	// set it as close as possible.
	// [0,0,0] is the hip location.
	
}
bool MathLeg::place_ankle_at_position	(MathVector ankle)
{

}
bool MathLeg::place_toe_at_position		(MathVector toe  )
{

}
bool MathLeg::place_heel_at_position	(MathVector toe  )
{

}
