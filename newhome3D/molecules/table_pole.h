/*

NAME    :  table_pole.h
PROJECT :  home3D

  Created by Stephen Tenniswood on 6/6/15.
  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.

  This class will generate OpenGL Vertex data
  for a restaurant style table. ie. 1 center pole, round top, feet beneath.
	
 */

#ifndef __home3D__table_pole__
#define __home3D__table_pole__

#include <stdio.h>
#include "txt_container.hpp"
#include "cylinder.hpp"
#include <vector>
using namespace std;

class glTableCenterPole : public glMolecule
{
public:
    glTableCenterPole( );
    
    void	Initialize	( );
    virtual void	create_components( );
    
    glCylinder          m_table_top;
    glCylinder          m_center_pole;
    vector<glBox>       m_feet;

    float	m_table_radius;		// Length along 1 side.
    float	m_table_num_feet;	// Distance across the table.
    float	m_table_height;		// how high off ground.
    
};

#endif /* defined(__home3D__table_pole__) */








