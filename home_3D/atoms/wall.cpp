/* Strategy:
		We'll keep a list of vertices in 2 dimensions; starting with 
		4 defining a rectangle length and height of wall.
		
		if a door is added.  the vertices are added.
		
		To generate the indices:
			scan the vertices starting at the left.
*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"
#include "wall.hpp"
#include "door.hpp"

#define Debug 0
#define  BUFFER_OFFSET(i) ((GLuint*)NULL + (i))


vector<int>  floor_marks;		// store the indices (not the value)
vector<int>  ceiling_marks;		// store the indices (not the value)


glWall::glWall( )
{ 
	m_wall_length = 10.*12.;			// default 10' long 
	m_wall_height = STANDARD_WALL_HEIGHT;
	m_texture 	  = NULL;
	m_windows_index_start = 0;
	m_color 	= 0xFFFF8F8F;
	clear();
}
glWall::glWall( float mLength )
{ 
	m_wall_length = mLength;
	m_wall_height = STANDARD_WALL_HEIGHT;
	m_texture = NULL;
	m_windows_index_start = 0;
	m_color = 0xFFFF8F8F;
	clear();	
}

void glWall::clear()
{
	m_vertices.clear();
	m_indices.clear();
	m_doors.clear();
	m_gl_doors.clear();
	m_windows.clear();
	floor_marks.clear();
	ceiling_marks.clear();
	m_layer_one_vertices = 0;
	m_layer_one_indices = 0;
	m_number_side_indices = 0;
}

void glWall::set_texture( int mSelection )
{
	m_texture = new Texture( );
	switch (mSelection)
	{
	case 0:		m_texture->load_image("textures/light-stucco-texture.jpg" 	);
				break;
	case 1:		m_texture->load_image("textures/stucco-texture.jpg" 		);
				break;
	case 2:		m_texture->load_image("textures/stucco-2-texture.jpg" 		);
				break;
	case 3:		m_texture->load_image("textures/wall-texture.jpg" 			);
				break;
	case 4:		m_texture->load_image("textures/drywall-texture.jpg" 		);
				break;
	case 5:		m_texture->load_image("textures/concrete-texture.jpg" 		);
				break;
	case 6:		m_texture->load_image("textures/bump-wall-texture.jpg" 		);
				break;
	case 7:		m_texture->load_image("textures/bamboo-fence-texture.jpg"	);
				break;
	default:  break;
	}

	generate_texture_coords 		( );
	m_texture->generate_TBO			( );	
	m_texture->generate_VBOTexCoords( );
}

void glWall::generate_texture_coords( )
{
	// We match vertices here, not indices.
	struct stTextCoord t;
	t.u = 0.;			t.v = 0.;				// Vertex 0,0
	m_texture->m_TexCoords.push_back( t );
	t.u = 700.;			t.v = 0.;				// Vertex 0,0	
	m_texture->m_TexCoords.push_back( t );
	t.u = 700.;			t.v = 700.;				// Vertex 0,0	
	m_texture->m_TexCoords.push_back( t );
	t.u = 0.;			t.v = 700.;				// Vertex 0,0	
	m_texture->m_TexCoords.push_back( t );

	for (int d=0; d<m_doors.size(); d++)
	{		
		// We'll add 6 vertices 
		// ( 4 of the doorway itself and 2 for the header meets the ceiling)	
		t.u = 1.0;			// because end of prev wall segment.
		t.v = 0.0;
		m_texture->m_TexCoords.push_back( t );

		t.u = 1.0;			// because end of prev wall segment.
		t.v = m_doors[d].Height / m_wall_height;
		m_texture->m_TexCoords.push_back( t );

		t.u = 1.0;			// because end of prev wall segment.
		t.v = 1.0;
		m_texture->m_TexCoords.push_back( t );

		t.u = 0.0;			// Corner 2
		t.v = 0.0;
		m_texture->m_TexCoords.push_back( t );

		t.u = 0.0;			// Corner 3
		t.v = m_doors[d].Height / m_wall_height;
		m_texture->m_TexCoords.push_back( t );

		t.u = 0.0;			// 
		t.v = 1.0;
		m_texture->m_TexCoords.push_back( t );
	}

}


void	glWall::generate_layer_vertices( )
{
	generate_wall_vertices	( );
	generate_door_vertices	( );
	generate_window_vertices( );	
	m_layer_one_vertices = m_vertices.size();
}


/* VERTICE PATTERN:
	0	floor start
	1	floor end
	2	ceiling end
	3	ceiling start
	d1 + 0  door floor
	d1 + 1  door height
	d1 + 2  door ceiling
	d1 + 3	door far floor
	d1 + 4	door far height
	d1 + 5	door far ceiling
	d2...
	w1 + 0	window floor
	w1 + 1	window sill
	w1 + 2	window top
	w1 + 3	window ceiling
	w1 + 4	window far floor
	w1 + 5	window far sill
	w1 + 6	window far top
	w1 + 7	window far ceiling 
*/
/* Generates the 4 corners of the wall. On 1 side only. */
void	glWall::generate_wall_vertices( )
{
	// GENERATE WALL:  (four points)
	struct Vertex v;
	v.color[0] = 0xBF; v.color[1] = 0xBF;  v.color[2] = 0xBF; v.color[3] = 0xBF;  
	v.position[0] = 0.0;
	v.position[1] = 0.0;
	v.position[2] = 0.0;	
	m_vertices.push_back( v );				// 0
	v.position[0] = m_wall_length;
	v.position[1] = 0.0;
	v.position[2] = 0.0;	
	m_vertices.push_back( v );				// 1
	v.position[0] = m_wall_length;
	v.position[1] = m_wall_height;
	v.position[2] = 0.0;	
	m_vertices.push_back( v );				// 2
	v.position[0] = 0.0;
	v.position[1] = m_wall_height;
	v.position[2] = 0.0;	
	m_vertices.push_back( v );				// 3
	//printf("generate_wall_vertices() verts size = %d\n", m_vertices.size() );
}

/* Generates the 4 corners of the door + 2 extra headers. On 1 side only. */
void glWall::generate_door_vertices( )
{
	struct Vertex v;
	v.color[0] = 0xFF; v.color[1] = 0x7F;  v.color[2] = 0xFF; v.color[3] = 0xFF;  
	for (int d=0; d<m_doors.size(); d++)
	{		
		// We'll add 6 vertices ( 4 of the doorway itself and 
		//   					  2 for the header meets the ceiling)	
		m_doors[d].FirstVertexIndex = m_vertices.size();
		//m_gl_doors[d].relocate( m_doors[d].PositionLengthwise, 0.0, 0.0 );
		
		v.position[0] = m_doors[d].PositionLengthwise;			// Corner 0 
		v.position[1] = 0.0;
		v.position[2] = 0.0;
		m_vertices.push_back( v );
		v.position[0] = m_doors[d].PositionLengthwise;			// Corner 1 
		v.position[1] = m_doors[d].Height;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );	
		v.position[0] = m_doors[d].PositionLengthwise;			// Also add first top of header
		v.position[1] = m_wall_height;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );									

		v.position[0] = m_doors[d].PositionLengthwise + m_doors[d].Width;	// Corner 2 
		v.position[1] = 0.0;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );
		v.position[0] = m_doors[d].PositionLengthwise + m_doors[d].Width;	// Corner 3 
		v.position[1] = m_doors[d].Height;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );

		v.position[0] = m_doors[d].PositionLengthwise+m_doors[d].Width;	// Also add 2nd top of header
		v.position[1] = m_wall_height;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );		
	}
	//printf("generate_door_vertices() %d doors : verts size = %d\n", m_doors.size(), m_vertices.size() );	
}

/* Generates the 4 corners of the window + 2 headers + 2 footers. On 1 side only. */
void glWall::generate_window_vertices( )
{
	struct Vertex v;
	v.color[0] = 0xFF; v.color[1] = 0xFF;  v.color[2] = 0xFF; v.color[3] = 0xFF;  	
	for (int w=0; w<m_windows.size(); w++)
	{		
		m_windows[w].FirstVertexIndex = m_vertices.size();
		
		// We'll add 8 vertices ( 4 of the window itself,
		//   					  2 for the header meets the ceiling,
		//						  2 where the window meets the floor )
		v.position[0] = m_windows[w].PositionLengthwise;			// front edge: Floor 
		v.position[1] = 0.0;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );
		v.position[0] = m_windows[w].PositionLengthwise;			// front edge: Sill
		v.position[1] = m_windows[w].SillHeight;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );
		v.position[0] = m_windows[w].PositionLengthwise;			// front edge: Top
		v.position[1] = m_windows[w].SillHeight + m_windows[w].Height;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );	
		v.position[0] = m_windows[w].PositionLengthwise;			// front edge: Ceiling
		v.position[1] = m_wall_height;		// Ceiling.
		v.position[2] = 0.0;	
		m_vertices.push_back( v );	

		v.position[0] = m_windows[w].PositionLengthwise + m_windows[w].Width;	// Far edge: Floor 
		v.position[1] = 0.0;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );
		v.position[0] = m_windows[w].PositionLengthwise + m_windows[w].Width;	// Far edge: Sill
		v.position[1] = m_windows[w].SillHeight;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );
		v.position[0] = m_windows[w].PositionLengthwise + m_windows[w].Width;	// Far edge: Window Top
		v.position[1] = m_windows[w].SillHeight + m_windows[w].Height;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );
		v.position[0] = m_windows[w].PositionLengthwise + m_windows[w].Width;	// Far edge: Ceiling
		v.position[1] = m_wall_height;
		v.position[2] = 0.0;	
		m_vertices.push_back( v );
	}
	if (Debug) printf("generate_window_vertices()  %lu vertices\n", m_vertices.size() );
}

bool	glWall::is_valid_location( float mPositionLengthwise, float mWidth )
{
	float start = 0;
	float stop  = 0;
	float NewDoorStop = 0;
	NewDoorStop = mPositionLengthwise+mWidth;
	
	// NO OVERLAPPING ANOTHER DOOR :
	for (int d=0; d<m_doors.size(); d++)
	{
		start = m_doors[d].PositionLengthwise;
		stop  = m_doors[d].PositionLengthwise+m_doors[d].Width;
		
		if ((mPositionLengthwise>start) && 
		    (mPositionLengthwise<stop) )
		{
			printf("Requested position is in middle of door #%d\n", d);
		    return false;
		}
		if ((NewDoorStop > start) && (NewDoorStop < stop) )
		{
			printf("Requested End Position is in middle of door #%d\n", d);
		    return false;			    
		}
	}	
	// OVERLAPPING A WINDOW?
	
	// WITHIN WALL LENGTH?
	if ((mPositionLengthwise + mWidth) > m_wall_length)
	{
		printf("Requested position is after wall end!\n");	
		return false;
	}
	if ((mPositionLengthwise) < 0.0)
	{
		printf("Requested position is before wall start!\n");
		return false;
	}
	return true;	
}

/*
return:	true 	if added.
		false	not added - another in the way.
*/
bool  glWall::add_door( float mPositionLengthwise, float mWidth, float mHeight )
{
	if (is_valid_location(mPositionLengthwise, mWidth)==false)
	{
		printf("Cannot add door at %6.1f of width=%6.1f, height=%6.1f.  Wall length is %6.1f\n",
					mPositionLengthwise, mWidth, mHeight, m_wall_length );
		return false;
	}
	struct stDoorWay d;
	d.FirstVertexIndex   = 0;
	d.PositionLengthwise = mPositionLengthwise;
	d.Width  = mWidth;
	d.Height = mHeight;
	m_doors.push_back(d);

	glDoor gld;
	gld.Initialize( mWidth, mHeight );
	gld.create();
	gld.m_door_body.grab_bottom();
	gld.m_door_body.grab_right();
	gld.open(0.5); 
	m_gl_doors.push_back( gld ); 
	return true;
}

/* Extracts and puts into the abWall object */
abWall* glWall::extract_2d_line_info()
{
	static abWall Abstract;
	Abstract.m_wall_length  = m_wall_length;
	Abstract.m_wall_height  = m_wall_height;
	Abstract.m_angle  	  	 = m_y_angle;	
	Abstract.m_x 			 = m_x;
	Abstract.m_z 			 = m_z;

	for (int d=0; d<m_doors.size(); d++)
		Abstract.m_doors.push_back( m_doors[d] );

	for (int w=0; w<m_windows.size(); w++)
		Abstract.m_windows.push_back( m_windows[w] );
	return &Abstract;
}

void glWall::test_push( )
{
//	m_gl_doors.push_back(1);
}

bool glWall::add_window( float mPositionLengthwise, struct stSize mSize, float mHeight )
{
	if (is_valid_location(mPositionLengthwise, mSize.width)==false)
	{
		printf("Cannot add window at %6.1f of width=%6.1f, height=%6.1f.  Wall length is %6.1f\n",
					mPositionLengthwise, mSize.width, mHeight, m_wall_length );
		return false;
	}
	struct stWindow w;	
	w.PositionLengthwise = mPositionLengthwise;
	w.Width  	 = mSize.width;
	w.Height 	 = mSize.height;
	w.SillHeight = mHeight;
	m_windows.push_back(w);
	return true;
}


// arrange doors and windows by ascending order
void glWall::sort()
{
	//m_doors.sort(  );	
	//m_windows.sort( );
}


/* Call this :
	After constructing, AFTER adding any doors.

NOTE: If you want to change the vertices color, it must be done inside this function.
	  (before generate_VBO() and after all vertices are created!)
*/
void	glWall::create( float mLength, int  mHeight )
{
	m_wall_length = mLength;
	m_wall_height = mHeight;

	// VERTICES :
	generate_layer_vertices	();	
	extrude_vertices( STANDARD_WALL_THICKNESS, 2 );		// generates new vertices. 

	// INDICES:
	generate_disc_indices( 0 );	
	generate_VBO();
	generate_IBO();

	struct Vertex v;
	//  Position the Doors:
	//printf("glWall::create  m_doors=%lu; m_gl_doors=%lu\n", m_doors.size(), m_gl_doors.size() );
	for (int d=0; d<m_doors.size(); d++)
	{
		//v = get_door_coord( d );
		//printf("%d : %6.2f, %6.2f, %6.2f \n", d, v.position[0], v.position[1], v.position[2]  );
		m_gl_doors[d].m_x = m_doors[d].PositionLengthwise;
		m_gl_doors[d].m_y = 0.0;
		m_gl_doors[d].m_z = 0.0;
		// Because all of the doors will be drawn from draw_body(), the wall's angle and 
		// positioning transform will already have occurred.		
		//m_gl_doors[d].relocate( v.position[0], v.position[1], v.position[2] );
	}
}

GLuint 	glWall::generate_disc_indices( GLuint mStartingVertexIndex )
{
	generate_marks			( );
	generate_wall_indices	( );		// GL_QUADS - m_layer_one_indices
	
	/*  Because there are multiple sets of sides, this is slightly different than
    	what glExtrusion will do.   */    
    // We need indices for drawing the other side.	
	generate_otherside_indices();			//   - m_layer_one_indices
	generate_wall_side_indices();			//  The edges of the wall (outside perimeter)
											//  (includes edges of the doorways)
	generate_window_side_indices();			//  The edges of each window.  
	return m_indices.size();
}
/* 
	Generates the side edge all around the wall and around the doorways.
	Does not do the sides edges of the windows (discontinuous).
*/
void glWall::generate_wall_side_indices()
{
	// assume the floors & marks are still active.
		// These indices :  Should go bottom, to next door.  trace around the doorway,
	//					then continue on bottom to next door, trace around doorway, etc.
	//					until end of wall.  then trace up the end, across the top and back
	//					down to the bottom starting point.
	int i_count = m_indices.size();
	m_indices.push_back( 0 );		m_indices.push_back( 0+m_layer_one_vertices );
	GLubyte fi;
	for (int d=0; d<m_doors.size(); d++)
	{
		// door floor
		fi = m_doors[d].FirstVertexIndex;
		m_indices.push_back( fi );		m_indices.push_back( fi+m_layer_one_vertices );
		// door height
		m_indices.push_back( fi+1 );	m_indices.push_back( fi+1+m_layer_one_vertices );
		// door far height
		m_indices.push_back( fi+4 );	m_indices.push_back( fi+4+m_layer_one_vertices );	
		// door far floor
		m_indices.push_back( fi+3 );	m_indices.push_back( fi+3+m_layer_one_vertices );	
	}
	// We now have the bottom (up to the last door) and all doorway frames.
	// Go across bottom to end of wall:
	m_indices.push_back( 1 );		m_indices.push_back( 1+m_layer_one_vertices );
	// To ceiling at end of wall:
	m_indices.push_back( 2 );		m_indices.push_back( 2+m_layer_one_vertices );
	// To ceiling at start of wall:
	m_indices.push_back( 3 );		m_indices.push_back( 3+m_layer_one_vertices );
	// To floor at start of wall:
	m_indices.push_back( 0 );		m_indices.push_back( 0+m_layer_one_vertices );
	m_number_side_indices = m_indices.size()-i_count;
}

// GOOD!
void glWall::generate_window_side_indices()
{
	m_windows_index_start = m_indices.size();
	GLuint fi;
	for (int w=0; w<m_windows.size(); w++)
	{
		fi = m_windows[w].FirstVertexIndex;
		// Do the four corners:
		m_indices.push_back( fi+1 );		m_indices.push_back( fi+1+m_layer_one_vertices );
		m_indices.push_back( fi+2 );		m_indices.push_back( fi+2+m_layer_one_vertices );
		m_indices.push_back( fi+6 );		m_indices.push_back( fi+6+m_layer_one_vertices );
		m_indices.push_back( fi+5 );		m_indices.push_back( fi+5+m_layer_one_vertices );
		// close out:
		m_indices.push_back( fi+1 );		m_indices.push_back( fi+1+m_layer_one_vertices );
	}
}

/* VERTICE PATTERN:
	0	floor start
	1	floor end
	2	ceiling end
	3	ceiling start
	d1 + 0  door floor
	d1 + 1  door height
	d1 + 2  door ceiling
	d1 + 3	door far floor
	d1 + 4	door far height
	d1 + 5	door far ceiling
	d2...
	w1 + 0	window floor
	w1 + 1	window sill
	w1 + 2	window top
	w1 + 3	window ceiling
	w1 + 4	window far floor
	w1 + 5	window far sill
	w1 + 6	window far top
	w1 + 7	window far ceiling
*/

/* 
	Mark the wall all along the way with both doors and Windows
	for simplicity of generating the indices.	
*/
void glWall::generate_marks( )
{
	int d = 0;
	int w = 0;
	float d_value = 0.0;
	float w_value = 0.0;
	floor_marks.push_back(0);		// start of wall.
	ceiling_marks.push_back(3);		// start of wall.	
	bool done = false;
	int prev_ceiling = 0;
	int prev_floor   = 0;
	int v_index = 0;
	while (!done)
	{
		if (d < m_doors.size())
			d_value = m_doors[d].PositionLengthwise;
		else 
			d_value = m_wall_length;
		//printf("generate_marks() d=%d; FirstIndex=%d  d_value=%6.1f\n", d, m_doors[d].FirstVertexIndex, d_value);
		//printf("wall end=%6.1f\n", m_wall_length);
		if (w<m_windows.size())
			w_value = m_windows[w].PositionLengthwise;
		else 
			w_value = m_wall_length;

		if (d_value < w_value)	{
			//printf("Adding door to indices marks\n");
			// ADD INDICES for drawing WALL PANELS (not headers)
			// So allow wall to come right up to the door:			
			v_index =  m_doors[d].FirstVertexIndex + 0;	// door floor
			floor_marks.push_back  ( v_index );
			v_index =  m_doors[d].FirstVertexIndex + 2;	// door ceiling index.
			ceiling_marks.push_back( v_index );
			
			// Then allow wall start again right after the door:			
			v_index =  m_doors[d].FirstVertexIndex + 3;	// door far floor
			floor_marks.push_back  ( v_index );
			v_index =  m_doors[d].FirstVertexIndex + 5;	// door far ceiling index.
			ceiling_marks.push_back( v_index );			
			// Then it comes back to ____.
			// 		index 3 for the first door.
			// for 2nd and later doors, comes back to prev door ceiling, then floor.				
			d++;
			//if (d>=m_doors.size()) done = true;
		} else if (w<m_windows.size()) {			
			// ADD INDICES for drawing WALL PANELS (not headers)
			// So allow wall to come right up to the door:			
			v_index =  m_windows[w].FirstVertexIndex + 0;	// window floor
			floor_marks.push_back  ( v_index );
			v_index =  m_windows[w].FirstVertexIndex + 3;	// window ceiling index.
			ceiling_marks.push_back( v_index );
			
			// Then allow wall start again right after the door:		
			v_index =  m_windows[w].FirstVertexIndex + 4;	// window far floor
			floor_marks.push_back  ( v_index );
			v_index =  m_windows[w].FirstVertexIndex + 7;	// window far ceiling index.
			ceiling_marks.push_back( v_index );			
			w++;
		}
		if ((d>=m_doors.size())	&& (w>=m_windows.size()))
			done = true;		
	}
	// Last add the far wall ends:
	floor_marks.push_back  ( 1 );
	ceiling_marks.push_back( 2 );
}

void glWall::print_marks( )
{
	printf("ceil:\t");
	for (int i=0; i<ceiling_marks.size(); i++)
	{
		printf("%d, ", ceiling_marks[i]);		
	}
	printf("\nfloor:\t");
	for (int i=0; i<floor_marks.size(); i++)
	{
		printf("%d, ", floor_marks[i] );
	}
	printf("\n");
}

/*  
	This makes:  the wall for 1 side.  Setup for GL_QUADS	
*/
void glWall::generate_wall_indices( )
{
	// We will be drawing as GL_QUADS.	They go counter clockwise.

	// FIRST DO ALL OF THE NON-DOOR/WINDOW PLACES:
	for (int i=0; i<floor_marks.size()-1; i+=2)
	{
		m_indices.push_back( floor_marks[i] );		// 0 is the floor start of wall.
		m_indices.push_back( floor_marks[i+1] );
		m_indices.push_back( ceiling_marks[i+1] );
		m_indices.push_back( ceiling_marks[i] );
	}	
	// NEXT DO THE DOOR HEADERS:
	for (int d=0; d<m_doors.size(); d++)
	{
		m_indices.push_back( m_doors[d].FirstVertexIndex + 1 );
		m_indices.push_back( m_doors[d].FirstVertexIndex + 4 );
		m_indices.push_back( m_doors[d].FirstVertexIndex + 5 );
		m_indices.push_back( m_doors[d].FirstVertexIndex + 2 );
	}

	// NEXT DO THE WINDOW HEADERS:
	for (int w=0; w<m_windows.size(); w++)
	{
		m_indices.push_back( m_windows[w].FirstVertexIndex + 2 );
		m_indices.push_back( m_windows[w].FirstVertexIndex + 6 );
		m_indices.push_back( m_windows[w].FirstVertexIndex + 7 );
		m_indices.push_back( m_windows[w].FirstVertexIndex + 3 );
	}
	// NEXT DO THE WINDOW FOOTERS (the part below the window):
	for (int w=0; w<m_windows.size(); w++)
	{
		m_indices.push_back( m_windows[w].FirstVertexIndex + 0 );
		m_indices.push_back( m_windows[w].FirstVertexIndex + 4 );
		m_indices.push_back( m_windows[w].FirstVertexIndex + 5 );
		m_indices.push_back( m_windows[w].FirstVertexIndex + 1 );
	}

	m_layer_one_indices = m_indices.size();
	//printf("wall indices = %d\n", m_indices.size() );
}


#define Door m_doors[mDoorIndex]
struct Vertex glWall::get_door_coord( int mDoorIndex )
{
	int index = m_doors[mDoorIndex].FirstVertexIndex;
	return m_vertices[index];
}

struct Vertex glWall::get_door_center_coord( int mDoorIndex )
{
	struct Vertex v1 =  m_vertices[Door.FirstVertexIndex];
	struct Vertex v2 =  m_vertices[Door.FirstVertexIndex+1];
	struct Vertex vc;
	vc.color[0] = v1.color[0];
	vc.color[1] = v1.color[1];
	vc.color[2] = v1.color[2];
	vc.color[3] = v1.color[3];
	vc.position[1] = v1.position[1];

	vc.position[0] = (v1.position[0] + v2.position[0]) / 2.;
	vc.position[2] = (v1.position[2] + v2.position[2]) / 2.;
	return vc;
}

struct Vertex glWall::get_door_far_coord(  int mDoorIndex  )
{
	return m_vertices[Door.FirstVertexIndex+1];
}

void	glWall::draw_body()
{
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer	(GL_ARRAY_BUFFER, 		  m_VBO	);
	glVertexPointer	(3, GL_FLOAT, 		  sizeof(struct Vertex), NULL);
	glColorPointer 	(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	glBindBuffer	(GL_ELEMENT_ARRAY_BUFFER, m_IBO );
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// FIRST IS DRAW WALL : 
	//if (m_texture)	m_texture->draw();
	glDrawElements(GL_QUADS, m_layer_one_indices, GL_UNSIGNED_INT, (GLvoid*)((char*)NULL));
	//if (m_texture)	m_texture->after_draw();
	glDrawElements(GL_QUADS, m_layer_one_indices, GL_UNSIGNED_INT, (GLvoid*)BUFFER_OFFSET(m_layer_one_indices) );

	// DRAW WALL EDGE:
	// These indices :  Should go bottom, to next door.  trace around the doorway,
	//					then continue on bottom to next door, trace around doorway, etc.
	//					until end of wall.  then trace up the end, across the top and back
	//					down to the bottom starting point.	
	glDrawElements(GL_QUAD_STRIP, m_number_side_indices, GL_UNSIGNED_INT, 
					(GLvoid*)BUFFER_OFFSET(m_layer_one_indices*2)  );

	// 	DRAW WINDOWS SIDES : (separate draw functions)
	// Note:  number of indices to draw is (4+1)*2=10  four vertices plus closeout *2
	int number_indices_to_draw = (4+1)*2;
	int verts_per_window = 8;
	for (int w=0; w<m_windows.size(); w++)	
		glDrawElements(GL_QUAD_STRIP, number_indices_to_draw, GL_UNSIGNED_INT, 
						(GLvoid*)BUFFER_OFFSET(m_windows_index_start + w*number_indices_to_draw) ); 
				
	for (int d=0; d<m_gl_doors.size(); d++)
		m_gl_doors[d].draw();
}
