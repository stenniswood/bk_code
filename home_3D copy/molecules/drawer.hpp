/*  A drawer is 3 extrusions plus a face plate (extrusion for now - may be more
	elaborate later)
	
	The drawer may be open [0..100] percent.
	m_x,m_y,m_z specify the closed position.
	It slides along the x axis.  Use rotate 90 deg about y,  if z is desired.
	
	When the drawer is open 100%, it may only be 90% of the depth.  (m_open_limit)
	
*/
#ifndef _DRAWER_H_
#define _DRAWER_H_

#include <vector>
using namespace std;


const float DEFAULT_SIDE_THICKNESS   = 0.325;
const float DEFAULT_BOTTOM_THICKNESS = 0.125;

/* 
The drawer positioning code could go either on the drawer, or on the cabinet.

The Drawer Origin is located:


*/
class glDrawer : public glObject
{
public:
	glDrawer(  );

	// depth is front to back.
	void	Initialize			( float mDepth, float mWidth, float mHeight );	
	void	initialize_faceplate( float mWidth, float mHeight, float mThick );

	void			open		( float mFraction );	
	void			close		( float mFraction );	

	float			get_width (  );
	
	void 			grab_left (  );
	void 			grab_right(  );

	void 			setup		( );
	void			create		( );	// create vertices,indices,VBO/IBO/TBO's
	virtual void	draw_body	( );	// 

	glContainer 		m_bottom;
	glContainer 		m_left_side;	
	glContainer 		m_right_side;		
	glContainer 		m_facePlate;	
	glContainer 		m_back;	
	//glHandle	 		m_handle;	
	
	float				m_fraction_open;
	float				m_travel_distance;		// Most it can move (inches)
	float				m_distance_open;
	
	float	m_depth;		// Front to back.
	float	m_width;		// Left to right.
	float	m_height;		// top to bottom (measured by side pieces)
		
	float 	m_side_thickness;
	float	m_bottom_thickness;
};


#endif
