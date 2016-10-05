/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
	
	This makes a simple Table: a thin box for the table top.
	and 4 legs (cylinders).
*/
#include "txt_container.hpp"
#include <vector>
#include "cylinder.hpp"

using namespace std;

#ifndef _TABLE_H_
#define _TABLE_H_

class glTable : public glMolecule
{
public:
	glTable(  );
    ~glTable(  );

	void	Initialize	( );
    void	create_components();


	txtContainer 		m_table_top;
	vector<glCylinder*>	m_table_legs;
	
	float	m_table_length;		// Length along 1 side.
	float	m_table_width;		// Distance across the table.
	float	m_table_height;		// how high off ground.

};


#endif
