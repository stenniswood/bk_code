#ifndef _POINT_MASS_H_
#define _POINT_MASS_H_

/* 
	Zeroeth law in creating this class!!!
	But what is Beyond Kinetics without some physics classes?
	
The object 
	Let's get right to it.		*/

class PointMass
{
public:
	PointMass	( float mMass, float mElasticity = 1.0  );
	PointMass	( PointMass* mpm 						);		// Copy ctor

	//void		 add_force				( Force* mForce 		);
	void		 set_position			( OneVector* ov	 		);
	//virtual void evaluate_diff_equation	(				 		);

	OneVector*	get_kinetic_energy		(  				 		);
	OneVector*	get_potential_energy	( float mGroundHeight 	);
    float		get_geometrical_distance( PointMass* mObject2 	);

	float 		get_speed			 	(				);				
	float 		get_horizontal_velocity (				);
	float 		get_horizontal_accel	(				);     
	float 		get_vertical_velocity	(				);     
	float 		get_vertical_accel		(				);

	/*void 		PrintPosition		( String mName );
	void 		PrintVelocity		( String mName );
	void 		PrintAcceleration	( String mName ); */

	OneVector*	Position;
	OneVector*	Velocity;
	OneVector*	Acceleration;

protected:
	float		Mass;                                              // Weight of everything but the Motor & Gearbox    

private:
//	AppliedForces*  Forces;
    float		Elasticity;
    
};

#endif

