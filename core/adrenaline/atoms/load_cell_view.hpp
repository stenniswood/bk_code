#ifndef _GYRO_VIEW_
#define _GYRO_VIEW_

#include "scroll_control.hpp"
/* This shows 4 load cells in a "foot" pattern.  ie four rectangles with color
	intensity (red/blue) representing positive/negative force on the load cell.
	
	4 Sensors make up 1 foot. So this will show how the weight is distributed on the foot.
	Ankles & toes - left and right side of foot.
*/

class LoadCellView : public Control 
{
public:
	LoadCellView( );
	LoadCellView( int Left, int Right, int Top, int Bottom );
	~LoadCellView();
	
	void			set_sensor_value	( int mSensorNumber, float mPoundsofForce );
	float			get_sensor_value	( int mSensorNumber);
	
	virtual void 	Initialize			  	(				 );
	void 			calc_metrics			( 				 );
	void			print_info				(	);
	virtual void 	set_width_height  	 	( int Width, int Height 		);
		
	void 			draw_a_sensor(  );
	virtual int   	draw   		(  );	
	

protected:
	float 		m_sensor_values[4];				// North is 0 degrees.

	float		m_sensor_left[4];
	float		m_sensor_bottom[4];
	
	float 		m_sensor_width;		// the rectangle representing the sensor.
	float 		m_sensor_height;	
		
private:
	float		m_left_margin;
	float		m_right_margin;

	
};

 
#endif
