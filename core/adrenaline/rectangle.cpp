#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "rectangle.hpp"


	
	
Rect::Rect(float left, float bottom, float Width, float Height)
{
	coords[LEFT]  	= left;
	coords[RIGHT] 	= left + Width;
	coords[TOP]		= bottom + Height;
	coords[BOTTOM]  = bottom;
}

Rect::Rect(int left, int right, int Width, int Height)
{
	coords[LEFT]  	= left;
	coords[RIGHT] 	= left + Width;
	coords[TOP]		= bottom + Height;
	coords[BOTTOM]  = bottom;
}

Rect::Rect(int Width, int Height)
{
	coords[LEFT]  	= 0;
	coords[RIGHT] 	= Width;
	coords[TOP]		= Height;
	coords[BOTTOM]  = 0;
}


void	Rect::move_to	( float left, float bottom )
{
	coords[TOP]   = bottom + get_height();
	coords[RIGHT] = left   + get_width();

	coords[LEFT]	= left;
	coords[BOTTOM]	= bottom;
}


void 	Rect::set_size	( float width, float height);
{
	coords[TOP]   = get_bottom() + height;
	coords[RIGHT] = get_left()   + width;
}


