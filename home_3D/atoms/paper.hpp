/*  A drawer is 3 extrusions plus a face plate (extrusion for now - may be more
	elaborate later)
	
	The drawer may be open [0..100] percent.
	m_x,m_y,m_z specify the closed position.
	It slides along the x axis.  Use rotate 90 deg about y,  if z is desired.
	
	When the drawer is open 100%, it may only be 90% of the depth.  (m_open_limit)
	
*/
#ifndef _PAPER_H_
#define _PAPER_H_

#include <vector>
using namespace std;



struct stTexCoord {
	float u;
	float v;
};

/* 
The drawer positioning code could go either on the drawer, or on the cabinet.

ORIGIN :  Bottom, Left 
*/
class glPaper : public glAtom
{
public:
	glPaper( int mSamples_per_inch = 2 );  // 100/8 = 25/2 ~= 12

	void			load_texture	( const char* mFilename );	
	
	// Creates a grid:
	void			Initialize		( float mWidth=8.5, float mHeight=11.0 );
	void			generate_indices(  );
	GLuint 			generate_TBO	(  );
	void 			generate_VBOTexCoords();

	void			create		( );	// create vertices,indices,VBO/IBO/TBO's
	virtual void	draw_body	( );	// 
	
	GLuint			m_TBO;				// The image 
	GLuint			m_VBOTexCoords;		// The coordinates
	vector<struct stTexCoord>	m_TexCoords;

	Mat				m_src;
	int				m_samples_per_inch;
	int				m_Xsamples;
	int				m_Ysamples;
};


#endif
