//
//  multi_path_route.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/11/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "all_objects.h"
#include "multi_path_route.h"
#include "vector.hpp"

glMultiRoute::glMultiRoute( )
{
    m_object_class  = 13;
}
/*void glMultiRoute::add_way_point ( struct Vertex mStart )
{   }  */

void glMultiRoute::add_way_point2( MathVectorTree& mNode, MathVector mPoint )
{
    MathVectorTree mvt;
    mvt.item    = mPoint;
    mNode.children.push_back( mvt );       // First Child
}
void glMultiRoute::start_over_at ( struct Vertex mStart )
{
    MathVector tmp("RouteStart", 3);
    tmp[0] = mStart.position[0];
    tmp[1] = mStart.position[1];
    tmp[2] = mStart.position[2];
    m_tree.item   = tmp;
}
void glMultiRoute::start_over_at2( MathVector    mStart )
{
    m_tree.item   = mStart;
}


void glMultiRoute::generate_sample()
{
    MathVectorTree mvt,mvt2;
    MathVector     tmpv(3);
    MathVector     tmpv2(3);

    tmpv.set_xyz(50, 10, 50);
    m_tree.item = tmpv;

    // Level 1 :
    tmpv2.set_xyz(25, 0, 0);
    mvt.item    = tmpv+tmpv2;
    m_tree.children.push_back( mvt );       // First Child

    tmpv2.set_xyz(25, 0, 25);
    mvt.item    = tmpv + tmpv2;
    m_tree.children.push_back( mvt );       // Second Child

    // Prep Level 2 :
    tmpv2.set_xyz( 25, 0, 50 );
    mvt.item    = tmpv + tmpv2;

    tmpv2.set_xyz( 25, 0, -25 );
    mvt2.item   = mvt.item + tmpv2;
    mvt.children.push_back   ( mvt2 );

    tmpv2.set_xyz( 25, 0,  0 );
    mvt2.item   = mvt.item + tmpv2;
    mvt.children.push_back   ( mvt2 );
    m_tree.children.push_back( mvt  );      // Third Child

    
/*    tmpv2.set_xyz( 70, 0,  50 );
    mvt.item    = tmpv + tmpv2;
    tmpv2.set_xyz( 75, 0,  75 );
    //mvt2.item   = tmpv + tmpv2;
    //mvt.children.push_back   ( mvt2 );
    m_tree.children.push_back( mvt  );      // Fourth Child
*/
}

void	glMultiRoute::generate_vertices( MathVectorTree* mTree )
{
    static int counter = 0;
    static int level_depth = -1;
    struct Vertex v1,v2;

    // COUNT NUM CHILDREN :
    int size = (int)mTree->children.size();
    if (size==0)
        return;
    
    // FIRST NODE:
    v1.position[0] = mTree->item[0];
    v1.position[1] = mTree->item[1];
    v1.position[2] = mTree->item[2];
    level_depth++;
    counter++;
    bool test;
    bool test2;
    long ssize = m_solution.size();
    if (level_depth<ssize)
        test  = (mTree == m_solution[level_depth]);
    else test = false;
    
    list<MathVectorTree>::iterator iter = mTree->children.begin();
    while ( iter != mTree->children.end() )
    {
        if (level_depth<(ssize-1))
            test2 = (  &(*iter)==m_solution[level_depth+1]  );
        else test2 = true;
        if ( size && test && test2 )
        {
            v1.color[0] = 0xFF; v1.color[1] = 0x7F; v1.color[2] = 0xFF; v1.color[3] = 0xFF;
            v2.color[0] = 0xFF; v2.color[1] = 0x7F; v2.color[2] = 0xFF; v2.color[3] = 0xFF;
        }
        else if ((level_depth%3)==0)
        {
            v1.color[0] = 0x2F; v1.color[1] = 0x0F; v1.color[2] = 0xFF; v1.color[3] = 0x2F;     // blue
            v2.color[0] = 0x2F; v2.color[1] = 0x0F; v2.color[2] = 0xFF; v2.color[3] = 0x2F;
        }
        else if ((level_depth%3)==1)
        {
            v1.color[0] = 0x0F; v1.color[1] = 0xFF; v1.color[2] = 0x2F; v1.color[3] = 0x2F;     // green
            v2.color[0] = 0x0F; v2.color[1] = 0xFF; v2.color[2] = 0x2F; v2.color[3] = 0x2F;
        }
        else if ((level_depth%3)==2)
        {
            v1.color[0] = 0xFF; v1.color[1] = 0x7F; v1.color[2] = 0x0F; v1.color[3] = 0x2F;     // red
            v2.color[0] = 0xFF; v2.color[1] = 0x7F; v2.color[2] = 0x0F; v2.color[3] = 0x2F;
        }

        v2.position[0] = iter->item[0];
        v2.position[1] = iter->item[1];
        v2.position[2] = iter->item[2];
        m_vertices.push_back( v1 );     // Line segment goes from v1 to v2
        m_vertices.push_back( v2 );     //
        
        // RECURSE ON iter :
        MathVectorTree* ptr = (MathVectorTree*)&(*iter);
        generate_vertices(ptr);
        iter++;
    }
    level_depth--;
}

void 	glMultiRoute::setup       ( )
{
    m_vertices.clear();
    generate_vertices( &m_tree );
}

// Better to override setup and
void 	glMultiRoute::create      ( )
{
    setup();
    gl_register();
}

void glMultiRoute::draw_body( )
{
    // Make the new VBO active. Repeat here incase changed since initialisation
    glBindBuffer( GL_ARRAY_BUFFER, 		   m_VBO );
    glLineWidth ( 2.0 );
    
    glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
    glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));
    
    // Establish array contains m_vertices (not normals, colours, texture coords etc)
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY );

    // Draw Bottom :
    glDrawArrays(GL_LINES, 0, (int)m_vertices.size() );
}

void glMultiRoute::print_step( int mIndex, bool mLeft )
{
    
}
