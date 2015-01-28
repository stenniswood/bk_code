/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
	
	This makes a simple Table: a thin box for the table top.
	and 4 legs (cylinders).
*/
#ifndef _GL_WALL_H_
#define _GL_WALL_H_

#include "txt_container.hpp"
#include "extrusion.hpp"

#include <vector>
using namespace std;


//const float WALL_THICKNESS	= 5.0; 
//const float WALL_HEIGHT 	= 10.0; 
const float STANDARD_WALL_HEIGHT 	= 10.*12.;	// inches
const float STANDARD_WALL_THICKNESS = 6.;		// inches


struct stDoorWay {
	int	  FirstVertexIndex;		// Stores the index into m_vertices
	float PositionLengthwise;	// 
	float Width;				// 
	float Height;				// 
};

struct stWindow {
	int	  FirstVertexIndex;		// Stores the index into m_vertices
	float PositionLengthwise;
	float Width;
	float Height;
	float SillHeight;	
};

struct stSize {
	float width;
	float height;
};

class glWall : public glExtrusion
{
public:
	glWall( );
	glWall( float mLength );

	void	set_texture		 (int mSelection);
	void	generate_texture_coords	( );
	
	void	clear			 (	);
	bool	is_valid_location( float mPositionLengthwise, float mWidth = 36.0 );
	bool	add_door	  	 ( float mPositionLengthwise, float mWidth = 36.0, float mHeight=96.0 );
	bool	add_window	  	 ( float mPositionLengthwise, struct stSize mSize, float mHeight=36.0 );
	// ADD DOORS & WIDOWS BEFORE CREATE()!
	void	create		  	 ( float mLength, int  mHeight = STANDARD_WALL_HEIGHT  );
	void	sort			 (	); 		// arrange doors and windows by ascending order

	// VERTICES:
	void	generate_wall_vertices	( );
	void	generate_door_vertices	( );
	void	generate_window_vertices( );		

	// INDICES:	
	void 	generate_marks			( );
	void	generate_wall_indices	( );
	void	generate_window_side_indices( );
	void 	generate_wall_side_indices();

	void	draw(); 
	void	print_marks();	

	float	m_wall_length;			// in inches
	float	m_wall_height;			// in inches
	
	float	m_angle;		// 0.0 is vertical (looking at the map)
	int		m_windows_index_start;
	vector<struct stDoorWay>	m_doors;
	vector<struct stWindow >	m_windows;	
	Texture*					m_texture;

	// protected data:	

};


#endif
