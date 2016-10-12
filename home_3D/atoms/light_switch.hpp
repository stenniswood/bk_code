/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
	
	This makes a simple Table: a thin box for the table top.
	and 4 legs (cylinders).
*/
#ifndef _LIGHTSWITCH_H_
#define _LIGHTSWITCH_H_

#include "txt_container.hpp"
#include <vector>
using namespace std;

// All plugs, switches, phone jacks, etc that attach to a wall.
class glFixture : public glMolecule
{
public:
    glFixture();
    ~glFixture();
    
    float m_distance_along_wall;
    int   m_wall_side;
};



/* This is the actual handle  1/4" square.  0.6" length. */
class glLightSwitch : public glFixture
{
public:
    glLightSwitch ( );
    ~glLightSwitch( );

    void            Initialize       ( );    
    virtual void	create_components( );

    void            toggle           ( );
    void            turn_up          ( );
    void            turn_down         ( );
    void            turn_on          ( );
    void            turn_off         ( );

    glBox   m_lever;
    glBox   m_plate;

    bool    m_position_is_up;
    bool    m_on_is_up;
    
    // panel, stick, 2 screws.
	float	m_panel_width;		// Distance across the table.
	float	m_panel_height;		// how high off ground.

};

class glDualLightSwitch : public glLightSwitch
{
public:
    glDualLightSwitch ( );
    ~glDualLightSwitch( );

    glBox   m_lever2;
};


#endif

/* Light Switch Face plate:  3" by 5" by 0.25" thick */
