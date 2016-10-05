/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
	
	This makes a simple Table: a thin box for the table top.
	and 4 legs (cylinders).
*/

#ifndef _GL_BAREWALL_H_
#define _GL_BAREWALL_H_

//#include "all_objects.h"
//#include "abstract_wall.hpp"
#include <vector>
#include "door.hpp"

using namespace std;


const float DEFAULT_DOOR_WIDTH	    = 30.;
const float DEFAULT_WALL_HEIGHT 	= 10.*12.;	// inches  (was 8.*12.)
const float DEFAULT_WALL_THICKNESS  = 6.;		// inches


struct doorway_info {
    int   FirstVertexIndex;
    float position_lengthwise;
    float width;
    float height;
};
struct stSize {
    float width;
    float height;
};

#define m_wall_thickness m_extrusion_length

class glBareWall : public glExtrusion
{
public:
	glBareWall ( );
    ~glBareWall( );

    void            clear                   (	);
    void            Initialize              (   );
    
    void            set_length_height       ( float mLength, int  mHeight=DEFAULT_WALL_HEIGHT );
	void            set_texture             ( int mSelection );
    void            setup                   (  );    
    float           get_visible_height      (   );

	// ADD DOORS & WIDOWS BEFORE CREATE()!
	bool            add_door                (   float mPositionLengthwise,
                                                float mWidth       =DEFAULT_DOOR_WIDTH,
                                                float mHeight      =79.0,
                                                bool  mDoorwayOnly =false );

	bool            add_window                  ( float mPositionLengthwise,
                                                  struct stSize mSize,
                                                  float mHeight=36.0        );
    bool            is_valid_location           ( float mPositionLengthwise, float mWidth = 36.0 );
    void            generate_window_texture_coords( );
    void            generate_texture_coords     (   );

	// VERTICES:
    virtual void	generate_vertices           ( );
    void			generate_wall_vertices      ( );
	void			generate_door_vertices      ( );
	void			generate_window_vertices    ( );

	// INDICES:	
    virtual void 	generate_indices            ( );
    void 			generate_marks              ( );
    void			generate_wall_indices       ( );
	void			generate_window_side_indices( );
	void 			generate_wall_side_indices  ( );

    // PRINT:
	void			print_marks                 ( );
    void            print_info                  ( );
    void            print_door_info             ( );
    void            print_window_info           ( );
    virtual void	draw_body                   ( );
    
    //=============== VARIABLES ==========================
    float           m_wall_length;              // in inches
    float           m_wall_height;              // in inches
    bool            m_show_half_height;
    int             m_windows_index_start;		// index into m_indices where the window edges start.
    
    
    // These are for punching out the holes only.  don't contain the actual door.  too much info in it.
    // swing, open, hinge side, etc.  The doors, plugs, lightswitches, etc will be stored in FullWall molecule.
    vector<struct stWindow>     m_windows ;
    vector<struct doorway_info> m_doorways;
};


#endif
