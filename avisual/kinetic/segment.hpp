#ifdef _SEGMENT_HPP_
#define _SEGMENT_HPP_

/* A segment is composed of a "bone", bolts, potentiometer, and 1 motor.	
	Since this is derived from a point mass, you may choose where the point
	mass is located along the segment.  Normally however it would make sense
	to put it at one end.  This class assumes it is, and end is the other end 
	of the segment.
	
Assumption:  For this class, the center of mass will be located along the 
			 segment axis.

Axis as pertains to the robot.  Let:
	x be left to right
	y be forward
	z be up
	
	Simplification - uses the YZ plane only.  
*/

class Segment : public PointMass
{
public:
	/* construction assume the segment is oriented along the z axis. */
	Segment ( float mMass, float mLength, float mcenter_of_mass );

		
	void		 set_inertia  			( float mInertia		);
	void		 set_c_o_m    			( float mInertia		);
	void		 rotate					( OneVector* eulerAngles);
 	float		 get_compression_force	(						);
 	virtual void evaluate_diff_equation	(				 		);


	/* All of the following should be in radians by default. */
	OneVector*	angular_postion;
	OneVector*	angular_velocity;
	OneVector*	angular_acceleration;
			
private:
	// Stance leg will be under compression.
	// Swing leg will be under extension.
	PointMass*  end;

	float distance_to_center_of_mass;
	float Length;				// inches (pivot point to pivot point)
	float Inertia;				// inertia of leg
}


#endif

/*	float calc_gravity_force_parallel		(	);
	float calc_gravity_force_perpendicular	(	);
	float calc_gravity_torque				( float mAngle	); */
