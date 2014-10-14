
#include "segment.hpp"

/**********************************************************************
Name	:	Constructor()
Purpose	:	Calculate the force of gravity along the limb length.  
Inputs	:	Mass			- Mass of the segment (lbs)
			Length			- Joint to Joint distance (inches)
			center_of_mass	- Distance from the joint to the c.o.m. (inches)
Return	:	Component of gravity in the direction of the limb.
*********************************************************************/
Segment::Segment(float mMass, float mLength, float mcenter_of_mass )	
: Mass(mMass), 
 Length(mLength),
 distance_to_center_of_mass(mcenter_of_mass)
{
	Stance_leg = TRUE;			// stance means weight placed on the leg
	distance_to_center_of_mass = 7;		// guestimate inches
	Length = 12.0;				// inches
	Mass   = 5;					// mass including motor, metal, bolts pots, connectors, etc.
	Inertia= 0;					// inertia of leg
	Angle  = 0;					// Rad	wrt previous segment
	AngularSpeed = 0;			// Rad / Second
}

/**********************************************************************
Name	:	calc_gravity_force_parallel()
Purpose	:	Calculate the force of gravity along the limb length.  
			This is used to find the Normal Force for friction calculations.
Inputs	:	HipInfo structure must be filled with Mass & Angle.
Return	:	Component of gravity in the direction of the limb.
*********************************************************************/
float Segment::calc_gravity_force_parallel()
{
	// swing leg gets mass of everything below the hip joint.
	float gravity_force  = Mass * g * cos(Angle);	 // parallel force of gravity
	return gravity_force;
}

/**********************************************************************
Name	:	calc_gravity_force_perpendicular()
Purpose	:	Calculates the component of gravity perpendicular to the limb.
			This component is what gives the torque due to gravity.
Inputs	:	HipInfo must be filled with Mass & Angle
Return	:	Force of gravity (Perpendicular to the limb)
*********************************************************************/
float Segment::calc_gravity_force_perpendicular()
{
	// swing leg gets mass of everything below the hip joint.
	float gravity_force  = Mass * g * sin(Angle);	 // perpendicular force of gravity
	return gravity_force;
}

/**********************************************************************
Name	:	SetInertia()
Purpose	:	Calculates the component of gravity perpendicular to the limb.
			This component is what gives the torque due to gravity.
Inputs	:	HipInfo must be filled with Mass & Angle
Return	:	Force of gravity (Perpendicular to the limb)
*********************************************************************/
void Segment::SetInertia  ( float mInertia		)	
{
	Inertia = Inertia;
}

/**********************************************************************
Name	:	SetAngle()
Purpose	:	Calculates the component of gravity perpendicular to the limb.
			This component is what gives the torque due to gravity.
Inputs	:	HipInfo must be filled with Mass & Angle
Return	:	Force of gravity (Perpendicular to the limb)
*********************************************************************/
void Segment::SetAngle	 ( float mAngle			)	
{
	Angle = mAngle;
}

/**********************************************************************
Name	:	SetSpeed()
Purpose	:	Calculates the component of gravity perpendicular to the limb.
			This component is what gives the torque due to gravity.
Inputs	:	HipInfo must be filled with Mass & Angle
Return	:	Force of gravity (Perpendicular to the limb)
*********************************************************************/
void Segment::SetSpeed	 ( float mAngularSpeed	)	
{
	AngularSpeed = mAngularSpeed;
}

/**********************************************************************
Name	:	SetStanceLeg()
Purpose	:	Calculates the component of gravity perpendicular to the limb.
			This component is what gives the torque due to gravity.
Inputs	:	HipInfo must be filled with Mass & Angle
Return	:	Force of gravity (Perpendicular to the limb)
*********************************************************************/
void Segment::SetStanceLeg(	)
{
	Stance_leg = TRUE;
}

/**********************************************************************
Name	:	SetSwingLeg()
Purpose	:	Calculates the component of gravity perpendicular to the limb.
			This component is what gives the torque due to gravity.
Inputs	:	HipInfo must be filled with Mass & Angle
Return	:	Force of gravity (Perpendicular to the limb)
*********************************************************************/
void Segment::SetSwingLeg (	)	
{
	Stance_leg = FALSE;
}

/**********************************************************************
Name	:	calc_gravity_torque()
Purpose	:	Calculates the component of gravity perpendicular to the limb.
			This component is what gives the torque due to gravity.
Inputs	:	HipInfo must be filled with Mass & Angle
Return	:	Force of gravity (Perpendicular to the limb)
*********************************************************************/
/**********************************************************************
Name	:	calc_gravity_torque()
Purpose	:	
Inputs(HipInfo)	:
			Stance_leg
			Distance_to_center_of_mass
			Angle
Return	:	
*********************************************************************/
float Segment::calc_gravity_torque( float mAngle	)
{
	float gravity_force = 0;
	float gravity_torque = 0;
	float center
	if (HipInfo.Stance_leg)
	{
		// weight of everything above the hip joint
		// center of gravity above the hip
		// sum of torques above hip (assume 0 for now)
		
		// assumption is the lower leg will not move

		// This is the force on the hip joint for friction calculation.
		float torque 		= abdomin_mass * g * sin(mAngle);
		return torque;
	}
	else
	{
		// Here we don't care about abdominal weight.  It is holding us up.
		// Rather we care about the leg weight (since it is suspended in air).
		float gravity_force = calc_gravity_force_perpendicular();
		// Cross product = A B sin theta
		gravity_torque 		= gravity_force * HipInfo.distance_to_center_of_mass * sin(HipInfo.Angle);
		return gravity_torque;
	}
	return 0;

}



