#ifndef _ROOM_HPP_
#define _ROOM_HPP_


#include <string>
#include "vector.hpp"


/* To start with this will be a simple rectangle and height.
    No other shaped rooms (hexagonal, etc are allowed) for ease of getting a basic room class done.
	for a box of given width, height, depth. 
*/

class Room
{
public:
	Room  ( );
    ~Room ( );
    
    void    create_walls();     // optional.  will create the walls  TBD

	float           m_height;
    std::string     m_name;       // regex
    MathVector  	m_origin;
    MathVector  	m_orientation;
};


#endif
