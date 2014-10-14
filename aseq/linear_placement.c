/*
This file handles placement of a linear object with accel, const vel, & decel.

Strategy for motion control will be to read in vectors of position -vs time.
Compute the velocity on each.

Ideally, Deceleration/Acceleration are not specified.  This makes it easier to 
specify a leg position without worrying about it's weight, motor torques, etc.
The position vectors then are desired locations without regard to physical realization.

Our goal is to reproduce the vectors with realworld hardware as closely as possible.
When thinking about how I move, I never think about how long it takes to speed up my
leg!  Though sometimes I would say gradually stop at this angle; or stop immediately when 
you get there.  This can still be specified in the position vectors (as a slower speed) 
without regard to the weight of the leg and torques. 


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

Driving is torque-time based.

Torque = vel + 
*/
