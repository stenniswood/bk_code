#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"


#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define TOP_AND_BOTTOM 2
#define CLOSE_OUT 1


glRoom::glRoom(  )
{
	m_x = 0.0;
	m_y = 0.0;
	m_z = 0.0;
	m_color = 0xFFFFFFFF;
	//m_is_closed = false;
}
 
void glRoom::create_apartment()
{
	glWall w;
	struct stSize s;		// for windows
//	w.set_texture(1);

	// Outside wall (sliding door)
	w.m_wall_length = 144.+36.+116.+120.;
	s.width = 46.25;    s.height = 46.25;
	w.add_window( 66.5+144.+36.   , s, 33.5 );
	w.add_window( 144.+36.+116.+3., s, 33.5 );
	w.create    ( 144.+36.+116.+120. );
	w.m_y_angle = 0.0;
	w.m_z = 0.;
	w.m_x = 0.;
	m_walls.push_back( w );
	w.clear( );

	// Hall way 1 :
	w.add_door( 122.     , 30. );
	w.add_door( 122.+40.5, 30. );
	w.add_door( 122.+40.5+64.5 );
	w.create  ( 36.+116.+120.  );
	w.m_y_angle = 0.0;
	w.m_z = 160.;
	w.m_x = 144.;
	m_walls.push_back( w );
	w.clear();

	// Hall way 2 :
	w.add_door( 164.5, 28. );
	//w.add_door( 164.5+52.,28. );		// really need short inside walls
	w.create( 36.+116.+120.-77.5 );
	w.m_y_angle = 0.0;
	w.m_z = 160.+36.;
	w.m_x = 144.;
	m_walls.push_back( w );
	w.clear(); 

	// Final (kitchen to neighbors)
	w.create( 144.+36.+116.+120. );
	w.m_y_angle = 0.0;
	w.m_z = 160.+36.+95.;
	w.m_x = 0.;
	m_walls.push_back( w );
	w.clear();

	////////////////////////// horizontals //////////////////////////
	// End of Hallway
	w.create( 70. );
	w.m_y_angle = -90.0;	
	w.m_z = 160.; 
	w.m_x = 144.+122.+40.5+30.+2.;			// 338.5
	m_walls.push_back( w );
	w.clear();

	// Bathroom
	w.add_door( 19.5, 28. );
	w.create( 416. - 338.5 );   
	w.m_y_angle = 0.0;	
	w.m_z = 160.+70.; 
	w.m_x = 144.+122.+40.5+30.+2.;
	m_walls.push_back( w );
	w.clear();

	// Divide (living & bed1)
	w.create( 160. );
	w.m_y_angle = -90.0;	
	w.m_z = 0.;
	w.m_x = 144.;	
	m_walls.push_back( w );
	w.clear();
	
	// Divide (bed1 & bed2)
	w.create( 160. );
	w.m_y_angle = -90.0;	
	w.m_z = 0.;
	w.m_x = 144.+36.+116.;
	m_walls.push_back( w );
	w.clear();

	// Outside End
	s.width = 34.0;  s.height = 34.0;	
	w.add_window( 97.5+6., s, 46.0 );
	w.create( 160.+36.+95. );
	w.m_y_angle = -90.0;
	w.m_z = 0.;
	w.m_x = 144.+36.+116.+120.;		// 416
	m_walls.push_back( w );
	w.clear();

	// Inside End:
	w.add_door( 160.+18.+5.  );		// Main Entrance Door
	w.create( 160.+36.+95. );
	w.m_y_angle = -90.0;
	w.m_z = 0.;
	w.m_x = 0.;
	m_walls.push_back( w );
	w.clear();
	
	// Divide (kitchen & bath)
	w.add_door( 33.0, 24.0 ); 	// pantry door.
	w.create( 95.0 );
	w.m_y_angle = -90.0;	
	w.m_z = 160.+36.;
	w.m_x = 144.+114;
	m_walls.push_back( w );
	w.clear();

	// Divide (pantry & linen)
	w.create( 31.5 );
	w.m_y_angle = 0.0;	
	w.m_z = 160.+36.+26;
	w.m_x = 144.+114;
	m_walls.push_back( w );
	w.clear();
	
	// Divide (kitchen & bath)
	w.add_door( 2.0, 23.5 ); 	// linen closet "door"
	w.create( 95.0 );
	w.m_y_angle = -90.0;	
	w.m_z = 160.+36.;
	w.m_x = 144.+114+31.5;
	m_walls.push_back( w );
	w.clear();
	
	// Stub (entry):
	w.create( 70.5-4.25, 43. );
	w.m_y_angle = 0.0;	
	w.m_z = 160.+18.;
	w.m_x = 0.;	
	m_walls.push_back( w );
	w.clear();

	// Post :
	w.create( 4.25 );
	w.m_y_angle = 0.0;	
	w.m_z = 160.+18.;
	w.m_x = 70.5-4.25;	
	m_walls.push_back( w );
	w.clear();

	// entry closet : 
	w.m_wall_length = 70.5;		// Critical Note: This wall length has to be set prior to add door!  
								// Got lucky above because the length is not normally set until create()
	w.add_door( 35., 28. ); 	// linen closet "door"	
	w.create( 70.5 );
	w.m_y_angle = -90.0;	
	w.m_z = 160.+36.+95.-70.5;
	w.m_x = 74.75;	
	m_walls.push_back( w );
	w.clear();

	w.create( 74.75 );
	w.m_y_angle = 0.0;	
	w.m_z = 160.+36.+95.-70.5;
	w.m_x = 0.;	
	m_walls.push_back( w );
	w.clear();
}

void glRoom::draw()
{
	for (int w=0; w<m_walls.size(); w++)
		m_walls[w].draw();
		
	m_2D.draw();
}

// SHORTCUT(ALIAS):
#define Wall m_walls[w]
static float radians( float Degrees)
{
	return Degrees * M_PI / 180.0;	
}

/*void glRoom::scale_drawing(float mScale)
{
	for (int v=0; v<m_2D.size(); v++)
	{
		m_2D[v].position[0] *= mScale;
		m_2D[v].position[2] *= mScale;
	}
}

void glRoom::create_2D_mark( float mStartx, float mStarty, long int mColor )
{
	struct Vertex v;
	v.color[0] = (mColor & 0xFF000000)>>24;	
	v.color[1] = (mColor & 0x00FF0000)>>16;	
	v.color[2] = (mColor & 0x0000FF00)>>8;	
	v.color[3] = (mColor & 0x000000FF);
	v.position[1] = 2.0;	// For all points!  Since 2D.

	v.position[0] = mStartx;	v.position[2] = mStarty-5;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );
	v.position[0] = mStartx;	v.position[2] = mStarty+5;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );
	v.position[0] = mStartx-5;	v.position[2] = mStarty;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );
	v.position[0] = mStartx+5;	v.position[2] = mStarty;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );
}

void glRoom::create_2D_marks( float mStartx, float mStarty, float mEndx, float mEndy)
{
	struct Vertex v;
	v.color[0] = 0xFF;	v.color[1] = 0x0F;	v.color[2] = 0xFF;	v.color[3] = 0x00;
	v.position[1] = 2.0;	// For all points!  Since 2D.
	
	// Green marks the beginning point.
	v.position[0] = mStartx;	v.position[2] = mStarty-5;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );		
	v.position[0] = mStartx;	v.position[2] = mStarty+5;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );		
	v.position[0] = mStartx-5;	v.position[2] = mStarty;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );		
	v.position[0] = mStartx+5;	v.position[2] = mStarty;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );		

	// Red marks end point.
	v.color[0] = 0xFF;	v.color[1] = 0xFF;	v.color[2] = 0x00;	v.color[3] = 0x00;
	v.position[0] = mEndx;		v.position[2] = mEndy-5;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );		
	v.position[0] = mEndx;		v.position[2] = mEndy+5;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );		
	v.position[0] = mEndx-5;	v.position[2] = mEndy;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );		
	v.position[0] = mEndx+5;	v.position[2] = mEndy;
	m_2D.push_back( v );		m_2D_indices.push_back( m_2D.size()-1 );		
	
	// 
	v.position[0] = 2.0;	v.position[2] = 2.0;
	m_2D.push_back( v );	m_2D_indices.push_back( m_2D.size()-1 );			
}*/

/* The wall from 2D point of view consists of:
		<m_x, m_z>			and
		m_y_angle 			(in degrees)
		m_wall_length		(in inches )			

 Creates Vertex  from the Wall 3D data    */
glMap2D* glRoom::create_2D_drawing( )
{	
	abWall* aw;
	for (int w=0; w<m_walls.size(); w++)
	{
		// Extracts the wall info plus copies the Doors and Windows info : 
		aw = m_walls[w].extract_2d_line_info( );
		m_2D.m_awalls.push_back( *aw );
	}
	//scale_drawing( 0.25 );
	return &m_2D;
}

/*void glRoom::draw_corners( )
{
	int w=0;
	int num_corners = m_2D.m_awalls[w].m_corners.size();
	float x,y;
	for (int c=0; c < num_corners; c++)
	{
		x = m_2D.m_awalls[w].m_corners[c].x;
		y = m_2D.m_awalls[w].m_corners[c].z;
		create_2D_mark( x, y, 0xFFFF0000 );	
	}
}*/


/*  // Creates Vertex  from the Wall 3D data 
void glRoom::create_2D_drawing( glMap2D mMap )
{
	struct Vertex v;
	v.color[0] = 0xFF;	v.color[1] = 0xFF;	v.color[2] = 0xCF;	v.color[3] = 0xFF;
	v.position[1] = 1.0;	// For all points!  Since 2D
	float rise;
	float run;
	float start_x;
	float start_y;

	* The wall from 2D point of view consists of:
			<m_x, m_z>			and
			m_y_angle 			(in degrees)
			m_wall_length		(in inches )			
	*
	for (int w=0; w<m_walls.size(); w++)
	{		
		m_2DM.m_awalls.push_back();
		
		// Starting Vertex:
		start_x = v.position[0] = Wall.m_x;
		start_y = v.position[2] = Wall.m_z;
		//printf("sx=%6.2f; sy=%6.2f;\n", start_x, start_y );

		m_2D.push_back( v );
		m_2D_indices.push_back( m_2D.size()-1 );		

		float rise = sin( -radians(Wall.m_y_angle) ) * Wall.m_wall_length;
		float run  = cos( -radians(Wall.m_y_angle) ) * Wall.m_wall_length;		// yes
		//printf("rise=%6.2f;  run=%6.2f;\n", rise, run );

		// interpolate distance along the wall to the distance along the rise,run		
		for (int d=0; d<m_walls[w].m_doors.size(); d++)
		{
			// Front edge of door:
			float fraction = Wall.m_doors[d].PositionLengthwise / Wall.m_wall_length;
			v.position[0] = run  * fraction + Wall.m_x;
			v.position[2] = rise * fraction + Wall.m_z;
			m_2D.push_back( v );
			m_2D_indices.push_back( m_2D.size()-1 );			

			// Far edge of door:
			float fraction2 = (Wall.m_doors[d].PositionLengthwise+Wall.m_doors[d].Width) / Wall.m_wall_length;			
			v.position[0] = run  * fraction2 + Wall.m_x;
			v.position[2] = rise * fraction2 + Wall.m_z;
			m_2D.push_back( v );
			m_2D_indices.push_back( m_2D.size()-1 );
		}
		
		// Calculate end point.
		v.position[0] =  + run	+ Wall.m_x;
		v.position[2] =  + rise	+ Wall.m_z;
		m_2D.push_back( v );								// Yes
		m_2D_indices.push_back( m_2D.size()-1 );
	}
	create_2D_marks( 144+58, 75, 40, 250 );	
	scale_drawing( 0.25 );
}
*/

/*void glRoom::generate_VBO_2D()
{
	printf("glRoom::generate_IBO_2D()\n");
	//Create a new VBO and use the variable id to store the VBO id
	glGenBuffers( 1, &m_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );	// Make the new VBO active  GL_COLOR_ARRAY

	// Generate & Upload vertex data to the video device 
	int size = m_2D.size() * sizeof(Vertex);
	glBufferData		 ( GL_ARRAY_BUFFER, size, m_2D.data(), GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// vertex positions	
	glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(struct Vertex,color)));	// color
	glBindBuffer		 ( GL_ARRAY_BUFFER, m_VBO );

	//Draw Triangle from VBO - do each time window, view point or data changes
	//Establish its 3 coordinates per vertex with zero stride in this array; necessary here

	//glVertexAttribPointer(1, 3, GL_FLOAT, 		GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(struct Vertex, normal)));	// normals
	glEnableVertexAttribArray( 0 );	
	glEnableVertexAttribArray( 2 );	// GLKVertexAttribColor	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	printf("glRoom::generate_IBO_2D() done\n");	
}

void glRoom::generate_IBO_2D()
{
	printf("glRoom::generate_IBO_2D()\n");
	//Create a new VBO and use the variable id to store the VBO id
	int     size = m_2D_indices.size();
	GLubyte* tmp = m_2D_indices.data();

	glGenBuffers( 1, &m_IBO );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (size)*sizeof(GLubyte), 
				 tmp, GL_STATIC_DRAW );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

	printf("glRoom::generate_IBO_2D() done\n");	
}*/
/*void glRoom::draw_2D()
{
	glTranslatef(m_x, m_y, m_z);

	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));
	
	//Establish array contains m_vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY );

	// Draw Bottom :  
	glDrawElements(GL_LINES, m_2D_indices.size(), GL_UNSIGNED_BYTE, 0 );

	glTranslatef(-m_x, -m_y, -m_z);		
} */
/*void glRoom::print_2D_info( )
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
}*/