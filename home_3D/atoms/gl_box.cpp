#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "all_objects.h"


// width, height, depth
float template_data[] = 
{ 	 /* Bottom */
	-1.0, -1.0,  1.0,	 // 0
	 1.0, -1.0,  1.0,	 // 1
	 1.0, -1.0, -1.0,	 // 2
	-1.0, -1.0, -1.0,	 // 3

	 /* Top     long side */
	-1.0, 1.0,  1.0,	 // 4
	 1.0, 1.0,  1.0,     // 5
	 1.0, 1.0, -1.0,     // 6
	-1.0, 1.0, -1.0,	 // 7
};
// CLOSED INDICES:           bottom   top       front     ....
GLubyte q_Indices     [] = { 0,1,2,3, 4,5,6,7,  0,1,5,4,  2,3,7,6, 0,4,7,3, 1,2,6,5  }; 
// OPEN INDICES:
GLubyte s_Indices     [] = { 0,1,2,3, 0,1,5,4,  2,3,7,6, 0,4,7,3, 1,2,6,5   }; 

//const int NUMBER_OF_CUBE_INDICES = sizeof(s_Indices);
//const int NUMBER_OF_QUAD_INDICES = sizeof(q_Indices);


glBox::glBox(  )
{
    m_object_type_name = "box";    
    m_object_class  = 4;
	m_color 	= 0xFFFFFFFF;
	m_is_closed = true;
}

void glBox::generate_vertices()
{
	struct Vertex_pnc v;
	float hw = width  / 2.;
	float hh = height / 2.;
	float hd = depth  / 2.;
    // ASSIGN COLOR:
    v.color[0] = ((m_color& 0x00FF0000)>>16);
    v.color[1] = ((m_color& 0x0000FF00)>>8);
    v.color[2] = ((m_color& 0x000000FF));
    v.color[3] = ((m_color& 0xFF000000)>>24);

    for (int i=0; i<8; i++)
	{
		// SCALE TEMPLATE:
		v.position[0] = template_data[i*3  ]*hw;
		v.position[1] = template_data[i*3+1]*hh;
		v.position[2] = template_data[i*3+2]*hd;
		m_vertices.push_back(v);
	}

	// For grab_top(), etc later:
	m_max.position[0] = +hw;
	m_max.position[1] = +hh;
	m_max.position[2] = +hd;
	
	m_min.position[0] = -hw;
	m_min.position[1] = -hh;
	m_min.position[2] = -hd;	

	if (m_is_closed)
		for (int i=0; i<sizeof(q_Indices); i++)
			m_indices.push_back( q_Indices[i] );
	else
		for (int i=0; i<sizeof(s_Indices); i++)
			m_indices.push_back( s_Indices[i] );
	//printf("glBox:: m_indices.size() = %lu\n", m_indices.size() );
}

void glBox::change_top_color( long mColor )
{
    m_color = mColor;
	for (int i=0; i<m_vertices.size(); i++)
	//if (m_vertices[i].position[1]>0)
    {
        m_vertices[i].color[0] = ((mColor & 0x00FF0000)>>16);
        m_vertices[i].color[1] = ((mColor & 0x0000FF00)>>8);
        m_vertices[i].color[2] = ((mColor & 0x000000FF));			
        m_vertices[i].color[3] = ((mColor & 0xFF000000)>>24);
    }
}

void glBox::change_bottom_color( long mColor )
{
	for (int i=0; i<m_vertices.size(); i++)
	//if (m_vertices[i].position[1]<0)
    {
        m_vertices[i].color[0] = ((mColor & 0x00FF0000)>>16);
        m_vertices[i].color[1] = ((mColor & 0x0000FF00)>>8);
        m_vertices[i].color[2] = ((mColor & 0x000000FF));			
        m_vertices[i].color[3] = ((mColor & 0xFF000000)>>24);
    }
}

void glBox::setup( )
{
	glAtom::setup();
	change_color( m_color );
}

void glBox::create( )
{
	setup();
    switch(m_grab_at)
    {
        case GRAB_top   : grab_top();               break;
        case GRAB_bottom: grab_bottom();            break;
        case GRAB_front : grab_front();             break;
        case GRAB_back  : grab_back();              break;
        case GRAB_left  : grab_left();              break;
        case GRAB_right : grab_right();             break;
        default: break;
    }
	gl_register();	 
}

glMolecule* glBox::join_closest_sides( glBox* mNeighbor )
{
    static glMolecule* ptr = new glMolecule();
    ptr->setup();
    ptr->m_components.push_back(this     );
    ptr->m_components.push_back(mNeighbor);
    
    // move the neighbor for proper alignment.
    // forward & backward.  Side to side.
    //mNeighbor->relocate( m_x+width, m_y, m_z );

    // Remove this & neighbor from theWorld.
    // Add the parent molecule.
    return ptr;
}

void glBox::print_info()
{
	printf("glBox()  <x,y,z> = <%6.3f %6.3f %6.3f> \n", m_x,m_y,m_z );
	printf(" width=%6.3f;  height=%6.3f; depth=%6.3f \n", width, height, depth );			
}

void glBox::draw_body()
{	
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex_pnc), NULL);
    /* hmmm... is GL_UNSIGNED_BYTE Correct?  color is Byte*4  */
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex_pnc), (GLvoid*)(offsetof(struct Vertex_pnc,color)));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	if (m_is_closed)	
		glDrawElements(GL_QUADS,   (int)m_indices.size(), GL_UNSIGNED_INT,
					   (GLvoid*)((char*)NULL));
	else 
		glDrawElements(GL_QUAD_STRIP, (int)m_indices.size(), GL_UNSIGNED_INT,
					   (GLvoid*)((char*)NULL));
}

// see https://en.wikipedia.org/wiki/List_of_moments_of_inertia
void glBox::compute_inertia     ( float mTotalMass )
{
    Inertia[0] = (height*height + depth*depth);
    Inertia[1] = (width*width + depth*depth);
    Inertia[2] = (width*width + height*height);
    Inertia *= mTotalMass/12.;
}



bool glBox::evaluate_collision( glSphere* mOther )
{
    // for now we assume it's a sphere:
    
    // IS IT ABOVE/BELOW the box?
/*    bool candidate = false;
    float extent = mOther->m_position[1] + mOther->m_radius;
    if (extent < m_y)                return false;
    extent = mOther->m_position[1] - mOther->m_radius;
    if (extent > m_y+m_wall_height)  return false;
    
    // IS IT PAST THE START or END of the WALL?
    MathVector delta = mOther->m_position - m_line.m_origin;
    float distance_along_wall = m_line.m_vector.dot(delta);
    if (distance_along_wall < -mOther->m_radius) return false;
    if (distance_along_wall > (m_wall_length+mOther->m_radius)) return false;
    
    // We are now within the extents of the wall.  Check the actual distance away:
    MathVector perp = m_line.m_vector.get_perp_xz();
    float distance_away_from_wall = fabs(perp.dot( delta ));
    
    if (distance_away_from_wall > mOther->m_radius)
        return false;
    
    // Only chance of no collision now is in a doorway:
    long size = m_doorways.size();
    for(int d=0; d<size; d++)
    {
        // If it's in the doorway, let it pass.
        if ((distance_along_wall > m_doorways[d].position_lengthwise) &&
            (distance_along_wall < (m_doorways[d].position_lengthwise+m_doorways[d].width)) )
            if (mOther->m_y < (m_doorways[d].height-mOther->m_radius))
                return false;
    } */
    return true;
}

int glBox::get_largest_axis    ()
{
    float max = width;
    float max_index = 0;
    if (height>max)
    {
        max =height;
        max_index = 1;
    }
    if (depth>max)
    {
        max =depth;
        max_index = 2;
    }
    return max_index;
}
