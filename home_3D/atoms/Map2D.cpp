#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"

static float radians( float Degrees)
{
	return Degrees * M_PI / 180.0;	
}



glMap2D::glMap2D(  )
{
	
}
glMap2D::~glMap2D(  )
{
	
}
void glMap2D::scale_drawing(float mScale)
{
	for (int v=0; v<m_vertices.size(); v++)
	{
		m_vertices[v].position[0] *= mScale;
		m_vertices[v].position[2] *= mScale;
	}
}

void glMap2D::compute_corners()
{
	// COMPUTE CORNERS:
	for (int w=0; w<m_awalls.size(); w++)
	{
		printf("WALL #%2d\n", w );
		m_awalls[w].extract_corners( &m_awalls );
		m_awalls[w].print_corners();
	}
}


void glMap2D::create_2D_mark( float mStartx, float mStarty, long int mColor )
{
	struct Vertex v;
	v.color[0] = (mColor & 0xFF000000)>>24;	
	v.color[1] = (mColor & 0x00FF0000)>>16;	
	v.color[2] = (mColor & 0x0000FF00)>>8;	
	v.color[3] = (mColor & 0x000000FF);
	v.position[1] = 2.0;	// For all points!  Since 2D.
	v.position[0] = mStartx;	v.position[2] = mStarty-5;
	m_vertices.push_back( v );
	m_indices.push_back( m_vertices.size()-1 );
	
	v.position[0] = mStartx;	v.position[2] = mStarty+5;
	m_vertices.push_back( v );		
	m_indices.push_back( m_vertices.size()-1 );
	
	v.position[0] = mStartx-5;	v.position[2] = mStarty;
	m_vertices.push_back( v );		
	m_indices.push_back( m_vertices.size()-1 );
	
	v.position[0] = mStartx+5;	v.position[2] = mStarty;
	m_vertices.push_back( v );		
	m_indices.push_back( m_vertices.size()-1 );
}
// SHORTCUT(ALIAS):
#define Wall m_awalls[w]

/* Creates Vertex  from the Wall 3D data */
void glMap2D::create_2D_drawing()
{
	struct Vertex v;
	v.color[0] = 0xFF;	v.color[1] = 0xFF;	v.color[2] = 0xCF;	v.color[3] = 0xFF;
	v.position[1] = 1.0;	// For all points!  Since 2D.
	float rise;
	float run;
	float start_x;
	float start_y;

	/* The wall from 2D point of view consists of:
			<m_x, m_z>			and
			m_angle 			(in degrees)
			m_wall_length		(in inches )			
	*/
	for (int w=0; w<m_awalls.size(); w++)
	{	
		// Starting Vertex:
		start_x = v.position[0] = Wall.m_x;
		start_y = v.position[2] = Wall.m_z;
		//printf("sx=%6.2f; sy=%6.2f;\n", start_x, start_y );

		m_vertices.push_back( v );
		m_indices.push_back( m_vertices.size()-1 );		

		float rise = sin( -radians(Wall.m_angle) ) * Wall.m_wall_length;
		float run  = cos( -radians(Wall.m_angle) ) * Wall.m_wall_length;		// yes
		//printf("rise=%6.2f;  run=%6.2f;\n", rise, run );

		// interpolate distance along the wall to the distance along the rise,run		
		for (int d=0; d<m_awalls[w].m_doors.size(); d++)
		{
			// Front edge of door:
			float fraction = Wall.m_doors[d].PositionLengthwise / Wall.m_wall_length;
			v.position[0] = run  * fraction + Wall.m_x;
			v.position[2] = rise * fraction + Wall.m_z;
			m_vertices.push_back( v );
			m_indices.push_back( m_vertices.size()-1 );			

			// Far edge of door:
			float fraction2 = (Wall.m_doors[d].PositionLengthwise+Wall.m_doors[d].Width) / Wall.m_wall_length;			
			v.position[0] = run  * fraction2 + Wall.m_x;
			v.position[2] = rise * fraction2 + Wall.m_z;
			m_vertices.push_back( v );
			m_indices.push_back( m_vertices.size()-1 );
		}
		
		// Calculate end point.
		v.position[0] =  + run	+ Wall.m_x;
		v.position[2] =  + rise	+ Wall.m_z;
		m_vertices.push_back( v );								// Yes.		
		m_indices.push_back( m_vertices.size()-1 );
	}  
	create_2D_mark( 144+58, 75, 0xFFFF0000 );
	create_2D_mark( 40,    250, 0xFF00FF00 );	
	scale_drawing( 0.25 );
}

void glMap2D::draw_corners( )
{
	int w=0;
	int num_corners = m_awalls[w].m_corners.size();
	float x,y;
	for (int c=0; c < num_corners; c++)
	{
		x = m_awalls[w].m_corners[c].x;
		y = m_awalls[w].m_corners[c].z;
		create_2D_mark( x, y, 0xFFFF0000 );	
	}
}


void glMap2D::print_2D_info( )
{
	printf("============ Abstract Wall Info ====================\n");
	for (int w=0; w<m_awalls.size(); w++)
	{	printf("WALL #%2d\t", w );
		m_awalls[w].print_info();
	}
	for (int w=0; w<m_awalls.size(); w++)
	{	printf("WALL #%2d\n", w );
		m_awalls[w].print_dw_info();
	}
	printf("============ Abstract Wall Info End =================\n");		
}


void glMap2D::draw_body()
{
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer( GL_ARRAY_BUFFER, 		   m_VBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));
	
	//Establish array contains m_vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY );

	// Draw Bottom :  
	glDrawElements(GL_LINES, m_indices.size(), GL_UNSIGNED_BYTE, 0 );
}


