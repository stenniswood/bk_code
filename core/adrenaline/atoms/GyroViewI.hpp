#ifndef _GYRO_VIEW_
#define _GYRO_VIEW_

#include "scroll_control.hpp"


class GyroView : public Control 
{
public:
	GyroView( );
	GyroView( int Left, int Right, int Top, int Bottom );
	GyroView( int Width, int Height );

	void			set_angle_deg			( float mDegrees );
	float			get_angle_deg			( );

	virtual void 	Initialize			  	(				 );
	void 			calc_metrics			( 				 );
	void			print_info				(	);
	virtual void 	set_width_height  	 	( int Width, int Height 		);
		
	void 			draw_line	(  );
	void 			draw_circle (  );
	virtual int   	draw   		(  );	
	

protected:
	float		m_angle_radians;
	
private:

	float		m_left_margin;
	float		m_right_margin;

	
};

 
#endif
