/*  A drawer is 3 extrusions plus a face plate (extrusion for now - may be more
	elaborate later)
	
	The drawer may be open [0..100] percent.
	m_x,m_y,m_z specify the closed position.
	It slides along the x axis.  Use rotate 90 deg about y,  if z is desired.

 The drawer positioning code could go either on the drawer, or on the cabinet.
 ORIGIN :  Bottom, Left

	When the drawer is open 100%, it may only be 90% of the depth.  (m_open_limit)  */
#ifndef _PAPER_H_
#define _PAPER_H_

//#include "all_objects.h"
#include "gl_atom.hpp"

#include <vector>
//using namespace cv;
using namespace std;

struct stTexCoord {
	float u;
	float v;
};

class glPaper : public glAtom
{
public:
	glPaper         ( int mSamples_per_inch = 1 );
    void            load_texture        ( const char* mFilename       );
    void            set_width_height    ( float mWidth, float mHeight );
    
	// Creates a grid :
	void			Initialize          ( float mWidth=8.5, float mHeight=11.0 );
	void			generate_indices    (  );
    virtual void    generate_vertices   (  );
    void            wave_it             ( float mAmplitude, float mWavelengths, float mPhase );
    void            fold_it             ( int mLeadingFoldSamples, float mSlope1, float mSlope2 );
    
	void			half_circle         ( float mRadius, int mStartXindex                  );
	void			pull_back_to        ( float mDistanceFromLeft, float mDistanceFromPage );
	void			setup               ( );
	void			create              ( );	// create vertices,indices,VBO/IBO/TBO's
	virtual void	draw_body           ( );
    virtual void	draw_primitive      ( );	//

	void			print_texCoords( );	

	int				m_samples_per_inch;
	int				m_Xsamples;
	int				m_Ysamples;
	
	float			m_width;
	float			m_height;
};


#endif

