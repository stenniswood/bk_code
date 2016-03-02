/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "text_view.hpp" 



Map2D( ) 
{ 
}

Map2D( int Left,  int Right, int Top, int Bottom ) 
{ 
}

Map2D( int Width, int Height ) 
{ 
}

~Map2D() 
{ 
}

void 			Map2D::Initialize() 
{ 
	strcpy (class_name, "Map2D");		
}

void 			Map2D::print_info			( 						) 
{ 
}
void			Map2D::add_poly_line		( Path2d mPath 			) 
{ 
}
void			Map2D::define_room			( Rectangle mRoom, String mName	) 
{ 
}
void			Map2D::add_door				( stDoor mDoor,    String mName	) 
{ 
}

void			Map2D::set_units			( eUnitType mUnitType 	) 
{ 
}
void			Map2D::set_floor_plan_name ( string mName 			) 
{ 
}
void			Map2D::set_level_name 		( string mName 			) 
{ 
}

string			Map2D::get_room_name		( int mIndex )	
{
 return m_rooms[mIndex]; 
} 
		
int   			Map2D::draw 				(				 ) 
{ 
}	
int				Map2D::HitTestRoom			( int x, int y	 ) 
{ 
	vector<Rectangle>::iterator iter = m_rooms.begin();
	while (iter != m_rooms.end())
	{		
		m_rooms.
		iter++;
	}
}

int		 		Map2D::onClick				( int x, int y, bool mouse_is_down ) 
{ 
}
