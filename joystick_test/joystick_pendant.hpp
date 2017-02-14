#ifndef _JOYSTICK_PENDANT_
#define _JOYSTICK_PENDANT_


#include "joystick.hpp"

#define NUM_LEG_AXES 5

#define LEFT_LEG_BASE_INDEX  0
#define RIGHT_LEG_BASE_INDEX NUM_LEG_AXES


class JoystickPendant : public Joystick
{
public:
	JoystickPendant();
	~JoystickPendant();

	virtual void	handle_event();
	virtual int		time_slice	();
	int				convert_to_8bit( short mSpeed );

	short 			m_robot_axes_speed[NUM_LEG_AXES*2];		// +32767 to -32767
	
};


#endif
