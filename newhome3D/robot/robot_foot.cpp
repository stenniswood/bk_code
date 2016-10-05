#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
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

int heel_index ;
int toe_index  ;

void print_foot_position(struct stFootPosition fp)
{
    printf("\nFootPosition:  heel=<%6.3f,%6.3f,%6.3f> \n", fp.heel[0], fp.heel[1], fp.heel[2]  );
    printf("\n            :  toe =<%6.3f,%6.3f,%6.3f> \n", fp.toe[0],  fp.toe[1],  fp.toe[2]   );
    printf("\n            :angle =<%6.3f,%6.3f,%6.3f> \n", fp.angle[0],fp.angle[1],fp.angle[2] );
}

glFootShape::glFootShape()
{
	m_color 	 = 0xFFFFFFFF;
	m_ends_color = 0xFF00FFFF;
	m_is_closed  = true;
	m_extrusion_axis = 3.;
}

void	glFootShape::generate_vertices( )
{
	struct Vertex v;
	v.position[1] =  0.0;
	
	v.position[0] =  3.;        // outside heel (   )
	v.position[2] =  0.;
	m_vertices.push_back( v ); 

	v.position[0] =  3.;        // pinky
	v.position[2] =  9.;
	m_vertices.push_back( v );

	v.position[0] =   1.;       // Big toe
	v.position[2] =  11.;
	m_vertices.push_back( v );

	v.position[0] =   0.;       // Big Toe (Use this vertex as the official "Toe location")
	v.position[2] =  11.;
	m_vertices.push_back( v );          // 3
    toe_index = (int)m_vertices.size()-1;

	v.position[0] =   0.;       // Inside Heel (Use this vertex as the official "Heel location")
	v.position[2] =   0.;
	m_vertices.push_back( v );          // 4
    heel_index = (int)m_vertices.size()-1;
    
/*    v.position[0] =   0.3;       // Inside Heel
    v.position[2] =   0.5;
    m_vertices.push_back( v );

    v.position[0] =   1.0;       // Inside Heel
    v.position[2] =   1.0;
    m_vertices.push_back( v ); */

    add_offset( -1.5, 0.0, 0.0 );
	m_layer_one_vertices = m_vertices.size();
}

size_t 	glFootShape::generate_disc_indices  ( GLuint mStart_of_top_index )
{
	// We're using GL_POLYGON. So no need for the center point.
	int vi;
	for (vi=0; vi<m_layer_one_vertices; vi++)
	{
		m_indices.push_back( vi+mStart_of_top_index );
	}
	//if (CLOSE_OUT)		Should not be needed because GL_POLYGON automatically closes out!
	//	m_indices.push_back(mStartingVertexIndex );
	m_layer_one_indices = m_indices.size();
	return (GLuint)m_indices.size();
}

void	glFootShape::draw_body			   ( )
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

	// Draw Bottom : 
	glDrawElements(GL_POLYGON, (int) m_layer_one_indices, GL_UNSIGNED_INT, 0 );

	// Draw the sides: m_number_side_indices
	glDrawElements(GL_QUAD_STRIP, m_layer_one_vertices*2., GL_UNSIGNED_INT,
				   (GLvoid*)BUFFER_OFFSET( m_layer_one_indices ) );
	// Draw Top : 
	//if (m_is_closed)
	glDrawElements(GL_POLYGON, (int) m_layer_one_indices, GL_UNSIGNED_INT,
					   (GLvoid*)BUFFER_OFFSET(m_layer_one_indices+m_number_side_indices) );
}

/****************************************************************************/

/****************************************************************************/
glFoot::glFoot()
{
    m_ankle_height  =  4 ;        // need real measurement.
    m_foot_length   = 12.;
    m_ankle_to_heel = 3./2.;
}

glm::vec4 glFoot::get_heel_robot_coords()      // foot coords
{
    glm::vec4 heel_location(m_robot_coords.heel[0],
                            m_robot_coords.heel[1],
                            m_robot_coords.heel[2], 1.);
    return heel_location;
}

glm::vec4 glFoot::get_toe_robot_coords()       // foot coords
{
    glm::vec4 toe_location (m_robot_coords.toe[0],
                            m_robot_coords.toe[1],
                            m_robot_coords.toe[2], 1.);
    return toe_location;
}

void glFoot::calc_angle( struct stFootPosition &mfp )
{
    float dX = mfp.toe[0] - mfp.heel[0];
    float dY = mfp.toe[1] - mfp.heel[1];
    float dZ = mfp.toe[2] - mfp.heel[2];
    float distance2D = sqrt(dX*dX + dZ*dZ);

    mfp.angle[0] = atan2(dY, distance2D);
    mfp.angle[1] = atan2(dX, dZ);
    mfp.angle[2] = 0.0;   // can't measure inside to outside!
}

/*void glFoot::make_flat_footed()
{
    float dx = m_world_coords.toe[0] - m_world_coords.heel[0];
    float dy = m_world_coords.toe[1] - m_world_coords.heel[1];
    float dz = m_world_coords.toe[2] - m_world_coords.heel[2];
    float distance = sqrt( dx*dx + dy*dy + dz*dz );
    
    // Find Ankle angle that would make heel and toe Y's equal.
    float angle = asin( dy / distance );
    m_x_angle = angle;
    
    //m_world_coords.toe[0] = m_world_coords.heel[0] + m_foot_length*dx/distance;
    //m_world_coords.toe[1] = m_world_coords.heel[1];
    //m_world_coords.toe[2] = m_world_coords.heel[0] + m_foot_length*dz/distance;
}*/

void glFoot::setup()
{
    const float extrude = 3.;
    m_gl_foot.set_la ( extrude, 1 );
    m_gl_foot.setup  ( );

    m_ankle_to_heel = extrude / 2.;
    m_gl_foot.add_offset ( 0., -m_ankle_to_heel, -2. );
//  m_gl_foot.gl_register( );
    //m_components.push_back( &m_gl_foot );

    // VECTOR OF HEEL/TOE:
    m_foot_coords.heel[0] = m_gl_foot.m_vertices[heel_index].position[0];
    m_foot_coords.heel[1] = 0.0;
    m_foot_coords.heel[2] = m_gl_foot.m_vertices[heel_index].position[2];

    /* Note we choose a toe_vertex which is in the center of the foot.  This has to be in alignment with the heel coordinate because
        we calculate angles of the leg rotation from how the foot is positioned.  To avoid subtraction from a nominal heel and toe
        position (thus simplifying and avoiding some needless complexity) we use heel and toes which are co-linear with the ankle projection.
     */
    
    m_foot_coords.toe[0] = m_gl_foot.m_vertices[toe_index].position[0];
    m_foot_coords.toe[1] = 0.0;
    m_foot_coords.toe[2] = m_gl_foot.m_vertices[toe_index].position[2];
}

MathVector glFoot::get_vector()
{
    MathVector vec(3);
    vec[0] = m_robot_coords.toe[0] - m_robot_coords.heel[0];
    vec[1] = m_robot_coords.toe[1] - m_robot_coords.heel[1];
    vec[2] = m_robot_coords.toe[2] - m_robot_coords.heel[2];
    vec.unitize();
    return vec;
}

/*   */
float glFoot::get_floor_projection_angle(  )
{
    MathVector unit_vector = get_vector();
    return atan2( unit_vector[0], unit_vector[2] );
}

// Y raise/lower
float glFoot::get_elevation_angle(  )
{
    MathVector unit_vector = get_vector();
    return asin(unit_vector[1]);
}

void glFoot::create()
{
    setup();
    gl_register();
}

void glFoot::gl_register     (  )
{
    m_gl_foot.gl_register();
}

void glFoot::gl_unregister   (  )
{
    m_gl_foot.gl_register();
}

void glFoot::draw_body()
{
    m_gl_foot.draw();
}

