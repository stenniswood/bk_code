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



#define  CLOSE_OUT 		1
#define  TOP_AND_BOTTOM 2
#define  BUFFER_OFFSET(i) ((GLuint*)NULL + (i))

#define Debug 0


glRoute::glRoute( )
{
    m_object_class  = 11;
    
	m_start.position[0] = 0.;
	m_start.position[2] = 0.;

	m_end.position[0] = 0.;
	m_end.position[2] = 0.;

    m_line_width = 10.;
    m_sample_spacing_inches = 2.0;
    
	// IBO / VBO:
	m_number_path_vertices = 0;
    
    // 
    m_direct_path.m_color = 0xFFFF0000;
}

float glRoute::get_angle_degrees( int mIndex )
{
    float dx = m_vertices[mIndex+1].position[0] - m_vertices[mIndex].position[0];
    float dy = m_vertices[mIndex+1].position[2] - m_vertices[mIndex].position[2];
    float angle = atan2( dx, dy );
    return (180.0 * angle / M_PI);
}

void glRoute::reset( )
{
    m_number_path_vertices = 0;
    m_xyz.clear();
    m_slopes.clear();
    glAtom::reset();
}

/* Starts the entire path over */
void glRoute::start_over_at( struct Vertex mStart )
{
    reset();
    m_start = mStart;
	m_vertices.push_back( mStart );
	m_number_path_vertices = (int)m_vertices.size();
}

void glRoute::start_over_at2( MathVector    mStart )
{
    struct Vertex v;
    v.position[0] = mStart[0];
    v.position[1] = mStart[1];
    v.position[2] = mStart[2];
    start_over_at( v );
}

void glRoute::show_direct_path( bool mDraw )
{
    MathVector tmp(3);
    tmp[0] = m_start.position[0];
    tmp[1] = m_start.position[1];
    tmp[2] = m_start.position[2];
    m_direct_path.m_Start = tmp;
    tmp[0] = m_end.position[0];
    tmp[1] = m_end.position[1];
    tmp[2] = m_end.position[2];
    m_direct_path.m_End   = tmp;
    m_direct_path.m_is_shown = mDraw;
}


/* Compute samples from current location (last vertex) to
	the specified location in a straight line.
    Sampling every 2 inches
    Linear Interpolate.
*/
void glRoute::add_way_point( struct Vertex mWay )
{
	int    lsize = (int)m_vertices.size();
	struct Vertex_pnc    v;

	// Initialize with Last Known Position : 
	memcpy( &v, &m_vertices[lsize-1], sizeof(struct Vertex_pnc) );
    
	MathVector delta(3);
	delta[0] = ( mWay.position[0] - v.position[0] );
    delta[1] = ( mWay.position[1] - v.position[1] );
	delta[2] = ( mWay.position[2] - v.position[2] );
    float magnitude = delta.magnitude();
    delta.unitize();
    delta *= m_sample_spacing_inches;
    
    // Normalize rr : 
	int samples      = floor(fabs(magnitude/m_sample_spacing_inches));
    set_vertex_color( v );
    
	for (int i=0; i<samples; i++)
	{
        v.position[0] += delta[0];
        v.position[1] += delta[1];
		v.position[2] += delta[2];
		m_vertices.push_back( v );
	}
	m_number_path_vertices = (int)m_vertices.size();
}

void glRoute::add_way_point2( MathVector   mPoint )
{
    struct Vertex v;
    v.position[0] = mPoint[0];
    v.position[1] = mPoint[1];
    v.position[2] = mPoint[2];
    add_way_point( v );
}

void glRoute::add_sample( MathVector   mPoint )
{
    struct Vertex v;
    set_vertex_color(v);
    
    v.position[0] = mPoint[0];
    v.position[1] = mPoint[1];
    v.position[2] = mPoint[2];
    m_vertices.push_back( v );
    if (m_vertices.size()==1)
        m_start = v;
    
    m_number_path_vertices = (int)m_vertices.size();
}


/*void glRoute::create_from_multi( glMultiRoute& mMulti )
{
    // Generate Final Path (Walking Route)
    reset           ( );
    start_over_at2  ( mMulti.FinalPath[0] );                 // NEED!!
    set_color       ( 0xFFFFFF00);
    
    long size = mMulti.FinalPath.size();
    for (long i=0; i<size; i++)
        add_way_point2( mMulti.FinalPath[i] );
    
    compute_slopes         ( );
    m_number_path_vertices = (int)m_vertices.size();
    m_end = m_vertices[m_number_path_vertices-1];
    show_direct_path();
    // call gl_register after this.
    // Not done here because virtual function.  derived classes may add more vertices (foot steps)
    // Need to call from the caller of this function.    
}*/


/* We only compute the rise and run! */
void glRoute::compute_slopes( )
{
	MathVector rr;
	m_slopes.clear();
	for (int v=0; v<m_number_path_vertices; v++)
	{
        rr = compute_delta( v );
        m_slopes.push_back ( rr );
	}
}

/* 
	Computes the slope between 2 consequative vertices on the 2 dimensional plane.
	(y = 0)   										
*/
MathVector glRoute::compute_delta( int mIndex )
{
	MathVector rr(3);
    rr[0] = 0.0; 	rr[1]=0.0;      rr[2]=0.0;
	if ( (mIndex == (m_vertices.size())-1) || (mIndex<0) )
		return rr;

	rr[0] = m_vertices[mIndex+1].position[0] - m_vertices[mIndex].position[0];
    rr[1] = m_vertices[mIndex+1].position[1] - m_vertices[mIndex].position[1];
	rr[2] = m_vertices[mIndex+1].position[2] - m_vertices[mIndex].position[2];
	return rr;
}

struct stRiseRun  glRoute::compute_perpendicular( int mIndex )
{
	struct stRiseRun rr;
	rr.run = 0.0;
    rr.rise=0.0;
	if ((mIndex == (m_vertices.size()-1)) || (mIndex<0))
		return rr;

	rr.run  = -(m_vertices[mIndex+1].position[0] - m_vertices[mIndex].position[0]);
	rr.rise =   m_vertices[mIndex+1].position[2] - m_vertices[mIndex].position[2];
	return rr;
}

MathVector glRoute::get_directional_vector( int mIndex )
{
    MathVector mv = compute_delta( mIndex );
    mv.unitize();
    return mv;
}

MathVector glRoute::get_perpendicular_vector( int mIndex )
{
    MathVector mv = compute_delta( mIndex );
    MathVector perp = mv.get_perp_xz();
    perp.unitize();
    return perp;
}
MathVector glRoute::get_perpendicular_pt( int mIndex, float mPerpendicular_Distance )
{
    MathVector mv = compute_delta( mIndex );
    MathVector perp = mv.get_perp_xz();
    MathVector center(3);
    center[0] = m_vertices[mIndex].position[0];
    center[1] = m_vertices[mIndex].position[1];
    center[2] = m_vertices[mIndex].position[2];
    return center + perp * mPerpendicular_Distance;
}


/*void	glRoute::generate_vertices		( )
{
    // Done automatically as the way points are added.
}*/

void	glRoute::draw_body			 	( )
{
    
	// Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer  ( GL_ARRAY_BUFFER, 		   m_VBO );
	//glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );
	glLineWidth   ( m_line_width );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	//Establish array contains m_vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY ); 

	// Draw Route :
	glDrawArrays  (GL_LINE_STRIP,  0,  m_number_path_vertices );
    
    // Show Red line connecting end points :
    m_direct_path.draw_body();
}


/*void glRoute::create_sample2( )
 {
	struct Vertex v;
	v.color[0] = 0xFF;	v.color[1] = 0xFF;	v.color[2] = 0;	v.color[3] = 0;
	v.position[1] = 2.0;
	
	v.position[0] = 20.;
	v.position[2] = 20.;
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
 }*/


// specific for my apartment.
/*void glRoute::create_sample()
 {
	struct Vertex v;
	v.color[0] = 0xFF;	v.color[1] = 0;	v.color[2] = 0;	v.color[3] = 0;
	v.position[1] =  0.1;		// always!
 
	float x = m_start.position[0];
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
	m_number_path_vertices = (int)m_vertices.size();
 }*/