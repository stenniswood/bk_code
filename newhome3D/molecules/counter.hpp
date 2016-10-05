/* This class will generate OpenGL object for a counter top.
	ie. it is composed of several cabinets below and a "formica"
	top piece.

	This makes a simple counter top.  ie. multiple cabinets	
*/
#ifndef _COUNTER_H_
#define _COUNTER_H_

#include <vector>
#include "verbal_object.h"

using namespace std;


class glCabinet;


class glCounter : public VerbalObject
{
public:
	glCounter( float mLength, float mWidth, float mHeightOffFloor );
	
	void	set_number_of_cabinets( int mNumber );
	void	Initialize	( );
	void 	create		( );
	//void	Relocate	( float mX, float mY, float mZ );
	
	void 	generate_IBO( );
	void 	generate_VBO( );		
	virtual void	draw_body( );	// Override this with open gl commands

	glBox               m_counter_top;
	vector<glCabinet> 	m_cabinets;
	
	void 	open(int mIndex, float mFraction );

	
/* These are stored in m_counter_top!
	float	m_counter_length;		// Length along longest side.
	float	m_counter_depth;		// Length along longest side.
	float	m_counter_height;		// Height along longest side.
*/
/*	GLuint	m_VBO;		 
	Don't need since it's a molecule (compound object).  Each sub component will have 
	it's own!
*/

};


#endif
