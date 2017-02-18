#ifndef _POWER_H_
#define _POWER_H_

#ifdef __cplusplus
extern "C" {
#endif

extern byte instance1;
extern byte instance2;
extern float x_alpha;		// 0.0 to 1.0   Percent/100
extern float y_alpha;		// 0.0 to 1.0   Percent/100


void configure_motor_boards( byte instance1, byte instance2 );

void AdjustThrust		 ( float mThrustFraction		);
void UpdateTiltReading	 ( float mx_tilt, float my_tilt );
void power_timeslice_50ms(								);

#ifdef __cplusplus
}
#endif

#endif

