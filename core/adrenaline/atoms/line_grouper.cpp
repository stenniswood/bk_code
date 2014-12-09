/*******************************************************
* Draws the Adrenaline Network Overview
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <vector>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <string.h>
#include <fontinfo.h>
#include <shapes.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "icon.hpp"
#include "line_grouper.hpp"


VerticalLineGroup::VerticalLineGroup(  )
{
	
}
VerticalLineGroup::~VerticalLineGroup(  )
{
	vector<point*>::iterator siter = m_source_points.begin();
	while ( siter != m_source_points.end() )
	{
		delete (*siter);
		siter++;
	}

	vector<point*>::iterator diter = m_destination_points.begin();
	while ( diter != m_source_points.end() )
	{
		delete (*diter);
		diter++;
	}	
}

int		VerticalLineGroup::add_source_top		( Control* mItem 	)
{
	float l = mItem->get_left();
	float w = mItem->get_width();
	l += (w/2.);
	float sy = mItem->get_bottom() + mItem->get_height();
	add_source_point( l, sy );
}

int		VerticalLineGroup::add_source_point		( float mx, float my 	)
{
	// Setup the Point structure:
	struct point* pt = new struct point( );
	pt->sx = mx;
	pt->sy = my;
	pt->point_type = 'S';
	m_source_points.push_back( pt );
}

int		VerticalLineGroup::add_destination	( Control* mItem 		)
{
	float l = mItem->get_left();
	float w = mItem->get_width();
	l += (w/2.);
	float sy = mItem->get_bottom() + mItem->get_height();
	add_destination_point( l, sy );
}

int		VerticalLineGroup::add_destination_point( float mx, float my 	)
{
	// Setup the Point structure:
	struct point* pt = new struct point( );
	pt->sx = mx;
	pt->sy = my;
	pt->point_type = 'E';
	m_destination_points.push_back( pt );
}

int		VerticalLineGroup::place_destination_center( )
{
	
}

// First we start by assuming vertical orientation:
int	VerticalLineGroup::draw( )
{
	Stroke_l( m_line_color );
	Fill_l  ( m_line_color );

	vector<point*>::iterator siter = m_source_points.begin();
	vector<point*>::iterator diter = m_destination_points.begin();

	// Define the half way Y coordinate:
	float y_half = ((*siter)->sy - (*diter)->sy) / 2. + (*siter)->sy;
	float min, max;

	// Draw all the Source stem lines:
	for (int i=0; i<m_source_points.size(); i++)
	{
		if ((*siter)->sx < min)
			min = (*siter)->sx;
		if ((*siter)->sx < max)
			max = (*siter)->sx;
		Line( (*siter)->sx,  (*siter)->sy,  (*siter)->sx, y_half );
	}
	
	// Now Horizontal Line:
	Line( min, y_half, max, y_half );

	// Now Draw Destination Stem :
	Line( (*diter)->sx,  y_half,  (*diter)->sx, (*diter)->sx );
}


