#include <math.h>
#include <stdio.h>

#include "joystick_pendant.hpp"


std::map<int,char> motor_letter_to_joypad_axis;
std::map<char,float> motor_letter_duty_cycles;	// holds the actual duties to send via PWM cmd.


JoystickPendant::JoystickPendant()
:Joystick()
{
	for (int i=0; i<NUM_LEG_AXES*2; i++)
		m_robot_axes_speed[i] = 0;

	motor_letter_duty_cycles['v'] = 0.0;
	motor_letter_duty_cycles['w'] = 0.0;
	motor_letter_duty_cycles['x'] = 0.0;
	motor_letter_duty_cycles['y'] = 0.0;
	motor_letter_duty_cycles['z'] = 0.0;
	
//	motor_letter_to_joypad_axis[PS_AXIS_LPOD_UP_DOWN] = 'v';
//	motor_letter_to_joypad_axis[PS_AXIS_RPOD_UP_DOWN] = 'w';
//	motor_letter_to_joypad_axis[PS_AXIS_LPOD_UP_DOWN] = 'x';	// Though the mapping is the same axis,
//	motor_letter_to_joypad_axis['y'] = PS_AXIS_RPOD_UP_DOWN;	// the motor duties will only get updated
//	motor_letter_to_joypad_axis['z'] = -1;						// when the L1 button is held.
}

JoystickPendant::~JoystickPendant()
{

}

float scale_joypad(short JoypadAxisValue)
{
	float  pod = JoypadAxisValue;
	float  result = (pod / 32767.0);
	return result;
}

#include "sequencer_memory.hpp"

// TODO:
// Create a C++ map which holds two integers.  
//  a) imdex into the m_axis_latest[] array and 
//  b) Index into the robot m_robot_axes_speed[]
void JoystickPendant::file_AND_convert()
{
	struct  sequencer_ipc_memory_map*  memory = seqIPC.get_memory_seq();
	 
	if ((m_ev.type==JS_EVENT_AXIS) && (m_ev.number<MAX_AXIS))
	{
		// Get Motor Letter : 
		if (m_buttons_latest[PS_LEFT_TOP] == 1)
		{		 
			if (m_ev.number == PS_AXIS_LPOD_UP_DOWN) {
				motor_letter_duty_cycles['x'] = scale_joypad( m_ev.value );		// Though the mapping is the same axis,
				memory->MotorArray[2].duty = scale_joypad( m_ev.value );		// Though the mapping is the same axis,
			}
			else if (m_ev.number == PS_AXIS_RPOD_UP_DOWN) {
				motor_letter_duty_cycles['y'] = scale_joypad( m_ev.value );		// Though the mapping is the same axis,
				memory->MotorArray[3].duty = scale_joypad( m_ev.value );		// Though the mapping is the same axis,				
			}
		}
		else {
			if (m_ev.number == PS_AXIS_LPOD_UP_DOWN) {
				motor_letter_duty_cycles['v'] = scale_joypad( m_ev.value );		// Though the mapping is the same axis,
				memory->MotorArray[0].duty = scale_joypad( m_ev.value );		// Though the mapping is the same axis,				
			}
			else if (m_ev.number == PS_AXIS_RPOD_UP_DOWN) {
				motor_letter_duty_cycles['w'] = scale_joypad( m_ev.value );		// Though the mapping is the same axis,			
				memory->MotorArray[1].duty = scale_joypad( m_ev.value );		// Though the mapping is the same axis,
			}				
		}
	}
}

void JoystickPendant::handle_event()
{
	Joystick::handle_event();

	//printf("Inside pendant handle_event()!\n");
	if ((m_ev.type==JS_EVENT_AXIS) && (m_ev.number<MAX_AXIS))
	{	
		file_AND_convert();
		
		if ( (m_ev.number==1)) {		// POD index (left pod)
			if (m_buttons_latest[PS_LEFT_TOP] )
				m_robot_axes_speed[LEFT_LEG_BASE_INDEX+1] = convert_to_8bit(m_ev.value);
			else if (m_buttons_latest[PS_LEFT_BOTTOM] )
				m_robot_axes_speed[LEFT_LEG_BASE_INDEX+2] = convert_to_8bit(m_ev.value);		
			else
				m_robot_axes_speed[LEFT_LEG_BASE_INDEX+0] = convert_to_8bit(m_ev.value);
		}
		else if ( (m_ev.number==3))		// POD index (right)
		{
			if (m_buttons_latest[PS_RIGHT_TOP] )
				m_robot_axes_speed[RIGHT_LEG_BASE_INDEX+1] = convert_to_8bit(m_ev.value);
			else if (m_buttons_latest[PS_RIGHT_BOTTOM] )
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

