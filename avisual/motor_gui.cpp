#include "motor_gui.hpp"



/*******************************************************************************
	Class - LinearMotorGuiControl
*******************************************************************************/

ServoMotorGuiControl::ServoMotorGuiControl()
{
	Initialize(); 
} 
ServoMotorGuiControl::~ServoMotorGuiControl()
{
 
} 
void ServoMotorGuiControl::Initialize()
{

}

void ServoMotorGuiControl::draw_motor()
{
	// Draw circle with dial:  (use an image? like the dials on android apps.  It 
	// would add some flare)
	 
} 
void  ServoMotorGuiControl::draw_stops()
{
 
} 	
void  ServoMotorGuiControl::draw_current_position()
{
 
} 
void  ServoMotorGuiControl::draw_destination_position()
{ 

} 	
void  ServoMotorGuiControl::draw_motor_stats()
{
	// Such as speed, torque, duty, etc.
} 	

int ServoMotorGuiControl::draw()
{
	Control::draw();
	
	draw_motor();
	draw_stops();
	draw_current_position();
	draw_destination_position();
	draw_motor_stats();
	return 1;
}

/*******************************************************************************
	Class - LinearMotorGuiControl
*******************************************************************************/


LinearMotorGuiControl::LinearMotorGuiControl()
{
	Initialize(); 
} 
LinearMotorGuiControl::~LinearMotorGuiControl()
{
 
} 
void LinearMotorGuiControl::Initialize()
{

}

void LinearMotorGuiControl::draw_motor()
{
	// Draw circle with dial:  (use an image? like the dials on android apps.  It 
	// would add some flare)
	 
} 
void  LinearMotorGuiControl::draw_stops()
{
 
} 	
void  LinearMotorGuiControl::draw_current_position()
{
 
} 
void  LinearMotorGuiControl::draw_destination_position()
{ 

} 	
void  LinearMotorGuiControl::draw_motor_stats()
{
	// Such as speed, torque, duty, etc.
} 	

int LinearMotorGuiControl::draw()
{
	Control::draw();
	
	draw_motor();
	draw_stops();
	draw_current_position();
	draw_destination_position();
	draw_motor_stats();
	return 1;	
}

