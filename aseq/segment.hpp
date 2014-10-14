#ifdef _SEGMENT_HPP_
#define _SEGMENT_HPP_

/* A segment is composed of a "bone", bolts, potentiometer, and 1 motor.	*/

class Segment
{
public:
	Segment(float mMass, float mLength, float mcenter_of_mass );
		
	void SetInertia  ( float mInertia		);
	void SetAngle	 ( float mAngle			);
	void SetSpeed	 ( float mAngularSpeed	);

	void SetStanceLeg(	);
	void SetSwingLeg (	);
 
	float calc_gravity_force_parallel		(	);
	float calc_gravity_force_perpendicular	(	);
	float calc_gravity_torque				( float mAngle	);

private:
	// Stance leg will be under compression.
	// Swing leg will be under extension.
	BOOL  Stance_leg;		// stance means weight placed on the leg.

	float distance_to_center_of_mass;
	float Length;				// inches (pivot point to pivot point)
	float Mass;					// mass of upper leg
	float Inertia;				// inertia of leg
	float Angle;				// Rad	wrt previous segment
	float AngularSpeed;			// Rad / Second
}


#endif
