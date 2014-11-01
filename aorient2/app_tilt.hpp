#ifndef _APP_TILT_H_
#define _APP_TILT_H_


extern bool	EmergencyStop;

float	compute_speed		( float AngularRate );
void 	update_segway_base	( float mMotorPercent);
BOOL 	fusion_complete		(		);


#endif
