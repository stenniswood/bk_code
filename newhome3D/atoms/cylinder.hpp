/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
*/
#ifndef _CYLINDER_H_
#define _CYLINDER_H_
#include "extrusion.hpp"


/* Stretches lengthwise on the Y axis (up / down)
 */
class glCylinder : public glExtrusion
{
public:
	glCylinder( int mNumberSamples );

	virtual void	generate_vertices       ( );	
	virtual size_t 	generate_disc_indices   ( GLuint mStart_of_top_index );
    void            set_height              ( float mHeight   );
    float           get_height              (                 );
    void            compute_inertia         ( float mTotalMass );
	virtual void	draw_body               (                 );

	float       m_radius;
    //float     m_height;             Use m_extrusion_length instead.  Baseclass.
	int         m_number_of_samples;

};


#endif

