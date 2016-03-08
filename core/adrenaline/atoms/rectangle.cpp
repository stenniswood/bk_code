#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <shapes.h>
#include "rectangle.hpp"


Rectangle::Rectangle(float Left, float Bottom, float Width, float Height)
{
	set_left  ( Left 			);
	set_right ( Left + Width    );
	set_top   ( Bottom + Height );
	set_bottom( Bottom 			);
}

Rectangle::Rectangle(float Width, float Height)
{
	set_left   ( 0 );
	set_bottom ( 0 );	
	set_width  ( Width  );
	set_height ( Height );	
}

Rectangle::Rectangle()
{
	set_left  ( 0. );
	set_right ( 0. );
	set_top   ( 0. );
	set_bottom( 0. );
}


void	Rectangle::move_to	( float left, float bottom )
{
	coords[TOP]   = bottom + get_height();
	coords[RIGHT] = left   + get_width();

	coords[LEFT]	= left;
	coords[BOTTOM]	= bottom;
}


void 	Rectangle::set_size	( float width, float height)
{
	coords[TOP]   = get_bottom() + height;
	coords[RIGHT] = get_left()   + width;
}

void Rectangle::print_positions()
{
	printf("left=%6.1f; bottom=%6.1f; width=%6.1f; height=%6.1f",
			get_left(), get_bottom(), get_width(), get_height() );	
}

void Rectangle::shrink( float mFraction )
{
	float w = get_width();
	float h = get_height();
	
	float nw = (1-mFraction) * w;		// shrink by 0.1 (10%) 
	float nh = (1-mFraction) * h;

	left += mFraction/2. * w;			// shift by half the shrinkage.
	bottom	+= mFraction/2. * h;
		
	coords[TOP]   = bottom + nh;
	coords[RIGHT] = left   + nw;

	coords[LEFT]	= left;
	coords[BOTTOM]	= bottom;
}

void	Rectangle::draw( )
{
	Rect( left, bottom, get_width(), get_height() );	
}

int		Rectangle::hit_test	(float x, float y)
{
	if ((x > left) && (x<get_right()))
	{
		if ((y > get_bottom()) && (y<get_top()))
			return 1;
	}
	return 0;
}

float Rectangle::get_width	( )		
{ return coords[RIGHT]-coords[LEFT];   }
float Rectangle::get_height	( )		
{ return coords[TOP] - coords[BOTTOM]; }
