/* How to store the vectors?
	A) malloc entire array
	B) malloc 1 vectors worth;  
		add to an array of pointers
		Not so good because length is unknown at start
	c) malloc 1 vectors worth;
		use linked list
		Yes.

Simple Pendulum motion:

Simple Linear Motion (no gravity)
Has an initial velocity & a distance to go.
We know the max deceleration.
Specify percentage of max decel.

Max_Decel = 1 yard per second per second

Typical Problem:
	stop in 5 yards.
	stop at 50% max.
	Vel = 100 yards / 30 seconds = 100/30 = 3.3333 yards/sec

Given Vel :
Time_to_stop = Vel / (Max_Decel * stop_rate) = 3.333 / (0.5) = 6.666 seconds
distance = at^2 /2 = (0.5) * (6.666)*(6.666) / 2 = 11.1 yards

Several Cases:
	a) Distance to_stop > distance requested
	b) Distance to stop < distance requested (maintain constant Vel until)
	c) Distance == Distance requested.
Solved
==============================================================
Extended problem:
	Starting at known Pos & Vel.
	Ending at a known Pos & Vel.  
In other words, same as before but the final velcotiy is not necessarily 
 zero.  For each timeslice, the acceleration will be constant.

a = Vel_Final - Vel_Initial

 Final Vel = InitVel + a * t
Where t is known time slice period.
a is computed based on change of velocity vectors.

======================================
Given: a sequence of vector postions.
Calculate : sequence of velocity vectors.  The delta between positions.

The whole reason for these calculations are because the inertias may be 
big compared to the torques.

Is there a short cut?
	Don't worry about accel/decel
	Just specify the angles and make duty based on PID
	Yes but bad motion quality.  ignores gravity so swings are weak going up
		and overly fast going down hill.
	Biggest downgrades due to gravity, static/kinetic friction

Driving is torque-time based
Sum of forces : 
Net Torque 				 = Motor Torque + Gravity Torque + Frictional (static or kinetic)
(Inertia * Acceleration) = (MaxMotorTorque * (Duty-PWM(maintainance_speed)/100) + (mass*gravity*sin(hip angle)) + (mu * normal (depends on stance leg or swing leg))
Solve for Duty:
Duty = Maintain Speed + Desired Acceleration + Gravity + Frictional (static or kinetic)

Motor Power = (K * vel) + (Inertia * Acceleration) + (mass*gravity*sin(hip angle))
	   
*/

/* Simplifications:
	When computing the hip angle torques, just use a straight knee for 
	inertia.  Not true but will get us close.  so the knee and ankle are always
	at 0 degrees.
*/

BOOL Stance_leg = TRUE;
float inertia = 0;		// inertia of leg
float g = 9.8;			// gravity 1g
float hip_angle;
float knee_angle;

/* Humorous, knee motor, lower leg, angle motor & foot - all combined. */
float upper_and_lower_leg_center_of_mass = 12.0;
/* lower leg, ankle motor, foot - all combined */
float lower_leg_center_of_mass = 12.0;


float calc_gravity_torque( float mAngle )
{
	float gravity_force = 0;
	float center
	if (Stance_leg)
	{
		// weight of everything above the hip joint.
		// center of gravity above the hip.
		// sum of torques above hip (assume 0 for now)
		
		// assumption is the lower leg will not move
		float force  =  mass * g * sin(mAngle);
		float torque = abdomin_mass * g * sin(mAngle)
	}
	else
	{
		// swing leg gets mass of everything below the hip joint.
		gravity_force  = mass * g * sin(mAngle);	 // perpendicular force of gravity
		upper_and_lower_leg_center_of_mass = 5;
		gravity_torque = gravity_force * center_of_mass * sin(mAngle);
		return gravity_torque;
	}	
}

float calc_normal_force()
{
	float normal = 0.0;
	// due to weight of robot, or leg depending on 
	if (Stance_leg)
	{
		// weight of everything above the hip joint.
	}
	else
	{
		// weight of everything below the hip joint.
	}
	return normal;
}

float calc_static_frictional_torque()
{
}

float calc_kinetic_frictional_torque()
{
}

