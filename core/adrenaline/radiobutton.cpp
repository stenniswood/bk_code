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
	HasBorder = false;
	checked   = false;
	box_rectangle[0]=left;
	box_rectangle[1]=left+width;	
	box_rectangle[2]=bottom+height;
	box_rectangle[3]=bottom;
	selected_color = 0xFF00FF00;
	background_color= 0x3F7f7f7f;	// greyish	
	
	Next = NULL;
	Prev = NULL;
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

	// gentle outline - keep underneath
	Stroke_l( half_intensity( border_color ) );
	Fill_l  ( 0x00000000 );
	Rect( left, bottom, width, height );
	
	Stroke_l( border_color );
	if (checked)
		Fill_l  ( selected_color );
	else
		Fill_l  ( background_color );
	float center_y = height/2.0 + bottom;
	Circle( left+RADIO_BUTTON_CIRCLE_RADIUS, 
			center_y, 
			RADIO_BUTTON_CIRCLE_RADIUS );
			
	float x = left+	4*RADIO_BUTTON_CIRCLE_RADIUS;
	float line_height = 1.5*text_size;
	float space_y = (height - line_height)/2.0;
	Fill_l  ( text_color );	
	Text(x, bottom+space_y, text, SerifTypeface, text_size );				
}

int	RadioButton::onClick()
{
	checked = !checked;
	draw();
	return -1;
}


