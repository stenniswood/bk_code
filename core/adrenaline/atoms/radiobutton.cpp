// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <fontinfo.h>
#include <shapes.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "radiobutton.hpp"
#include "display.h"



#define margin_percent 0.07

RadioButton::RadioButton(int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	Initialize();
}

RadioButton::RadioButton(int Width, int Height )
{
	set_position( 0,0,0,0 );
	Initialize();
	width = Width;
	height = Height;
}

RadioButton::RadioButton()
{
	set_position( 0, 0, 0, 0 );
	Initialize();
}

void RadioButton::Initialize()
{
	strcpy (class_name, "RadioButton");				
	HasBorder = false;
	checked   = false;
	box_rectangle[0]=left;
	box_rectangle[1]=left+width;	
	box_rectangle[2]=bottom+height;
	box_rectangle[3]=bottom;
	selected_color  = 0xFF00FF00;
	background_color= 0x3F7f7f7f;	// greyish	
	border_color    = 0x00000000;
	
	Next = NULL;
	Prev = NULL;
}
void RadioButton::wrap_content( )
{
	VGfloat text_width = TextWidth( (char*)text, SerifTypeface, (int)text_size );
	
	if (width==-1.)
		width = text_width*1.2 + 4*RADIO_BUTTON_CIRCLE_RADIUS;
	if (height==-1.)
		height = text_size*1.5;
	printf("\t\tControl::wrap_content() Got Called! w=%6.1f h=%6.1f\n", width, height);	
}	

int	RadioButton::select(  )
{
	checked = true;
	// Unselect all others!
	// Need to traverse both ways.

	// Go in the Next way :
	RadioButton* tmp = Next;
	while (tmp)
	{
		tmp->checked = false;
		tmp = tmp->Next;
	}
	
	// Now go in the Prev way:
	tmp = Prev;
	while (tmp)
	{
		tmp->checked = false;	
		tmp = tmp->Prev;
	}
	Invalidate();
}

float RadioButton::get_longest_width(  )
{	
	float max_width = width;

	//  Need to traverse both ways.

	// Go in the Next way :
	RadioButton* tmp = Next;
	while (tmp)
	{
		if (tmp->width > max_width)
			max_width = tmp->width;

		tmp = tmp->Next;
	}
	
	// Now go in the Prev way:
	tmp = Prev;
	while (tmp)
	{
		if (tmp->width > max_width)
			max_width = tmp->width;
		tmp = tmp->Prev;
	}
	return max_width;
}

// make all radiobuttons in this group the width of the longest.
void RadioButton::expand_group_widths()		
{
	float new_width = get_longest_width();
	set_width_height( new_width, height );
	
	// Go in the Next way :
	RadioButton* tmp = Next;
	while (tmp)
	{
		tmp->set_width_height( new_width, tmp->height );
		tmp = tmp->Next;
	}
	
	// Now go in the Prev way:
	tmp = Prev;
	while (tmp)
	{
		tmp->set_width_height( new_width, tmp->height );
		tmp = tmp->Prev;
	}
}

int	RadioButton::join_group( RadioButton* mNewButton )
{
	// mNewButton should not have any other attached RadioButtons.
	// ie. Next==NULL; Prev==NULL;  anything else will be disregarded.

	// The new control goes Next
	// Prev remains unchanged!

	if (Next)
		Next->Prev = mNewButton;

	// Any way it's going to be Next;
	mNewButton->Next = Next;
	Next 			 = mNewButton;
	mNewButton->Prev = this;
}

long int half_intensity( long int mColor )
{
	static long int Half;
	int tmp = ((mColor & 0xFF000000)>>1)&0xFF000000;
	Half = tmp;
	    tmp = ((mColor & 0x00FF0000)>>1)&0x00FF0000;
	Half |= tmp;
		tmp = ((mColor & 0x0000FF00)>>1)&0x0000FF00;
	Half |= tmp;		
		tmp = ((mColor & 0x000000FF)>>1)&0x000000FF;
	Half |= tmp;		
	return Half;
}

int RadioButton::draw()
{	
	Control::draw();
	float text_size = 14.0;

	// Gentle outline - keep underneath
	Stroke_l( half_intensity( border_color ) );
	Fill_l  ( 0x00000000 );
	Rect( left, bottom, width, height );
	
	// DRAW CIRCLE INDICATOR:
	Stroke_l( border_color );
	if (checked)
		Fill_l  ( selected_color );
	else
		Fill_l  ( background_color );
	float center_y = height/2.0 + bottom;
	Circle( left+RADIO_BUTTON_CIRCLE_RADIUS, 
			center_y, 
			RADIO_BUTTON_CIRCLE_RADIUS );

	// DRAW TEXT:		
	float x = left+	4*RADIO_BUTTON_CIRCLE_RADIUS;
	float line_height = 1.5*text_size;
	float space_y = (height - line_height)/2.0;
	Fill_l  ( text_color );	
	Text(x, bottom+space_y, text, SerifTypeface, text_size );				
}

int	RadioButton::onClick(int x, int y, bool mouse_is_down)
{
	printf("RadioButton::onClick() %s\n", text);
//	checked = !checked;
	select();
//	draw();
	return -1;
}


