#ifndef _JOYSTICK_PENDANT_
#define _JOYSTICK_PENDANT_


#include <map>
#include "joystick.hpp"

#define NUM_LEG_AXES 5

#define LEFT_LEG_BASE_INDEX  0
#define RIGHT_LEG_BASE_INDEX NUM_LEG_AXES

extern std::map<char,float> motor_letter_duty_cycles;	// holds the actual duties to send via PWM cmd.


class JoystickPendant : public Joystick
{
public:
	JoystickPendant();
	~JoystickPendant();

	void			file_AND_convert();

	virtual void	handle_event();
	virtual int		time_slice	();
	int				convert_to_8bit( short mSpeed );

	short 			m_robot_axes_speed[NUM_LEG_AXES*2];		// +32767 to -32767	
};


#endif
