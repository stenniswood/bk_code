#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "all_objects.h"


const float offset_from_door_edge = 3.0;

glDoorWay::glDoorWay( )
{
    m_width  = 0;
    m_height = 0;
    m_door_type = DOOR_TYPE_DOORWAY_ONLY;
}

glDoorWay::~glDoorWay()
{
    
}

glDoor::glDoor(  )
{
    m_object_class  = 5;
    Initialize();
}

void glDoor::Initialize( )
{
    m_width  = 0;
    m_height = 0;

    m_object_type_name   = "door";
    m_hinge_side_near    = true;
	m_direction_positive = true;
	m_color              = 0xFF5F3F20;
    m_fraction_open      = 0.0;
    m_direction_positive = true;
    m_handle_height      = DEFAULT_HANDLE_HEIGHT;

    m_door_type = DOOR_TYPE_NORMAL_HINGED;
	m_min_angle = 0.;
	m_max_angle = 90.;
}

void glDoor::set_params( float mWidth, float mHeight, float mHandleHeight, float mThick  )
{
    m_width     = mWidth;
    m_height    = mHeight;
    m_thickness = mThick;
    m_handle_height = mHandleHeight;
}

void glDoor::apply_force     ( float radius, float perpendicularForce )
{
    m_R_net_torques += radius * perpendicularForce;    
}

void glDoor::create_handle_path( glRoute& mPath )
{
    mPath.reset();
    mPath.set_color( 0xFF0000FF);

    float NumberSamplesRequested = 10;
    glm::vec4 HandleLocation( m_handle.m_x, m_handle.m_y, m_handle.m_z, 1.0 );
    glm::mat4 door_mat        = get_body_matrix( );
    MathVector data_point(3);
    
    float fraction = 0.0;
    for (int i=0; i<NumberSamplesRequested; i++)
    {
        open( fraction );
        door_mat      = get_body_matrix( );
        glm::vec4  wall_coords = door_mat * HandleLocation;
        data_point[0] = wall_coords[0];
        data_point[1] = wall_coords[1];
        data_point[2] = wall_coords[2];
        mPath.add_sample( data_point );
        fraction += (1.0 / NumberSamplesRequested);
    }
    // The coordinates will be in wall coords.  Ie need to map from wall to world after this.
    
}

void	glDoor::open( float mFraction )
{
	if (mFraction > 1.0) mFraction = 1.0;
	if (mFraction < 0.0) mFraction = 0.0;
	m_fraction_open = mFraction;
	m_y_angle = get_angle();
//    m_handle.m_y_angle = m_y_angle;
}

void	glDoor::close( float mFraction )
{
	if (mFraction > 1.0) mFraction = 1.0;
	if (mFraction < 0.0) mFraction = 0.0;
	m_fraction_open = 1.0 - mFraction;
	m_y_angle       = get_angle();
}

/* Remember the door always rotates around the 0.0 x and z vertex values. 
    m_x,m_z don't change the rotation axis, only location of that axis.  Clear as mud?
    inother words it's the internal m_vertices x,z= 0.0.
    Therefore for a hing on near or far side, all we do is add 180 degrees and swap the fraction open
    to maintain the same positive direction.  This is all under - open_fraction()
 
 */
// Must be called before create() gl_register()!
void	glDoor::set_hinge	( bool mNearSide )
{
    m_hinge_side_near = mNearSide;
    if (mNearSide) {
        m_y_angle = 0;
//        m_door.grab_left();     // 0 is on left side of door;   positive is same in both cases.
    } else {
        m_y_angle = 180;
//        m_door.grab_right();    // 0 is on right side of door   positive is same in both cases.
    }
}

/* pick a side of the wall for positive swing */
// so the open angle will always be positive!
void glDoor::set_swing_side( bool  mPositive_Y_angle )
{
    m_direction_positive = mPositive_Y_angle;
}

float	glDoor::get_angle( )
{
    if (m_hinge_side_near==false)
        m_min_angle = 180;

    float range = (m_max_angle - m_min_angle);
	float angle = (m_fraction_open * range) + m_min_angle;

    if (m_direction_positive==false)
    {
        angle = -angle;
    }    
	return angle;
}

void glDoor::set_range( float mMaxAngle )
{
	m_min_angle = 0.0;
	m_max_angle = mMaxAngle;
}

void glDoor::select_texture  ( int mSelection )
{
    Mat* txt = NULL;
    switch(mSelection)
    {
        case 0 : txt = m_door.load_image( "textures/door_carmelle.jpg");
            break;
        case 1 : txt = m_door.load_image( "textures/door_lagoon.jpg");
            break;
        case 2 : txt = m_door.load_image( "textures/door_steel.jpg");
            break;
        case 3 : txt = m_door.load_image( "textures/door_milano_luna.jpg");
            break;
        case 4 : txt = m_door.load_image( "textures/door_front.jpg");
            break;
        default:
            break;
    }
    ((CuboidTexture*)m_door.m_texture)->apply_front  (txt,3);
    ((CuboidTexture*)m_door.m_texture)->apply_back   (txt,3);
}

void glDoor::create_components( )
{
    m_door.width        = m_width;
    m_door.height       = m_height;
    m_door.depth        = m_thickness;
    m_door.set_color    ( m_color );
    m_door.setup        ( );
    m_door.grab_bottom  ( );
    m_door.grab_back    ( );
    m_door.grab_right   ( );
    //m_door.set_hinge  ( );
    /* Note:  memory could be significantly save by holding the textures in the dwelling_level class
        so that only 1 texture per door style is loaded!
     */
    select_texture( 3 );
    m_components.push_back(&m_door);

    m_handle.setup();
    m_components.push_back(&m_handle);
    m_handle.relocate( m_width-offset_from_door_edge, m_handle_height, m_thickness );
}


