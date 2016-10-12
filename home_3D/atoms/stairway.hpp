#ifndef _STAIRWAY_H_
#define _STAIRWAY_H_

#include "extrusion.hpp"
//#include "

/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
*/

class glStairway : public glExtrusion
{
public:
	glStairway	(  );
    glStairway	( int mNumberOfSteps );
    glStairway	( int mNumberOfSteps, float rise, float run );
    
	virtual void	generate_vertices  (  );
    
    float           get_height    ( int mstep=-1 );     // default is max height
    float           get_front_edge( int mstep=-1 );     // default is top step
    virtual void	draw_body     (              );
    
	float 	 m_rise;
	float 	 m_run;		
	//float 	 m_width;           USE m_extrusion_length!
	int		 m_number_of_steps;
    
};


#endif
