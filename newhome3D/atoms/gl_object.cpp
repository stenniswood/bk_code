
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <list>
#include "vector.hpp"

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gl_object.hpp"
#include "physical_world.h"

//#include "all_objects.h"

static long object_instance_counter = 0;      // each atom will have an id.


glObject::glObject( ) 
{
    m_registered = false;
    m_object_id     = object_instance_counter++;
    m_object_class  = 1;
    
	m_x = 0;
	m_y = 0;
	m_z = 0;
	m_x_angle = 0;
	m_y_angle = 0;
	m_z_angle = 0;
}

glObject::~glObject( )
{
    if (m_registered)
        gl_unregister();
}


void glObject::setup           ( )
{
    
}

void glObject::create          ( )
{
    setup();
    gl_register();
}

void glObject::relocate( double mX, double mY, double mZ )
{
	m_x = mX;
	m_y = mY;
	m_z = mZ;
}

void glObject::relocate( MathVector& mNewLocation )
{
    /*double x= mNewLocation[0];
     double y= mNewLocation[1];
     double z= mNewLocation[2]; */
    //relocate( x,y,z );
    relocate( mNewLocation[0], mNewLocation[1], mNewLocation[2] );
    //relocate(1.0, 2.0, 3.0);
}


void glObject::set_color(  Color mColor    )
{
    m_color  = (mColor.m_alpha<<24);
    m_color += (mColor.m_red  <<16);
    m_color += (mColor.m_green<<8);
    m_color += (mColor.m_blue);
}

void  glObject::set_color( long mColor )
{ 
	m_color = mColor;
}

void 	glObject::stamp_color( struct Vertex& mV )
{
    mV.color[0] = (m_color & 0xFF000000) >> 24;
    mV.color[1] = (m_color & 0x00FF0000) >> 16;
    mV.color[2] = (m_color & 0x0000FF00) >> 8;
    mV.color[3] = (m_color & 0x000000FF);
}

void glObject::print_location()
{
	printf("Object at:  x,y,z = %6.3f,%6.3f,%6.3f \n",
			m_x, m_y, m_z );
}

/*
// Returns a matrix with translate and rotations (same as drawn) for compute purposes.
 INPUTS:    allow different ordering of the rotations (default arguments go rotate X,rotate Y,rotate Z)
*/
glm::mat4 glObject::get_body_matrix( int mOne, int mTwo, int mThree )
{
    glm::vec3 Translate( m_x, m_y, m_z );
    glm::mat4 ViewTranslate = glm::translate( glm::mat4(1.0f), Translate );

    // m_x_angle should be in degrees! 
    float x_angle = m_x_angle*M_PI/180.;
    float y_angle = m_y_angle*M_PI/180.;
    float z_angle = m_z_angle*M_PI/180.;
    glm::mat4 ViewRotate;
    
    switch (mOne)
    {
        case 0: ViewRotate = glm::rotate( ViewTranslate, x_angle, glm::vec3(1.0f, 0.0f, 0.0f) );    break;
        case 1: ViewRotate = glm::rotate( ViewTranslate, y_angle, glm::vec3(0.0f, 1.0f, 0.0f) );    break;
        case 2: ViewRotate = glm::rotate( ViewTranslate, z_angle, glm::vec3(0.0f, 0.0f, 1.0f) );    break;
        default: ViewRotate = ViewTranslate;  break;
    }
    switch (mTwo)
    {
        case 0: ViewRotate = glm::rotate( ViewRotate, x_angle, glm::vec3(1.0f, 0.0f, 0.0f) );    break;
        case 1: ViewRotate = glm::rotate( ViewRotate, y_angle, glm::vec3(0.0f, 1.0f, 0.0f) );    break;
        case 2: ViewRotate = glm::rotate( ViewRotate, z_angle, glm::vec3(0.0f, 0.0f, 1.0f) );    break;
        default: break;
    }
    switch (mThree)
    {
        case 0: ViewRotate = glm::rotate( ViewRotate, x_angle, glm::vec3(1.0f, 0.0f, 0.0f) );    break;
        case 1: ViewRotate = glm::rotate( ViewRotate, y_angle, glm::vec3(0.0f, 1.0f, 0.0f) );    break;
        case 2: ViewRotate = glm::rotate( ViewRotate, z_angle, glm::vec3(0.0f, 0.0f, 1.0f) );    break;
        default: break;
    }
    return ViewRotate;
}

glm::mat4 glObject::get_body_matrix_inverse ( int mOne, int mTwo, int mThree )
{
    glm::vec3 Translate( -m_x, -m_y, -m_z );
    
    // m_x_angle should be in degrees!
    float x_angle = -m_x_angle *M_PI/180.;
    float y_angle = -m_y_angle *M_PI/180.;
    float z_angle = -m_z_angle *M_PI/180.;
    glm::mat4 ViewRotate = glm::mat4(1.0f);

    switch (mOne)
    {
        case 0: ViewRotate = glm::rotate( ViewRotate, x_angle, glm::vec3(1.0f, 0.0f, 0.0f) );    break;
        case 1: ViewRotate = glm::rotate( ViewRotate, y_angle, glm::vec3(0.0f, 1.0f, 0.0f) );    break;
        case 2: ViewRotate = glm::rotate( ViewRotate, z_angle, glm::vec3(0.0f, 0.0f, 1.0f) );    break;
        default:  break;
    }
    switch (mTwo)
    {
        case 0: ViewRotate = glm::rotate( ViewRotate, x_angle, glm::vec3(1.0f, 0.0f, 0.0f) );    break;
        case 1: ViewRotate = glm::rotate( ViewRotate, y_angle, glm::vec3(0.0f, 1.0f, 0.0f) );    break;
        case 2: ViewRotate = glm::rotate( ViewRotate, z_angle, glm::vec3(0.0f, 0.0f, 1.0f) );    break;
        default: break;
    }
    switch (mThree)
    {
        case 0: ViewRotate = glm::rotate( ViewRotate, x_angle, glm::vec3(1.0f, 0.0f, 0.0f) );    break;
        case 1: ViewRotate = glm::rotate( ViewRotate, y_angle, glm::vec3(0.0f, 1.0f, 0.0f) );    break;
        case 2: ViewRotate = glm::rotate( ViewRotate, z_angle, glm::vec3(0.0f, 0.0f, 1.0f) );    break;
        default: break;
    }
    glm::mat4 ViewTranslate  = glm::translate( ViewRotate, Translate );
    return ViewTranslate;
}

MathVector glObject::map_coords ( MathVector mObjectCoordinate )
{
    glm::vec4 oc;
    oc[0] = mObjectCoordinate[0];
    oc[1] = mObjectCoordinate[1];
    oc[2] = mObjectCoordinate[2];
    if (mObjectCoordinate.m_elements.size() == 3 )
        oc[3] = 1.0;
    else
        oc[3] = mObjectCoordinate[2];

    oc = map_coords(oc);
    MathVector result(3);
    result[0] = oc[0];
    result[1] = oc[1];
    result[2] = oc[2];
    return result;
}

MathVector glObject::map_to_object_coords    ( MathVector mParentCoordinate )
{
    glm::vec4 oc;
    oc[0] = mParentCoordinate[0];
    oc[1] = mParentCoordinate[1];
    oc[2] = mParentCoordinate[2];
    oc[3] = 1.0;
    oc = map_coords(oc);
    MathVector result(3);
    result[0] = oc[0];
    result[1] = oc[1];
    result[2] = oc[2];
    return result;
}

glm::vec4 glObject::map_coords ( glm::vec4 mObjectCoordinate )
{
    glm::mat4 object_mat       = get_body_matrix( );
    glm::vec4 world_coords     = object_mat * mObjectCoordinate;
    return    world_coords;
}

glm::vec4 glObject::map_to_object_coords ( glm::vec4 mParentCoordinate )
{
    glm::mat4 inv_obj_mat   = get_body_matrix_inverse();
    glm::vec4 object_coords = inv_obj_mat * mParentCoordinate;
    return    object_coords;
}


glm::vec4 glObject::get_position()
{
    glm::vec4 translate( m_x, m_y, m_z, 1.0 );
    return translate;
}

void glObject::gl_register()
{
    theWorld.add_object(this);
}

void glObject::gl_unregister()
{
    theWorld.remove_object(this);
}

void	glObject::draw_body( )	// Override this with open gl commands.
{
}

void	glObject::draw( )
{ 
	glPushMatrix();

	glTranslatef(m_x, m_y, m_z);
	glRotatef(m_x_angle, 1.0, 0.0, 0.0 );
	glRotatef(m_y_angle, 0.0, 1.0, 0.0 );
	glRotatef(m_z_angle, 0.0, 0.0, 1.0 );
	draw_body();

	glPopMatrix();
}


