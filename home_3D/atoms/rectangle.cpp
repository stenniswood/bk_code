#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
//#include <shapes.h>
#include "rectangle.h"


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


void Rectangle::move_to	( float mleft, float mbottom )
{
    left   = coords[LEFT]	= mleft;
    bottom = coords[BOTTOM]	= mbottom;

    coords[TOP]   = bottom + height;
    coords[RIGHT] = left   + width;
}


void Rectangle::set_size	( float width, float height)
{
    set_width ( width  );
    set_height( height );
    
    //coords[TOP]   = get_bottom() + height;
    //coords[RIGHT] = get_left()   + width;
}

void Rectangle::print_positions()
{
    printf("left=%6.1f; bottom=%6.1f; width=%6.1f; height=%6.1f",
           get_left(), get_bottom(), get_width(), get_height() );
}

void	Rectangle::draw( )
{
   // Rect( left, bottom, get_width(), get_height() );
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

