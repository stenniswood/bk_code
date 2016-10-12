#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "gl_atom.hpp"
#include "global.h"
//#include "bk_system_defs.h"
#include "robot_leg_piece.hpp"

const float shank_radius = 3./16.;
const float round_radius = 3./16.;
const float LEG_THICKNESS= 2.5;
const int   SHANK_SAMPLES= 36.;

#define  TOP_AND_BOTTOM 2
#define  BUFFER_OFFSET(i) ((GLuint*)NULL + (i))


/* 
Outer and Inner could be the same 2D shape!  
just turn them around from each other and make the inner thicker.
*/
glInner::glInner()
:glExtrusion()
{
	m_extrusion_length  = 3.;		// inches
	m_layer_one_indices	= 12;
}


// Cut a hole for the "axle"
void glInner::generate_shank_vertices( )		// 2D
{
	float angle       = 0.0;
	float increment   = 2.*M_PI/SHANK_SAMPLES;
    struct Vertex_pnc v;
    set_vertex_color(v);
    
	for (int i=0; i<SHANK_SAMPLES; i++)	
	{		
		v.position[0] = shank_radius*sin(angle);
		v.position[1] = 0.0;
		v.position[2] = shank_radius*cos(angle);
		m_vertices.push_back( v );
		angle += increment;
	}
	//m_number_of_vertices += SAMPLES;
}

// The outer round piece:
void glInner::generate_round_vertices( )		// 2D
{
	const int SAMPLES = 18;
	// not sure which quadrant we'll need.  Let's try going from [-PI/2 to +PI/2]
	float angle       = -2.*M_PI/4.;			
	float increment   = M_PI/SAMPLES;	// half circle only!
	struct Vertex_pnc v;
    set_vertex_color(v);
	for (int i=0; i<SAMPLES; i++)	
	{
		v.position[0] = round_radius*sin(angle);
		v.position[1] = 0.0;
		v.position[2] = round_radius*cos(angle);
		m_vertices.push_back( v );
		angle += increment;
	}
}

// Center Rectangle : 
void	glInner::generate_layer_vertices( )
{
	m_layer_one_vertices = 13;
	struct Vertex_pnc v;
    set_vertex_color(v);
	
	// Go along bottom first:
	v.position[0] =  0.0;
	v.position[1] =  0.0;
	v.position[2] =  0.0;
	m_vertices.push_back( v );

	v.position[0] =  m_leg_length;
	v.position[1] =  0.;
	v.position[2] =  0.0;
	m_vertices.push_back( v );

	v.position[0] =  m_leg_length;
	v.position[1] =  0.;
	v.position[2] =  LEG_THICKNESS;
	m_vertices.push_back( v );

	v.position[0] =  0.0;
	v.position[1] =  0.0;
	v.position[2] =  LEG_THICKNESS;
	m_vertices.push_back( v );
}

void glInner::generate_vertices( )
{
	generate_shank_vertices();
	generate_round_vertices();
	generate_layer_vertices();
}

GLbyte 	glInner::generate_disc_indices( GLbyte mStartingVertexIndex )
{
	for (int i=0; i<SHANK_SAMPLES; i++)
		m_indices.push_back( i+mStartingVertexIndex );
	return 0;
}

/*************************************************************************************
        glLegSegment
*************************************************************************************/
// Ctor:
glLegSegment::glLegSegment( float mLength )
: glCylinder( 16 )
{
	m_radius              = 2.;
    m_joint_length        = mLength; //LEG_SEGMENT_LENGTH;
    m_extrusion_length    = mLength-FORK_CENTER_LENGTH-TONGUE_CENTER_LENGTH;
	m_number_fork_indices = 0;
}
glLegSegment::~glLegSegment()
{    
}
void glLegSegment::setup( )
{
    glCylinder::set_la( -1.0, 1 );
	glCylinder::setup( );
    
    m_fork_start_vertex   = (int)m_vertices.size();

	//Let the Center of the fork be halfway between these two :
    long color = m_color;
    set_color( 0xFF4f4f4f );
	generate_fork_vertices( TONGUE_WIDTH, m_extrusion_length+0.0 );
	generate_fork_vertices( TONGUE_WIDTH, m_extrusion_length+FORK_CENTER_LENGTH*2 );
    set_color( color );

    generate_fork_indices ( );
	m_stud_start_vertex   = (int)m_indices.size();
    set_color( 0xFF8f8f8f );
    generate_stud_vertices( TONGUE_WIDTH, TONGUE_CENTER_LENGTH, TONGUE_WIDTH*0.95 );		// mRadius, mLength, mWidth
    set_color( color );
    //printf("Total LegSegment vertices = %lu \n", m_vertices.size() );
}

/* 
	The fork piece is a cylinder with a rectangular key cut into it.  
Parameters:  mKeyWidth is the width of the slot (from 1 edge to the next)
			It goes all the way across in the other direction.

The leg segment will consist of a cylinder plus one of these.  OR
								a cylinder plus the stud portion.
We'll not draw any axle or hole between the 2.  Because the translation will align them.
*/
void glLegSegment::generate_fork_vertices( float mKeyWidth, float mYlocation )
{
	struct Vertex v;
    set_vertex_color(v);
	
	float TwoPi     = 2.*M_PI;
	float increment = (TwoPi / ((float)m_number_of_samples));
	int   i;
	float a=0.;
	float x,z;
	float half_key_width = mKeyWidth / 2.0;
	GLuint fi=0;
	int first_add = 0;
	m_number_fork_indices = 0;		// RESET EACH CALL HERE.
	// will hold the number for both sides of the fork!
	
	// We probably want a vertex right on the key width edge too.  The sampling below will
	//		result in inaccuracy of the width.  But this is good enough for now.	
	for (i=0; i<m_number_of_samples; a+=increment, i++)
	{
		x = m_radius * sin(a);	// x starts by going positive.
		z = m_radius * cos(a);
		if ((x<-half_key_width) || (x>half_key_width))
		{
			if (first_add==-1)
				first_add = i;				// Remember for closing.

			v.position[0] =  x;
			v.position[1] =  mYlocation;	// start here
			v.position[2] =  z;		
			m_vertices.push_back( v );
			//printf("fork_vertices: %6.3f %6.3f %6.3f \n", v.position[0], v.position[1], v.position[2] );

			fi = (int)(m_vertices.size()-1);
			m_indices.push_back( fi );
			m_number_fork_indices++;
		}
		else if (first_add>0)  	// look for the first stop now
		{
			m_indices.push_back( first_add );		// close out
			m_number_fork_indices++;			
			first_add = -1;		// reset for other side of the key.
		}
	}
}

void glLegSegment::generate_fork_indices( )
{
	// Side pieces of the fork: 
	int f;
	for (int i=0; i<(m_number_fork_indices); i++ )
	{
		f =  m_fork_start_vertex+i;
		m_indices.push_back( f );
		m_indices.push_back( f+m_number_fork_indices );
		//printf("LegSegment::fork_indices()  %d - %d \t", f, f+m_number_fork_indices );
		//printf(" %6.3f, %6.3f, %6.3f \t", m_vertices[f].position[0], m_vertices[f].position[1], m_vertices[f].position[2] );
		f += m_number_fork_indices;
		//printf(" %6.3f, %6.3f, %6.3f \n", m_vertices[f].position[0], m_vertices[f].position[1], m_vertices[f].position[2] );		
	}
}

/* For the tongue */
void glLegSegment::generate_round_vertices( )
{
	struct Vertex v;
    set_vertex_color(v);
	const int SAMPLES = 18;
	// Not sure which quadrant we'll need.  Let's try going from [-PI/2 to +PI/2]
	float angle       = -2.*M_PI/4.;	
	float increment   = M_PI/SAMPLES;	// half circle only!
	
	for (int i=0; i<SAMPLES; i++)	
	{
		v.position[0] = round_radius*sin(angle);
		v.position[1] = 0.0;
		v.position[2] = round_radius*cos(angle);
		m_vertices.push_back( v );
		angle += increment;
	}
}

void print_vetex(struct Vertex v)
{
	printf(" %6.3f, %6.3f, %6.3f \n", v.position[0], v.position[1], v.position[2] );
}

/* 
	The stud piece will be a rounded rectangle.  ie. half circle plus a rectangle. 
	Here we do the rectangular portion.
*/
void glLegSegment::generate_stud_vertices( float mRadius, float mLength, float mWidth )
{
	struct Vertex   v;
//	v.color[0] = 0x9F;		v.color[1] = 0x7F;		v.color[2] = 0x9F;		v.color[3] = 0x7F;
//  v.color[0] = 0x7F;		v.color[1] = 0x7F;		v.color[2] = 0x7F;		v.color[3] = 0x7F;
    set_vertex_color(v);
	int   i;
	float half_key_width = mWidth / 2.0;
	i = (int)m_vertices.size();

	v.position[0] =  -half_key_width;
	v.position[1] =  0;
	v.position[2] =  mRadius;
	m_vertices.push_back( v );			

	v.position[0] =  half_key_width;
	v.position[1] =  0;
	v.position[2] =  mRadius;
	m_vertices.push_back( v );			

	v.position[0] =  half_key_width;
	v.position[1] =  0;
	v.position[2] =  -mRadius;
	m_vertices.push_back( v );			

	v.position[0] =  -half_key_width;
	v.position[1] =  0;
	v.position[2] =  -mRadius;
	m_vertices.push_back( v );			

	//**************************************
	v.position[0] =  -half_key_width;	// 4
	v.position[1] =  -mLength;
	v.position[2] =  mRadius;
	m_vertices.push_back( v );			

	v.position[0] =  half_key_width;	// 5
	v.position[1] =  -mLength;
	v.position[2] =  mRadius;
	m_vertices.push_back( v );			

	v.position[0] =  half_key_width;	// 6
	v.position[1] =  -mLength;
	v.position[2] =  -mRadius;
	m_vertices.push_back( v );			

	v.position[0] =  -half_key_width;	// 7
	v.position[1] =  -mLength;
	v.position[2] =  -mRadius;			
	m_vertices.push_back( v );
	//printf("Total LegSegment vertices = %d \n", m_vertices.size() );
	
	//**********************************
	// There are 5 sides visible:
	m_indices.push_back( i+0 );		//1 front
	m_indices.push_back( i+1 );
	m_indices.push_back( i+5 );
	m_indices.push_back( i+4 );

	m_indices.push_back( i+4 );		// bottom
	m_indices.push_back( i+5 );
	m_indices.push_back( i+6 );
	m_indices.push_back( i+7 );

	m_indices.push_back( i+3 );		//3 back
	m_indices.push_back( i+2 );
	m_indices.push_back( i+6 );
	m_indices.push_back( i+7 );

	m_indices.push_back( i+3 );		//4
	m_indices.push_back( i+0 );
	m_indices.push_back( i+4 ); 
	m_indices.push_back( i+7 );

	m_indices.push_back( i+2 );		//5
	m_indices.push_back( i+1 ); 
	m_indices.push_back( i+5 );
	m_indices.push_back( i+6 );

	m_number_stud_indices = 4 * 5; 
/*	printf("m_number_stud_indices=%d\t",      m_number_stud_indices);
	printf("m_stud_start_vertex  =%d  %lu\n", m_stud_start_vertex, m_indices.size()  );
	int index;
	for (int i=0; i<m_number_stud_indices; i++)
	{
		index = m_indices[i+m_stud_start_vertex];
		printf("index=%d\t", index);
		print_vetex( m_vertices[index] );
	} */
	// good enough for now.
}

void glLegSegment::draw_body()
{
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	//Establish array contains vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY );

	// We draw the top and bottoms as GL_POLYGON and the sides as GL_QUAD_STRIP.
	// Both sets of indices stored in GL_INDEX_ARRAY.  With a known offset for each 
	// of the 3 sets.
	//glLineWidth( 2.0 );
	
	// Draw Bottom : 
	glDrawElements(GL_POLYGON, (int)m_layer_one_indices, GL_UNSIGNED_INT, 0 );
 
	// Draw the sides: m_number_side_indices
	glDrawElements(GL_QUAD_STRIP, m_layer_one_vertices*2., GL_UNSIGNED_INT,
				   (GLvoid*)BUFFER_OFFSET( m_layer_one_indices ) );
	// Draw Top : 
	size_t offset = m_layer_one_indices+m_number_side_indices;
	if (m_is_closed)
		glDrawElements(GL_POLYGON, (int)m_layer_one_indices, GL_UNSIGNED_INT,
					   (GLvoid*)BUFFER_OFFSET(offset) );

	// Top of fork
	int nfi = ceil(m_number_fork_indices/2.0);
	offset += m_layer_one_indices;
	glDrawElements(GL_POLYGON, nfi, GL_UNSIGNED_INT,  (GLvoid*)BUFFER_OFFSET(offset) );				  
	offset += nfi;
	glDrawElements(GL_POLYGON, nfi, GL_UNSIGNED_INT,  (GLvoid*)BUFFER_OFFSET(offset) );				  

	// Bottom of fork 
	offset += nfi;
	glDrawElements(GL_POLYGON, nfi, GL_UNSIGNED_INT, (GLvoid*)BUFFER_OFFSET(offset) );				   
	offset += nfi;
	glDrawElements(GL_POLYGON, nfi, GL_UNSIGNED_INT, (GLvoid*)BUFFER_OFFSET(offset)  );
	
	// Sides of fork:
	offset += nfi;
	glDrawElements(GL_QUAD_STRIP, m_number_fork_indices, GL_UNSIGNED_INT,
				   (GLvoid*)BUFFER_OFFSET( offset ) );

	offset += m_number_fork_indices;
	glDrawElements(GL_QUAD_STRIP, m_number_fork_indices, GL_UNSIGNED_INT,
				   (GLvoid*)BUFFER_OFFSET( offset ) );

	// Why isn't this drawing?!
	//printf("draw legsegment %d, %d\n", m_number_stud_indices, m_stud_start_vertex );	
	glDrawElements(GL_QUADS, m_number_stud_indices,  GL_UNSIGNED_INT,
				   (GLvoid*)BUFFER_OFFSET( m_stud_start_vertex ) );
}



/***************************************************************************/
/*************************   glLeg   ***************************************/
/***************************************************************************/
glLeg::glLeg()
:m_upper_leg(UPPER_LEG_LENGTH),
m_lower_leg (LOWER_LEG_LENGTH)
{
	m_hip_fb_swing_angle = 0.;
	m_hip_rotate_angle   = 0.;
	m_knee_angle	     = 0.;
	m_ankle_angle	     = 0.;
}

void glLeg::Initialize( )
{
	// From center of torso :
    m_upper_leg.m_color      = 0xFFFF0000;
    m_upper_leg.m_ends_color = 0xFFFF0000;
    m_lower_leg.m_color      = 0xFF0000FF;
    m_lower_leg.m_ends_color = 0xFF0000FF;
    m_free_hanging           = true;    
}

void glLeg::create_components()
{
	Initialize();
    m_upper_leg.set_color(0xFFFF0000);
	m_upper_leg.setup();
    
    m_lower_leg.set_color(0xFFFF0000);
    m_lower_leg.setup();
    
    m_foot.set_color(0xFFFF9F00);
    m_foot.setup(  );

    m_components.push_back( &m_upper_leg );
    m_components.push_back( &m_lower_leg );
    m_components.push_back( &m_foot      );
    
	float offset = m_upper_leg.m_extrusion_length + FORK_CENTER_LENGTH;
	m_upper_leg.add_offset( 0, -offset, 0 );    // ie hold at top.

	// Want it to pivot at the knee:
	offset = m_lower_leg.m_extrusion_length + FORK_CENTER_LENGTH;
	m_lower_leg.add_offset( 0, -offset, 0 );    // ie hold lower leg at its top.

	offset += TONGUE_CENTER_LENGTH;
    m_upper_leg.relocate( 0.0,  0.0,    0.0 );
    m_lower_leg.relocate( 0.0, -UPPER_LEG_LENGTH,  0.0 );
    
    // offset += TONGUE_CENTER_LENGTH;
    m_foot.relocate( 0.0, -offset, 0.0);
}

/* 
return:		true if all okay
*/
bool glLeg::check_angle_limits( )
{
	if (m_knee_angle<MIN_KNEE_ANGLE)  		return false;
	if (m_knee_angle>MAX_KNEE_ANGLE)  		return false;

	if (m_hip_fb_swing_angle<MIN_HIP_ANGLE)  		return false;
	if (m_hip_fb_swing_angle>MAX_HIP_ANGLE)  		return false;

	if (m_hip_rotate_angle<MIN_HIP_ROTATE_ANGLE)  return false;
	if (m_hip_rotate_angle>MAX_HIP_ROTATE_ANGLE)  return false;

	if (m_ankle_angle<MIN_ANKLE_ANGLE)  	return false;
	if (m_ankle_angle>MAX_ANKLE_ANGLE)  	return false;

	return true;
}

bool	glLeg::set_knee_angle	( float mDegrees )
{
	if (mDegrees<MIN_KNEE_ANGLE)  return false;
	if (mDegrees>MAX_KNEE_ANGLE)  return false;
	
	m_knee_angle = mDegrees;
	return true;
}
bool	glLeg::set_hip_angle	( float mDegrees )
{
	if (mDegrees<MIN_HIP_ANGLE)  return false;
	if (mDegrees>MAX_HIP_ANGLE)  return false;
	
	m_hip_fb_swing_angle = mDegrees;
	return true;
}
bool	glLeg::set_hip_rotate_angle( float mDegrees )
{
	if (mDegrees<MIN_HIP_ROTATE_ANGLE)  return false;
	if (mDegrees>MAX_HIP_ROTATE_ANGLE)  return false;
	m_hip_rotate_angle = mDegrees;
	return true;
}
bool	glLeg::set_hip_sideways_swing_angle( float mDegrees )
{
    if (mDegrees<MIN_HIP_SS_SWING_ANGLE)  return false;
    if (mDegrees>MAX_HIP_SS_SWING_ANGLE)  return false;
    m_hip_ss_swing_angle = mDegrees;
    return true;
}
bool	glLeg::set_ankle_angle( float mDegrees )
{
	if (mDegrees<MIN_ANKLE_ANGLE)  return false;
	if (mDegrees>MAX_ANKLE_ANGLE)  return false;
	
	m_ankle_angle = mDegrees;
	return true;	
}	

bool	glLeg::increase_knee_angle	( float mRelativeAngle )
{
	m_knee_angle += mRelativeAngle;
	if (m_knee_angle<MIN_KNEE_ANGLE)  m_knee_angle = MIN_KNEE_ANGLE;
	if (m_knee_angle>MAX_KNEE_ANGLE)  m_knee_angle = MAX_KNEE_ANGLE;
	return true;
}
bool	glLeg::increase_hip_angle	( float mRelativeAngle )
{
	m_hip_fb_swing_angle += mRelativeAngle;
	if (m_hip_fb_swing_angle<MIN_HIP_ANGLE)  m_hip_fb_swing_angle = MIN_HIP_ANGLE;
	if (m_hip_fb_swing_angle>MAX_HIP_ANGLE)  m_hip_fb_swing_angle = MAX_HIP_ANGLE;
	return true;
}

bool	glLeg::increase_hip_rotate_angle( float mRelativeAngle )
{
	m_hip_rotate_angle += mRelativeAngle;	
	if (m_hip_rotate_angle<MIN_HIP_ROTATE_ANGLE)  m_hip_rotate_angle = MIN_HIP_ROTATE_ANGLE;
	if (m_hip_rotate_angle>MAX_HIP_ROTATE_ANGLE)  m_hip_rotate_angle = MAX_HIP_ROTATE_ANGLE;
	return true;
}
bool	glLeg::increase_hip_swing_angle( float mRelativeAngle )
{
    m_hip_ss_swing_angle += mRelativeAngle;
    if (m_hip_ss_swing_angle<MIN_HIP_ROTATE_ANGLE)  m_hip_ss_swing_angle = MIN_HIP_ROTATE_ANGLE;
    if (m_hip_ss_swing_angle>MAX_HIP_ROTATE_ANGLE)  m_hip_ss_swing_angle = MAX_HIP_ROTATE_ANGLE;
    return true;
}

bool glLeg::increase_ankle_angle( float mRelativeAngle )
{
	m_ankle_angle += mRelativeAngle;	
	if (m_ankle_angle < MIN_ANKLE_ANGLE)  m_ankle_angle = MIN_ANKLE_ANGLE;
	if (m_ankle_angle > MAX_ANKLE_ANGLE)  m_ankle_angle = MAX_ANKLE_ANGLE;
	return true;
}

float glLeg::get_length()  // from hip to heel
{
    // can modify this later for when the knee is bent.
    // but it works okay for our robot.place heel at (since the leg is straight)
    return ( m_upper_leg.m_joint_length + m_lower_leg.m_joint_length + m_foot.m_ankle_height );
}

/* Computation can be reduced significantly by storing the matrices.  Marking them 
    invalid and recomputing only when a change occurs.  */
glm::vec4 glLeg::map_foot_to_robot_coords( glm::vec4 mFootCoord )
{
    glm::vec4 leg_coord   = map_foot_to_leg_coords( mFootCoord );
    glm::mat4 leg_matrix  = get_body_matrix( );      // only translates
    glm::vec4 robot_coord = leg_matrix * leg_coord;
    return robot_coord;
}

/* Computation can be reduced significantly by storing the matrices.  Marking them
 invalid and recomputing only when a change occurs.  */
glm::vec4 glLeg::map_foot_to_leg_coords( glm::vec4 mFootCoord )
{
    transfer_angles();
    glm::mat4 upper_leg_mat    = m_upper_leg.get_body_matrix( 1,0,2 );  // rotates for the hip
    glm::mat4 lower_leg_mat    = m_lower_leg.get_body_matrix( );        // is rotate and lower leg length
    glm::mat4 foot_mat         = m_foot.get_body_matrix     ( );
    glm::mat4 combined_mat     = upper_leg_mat * lower_leg_mat * foot_mat;

//    glm::vec4 leg_coord  = upper_leg_mat * mFootCoord;
    glm::vec4 leg_coord  = combined_mat * mFootCoord;
    return leg_coord;
}

glm::vec4 glLeg::map_robot_to_foot_coords( glm::vec4 mRobotCoord )
{
    glm::mat4 leg_matrix  = get_body_matrix_inverse( );      // only translates
    glm::vec4 leg_coord   = leg_matrix * mRobotCoord;

    glm::vec4 foot_coord   = map_leg_to_foot_coords( leg_coord );
    return foot_coord;
}
glm::vec4 glLeg::map_leg_to_foot_coords( glm::vec4 mLegCoord )    // result in m_robot_coords
{
    glm::mat4 upper_leg_mat    = m_upper_leg.get_body_matrix_inverse( 1,0,2 );  // rotates for the hip
    glm::mat4 lower_leg_mat    = m_lower_leg.get_body_matrix_inverse( );        // is rotate and lower leg length
    glm::mat4 foot_mat         = m_foot.get_body_matrix_inverse     ( );
    glm::mat4 combined_mat     = foot_mat * lower_leg_mat * upper_leg_mat;
    
    glm::vec4 foot_coord  = combined_mat * mLegCoord;
    return foot_coord;
}


/* Gets the HEEL position in robot coordinates and stores this in the foot. */
glm::vec4 glLeg::get_knee_in_robot_coords( )
{
    transfer_angles();
    // Start in Foot Coordinates:
    glm::vec4 knee_location( 0.0, -m_upper_leg.m_joint_length, 0.0, 1. );     // upper leg coords
    glm::mat4 upper_leg_mat  = m_upper_leg.get_body_matrix( 1,0,2 );          // upper leg -> leg coords
    glm::vec4 knee_location2 = upper_leg_mat * knee_location;
    glm::vec4 knee_location3 = map_coords( knee_location2 );

    //glm::vec4 knee_in_robot_coords = map_foot_to_robot_coords( knee_location );
    return knee_location3;
}

/* Gets the HEEL position in robot coordinates and stores this in the foot. */
glm::vec4 glLeg::get_heel_in_robot_coords( )
{
    // Start in Foot Coordinates:
    glm::vec4 heel_location( m_foot.m_foot_coords.heel[0], m_foot.m_foot_coords.heel[1], m_foot.m_foot_coords.heel[2], 1. );
    glm::vec4 heel_in_robot_coords = map_foot_to_robot_coords( heel_location );
    
    // ALSO STORE IN FOOT :
    m_foot.m_robot_coords.heel[0] = heel_in_robot_coords[0];
    m_foot.m_robot_coords.heel[1] = heel_in_robot_coords[1];
    m_foot.m_robot_coords.heel[2] = heel_in_robot_coords[2];
    return heel_in_robot_coords;
}

/* Gets the TOE position in robot coordinates and stores this in the foot. */
glm::vec4 glLeg::get_toe_in_robot_coords( )
{
    glm::vec4 toe_location ( m_foot.m_foot_coords.toe[0],  m_foot.m_foot_coords.toe[1],  m_foot.m_foot_coords.toe[2],  1. );
    glm::vec4 toe_in_robot_coords = map_foot_to_robot_coords( toe_location );

    // ALSO STORE IN FOOT :
    m_foot.m_robot_coords.toe[0] = toe_in_robot_coords[0];
    m_foot.m_robot_coords.toe[1] = toe_in_robot_coords[1];
    m_foot.m_robot_coords.toe[2] = toe_in_robot_coords[2];
    return toe_in_robot_coords;
}

float glLeg::get_lower_leg_angle_wrt_gravity()
{
    // The m_hip_fb_swing_angle angles are negative in front of the body.  and Positive behind.  No!
    // The knee angles are always positive.
    
    //float result = (m_hip_fb_swing_angle - m_knee_angle);
    float result = (m_hip_fb_swing_angle + m_knee_angle);
    return result;
}

static double radians(double deg)
{
    return deg * M_PI / 180.0;
}

// distance from hip to heel vertically
float glLeg::get_vertical_height( bool mAnkleOnly )
{
    float retval = 0.0;
    // UPPER VERTICAL :
    float angle        = radians(m_hip_fb_swing_angle);
    float upper_height = m_upper_leg.m_joint_length * cos(angle);
    retval += upper_height;


    // LOWER VERTICAL :
    angle = radians (get_lower_leg_angle_wrt_gravity());
    float lower_height = m_lower_leg.m_joint_length * cos(angle);
    retval += lower_height;
    
    // FOOT VERTICAL :
    if (mAnkleOnly==false)
    {
        angle += m_ankle_angle;
        float foot_height = m_foot.m_ankle_to_heel * cos(angle);
        retval += foot_height;
    }
    return retval;
}

float glLeg::get_fore_aft( bool mHeelOnly )             // distance from hip to heel horizontally
{
    // UPPER VERTICAL :
    float angle      = - radians(m_hip_fb_swing_angle);
    float upper_fore = m_upper_leg.m_joint_length * sin(angle);

    // LOWER VERTICAL :
    angle            = - radians( get_lower_leg_angle_wrt_gravity() );      // Positive angles defined for knee bends.  so subtract not add!
    float lower_fore = m_lower_leg.m_joint_length * sin(angle);
    float retval     = upper_fore + lower_fore;

    // FOOT VERTICAL :
    if (mHeelOnly==false)  // ie not toes
    {
        angle  += radians(m_ankle_angle);
        float foot_fore = m_foot.m_foot_length * sin(angle);
        retval += foot_fore;
    }
    return retval;
}

// given that the foot is firmly planted.  where is the hip?  (robot coordinates)
MathVector glLeg::get_hip_anchor_position( MathVector mHeelPosition )
{
    MathVector anchor(3);
//    mHeelPosition
    return anchor;
}

/* This will set the ankle motor pivot point to position leg_coords (x,y,z)
        ie. the ankle or position along the foot is not calculated.
 
 */
void glLeg::inverse_kinematic_ankle_2d( MathVector leg_coord )
{
    MathVector delta = leg_coord - m_position;
    
    /* Are there multiple solutions or just one?  The sum of 2 vectors:
            of predetermined lengths.
     
     */
    // DEG #1 Hip Swing:  (split the height)
    float dh = delta[2] / 2.0;
    //float dx = delta[0] / 2.0;
    m_hip_ss_swing_angle = atan2( dh, dh );
    
    
    // possible knee positions:
    
    // DEG #2 Knee Swing:
    m_knee_angle = 0;
    
    
}
void glLeg::inverse_kinematic_ankle_3d ( MathVector leg_coord )
{
    
    
}

void glLeg::place_heel_at( glm::vec4 leg_coords )
{

}


void glLeg::make_foot_level(  )       // with ground
{
    // knee angle will always be positive!
    // Ankle angle is positive when ... 
    // so ankle should be :
    /*  +20 +20 ==>  +0
        +20 +10 ==>  +10
        +10 +10 means  0
        -10 +10 means 20;
        -20 +10 means 30;
    */
    float angle = get_lower_leg_angle_wrt_gravity();
    set_ankle_angle( angle );
}

void glLeg::standing_position( )
{
    set_hip_angle( 0.0 );
    set_knee_angle( 0.0 );
    set_ankle_angle( 0.0 );
    set_hip_rotate_angle( 0.0 );
}

void glLeg::transfer_angles()
{
    m_hip_ss_swing_angle = 0.0;
    m_lower_leg.m_y_angle = 0.0;
    m_lower_leg.m_z_angle = 0.0;    

    //    m_upper_leg.m_x_angle = -m_hip_fb_swing_angle;
    //    m_upper_leg.m_y_angle = m_hip_rotate_angle;
    //    m_lower_leg.m_x_angle = m_knee_angle;
    //    m_foot.m_x_angle      = m_ankle_angle;
}

void glLeg::draw_body( )
{
	glPushMatrix();
    transfer_angles();

    glTranslatef( m_upper_leg.m_x, m_upper_leg.m_y, m_upper_leg.m_z );
    glRotatef   ( m_hip_rotate_angle,   0.0, 1.0,  0.0 );
    glRotatef   ( m_hip_fb_swing_angle, 1.0, 0.0,  0.0 );
    //glRotatef ( m_hip_ss_swing_angle, 0.0, 0.0,  1.0 );     // add later!
	m_upper_leg.draw_body();

	glTranslatef( m_lower_leg.m_x,   m_lower_leg.m_y, m_lower_leg.m_z );
	glRotatef   ( m_knee_angle, 1.0, 0.0, 0.0 );
    m_lower_leg.draw_body();

    glTranslatef( m_foot.m_x,   m_foot.m_y,  m_foot.m_z );
    glRotatef   ( -m_ankle_angle, 1.0, 0.0, 0.0 );
	m_foot.draw_body();

	glPopMatrix();
}

