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
	s.width = 46.25;  s.height = 46.25;
	w.add_window( 66.5+144.+36., s, 33.5 );
	w.add_window( 144.+36.+116.+3., s, 33.5 );	
	w.create( 144.+36.+116.+120. );
	w.m_angle = 0.0;
	w.m_z = 0.;
	w.m_x = 0.;	
	m_walls.push_back( w );
	w.clear();

	// Hall way 1 :
	w.add_door( 122. , 30.      );
	w.add_door( 122.+40.5, 30.  );
	w.add_door( 122.+40.5+64.5);
	w.create( 36.+116.+120.    );
	w.m_angle = 0.0;
	w.m_z = 160.;
	w.m_x = 144.;
	m_walls.push_back( w );
	w.clear();

	// Hall way 2 :
	w.add_door( 164.5, 28. );
	//w.add_door( 164.5+52.,28. );		// really need short inside walls
	w.create( 36.+116.+120.-77.5 );
	w.m_angle = 0.0;
	w.m_z = 160.+36.;
	w.m_x = 144.;
	m_walls.push_back( w );
	w.clear(); 

	// Final (kitchen to neighbors)
	w.create( 144.+36.+116.+120. );
	w.m_angle = 0.0;
	w.m_z = 160.+36.+95.;
	w.m_x = 0.;
	m_walls.push_back( w );
	w.clear();
	
	////////////////////////// horizontals //////////////////////////
	// End of Hallway
	w.create( 70. );
	w.m_angle = -90.0;	
	w.m_z = 160.; 
	w.m_x = 144.+122.+40.5+30.+2.;			// 338.5
	m_walls.push_back( w );
	w.clear();

	// Bathroom
	w.add_door( 19.5, 28. );
	w.create( 416. - 338.5 );   
	w.m_angle = 0.0;	
	w.m_z = 160.+70.; 
	w.m_x = 144.+122.+40.5+30.+2.;
	m_walls.push_back( w );
	w.clear();

	// Divide (living & bed1)
	w.create( 160. );
	w.m_angle = -90.0;	
	w.m_z = 0.;
	w.m_x = 144.;	
	m_walls.push_back( w );
	w.clear();
	
	// Divide (bed1 & bed2)
	w.create( 160. );
	w.m_angle = -90.0;	
	w.m_z = 0.;
	w.m_x = 144.+36.+116.;
	m_walls.push_back( w );
	w.clear();

	// Outside End
	s.width = 34.0;  s.height = 34.0;	
	w.add_window( 97.5+6., s, 46.0 );
	
	w.create( 160.+36.+95. );
	w.m_angle = -90.0;
	w.m_z = 0.;
	w.m_x = 144.+36.+116.+120.;		// 416
	m_walls.push_back( w );
	w.clear();

	// Inside End:
	w.add_door( 160.+18.+5.  );		// Main Entrance Door
	w.create( 160.+36.+95. );
	w.m_angle = -90.0;
	w.m_z = 0.;
	w.m_x = 0.;
	m_walls.push_back( w );
	w.clear();
	
	// Divide (kitchen & bath)
	w.add_door( 33.0, 24.0 ); 	// pantry door.
	w.create( 95.0 );
	w.m_angle = -90.0;	
	w.m_z = 160.+36.;
	w.m_x = 144.+114;
	m_walls.push_back( w );
	w.clear();

	// Divide (pantry & linen)
	w.create( 31.5 );
	w.m_angle = 0.0;	
	w.m_z = 160.+36.+26;
	w.m_x = 144.+114;
	m_walls.push_back( w );
	w.clear();
	
	// Divide (kitchen & bath)
	w.add_door( 2.0, 23.5 ); 	// linen closet "door"
	w.create( 95.0 );
	w.m_angle = -90.0;	
	w.m_z = 160.+36.;
	w.m_x = 144.+114+31.5;
	m_walls.push_back( w );
	w.clear();
	
	// Stub (entry):
	w.create( 70.5-4.25, 43. );
	w.m_angle = 0.0;	
	w.m_z = 160.+18.;
	w.m_x = 0.;	
	m_walls.push_back( w );
	w.clear();

	// Post :
	w.create( 4.25 );
	w.m_angle = 0.0;	
	w.m_z = 160.+18.;
	w.m_x = 70.5-4.25;	
	m_walls.push_back( w );
	w.clear();

	// entry closet : 
	w.m_wall_length = 70.5;		// Critical Note: This wall length has to be set prior to add door!  
								// Got lucky above because the length is not normally set until create()
	w.add_door( 35., 28. ); 	// linen closet "door"	
	w.create( 70.5 );
	w.m_angle = -90.0;	
	w.m_z = 160.+36.+95.-70.5;
	w.m_x = 74.75;	
	m_walls.push_back( w );
	w.clear();

	w.create( 74.75 );
	w.m_angle = 0.0;	
	w.m_z = 160.+36.+95.-70.5;
	w.m_x = 0.;	
	m_walls.push_back( w );
	w.clear();
}

void glRoom::draw()
{
	for (int w=0; w<m_walls.size(); w++)
		m_walls[w].draw();
}
	