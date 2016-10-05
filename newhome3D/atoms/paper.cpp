#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "Texture.hpp"
#include "paper.hpp"
#include "all_objects.h"

// Include OpenCV here for the imread function!  
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
// Include OpenCV here for the imread function!

using namespace cv;
using namespace std;


glPaper::glPaper( int mSamples_per_inch )
{
    m_object_type_name  = "paper";
    m_samples_per_inch  = mSamples_per_inch;
    Initialize( 10, 10.0);
}

void glPaper::Initialize( float mWidthInches, float mHeightInches )
{
    //printf("samples_per_inch=%d\n", m_samples_per_inch );
    m_object_class  = 8;
    m_width    = mWidthInches;
    m_height   = mHeightInches;
    m_Xsamples = m_samples_per_inch * mWidthInches ;
    m_Ysamples = m_samples_per_inch * mHeightInches;
    //printf("samps_per_inch=%d\n",           m_samples_per_inch      );
    //printf("xsamples=%d;  ysamples=%d  \n", m_Xsamples, m_Ysamples  );
    //printf("mWidth=%6.1f; mHeight=%6.1f\n", mWidthInches, mHeightInches );
}

void glPaper::set_width_height( float mWidth, float mHeight )
{
    Initialize( mWidth, mHeight );
}

void glPaper::print_texCoords( )
{
    for (int i=0; i<m_texture->m_TexCoords.size(); i++)
        printf(" TexCoord[%d] =  %6.3f, %6.3f  \n", i, m_texture->m_TexCoords[i].u, m_texture->m_TexCoords[i].v );
}

void glPaper::wave_it( float mAmplitude, float mWavelengths, float mPhase )
{
    int index = 0;
    float period = mWavelengths * 2.*M_PI / (float)m_Xsamples;
    float argument = 0;
    for (int y=0; y<m_Ysamples; y++)
    {
        for (int x=0; x<m_Xsamples; x++)
        {
            argument = ((float)x) * period + mPhase;
            m_vertices[index].position[1] = mAmplitude * sin ( argument );
            //m_vertices[index].position[1] = mAmplitude * x;
            index++;
        }
    }
}
void glPaper::fold_it( int mLeadingFoldSamples, float mSlope1, float mSlope2 )
{
    int index      = 0;
    float distance = 0;
    int   x=0;
    for (int y=0; y<m_Ysamples; y++)
    {
        for (x=0; x<mLeadingFoldSamples; x++)
        {
            distance = mSlope1 * x;
            m_vertices[index].position[1] = distance;
            index++;
        }
        for ( ; x<m_Xsamples; x++)
        {
            m_vertices[index].position[1] = distance - mSlope2 * x;
            index++;
        }
    }
}

void glPaper::generate_vertices( )
{
    float Xincrement = m_width / m_Xsamples;
    float Yincrement = m_height/ m_Ysamples;
    struct Vertex v;
    v.color[0] = 0xFF; v.color[1] = 0x7F;   v.color[2] = 0x7F; v.color[3] = 0xFF;
    float  xpos,ypos;
    printf("samps_per_inch=%d\n",           m_samples_per_inch      );
    printf("xsamples=%d;  ysamples=%d  \n", m_Xsamples, m_Ysamples  );
    
    // Create a grid of vertices :
    for (int y=0; y<m_Ysamples; y++)
    {
        ypos = y*Yincrement;
        for (int x=0; x<m_Xsamples; x++)
        {
            xpos = x*Xincrement;
            v.position[0] = xpos;
            v.position[1] = ypos;
            v.position[2] = 0.0;
            m_vertices.push_back(v);
        }
    }
    printf("m_vertices.size = %lu\n", m_vertices.size() );
}

void glPaper::generate_indices( )
{
	int index,index2;
	m_indices.clear();
	for (int y=0; y<m_Ysamples-1; y++)
	{
		index  = y * m_Xsamples;
		index2 = index + m_Xsamples;        // line above
		for (int x=0; x<(m_Xsamples-1); x++)
		{
			m_indices.push_back( index + x+0 );
			m_indices.push_back( index + x+1 );
			m_indices.push_back( index2+ x+1 );
			m_indices.push_back( index2+ x+0 );
		}
	}
}

void glPaper::setup( )
{
    if (m_texture==NULL)
        m_texture = new Texture();
    
    generate_vertices( );
    generate_indices ( );
    //wave_it( 10, 2, 0.0 );
    
    m_texture->generate_grid_coords( m_Xsamples, m_Ysamples);
    m_texture->generate_TBO( );
    m_texture->generate_VBOTexCoords();
}

void glPaper::create( )
{
    setup();
    gl_register ( );
}

void glPaper::half_circle( float mRadius, int mStartXindex )
{
	// will go in y,z plane:
	int yindex;
	int samps_per_circle = mRadius * M_PI / m_samples_per_inch;
	float angle;
	float bottom_length  = 0.;	
	float xpos,ypos;
	
	// Create a grid of vertices : 
	for (int y=0; y<m_Ysamples; y++)
	{
		yindex = y*m_Xsamples;
		angle = -M_PI/2.0;
		for (int x=mStartXindex; x<mStartXindex+samps_per_circle; x++)
		{
			xpos = mRadius * cos(angle) + bottom_length;
			ypos = mRadius * sin(angle) + mRadius;	// start at y=0 until y=2*mRadius
			
			m_vertices[yindex+x].position[1] = ypos;
			m_vertices[yindex+x].position[2] = xpos;
		}
	}	
}

void glPaper::pull_back_to( float mDistanceFromLeft, float mDistanceFromPage )
{
	// Create a line plus half circle (to simulate pulling the page back)
	float radius = mDistanceFromPage / 2.;
	float half_circumference = radius * M_PI;
	float top_length = 0.0;
	float bottom_length = 0.0;
//	float DistanceFromRight = m_width - mDistanceFromLeft;
	if (mDistanceFromLeft>0.0)
	{
		/* For firm crease fold, We know:  
					top_length + bottom_length = m_width
		   For rolled fold, we know:
		   	1) top_length + bottom_length + half_circumference = m_width
		   	   top_length + bottom_length = m_width - half_circumference
		   	2) mDistanceFromLeft + top_length = bottom_length
		   	
				mDistanceFromLeft + top_length = (m_width-half_circumference) - top_length
				2*top_length = (m_width-half_circumference-mDistanceFromLeft)
		   	So to find top_length = 
		*/		
		top_length = (m_width-half_circumference-mDistanceFromLeft) / 2.0;
		bottom_length = top_length + mDistanceFromLeft;

	} else {
		/* For firm crease fold, We know:  
					top_length + bottom_length = m_width
		   For rolled fold, we know:
		   	1) top_length + bottom_length + half_circumference = m_width
		   	   top_length + bottom_length = m_width - half_circumference
		   	2) mDistanceFromLeft + top_length = bottom_length
		   	
				mDistanceFromLeft + top_length = (m_width-half_circumference) - top_length
				2*top_length = (m_width-half_circumference-mDistanceFromLeft)
		   	So to find top_length = 
		*/		
		top_length = (m_width-half_circumference-mDistanceFromLeft) / 2.0;
		bottom_length = top_length + mDistanceFromLeft;
	}
}

void glPaper::draw_body	( )
{
    glAtom::draw_body();
}

void glPaper::draw_primitive( )
{
    glDrawElements(GL_QUADS, (int)m_indices.size(), GL_UNSIGNED_INT, 0 );
//    glAtom::draw_primitive();
}

void glPaper::load_texture( const char* mFilename )
{
    if (m_texture==NULL)
        m_texture = new Texture();
     m_texture->load_image( mFilename );
     //printf("glPaper::load_image( %s )\n", mFilename );
}

