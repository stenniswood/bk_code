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
	Initialize();
}

Button::Button()
:Control()
{
	Initialize();
	set_position( 0, 0, 0, 0 );
}

Button::Button( int Width, int Height )
:Control( Width, Height )
{
	width = Width;		// -1 means wrap content
	height= Height;		// -1 means wrap content
	Initialize();
}

Button::~Button()
{

}

void Button::Initialize()
{
	text_size = 14.0;
	sunken    = false;

	text_color		 = 0xFFCfCf3f;
	border_color 	 = 0xFFffffff;
	background_color = 0xFf202020;
}

int   	Button::draw_sunken (	)
{
	//Fill_l(text_color);
	Stroke_l ( background_color );
	Fill_l   ( background_color );
	Roundrect( left, bottom, width, height, 15.0, 15.0);

	if (text==NULL) return 1;
	Fill_l (text_color);
	float y = bottom + (height - text_size)/2.0;
	TextMid(left+width/2.0, y, text, SerifTypeface, text_size );
}

int   	Button::draw_raised (	)
{
	// Draw Border:
	Stroke_l ( 0xFF000000 );
	Roundrect( left, bottom, width, height, 15.0, 15.0);
			// use 2 rectangles:
	Stroke_l ( border_color );
	Fill_l   ( background_color );
	Roundrect( left+1, bottom+1, width-2, height-2, 15.0, 15.0);

	// DRAW TEXT:
	float y = bottom + (height - text_size)/2.0;	
	if (text==NULL) return 1;
	Fill_l (text_color);
	TextMid(left+width/2.0, y, text, SerifTypeface, text_size );
}
 
/********************************************************************
 Comment on RoundRect & TextMid!!!!
	RoundRect Uses width / height as 2nd and 3rd parameters.
	Text characters come out the color of the Fill, not stroke!!
*********************************************************************/
int Button::draw()
{	
	//Control::draw(); 
	Stroke_l   ( border_color );		// box color
	StrokeWidth(2);

	printf("text_size=%6.1f; %s\n", text_size, text);
	if (sunken)
	{
		draw_sunken();
	}
	else 
	{
		draw_raised();
	}  
}

int	Button::onClick()
{
	sunken = true;
	draw();
//	sunken = false;
//	draw();
	return -1;
}


