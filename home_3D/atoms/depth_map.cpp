#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include "global.h"

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif
#include "all_objects.h"
#include "depth_map.hpp"

int i_width  = 100;
int i_height = 100;
uint16_t depth[50*50];

/* Will compute slope over a grid of gradients */
struct stGradient {
	float Mx;
	float My;
	float Bx;
	float By;
};
std::vector<struct stGradient> Grid;


uint16_t& get_depth( uint16_t* mImage, int x, int y )
{
	return mImage[x+y*i_width];
}



/* 
	n    = number of data points
	x,y  = arrays of data
	*b   = output intercept
	*m   = output slope
	*r   = output correlation coefficient (can be NULL if you don't want it)
*/
int linreg(int n, const float x[], const float y[], float* m, float* b, float* r)
{
        float   sumx = 0.0;         /* sum of x        */
        float   sumx2 = 0.0;        /* sum of x**2     */
        float   sumxy = 0.0;        /* sum of x * y    */
        float   sumy = 0.0;         /* sum of y        */
        float   sumy2 = 0.0;        /* sum of y**2     */

       for (int i=0;i<n;i++)   
          { 
          sumx  += x[i];       
          sumx2 += sqr(x[i]);  
          sumxy += x[i] * y[i];
          sumy  += y[i];      
          sumy2 += sqr(y[i]); 
          } 

       float denom = (n * sumx2 - sqr(sumx));
       if (denom == 0) {
           // singular matrix. can't solve the problem.
           *m = 0;
           *b = 0;
           if (r) *r = 0;
           return 1;
       }

       *m = (n * sumxy  -  sumx * sumy) / denom;
       *b = (sumy * sumx2  -  sumx * sumxy) / denom;
       if (r!=NULL) {
          *r = (sumxy - sumx * sumy / n) /          /* compute correlation coeff     */
                sqrt((sumx2 - sqr(sumx)/n) *
                (sumy2 - sqr(sumy)/n));
       }
       return 0; 
    
}

bool in_range( uint16_t mValue, uint16_t mClose, uint16_t mFar )
{
	if ((mValue > mClose) && (mValue < mFar))
		return true;
	return false;
}

void compute_slope( uint16_t* mImage, int xStart, int xPixels, 
									  int yStart, int yPixels,
									  struct stGradient* mGrad )
{
	uint16_t near, far;
	float xs[xPixels*yPixels];
	float ys[xPixels*yPixels];
	int n=0;
	int index;
	int x,y;
	for (x=xStart; x < xStart+xPixels; x++)
		for (y=yStart; y < yStart+yPixels; y++)
		{
			index = y*i_width + x;
			//if ( in_range(mImage[index], near, far )
			xs[n] = (float)x;
			ys[n] = (float)get_depth(mImage, x, y );
			n++;
		}
	linreg( n, xs, ys, &(mGrad->Mx), &(mGrad->Bx), NULL );

	n=0;
	for (x=xStart; x < xStart+xPixels; x++)
		for (y=0; y < yStart+yPixels; y++)
		{
			index = y*i_width + x;
			//if ( in_range(mImage[index], near, far )
			xs[n] = y;
			ys[n] = get_depth(mImage, x, y );
			n++;
		}
	linreg( n, xs, ys, &(mGrad->My), &(mGrad->By), NULL);
}

/* 
	Scan the grid (xPixels by yPixels) over the whole image.
*/
void compute_slopes( uint16_t* mImage, int xPixels, int yPixels )
{
	struct stGradient grad;
	int xbins = trunc( i_width / xPixels );
	int ybins = trunc( i_width / yPixels );
	for (int x=0; x<xbins; x++)
	  for (int y=0; y<ybins; y++)
	  {
	  	compute_slope( mImage, x*xPixels, xPixels, y*yPixels, yPixels, &grad );
		Grid.push_back( grad );		
	  }	  
}

void vector_sum_gradients( struct stGradient  grad )
{
	
}
/*
	
*/
void draw_gradients( uint16_t* mImage, int xPixels, int yPixels )
{
	struct stGradient grad;
	int xbins = trunc( i_width / xPixels );
	int ybins = trunc( i_width / yPixels );
	int X,Y;
	for (int x=0; x<xbins; x++)
	  for (int y=0; y<ybins; y++)
	  {
	  	X= x*xPixels;
	  	Y= y*yPixels;
	  	//drawLine( X, Y, X+Grid[], Y+Grid[] );
		Grid.push_back( grad );		
	  }	  
}


/****************************************************************/
/***														  ***/
/****************************************************************/
glDepthMap::glDepthMap ()
{
	set_demo();
}
glDepthMap::~glDepthMap()
{
	
}

void glDepthMap::set_demo( )
{
	// Setup experimental image :
	for (int x=0; x<i_width; x++)
		for (int y=0; y<i_height; y++)
		{
			float sqr = (y-25)*(y-25);
			uint16_t val = 10 +2*x + trunc(sqr);
			get_depth( depth, x,y ) = val;
		}
}

void glDepthMap::set_image( char* mImage, int width, int height )
{
	// Setup experimental image :
	for (int x=0; x<i_width; x++)
		for (int y=0; y<i_height; y++)
		{
			float sqr = (y-25)*(y-25);
			get_depth( depth, x,y ) = 10 +2*x + trunc(sqr);
		}
}

void glDepthMap::load_image( Mat* mLevel )
{

}

uint16_t glDepthMap::compute_depth(int x, int y)
{
	return get_depth(depth, x,y);
}

void glDepthMap::generate_vertices( )
{
	struct Vertex_pnc v;
	for (int x=0; x<i_width; x++)
		for (int y=0; y<i_height; y++)
		{
			uint16_t Depth = compute_depth( x,y );
			v.position[0] = x;
			v.position[1] = Depth;
			v.position[2] = y;
			m_vertices.push_back( v );						
		}
}

void glDepthMap::draw_body()
{
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));
        /* color pointer should be GL_UNSIGNED_LONG shouldn't it?! */    
    
	//Establish array contains vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY );

	// We draw the top and bottoms as GL_POLYGON and the sides as GL_QUAD_STRIP.
	// Both sets of indices stored in GL_INDEX_ARRAY.  With a known offset for each 
	// of the 3 sets.
	glPointSize( 5.0f );
	
	// Draw Bottom : 
	glDrawArrays (GL_POINTS, (int)0, m_vertices.size() );
	//glDrawElements(GL_POINTS, (int)m_vertices.size(), GL_UNSIGNED_INT, 0 );
 
	// Draw the sides: m_number_side_indices
//	glDrawElements(GL_QUAD_STRIP, m_layer_one_vertices*2., GL_UNSIGNED_INT,
//				   (GLvoid*)BUFFER_OFFSET( m_layer_one_indices ) );
}
