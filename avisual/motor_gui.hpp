
#include "control.hpp"
/*
Should have modes or separate classes for 
	Linear control
	Servo style control   and
	DC motor (opt with encoders)
	
Also should graphically show software & hardware stops.
	Current position,
	Destination position with an arc showing the distance to travel.
	lights up when the mechanical stop is pushed.


*/
class MotorGuiBaseControl : public Control
{
public:
	MotorGuiBaseControl();
	~MotorGuiBaseControl();
	virtual void		Initialize();
	virtual void		draw_motor()=0;
	virtual void		draw_stops()=0;	
	virtual void		draw_current_position()=0;
	virtual void		draw_destination_position()=0;
	
};

class ServoMotorGuiControl : public MotorGuiBaseControl
{
public:
	ServoMotorGuiControl();
	~ServoMotorGuiControl();
	virtual void		Initialize();
		
	virtual void		draw_motor();
	virtual void		draw_stops();	
	virtual void		draw_current_position();
	virtual void		draw_destination_position();	
	virtual void  		draw_motor_stats();
	virtual int			draw();	
};

class LinearMotorGuiControl : public MotorGuiBaseControl
{
public:
	LinearMotorGuiControl();
	~LinearMotorGuiControl();
	virtual void		Initialize();
		
	virtual void		draw_motor();
	virtual void		draw_stops();	
	virtual void		draw_current_position();
	virtual void		draw_destination_position();	
	virtual void  		draw_motor_stats();
	virtual int			draw();
	
};



/* WAS:
class MotorGuiControl : public Control
{
public:

	MotorGuiControl();
	~MotorGuiControl();
	
	void		draw_servo();	
	void		draw_linear();
	
	void		draw_servo_stops();
	void		draw_linear_stops();
	
	void		draw_servo_current_position();
	void		draw_linear_current_position();
};
*/