/*  A drawer is 3 extrusions plus a face plate (extrusion for now - may be more
	elaborate later)
	
	The drawer may be open [0..100] percent.
	m_x,m_y,m_z specify the closed position.
	It slides along the x axis.  Use rotate 90 deg about y,  if z is desired.
	
	When the drawer is open 100%, it may only be 90% of the depth.  (m_open_limit)
	
*/
#ifndef _DOOR_H_
#define _DOOR_H_

#include <vector>
using namespace std;

const float DEFAULT_THICKNESS   = 2.25;
const float DEFAULT_DOOR_HEIGHT = 79.25;

#include "routing.hpp"


/* 
The drawer positioning code could go either on the drawer, or on the cabinet.

ORIGIN :  Bottom, Left 
*/
class glDoor : public glObject
{
public:
	glDoor(  );

	void			Initialize	( float mWidth, float mHeight=DEFAULT_DOOR_HEIGHT, float mThick=DEFAULT_THICKNESS );
	void			open		( float mFraction );	
	void			close		( float mFraction );

	void			create		( );	// create vertices,indices,VBO/IBO/TBO's
	virtual void	draw_body	( );	// 
	//struct Location get_center_coordinate( );		// go to wall.hpp
	
	glContainer 		m_door_body;
	//glHandle	 		m_handle;	
	
	float				m_fraction_open;
	float				m_max_angle;	// [0..___] degrees

	//float				m_angle;
	// use glObjects inherent m_x_angle instead!
};


#endif
