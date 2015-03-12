#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"



#define  CLOSE_OUT 		1
#define  TOP_AND_BOTTOM 2
#define  BUFFER_OFFSET(i) ((GLuint*)NULL + (i))

#define Debug 0


glRoute::glRoute( )
{ 
	m_show_robot_feet = false;

	m_start.position[0] = 50.;
	m_start.position[2] = 0.;

	m_end.position[0] = 18.;
	m_end.position[2] = 144.+122.+50.;
		
	// IBO / VBO:
	//m_number_path_indices = 0;
	m_number_robot_indices= 0;	

	m_number_path_vertices = 0;
	m_number_robot_vertices = 0;
}

void glRoute::start_over_at( struct Vertex mStart )
{
	m_vertices.clear();
	m_vertices.push_back( mStart );
	m_number_path_vertices = m_vertices.size();	
}

/* Compute samples from current location (last vertex) to
	The specified location.  Sampling every 2 inches
*/
void glRoute::add_way_point( struct Vertex mWay )
{
	int lsize = m_vertices.size();
	struct stRiseRun rr;
	struct Vertex v;

	// Initialize with Last Known Position : 
	memcpy (v.position, m_vertices[lsize-1].position, sizeof(struct Vertex) );
	v.color[0] = 0xFF;	v.color[1] = 0xFF;	v.color[2] = 0;	v.color[3] = 0; 
	v.position[1] = 2.0;

	rr.rise = ( mWay.position[0] - v.position[0] );
	rr.run  = ( mWay.position[2] - v.position[2] );

    // Normalize rr : 
    float magnitude  = sqrt( rr.rise*rr.rise + rr.run*rr.run );
	float Xincrement = 2.0 * rr.rise / magnitude;
	float Yincrement = 2.0 * rr.run / magnitude;
	int samples = floor(fabs(magnitude/2.));

	for (int i=0; i<samples; i++)
	{
		v.position[0] += Xincrement;
		v.position[2] += Yincrement;
		m_vertices.push_back( v );
	}
	m_number_path_vertices = m_vertices.size();
}

void glRoute::create_sample2( )
{
	struct Vertex v;	
	v.color[0] = 0xFF;	v.color[1] = 0xFF;	v.color[2] = 0;	v.color[3] = 0;
	v.position[1] = 2.0;
	
	v.position[0] = 10.;
	v.position[2] = 10.;
	start_over_at( v );
 
	v.position[0] = 124.;
	v.position[2] = 160.+18.;
	add_way_point( v );

	v.position[0] = 144.+100.;
	v.position[2] = 160.+18.;
	add_way_point( v );

	v.position[0] = 144.+70.;
	v.position[2] = 24.;
	add_way_point( v );

	m_number_path_vertices = m_vertices.size();
}

// specific for my apartment.
void	glRoute::create_sample()
{ 
	struct Vertex v;
	v.color[0] = 0xFF;	v.color[1] = 0;	v.color[2] = 0;	v.color[3] = 0;	
	v.position[1] =  0.1;		// always!

	float x = m_start.position[0];
	float y = 0.;
	float z = m_start.position[2];
	
	// Go along z first:
	for (; z<160.+18.; z+=2.0)
	{
		v.position[0] =  x;
		v.position[2] =  z;
		m_vertices.push_back( v );
	}

	for (; x<144.+122.+18.; x+=2.0)
	{
		v.position[0] =  x;
		v.position[2] =  z;
		m_vertices.push_back( v );
	}

	// come back on Z.
	for (; z>18.; z-=2.0)
	{
		v.position[0] =  x;
		v.position[2] =  z;
		m_vertices.push_back( v );
	}
	m_number_path_vertices = m_vertices.size();	
} 		

/* We only compute the rise and run! */
void glRoute::compute_slopes( )
{
	struct stRiseRun rr;
	m_slopes.clear();
	for (int v=0; v<m_number_path_vertices; v++)
	{
		rr.run  = m_vertices[v+1].position[0] - m_vertices[v].position[0];
		rr.rise = m_vertices[v+1].position[2] - m_vertices[v].position[2];
		m_slopes.push_back ( rr );
	}
}

/* 
	Computes the slope between 2 consequative vertices on the 2 dimensional plane.
	(y = 0)   										
*/
struct stRiseRun glRoute::compute_slope( int mIndex )
{
	struct stRiseRun rr;
	rr.run = 0.0; 	rr.rise=0.0;
	if (mIndex == m_vertices.size())
		return rr;

	rr.rise = m_vertices[mIndex+1].position[0] - m_vertices[mIndex].position[0];
	rr.run  = m_vertices[mIndex+1].position[2] - m_vertices[mIndex].position[2];
	return rr;
}

struct stRiseRun  glRoute::compute_perpendicular( int mIndex )
{
	struct stRiseRun rr;
	rr.run = 0.0; 	rr.rise=0.0;
	if (mIndex == m_vertices.size())
		return rr;	

	rr.run  = m_vertices[mIndex+1].position[0] - m_vertices[mIndex].position[0];
	rr.rise = m_vertices[mIndex+1].position[2] - m_vertices[mIndex].position[2];
	return rr;
}

float glRoute::compute_robot_feet_standing( float mStride, int mIndex )
{
	// Stride is the hypotenus 
	// Stride*Stride = run*run + rise*rise	
	float run  = m_vertices[mIndex+1].position[0] - m_vertices[mIndex].position[0];
	float rise = m_vertices[mIndex+1].position[2] - m_vertices[mIndex].position[2];

	struct stRiseRun cp = compute_perpendicular( mIndex );
	struct stFootPosition fp;
	fp.heel[0] = 0.;
	fp.heel[1] = 0.;
	fp.heel[2] = 0.;

	fp.angle[0] = 0.;
	fp.angle[1] = atan2(rise,run);
	fp.angle[2] = 0.;
	
	m_left_steps.push_back( fp );
	m_right_steps.push_back( fp );
}

/* We've placed vertices every 2 inches.  
Sometimes stride is considered every 2 steps (ie. left, right).
Anthropromorphic data says average human stride is ~31 inches.
We'll take 30 inches ==> every 15 vertices
But we're going to use it as: left to right.
This will give orientation as well as position. 
*/
void glRoute::compute_robot_steps( float mStride, bool mLeftFootFirst )
{
	struct Vertex v;
	v.position[1] = 0.0;
	struct stFootPosition fp;
	struct stRiseRun	  rr;
	bool left_foot_turn   = mLeftFootFirst;
	fp.angle[0] = 0.;
	fp.angle[2] = 0.;
	float path_center_to_heel = RobotFeetSeparation / 2.0;
	
	for (int i=0; i<m_vertices.size(); i+=15)
	{
		m_vertices[i].color[1] =   0.;
		m_vertices[i].color[2] = 0xFF;
		rr = compute_perpendicular( i );
		//rr = compute_slope( i );
		float hypotenus = sqrt(rr.run*rr.run + rr.rise*rr.rise);
		float multiplier = ( path_center_to_heel / hypotenus );

		if (left_foot_turn)
		{
			// HEEL:
			fp.heel[0] = m_vertices[i].position[0] + rr.rise*multiplier;
			fp.heel[1] = m_vertices[i].position[1];
			fp.heel[2] = m_vertices[i].position[2] - rr.run*multiplier;
			fp.angle[1]= atan2(rr.rise, rr.run);

			// TOE:
			multiplier = (RobotFeetLength / hypotenus);
			fp.toe[0] = fp.heel[0] + rr.run * multiplier;
			fp.toe[1] = fp.heel[1];
			fp.toe[2] = fp.heel[2] + rr.rise * multiplier;
			m_left_steps.push_back( fp );

		} else {
			// We have to follow the path, so we can't key off of the left foot.
			// HEEL:	
			fp.heel[0] = m_vertices[i].position[0] - rr.rise*multiplier;
			fp.heel[1] = m_vertices[i].position[1];
			fp.heel[2] = m_vertices[i].position[2] + rr.run*multiplier;
			fp.angle[1] = atan2(rr.rise, rr.run);			

			// TOE:
			multiplier = (RobotFeetLength / hypotenus);
			fp.toe[0] = fp.heel[0] + rr.run * multiplier;
			fp.toe[1] = fp.heel[1];
			fp.toe[2] = fp.heel[2] + rr.rise * multiplier;
			fp.angle[1] = atan2(rr.rise, rr.run);
			m_right_steps.push_back( fp );
		}
		left_foot_turn = !left_foot_turn;
	}
}

/* Translates the m_left_steps<> and m_right_steps<> into vertices  */
void	glRoute::generate_steps_vertices( )
{
	struct Vertex v;
	v.color[0]= 0xFF;  	v.color[1]= 0xFF;  	v.color[2]= 0;  	v.color[3]= 0xFF;  	
	int lsize = m_left_steps.size();
	int rsize = m_right_steps.size();

	// Left Foot First:
	for (int i=0; i<lsize; i++)
	{
		// HEEL:
		memcpy (v.position, m_left_steps[i].heel, sizeof(float)*3 );
		m_vertices.push_back( v );
		
		// TOE :	(same location as heel plus 1 foot delta along rise/run)
		memcpy (v.position, m_left_steps[i].toe, sizeof(float)*3 );
		m_vertices.push_back( v );
	}

	// Right Foot First:
	for (int i=0; i<rsize; i++)
	{
		// HEEL:
		memcpy (v.position, m_right_steps[i].heel, sizeof(float)*3 );
		m_vertices.push_back( v );
		
		// TOE :	(same location as heel plus 1 foot delta along rise/run)
		memcpy (v.position, m_right_steps[i].toe, sizeof(float)*3 );
		m_vertices.push_back( v );
	}

	m_number_robot_vertices = lsize*2 + rsize*2;	
	printf("Route::generate_steps_vertices() num_robot_vertices= %lu \n", m_number_robot_vertices );
}

void	glRoute::generate_layer_vertices	( )
{ 
	create_sample();
} 

void 	glRoute::change_vertices_colors	( )
{
 
} 	// makes robot feet in red.

void	glRoute::generate_vertices		( )
{ } 

void	glRoute::draw_body			 	( )
{
	// Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer( GL_ARRAY_BUFFER, 		   m_VBO );
	//glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );
	glLineWidth( 20.0 );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	//Establish array contains m_vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY ); 

	// Draw Route :
	glDrawArrays  (GL_LINE_STRIP,  0,  m_number_path_vertices );

	// Draw Foot steps :
	if ( m_number_robot_vertices ) 
		glDrawArrays(GL_LINES,  m_number_path_vertices, m_number_robot_vertices );
} 


