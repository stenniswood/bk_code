
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
	virtual void		Initialize()=0;
	virtual void		draw_motor()=0;
	virtual void		draw_stops()=0;	
	virtual void		draw_current_position()=0;
	virtual void		draw_destination_position()=0;
	
};

class ServoMotorGuiControl : public Control
{
public:
	ServoMotorGuiControl();
	~ServoMotorGuiControl();
	virtual void		Initialize();
	virtual void 		set_width_height  	  ( int Width, int Height 		 );
	virtual void  		move_to	  		  	  ( float Left,   float  Bottom	 );
	virtual void		calc_metrics			( );
		
	virtual void		draw_motor				( );
	virtual void		draw_stops				( );	
	virtual void		draw_current_position	( );
	virtual void		draw_destination_position( );	
	virtual void  		draw_motor_stats		( );
	virtual int			draw					( );	
	
	float			m_range_of_motion;			// in degrees
	float			m_zero_offset;				// zero at center of range of motion.
	float			m_stop1;
	float			m_stop2;
	float 			m_current_pos;
	float 			m_destination;

	// Drawing Info:
	float			m_circle_radius;
	float			m_center_x;
	float			m_center_y;
};

class LinearMotorGuiControl : public MotorGuiBaseControl
{
public:
	LinearMotorGuiControl();
	~LinearMotorGuiControl();
	virtual void		Initialize();
	virtual void 		set_width_height  	  ( int Width, int Height 		 );
	virtual void  		move_to	  		  	  ( float Left,   float  Bottom	 );
	virtual void		calc_metrics			( );

		
	virtual void		draw_motor();
	virtual void		draw_stops();	
	virtual void		draw_current_position();
	virtual void		draw_destination_position();	
	virtual void  		draw_motor_stats();
	virtual int			draw();
	
	float			m_start_x;
	float			m_end_x;
	float			m_travel_bottom;
	
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