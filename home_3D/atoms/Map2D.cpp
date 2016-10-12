#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "all_objects.h"



glMap2D::glMap2D(  )
{
    m_object_type_name = "map";
}

glMap2D::~glMap2D(  )
{
}

/* Creates a '+' type mark */
void glMap2D::create_2D_mark( float mX, float mY, long int mColor )
{
	struct Vertex v;
    v.position[1] = FLOOR_HEIGHT;	// For all points!  Since 2D.
    v.color[0] = (mColor & 0xFF000000)>>24;
	v.color[1] = (mColor & 0x00FF0000)>>16;	
	v.color[2] = (mColor & 0x0000FF00)>>8;	
	v.color[3] = (mColor & 0x000000FF);

    v.position[0] = mX;     v.position[2] = mY-5;
	m_vertices.push_back( v );
    int end = (int)m_vertices.size()-1;
    m_indices.push_back ( end );

	v.position[0] = mX;     v.position[2] = mY+5;
	m_vertices.push_back( v );
    m_indices.push_back ( end+1 );
	
	v.position[0] = mX-5;	v.position[2] = mY;
	m_vertices.push_back( v );
    m_indices.push_back ( end+2 );
	
	v.position[0] = mX+5;	v.position[2] = mY;
	m_vertices.push_back( v );
    m_indices.push_back ( end+3 );
}

// SHORTCUT(ALIAS):
#define Wall m_fwalls[w]->m_bare_wall

/* Creates Vertex data  from the Wall 3D data */
void glMap2D::create_2D_drawing()
{
	struct Vertex v;
	v.color[0] = 0xFF;	v.color[1] = 0xFF;	v.color[2] = 0xCF;	v.color[3] = 0xFF;
	v.position[1] = 2.0;	// For all points!  Since 2D.

    printf("create_2D_drawing: %lu walls. \n", m_fwalls.size());
    // FOR ALL WALLS :
    for (int w=0; w<m_fwalls.size(); w++)
	{
		// Starting Vertex :
		v.position[0] = Wall.m_line.m_origin[0];
		v.position[2] = Wall.m_line.m_origin[2];
        m_vertices.push_back( v );
		m_indices.push_back( (int)m_vertices.size()-1 );

        // interpolate distance along the wall to the distance along the rise,run
		for (int d=0; d < m_fwalls[w]->m_doors.size(); d++)
		{
			// Front edge of door:
            MathVector door_s = Wall.get_door_near_coord_plus(d,0);
            v.position[0] = door_s[0];
            v.position[2] = door_s[2];
			m_vertices.push_back( v );
            int index =(int)m_vertices.size()-1;
			m_indices.push_back( index );

			// Far edge of door:
            MathVector door_e = Wall.get_door_far_coord_plus(d,0);
            v.position[0] = door_e[0];
            v.position[2] = door_e[2];
            m_vertices.push_back( v );
			m_indices.push_back( index+1 );
		}

		// Calculate end point:
        MathVector far_e = Wall.get_far_end();
        v.position[0] = far_e[0];
        v.position[2] = far_e[2];
		m_vertices.push_back( v );
        //printf("end: %6.3f %6.3f \n", v.position[0], v.position[2] );
		m_indices.push_back( (int)m_vertices.size()-1 );
	}  
	create_2D_mark( 144, 160, 0xFFFF0000 );
	create_2D_mark( 291, 416, 0xFF00FF00 );
}

void glMap2D::draw_corners( )
{
/*	int w=0;
	int num_corners = (int)m_fwalls[w].m_corners.size();
	float x,y;
	for (int c=0; c < num_corners; c++)
	{
		x = m_fwalls[w].m_corners[c].x;
		y = m_fwalls[w].m_corners[c].z;
		create_2D_mark( x, y, 0xFFFF0000 );	
	} */
}

void glMap2D::print_2D_info( )
{
	printf("============ Map2D Abstract Wall Info ====================\n");
	for (int w=0; w<m_fwalls.size(); w++)
	{	printf("WALL #%2d\t", w );
		Wall.print_info();
        Wall.print_door_info();
        Wall.print_window_info();
	}
	printf("============ Abstract Wall Info End =================\n");
}


void glMap2D::draw_body()
{
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer( GL_ARRAY_BUFFER, 		   m_VBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );
    glLineWidth ( 2.0 );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL    );
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	//Establish array contains m_vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY );

	// Draw Bottom :
	glDrawElements(GL_LINES, (int)m_indices.size(), GL_UNSIGNED_INT, 0 );
}

struct wall_intersection_info
{
    int         index;
    float       distance;
    MathVector  intersection;
};

MathVectorTree              tree;
list<MathVector>            Alternates;                 // List of all suggested ways around a single wall.
vector<MathVectorTree*>     leaves;                     //
vector<MathVectorTree*>     buds;                       //
vector<struct wall_intersection_info>  wall_list;       // List of walls which were intersected
MathVectorTree*             FinalNode = NULL;

/* Takes 2 XYZ points and sees if they collide with any walls
    if it does, it returns the point at the end of the wall.
    OR closest door.
 
    Moves to end of any intersecting wall
RETURN:
    bool    True  => adjustment was done.
    bool    False => no adjustment needed; can directly connect mSource to mDestination
    mNewPoint   contains the way point adjustment (if needed)
 */
int glMap2D::find_closest_intersected_wall( MathVector mSource, MathVector mDestination )
{
    struct wall_intersection_info winfo;
    winfo.intersection.dimension(3);
    winfo.intersection[1] = 0.0;
    wall_list.clear();
    
    LineSegment ls1;
    int size = (int)m_fwalls.size();
    ls1.m_origin.m_name = "wall";
    double  inter_X, inter_Y;
    int     closest_index = -1;
    float  min_distance=999999.0;
    MathVector intersection(3);
    
    // SCAN ALL WALLS (for an intersection) :
    LineSegment ls2(mSource, mDestination);
    ls2.m_origin.m_name = "source/dest";
    float s;
    for (int w=0; w<size; w++)
    {
        ls1.m_origin = Wall.m_line.m_origin;
        ls1.m_vector = Wall.m_line.m_vector;
        ls1.m_length = Wall.m_wall_length;
        bool inter = ls2.line_seg_intersection( ls1, s, &inter_X, &inter_Y );
        
        // Will be true if intersection is within the ls end points.
        // Does not test the m_line end points however.
        if (inter)
        {
            winfo.index = w;
            winfo.intersection[0] = (float)inter_X;
            winfo.intersection[2] = (float)inter_Y;
            winfo.distance = mSource.get_distance(winfo.intersection);
            wall_list.push_back( winfo );
            if (winfo.distance < min_distance)
            {
                closest_index = w;
                min_distance  = winfo.distance;
            }
        }
    }
    printf( "Wall %d Intersects Path ", closest_index );  mSource.print();  mDestination.print();
    return closest_index;
}

void glMap2D::prune_alternates( MathVector mSource )
{
    // For each Alternate, make sure it doesn't cross a wall:
    list<MathVector>::iterator iter = Alternates.begin();
    while (iter != Alternates.end())
    {
        // Path: Source Point, Alternate position.
        // Does the path intersect a wall?
        //   If so, the alternate is no good.
        //int windex =
        find_closest_intersected_wall( mSource, *iter );
        
        //if (windex>=0)
        //    Alternates.remove( *iter );  put back in!
        iter++;
    }
}
void glMap2D::print_alternates(  )
{
    printf( " - %lu Alternates\n", Alternates.size() );
    list<MathVector>::iterator iter =Alternates.begin();
    while (iter != Alternates.end())
    {
        iter->print();
        iter++;
    }
    printf("\n");
}

/* Adds to the Tree & the buds list. */
void glMap2D::add_alternates( MathVectorTree& mTree )
{
    mTree.add_nodes(Alternates);
    
    // Add all children to bud list:
    MathVectorTree* ptr;
    list<MathVectorTree>::iterator iter = mTree.children.begin();
    while (iter != mTree.children.end())
    {
        ptr = (MathVectorTree*) &(*iter);
        buds.push_back( ptr );
        iter++;
    }
}

/* Add Routing point on first side of the door.   */
void glMap2D::add_before_door( MathVectorTree& mTree, int mWallIndex, list<float>& mDistancesAlong, float mSide )
{
    MathVector tmp;
    list<float>::iterator iter = mDistancesAlong.begin();   // Now, flip them all to the other side of the wall.
    while( iter != mDistancesAlong.end() )         // so that it wont detect a collision with the wall towards
    {
        tmp = m_fwalls[mWallIndex]->m_bare_wall.get_point_away_from( *iter, mSide );
        tmp[1] = 2.0;
        Alternates.push_back( tmp );
        iter++;
    }
    print_alternates();
    mTree.add_nodes( Alternates );
    // don't add to the bud list!
}

/* Add Routing point on first side of the door.   */
void glMap2D::add_after_door( MathVectorTree& mTree, int mWallIndex, list<float>& mDistancesAlong, float mSide )
{
    MathVector tmp(3);
    Alternates.clear();
    list<MathVectorTree>::iterator c_iter = mTree.children.begin();
    list<float>::iterator          iter   = mDistancesAlong.begin();   // Now, flip them all to the other side of the wall.
    while( iter != mDistancesAlong.end() )         // so that it wont detect a collision with the wall towards
    {
        tmp = m_fwalls[mWallIndex]->m_bare_wall.get_point_away_from( *iter, -mSide );
        tmp[1] = 2.0;
        c_iter->add_node( tmp );

        Alternates.push_back( tmp );
        MathVectorTree* ptr = (MathVectorTree*) &(*c_iter->children.begin());
        buds.push_back( ptr );
        iter++;
        c_iter++;
    }
    printf("buds: ");
    print_alternates();
}

/*
Leaves are all between the Source and Destination.
Path1:     leaf to destination. 
Path2:     mSource to leaf
If Path collides with a wall, then find alternate routes (buds).
For each bud, check if the Source to bud collides with a wall.

RETURN:  true    => Destination Reached!
         false   => Keep going
*/
const float WALL_SPACE_DISTANCE = 12.0;

/* Create a vector between the leaf and the mDestination.
   If it intersects a wall, 
 */
bool glMap2D::scan_leaves(  MathVector mDestination )
{
    int windex = -1;
    long size  = leaves.size();
    for (int l=0; l<size; l++)
    {
        windex = find_closest_intersected_wall( leaves[l]->item, mDestination );    // Path 1
        if (windex==-1)
        {
            // We have arrived at the destination!!
            // Add it to the Tree:
            Alternates.clear();
            Alternates.push_back(mDestination);
            add_alternates( *leaves[l] );
            printf("Route Completed - Solution Found.\n\n");
            FinalNode = (MathVectorTree*) &(*leaves[l]->children.begin());
            return true;
        }
        else if (windex>=0)     // Yes, a Wall is in the way
        {
            // Find ways around the wall :
            Alternates.clear();
            float side =  WALL_SPACE_DISTANCE * m_fwalls[windex]->m_bare_wall.is_on_which_side( leaves[l]->item );
            
            list<float> AlternateDistances;
            m_fwalls[windex]->m_bare_wall.find_paths_around( AlternateDistances );
            
            // Both will get added to the tree hierarchy.
            // Before door nodes each have 1 child node (the after door).
            add_before_door( *leaves[l], windex, AlternateDistances, side );

            add_after_door ( *leaves[l], windex, AlternateDistances, side );
            //add_alternates( *leaves[l] );   // add to the bud list!
            //print_alternates();
            

            // We add 1" so that the line segment intersection code won't
            // detect our new alternates as colliding with it's own wall towards the source.
            //prune_alternates( leaves[l]->item );
        }
    }
    return false;
}

/*
 
 */
void glMap2D::map_route( glMultiRoute& mRoute, MathVector mSource, MathVector mDestination )
{
    bool complete=false;
    mRoute.m_tree.clear( );
    mSource[1] = 2.0;
    mDestination[1] = 2.0;
    mRoute.start_over_at2( mSource );

    MathVectorTree*  parent = &(mRoute.m_tree);
    parent->item            = mSource;

    // Make Source the first leaf :
    leaves.clear();
    leaves.push_back( parent );
    int counter=0;
    while (complete == false)
    {
        buds.clear();
        complete = scan_leaves( mDestination );
        leaves = buds;
        counter++;
    }
    mRoute.m_solved_end_node = FinalNode;

    // FINAL PATH:
    mRoute.FinalPath.clear();
    mRoute.m_solution.clear();
    FinalNode->extract_path( mRoute.FinalPath );
    FinalNode->extract_path_pointers(mRoute.m_solution);
}

// Recursive :
/*bool glMap2D::keep_adjusting( MathVectorTree& mTree, MathVector mSource, MathVector mDestination )
{
    int windex = find_closest_intersected_wall( mSource, mDestination );         // leaves may get added
    // first finish any else at current level.
    if (windex>=0)
    {
        add_alternates( mTree );
        
        list<MathVectorTree>::iterator iter =  mTree.children.begin();
        while ( iter != mTree.children.end() )
        {
            //            adjust = adjust_route( *iter, mSource, mDestination );      // leaves may get added
            iter++;
        }
    }
    return false;
}*/
