/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
*/
#ifndef _MAP2D_H_
#define _MAP2D_H_

#include <vector>
using namespace std;
#include "wall.hpp"


class glMap2D : public glAtom
{
public:
	glMap2D(  );
	~glMap2D(  );	

	void	compute_corners	 ();
	
	// FOR 2D Drawing (and maze solving) : 
	void	create_2D_drawing( );
	void 	create_2D_mark   ( float mStartx, float mStarty, long int mColor );
	void 	generate_VBO_2D  ( );
	void 	generate_IBO_2D  ( );
	void	scale_drawing    ( float mScale );
	void	draw_2D		     ( ); 	

	void	extract_2D    ( ); 		//	abstract Walls / computes corners too.
	void	print_2D_info ( );		// 
	void	draw_corners  ( );		// corners 

	virtual void	draw_body		( );	// Override this with open gl commands.

	vector<abWall>    m_awalls;
	
};

#endif
