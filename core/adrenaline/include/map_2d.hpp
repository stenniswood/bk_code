#ifndef _MAP_2D_
#define _MAP_2D_

#include "control.hpp"

#include <vector>
using namespace std;


struct stPoint {
	float x;
	float y;	
};
typedef vector<stPoint> Path2d;

struct stDoor {
	stPoint hinge;
	stPoint handle;
};

/* A 2D floor plan
	Combination of poly lines.
	The user may section of rooms and label them.

	This 2D map can be used for defining the robot position.
	And if combined with vision system may be used to determine the
	locations of walls.

	For a 2, 3, or more story house, combine these classes.
	This only does 1 floor.	
*/

enum eUnitType {
	UNIT_INCHES,
	UNIT_FEET,
	UNIT_METERS,
	UNIT_CM
};


class Map2D : public Control 
{
public:
	Map2D( );
	Map2D( int Left,  int Right, int Top, int Bottom );
	Map2D( int Width, int Height );	
	~Map2D();

	void 			Initialize();
	
	void 			print_info			( 						);
	void			add_poly_line		( Path2d mPath 			);
	void			define_room			( Rectangle mRoom, string mName	);
	void			add_door			( stDoor mDoor,    string mName	);

	void			set_units			( eUnitType mUnitType 	);
	void			set_floor_plan_name ( string mName 			);
	void			set_level_name 		( string mName 			);

	Rectangle		get_room			( int mIndex )	{ return m_rooms[mIndex]; };
	string			get_room_name		( int mIndex );//	{ return m_room_names[mIndex]; };
			
	virtual int   	draw 				(				 );	

	int				HitTestRoom			( int x, int y	 );
	int		 		onClick				( int x, int y, bool mouse_is_down );

protected:
	// Floor plan is a combination of poly lines.  Not every thing is connected.  Maybe islands, etc.	
	vector<Path2d>		m_floor_plan;
	vector<Rectangle>	m_rooms;
	vector<string>		m_room_names;	
	string				m_floor_plan_name;	// My apartment, my house, friends house, office.

private:

};

 
#endif
