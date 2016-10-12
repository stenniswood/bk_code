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

const float DEFAULT_THICKNESS     = 2.25;
const float DEFAULT_DOOR_HEIGHT   = 79.25;
const float DEFAULT_HANDLE_HEIGHT = 35;

const float DEFAULT_SOLID_CORE_WEIGHT = 77;     // lbs (~35Kg)
const float DEFAULT_HALLOW_CORE_WEIGHT = 25;    // lbs


#include "routing.hpp"
#include "door_handle.h"
//#include "gl_container.hpp"
#include "hinge.h"
#include "route.hpp"
#include "face_box.h"

const int DOOR_TYPE_DOORWAY_ONLY  = 0;
const int DOOR_TYPE_NORMAL_HINGED = 1;
const int DOOR_TYPE_BIFOLD        = 2;
const int DOOR_TYPE_SLIDING       = 3;

/* This is an empty doorway - no door present.  It serves as a place holder however, so that
 m_bare_wall and m_fwall are always in sync.  When passing thru a door way some routines
 return a handle to something.
 
 */
class glDoorWay : public glMolecule
{
public:
    glDoorWay( );
    ~glDoorWay();
    
    float      m_width;
    float      m_height;
    int        m_door_type;
};

/* 
The drawer positioning code could go either on the drawer, or on the cabinet.

This would be better as an object derived from glAtom wouldn't it?

ORIGIN :  Bottom, Left 
    Should really be a glMolecule!
*/
class glDoor : public glDoorWay
{
public:
	glDoor(  );
    void            set_params  (float mWidth,
                                 float mHeight       = DEFAULT_DOOR_HEIGHT,
                                 float mHandleHeight = DEFAULT_HANDLE_HEIGHT,
                                 float mThick        = DEFAULT_THICKNESS );

    void            apply_force     ( float radius, float perpendicularForce );
    void            create_handle_path( glRoute& mPath );

	void			Initialize      (                 );
	void			set_hinge       ( bool  mNearSide = true            );
    void			set_swing_side  ( bool  mPositive_Y_angle = true    );  // so the open angle will always be positive!
    void			set_range       ( float mMaxAngle  );
	float			get_angle       (                 );                    // interpolates based on fraction open
	void			open            ( float mFraction );
	void			close           ( float mFraction );
    void            select_texture  ( int mSelection  );
    
    void            create_components( );

    glFaceBox           m_door;
    glDoorHandle	 	m_handle;
    glHinge             m_hinge;

    float               m_thickness;
    float               m_handle_height;

	float				m_fraction_open;        // have to use the open() function!
	float				m_min_angle;            // Always 0 (closed)
	float				m_max_angle;            //
	bool				m_direction_positive;	// changes swing side.
    bool                m_hinge_side_near;
};




#endif
