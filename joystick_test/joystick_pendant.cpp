#include <math.h>
#include <stdio.h>
#include "joystick_pendant.hpp"


JoystickPendant::JoystickPendant()
:Joystick()
{
	for (int i=0; i<NUM_LEG_AXES*2; i++)
		m_robot_axes_speed[i] = 0;
	
}
JoystickPendant::~JoystickPendant()
{

}

void JoystickPendant::handle_event()
{
	Joystick::handle_event();

	//printf("Inside pendant handle_event()!\n");
	if ((m_ev.type==JS_EVENT_AXIS) && (m_ev.number<4))
	{	
		if ( (m_ev.number==1)) {		// must be the POD index (left pod)
			if (m_buttons_latest[PS3_LEFT_TOP] )
				m_robot_axes_speed[LEFT_LEG_BASE_INDEX+1] = convert_to_8bit(m_ev.value);
			else if (m_buttons_latest[PS3_LEFT_BOTTOM] )
				m_robot_axes_speed[LEFT_LEG_BASE_INDEX+2] = convert_to_8bit(m_ev.value);		
			else
				m_robot_axes_speed[LEFT_LEG_BASE_INDEX+0] = convert_to_8bit(m_ev.value);
		}
		else if ( (m_ev.number==3))
		{
			if (m_buttons_latest[PS3_RIGHT_TOP] )
				m_robot_axes_speed[RIGHT_LEG_BASE_INDEX+1] = convert_to_8bit(m_ev.value);
			else if (m_buttons_latest[PS3_RIGHT_BOTTOM] )
				m_robot_axes_speed[RIGHT_LEG_BASE_INDEX+2] = convert_to_8bit(m_ev.value);
			else
				m_robot_axes_speed[RIGHT_LEG_BASE_INDEX+0] = convert_to_8bit(m_ev.value);
		}
	}
}

int JoystickPendant::convert_to_8bit( short mSpeed )
{
	float tmp    = mSpeed / 32768.0;
	int   retval = round(tmp*127);
	return retval;
}

int	 JoystickPendant::time_slice()
{
	Joystick::time_slice();
}

