







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
void			Map2D::add_door			( stDoor mDoor,    String mName	) 
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
}

int		 		Map2D::onClick				( int x, int y, bool mouse_is_down ) 
{ 
}
