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
#include "button.hpp"
#include "display.h"

#define margin_percent 0.07

Button::Button(int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{
	Visible   = FALSE;
	text_size = 14.0;
	sunken    = false;
	
	TextColor		= 0xFF9f9f0f;
	BorderColor 	= 0xFFffffff;
	BackgroundColor = 0xFf202020;
}

Button::Button()
{
	set_position( 0, 0, 0, 0 );
	Visible   = FALSE;
	text_size = 14.0;	
	sunken    = false;

	TextColor		= 0xFF9f9f0f;
	BorderColor 	= 0xFFffffff;
	BackgroundColor = 0xFf202020;	

}


	/********************************************************************
	 Comment on RoundRect & TextMid!!!!
		RoundRect Uses width / height as 2nd and 3rd parameters.
		Text characters come out the color of the Fill, not stroke!!
	*********************************************************************/

int Button::draw()
{	
	Control::draw();

	Stroke_l( BorderColor );		// box color
	StrokeWidth(2);

	//printf("text_size=%6.1f; %s\n", text_size, text);
	if (sunken)
	{
		Fill_l(TextColor);		// grey
		Roundrect( left, bottom, +width, +height, 15.0, 15.0 );
		if (text==NULL) return 1;
		Fill_l(BackgroundColor);		// grey
		TextMid  (left+width/2.0, bottom+height/2.0, text, SerifTypeface, text_size );
	}
	else 
	{
		Fill_l(BackgroundColor);		// grey
		Roundrect( left, bottom, +width, +height, 15.0, 15.0 );
		if (text==NULL) return 1;
		Fill_l(TextColor);		// grey				
		TextMid  (left+width/2.0, bottom+height/2.0, text, SerifTypeface, text_size );
	} 
}

int	Button::onClick()
{
	sunken = true;	
	draw();
	sunken = false;	
	draw();
	return -1;
}


