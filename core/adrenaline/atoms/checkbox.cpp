// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "Graphbase.hpp"
#include "control.hpp"
#include "checkbox.hpp"
#include "display.h"

#define margin_percent 0.07

CheckBox::CheckBox(int Left, int Right, int Top, int Bottom )
:Control(Left,Right,Top,Bottom)
{
	Initialize();	
}
CheckBox::CheckBox( int Width, int Height )
:Control(Width, Height)
{
	Initialize();
}
CheckBox::CheckBox()
:Control()
{
	set_position( 0, 0, 0, 0 );
	Initialize();
}

void CheckBox::Initialize()
{
	Control::Initialize();
	strcpy (class_name, "Checkbox");	
	checked 			= false;
	HasBorder 			= true;			// light transparent border
	text_color 			= 0xFFFFFFFF;
	border_color		= 0x9F7f4f1f;	// yellowish
	background_color	= 0x3F7f7f7f;	// greyish	
	
	box_border_color     = 0xEFAfDf7f;
	box_background_color = 0x7F1f1f1f;
	check_box_side_length=10;

	check_box_side_length = text_size;
	
	on_click_callback = NULL;
	on_click_context  = NULL;	
}

void CheckBox::wrap_content( )
{
	printf("\t\tCheckBox::wrap_content() Got Called! w=%6.1f h=%6.1f\n", width, height);
	float text_width  = TextWidth( text, SerifTypeface, text_size ) * 1.2;
	float total_width = text_width + 2.0*check_box_side_length + 2.0*BOX_PADDING;

	width = total_width;
	height = text_size*1.5;	
}

void CheckBox::set_text( const char* NewText, bool mWrapContent )	// Allocates and copies!		
{
	Control::set_text( NewText, true);
}

void	CheckBox::set_on_click_listener( void (*callback)(void*), void* mOn_click_context )
{
	on_click_context  = mOn_click_context;
	on_click_callback = callback;
}

int CheckBox::draw_check_mark()
{
	StrokeWidth(2.0);
	Stroke_l( box_border_color   );
	Fill_l( box_background_color );	
	Line(box_rectangle[0], box_rectangle[1], box_rectangle[2], box_rectangle[3] );
	Line(box_rectangle[0], box_rectangle[3], box_rectangle[2], box_rectangle[1] );
	return 1;
}

int CheckBox::draw()
{	
	Control::draw();
	check_box_side_length = text_size;
	
	box_rectangle[0] = left+BOX_PADDING;
	box_rectangle[1] = bottom+(height-check_box_side_length)/2.0;
	box_rectangle[2] = box_rectangle[0]+check_box_side_length;
	box_rectangle[3] = box_rectangle[1]+check_box_side_length;

	// Draw the box:	
	StrokeWidth(2.0);
	Stroke_l( box_border_color );
	Fill_l  ( box_background_color );	
	float space_y    = (height-check_box_side_length) / 2.0;
	Rect    ( box_rectangle[0], box_rectangle[1], check_box_side_length, check_box_side_length);
	
	// Draw the check:
	if (checked)
		draw_check_mark();
	
	// Draw the text:
	Fill_l( text_color );		
	Text(box_rectangle[2]+BOX_PADDING, bottom+space_y, text, SerifTypeface, text_size );
	return 1;
}

/* Use -1, -1 for the width,height constructor.
	Will be adjusted on "set_text()" call.

int	CheckBox::set_height_to_text()
{
	height = 1.5*text_size;
}*/

Control* CheckBox::HitTest ( int x, int y 	)
{
	Control* retval=NULL;

	retval = Control::HitTest(x,y);
	printf("CheckBox::HitTest() %p\n", retval);
	print_positions();
	return retval;
}


int	CheckBox::set_check(bool CheckOn)
{
	checked = CheckOn;
	return 1;
}

int	CheckBox::onClick(int x, int y, bool mouse_is_down)
{
	printf("CheckBox::onClick()\n");
	checked = !checked;		// some listeners want the check to change before
							// so that they know the new state.
							// what if others want the old state?

	if (on_click_callback)
		on_click_callback( on_click_context );

	draw();
	return -1;
}


