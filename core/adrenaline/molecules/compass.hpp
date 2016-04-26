#ifndef _COMPASS_VIEW_
#define _COMPASS_VIEW_

#include "control.hpp"


#ifndef _ICON_VIEW_
struct image_info
{
	unsigned int width;
	unsigned int height;
	unsigned int dstride;
	unsigned int dbpp;
	VGImage  image;	
};
extern VGImage createImageFromJpeg(const char *filename, struct image_info* II);
#endif


class CompassView : public Control 
{
public:
	CompassView( );
	CompassView( int Left, int Right, int Top, int Bottom );
	CompassView( int mCenterX, int CenterY, float mRadius );
	~CompassView();
	
	void			set_heading_angle_deg	( float mDegrees );
	float			get_heading_angle_deg	( );

	virtual void 	Initialize			  	(				 );
	void			load_resources			(				 );
	void 			calc_metrics			( 				 );
	void			find_circle_intersection( float mSlope, float mIntercept, 
											  float& mX, float& mY, bool mPositiveRoot );
	int				handle_incoming_msg		( struct sCAN* msg );

	void			print_info				(				 		);
	virtual void 	set_width_height  	 	( int Width, int Height );

	virtual int		onCreate	  (  );	
	void 			draw_pointer(  );
	void 			draw_dial   (  );
	
	void			draw_heading(  );
	void			draw_info   (  );
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
	bool		m_airplane_mode;		// draw pitch/roll like a ground plane horizon. Else do plumbline.
	float		m_left_margin;
	float		m_right_margin;
};

 
#endif
