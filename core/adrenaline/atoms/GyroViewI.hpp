#ifndef _GYRO_VIEW_
#define _GYRO_VIEW_

#include "scroll_control.hpp"


class GyroView : public Control 
{
public:
	GyroView( );
	GyroView( int Left, int Right, int Top, int Bottom );
	GyroView( int mCenterX, int CenterY, float mRadius );
	~GyroView();
	
	void			set_roll_angle_deg		( float mDegrees );
	float			get_roll_angle_deg		( );
	
	void			set_pitch_angle_deg		( float mDegrees );
	float			get_pitch_angle_deg		( );

	void			set_heading_angle_deg	( float mDegrees );
	float			get_heading_angle_deg	( );

	virtual void 	Initialize			  	(				 );
	void 			calc_metrics			( 				 );
	void			print_info				(	);
	virtual void 	set_width_height  	 	( int Width, int Height 		);
		
	void 			draw_line	(  );
	void 			draw_circle (  );
	void			draw_heading(  );
	virtual int   	draw   		(  );	
	

protected:
	float 		m_heading;				// North is 0 degrees.
	float		m_roll_angle_radians;
	float		m_pitch_angle_radians;
	float		m_radius;	
	float		m_center_x;
	float 		m_center_y;
	
	// Line points:
	float		m_delta_x;
	float		m_delta_y;
	float		m_delta_x_heading;
	float		m_delta_y_heading;
	
private:

	float		m_left_margin;
	float		m_right_margin;

	
};

 
#endif
