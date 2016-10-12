
#ifndef _FOOT_H_
#define _FOOT_H_
/* 
   The foot will be an extrusion
	with a fork at the ankle - we don't need that right now. it will just look discontected. 
	
*/
#include "verbal_object.h"
#include "extrusion.hpp"

class glFootShape : public glExtrusion
{
public:
	glFootShape();

	virtual void	generate_vertices      ( );
	virtual size_t 	generate_disc_indices  ( GLuint mStart_of_top_index );
	virtual void	draw_body			   ( );
};

struct stFootPosition {
    float heel[3];      // x,y,z positions
    float toe[3];       // x,y,z positions
    float angle[3];		// x,y,z angles
};

void print_foot_position(struct stFootPosition fp);


class glFoot : public VerbalObject
{
public:
	glFoot();
    void            setup           (  );
    void            create          (  );
    virtual void	gl_register     (  );
    virtual void    gl_unregister   (  );

    virtual void        draw_body        ( );
    void                calc_angle       ( struct stFootPosition &mfp );
    //void                make_flat_footed ( );     Deprecated - call the Leg function instead (leg has knowledge of all leg angles)
    
    MathVector          get_vector();    
    float               get_floor_projection_angle(  );     // X,Z coordinates
    float               get_elevation_angle(  );            // Y raise/lower
    
    glm::vec4           get_heel_robot_coords( );      // foot coords
    glm::vec4           get_toe_robot_coords ( );       // foot coords
    
	glFootShape         m_gl_foot;

    struct stFootPosition m_world_coords;
    struct stFootPosition m_robot_coords;
    struct stFootPosition m_foot_coords;     // heel & toe location in foot coordinates
    
    float           m_ankle_height;      // floor to center of ankle pivot;
    float           m_foot_length;
    float           m_ankle_to_heel;    // distance
};



#endif

