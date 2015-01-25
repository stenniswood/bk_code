#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "gl_container.hpp"

float OLD_template_data[] = 
{ 	 /* Bottom */
	 1.0, -1.0,  1.0,	 // 0
	 1.0, -1.0, -1.0,	 // 1
	-1.0, -1.0,  1.0,	 // 2
	-1.0, -1.0, -1.0,	 // 3
//	 1.0, -1.0,  1.0,	 //  back home.

	 /* Top */
	-1.0, 1.0,  1.0,	 // 4
	-1.0, 1.0,  -1.0,    // 5
	 1.0, 1.0,  1.0,     // 6
	 1.0, 1.0,  -1.0,	 // 7
//	 1.0, 1.0,  1.0		// back home
};

float template_data[] = 
{ 	 /* Bottom */
	 1.0, -1.0,  1.0,	 // 0
	 1.0, -1.0, -1.0,	 // 1
	-1.0, -1.0, -1.0,	 // 2
	-1.0, -1.0,  1.0,	 // 3

	 /* Top */
	 1.0, 1.0,  1.0,	 // 4
	 1.0, 1.0, -1.0,    // 5
	-1.0, 1.0, -1.0,     // 6
	-1.0, 1.0,  1.0,	 // 7
};

// 2,6,5, 5,3,2,
// good 4,5, 6,7,0,1, 7,1, 5,3
// CLOSED INDICES:
GLubyte OLD_q_Indices     [] = { 0,1,3,2, 4,5,7,6,  0,1,7,6,  2,3,5,4, 0,2,4,6, 1,3,5,7  }; // , 5,1, 2,6,7,3
GLubyte q_Indices     [] = { 0,1,2,3, 4,5,6,7,  0,1,5,4,  2,3,7,6, 0,4,7,3, 1,2,6,5  }; // , 5,1, 2,6,7,3
// OPEN INDICES:
GLubyte s_Indices     [] = { 0,1,2,3, 2,4, 3,5, 1,7, 0,6, 2,4   }; 
const int NUMBER_OF_CUBE_INDICES = sizeof(s_Indices);
const int NUMBER_OF_QUAD_INDICES = sizeof(q_Indices);

float normals_data[] = 
{
	1.0,  0.0,  0.0,
	0.0,  1.0,  0.0,
	0.0,  0.0,  1.0, 
	-1.0,  0.0,  0.0,
	 0.0, -1.0,  0.0,
	 0.0,  0.0, -1.0
};

glContainer::glContainer(  )
{
	m_x = 0.0;
	m_y = 0.0;
	m_z = 0.0;
	m_color = 0xFFFFFFFF;
	m_is_closed = false;
	m_number_of_vertices = 8;

	generate_vertices();
}

void glContainer::generate_vertices()
{	
	float hw = width  / 2.;
	float hh = height / 2.;
	float hd = depth  / 2.;	
	for (int i=0; i<m_number_of_vertices; i++)
	{
		// SCALE TEMPLATE:
		vertices[i].position[0] = template_data[i*3  ]*hw;
		vertices[i].position[1] = template_data[i*3+1]*hh;
		vertices[i].position[2] = template_data[i*3+2]*hd;

		// ASSIGN COLOR:
		// vertices[i].color[1] = 0xFF;		
		vertices[i].color[0] = ((m_color& 0x00FF0000)>>16);
		vertices[i].color[1] = ((m_color& 0x0000FF00)>>8);
		vertices[i].color[2] = ((m_color& 0x000000FF));								
		vertices[i].color[3] = ((m_color&0xFF000000)>>24);
	}
}

void glContainer::change_top_color( long mColor )
{
	for (int i=0; i<m_number_of_vertices; i++)
		if (vertices[i].position[0]>0)
		{
			vertices[i].color[0] = ((mColor & 0x00FF0000)>>16);
			vertices[i].color[1] = ((mColor & 0x0000FF00)>>8);
			vertices[i].color[2] = ((mColor & 0x000000FF));			
			vertices[i].color[3] = ((mColor & 0xFF000000)>>24);
		}
}

void glContainer::change_bottom_color( long mColor )
{
	for (int i=0; i<m_number_of_vertices; i++)
		if (vertices[i].position[0]<0)
		{
			vertices[i].color[0] = ((mColor & 0x00FF0000)>>16);
			vertices[i].color[1] = ((mColor & 0x0000FF00)>>8);
			vertices[i].color[2] = ((mColor & 0x000000FF));			
			vertices[i].color[3] = ((mColor & 0xFF000000)>>24);
		}
}

/* 
Use this to grasp the object in a different location.
ie. to swing a door, it's better to set the 0.0 at the edge of the box.

Note:  Use glTranslate() to move the object (ie. set m_x,m_y,m_z)
*/
void glContainer::add_offset(float mX, float mY, float mZ)
{
	for (int i=0; i<m_number_of_vertices; i++)
	{
		vertices[i].position[0] += mX;
		vertices[i].position[1] += mY;
		vertices[i].position[2] += mZ;
	}
}

void glContainer::grab_top( )
{
	add_offset( 0., -height/2., 0. );
}
void glContainer::grab_bottom( )
{
	add_offset( 0., +height/2., 0. );
}
void glContainer::grab_left( )
{
	add_offset( -width/2., 0., 0. );
}
void glContainer::grab_right( )
{
	add_offset( width/2., 0., 0. );
}


void glContainer::generate_IBO()
{
	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	if (m_is_closed)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUMBER_OF_QUAD_INDICES*sizeof(GLubyte), 
				q_Indices, GL_STATIC_DRAW);
	
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUMBER_OF_CLOSED_INDICES*sizeof(GLubyte), 
//				closed_Indices, GL_STATIC_DRAW);
	else 
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUMBER_OF_CUBE_INDICES*sizeof(GLubyte), 
				s_Indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
}

void glContainer::generate_VBO()
{
	//Create a new VBO and use the variable id to store the VBO id
	glGenBuffers( 1, &m_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );	//Make the new VBO active  GL_COLOR_ARRAY

	// Generate & Upload vertex data to the video device 
	int size = number_of_vertices * sizeof(float);
	size = number_of_vertices * sizeof(Vertex);
	glBufferData( GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW );
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// vertex positions	
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(struct Vertex,color)));	// color
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );	

	//Draw Triangle from VBO - do each time window, view point or data changes
	//Establish its 3 coordinates per vertex with zero stride in this array; necessary here

	//glVertexAttribPointer(1, 3, GL_FLOAT, 		GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(struct Vertex, normal)));	// normals

	glEnableVertexAttribArray( 0 );	
	//glEnableVertexAttribArray( 1 );	
	glEnableVertexAttribArray(2);	// GLKVertexAttribColor	
	
	//glVertexPointer(3, GL_FLOAT, 0, NULL);
	//glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}

void glContainer::print_info()
{
	printf("glContainer()  <x,y,z> = <%6.3f %6.3f %6.3f> \n", 
			m_x,m_y,m_z );
	printf(" width=%6.3f;  height=%6.3f; depth=%6.3f> \n", 
			width, height, depth );
			
}
	
void glContainer::draw()
{
	glTranslatef(m_x, m_y, m_z);
	
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO			);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	if (m_is_closed)	// GL_QUADS
		glDrawElements(GL_QUADS, NUMBER_OF_QUAD_INDICES, GL_UNSIGNED_BYTE, 
					(GLvoid*)((char*)NULL));
	//else 	//GL_QUAD_STRIP
	glDrawElements(GL_QUAD_STRIP, NUMBER_OF_CUBE_INDICES, GL_UNSIGNED_BYTE, 
					(GLvoid*)((char*)NULL));

	// glDrawArrays(GL_TRIANGLE_STRIP, 0, m_number_of_vertices );
	// GL_POLYGON, GL_LINE_LOOP
	glTranslatef(-m_x, -m_y, -m_z);		
}
