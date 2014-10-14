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
#include "display.h"


#define margin_percent 0.07

Control::Control(int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	text		= NULL;
	title 		= NULL;
	HasBorder = true;	
	Visible   = true;
	text_size = 14.0;
	border_roundness_x = 15.0;
	border_roundness_y = 15.0;	
	BackgroundColor = 0x7F202020;
}
 
Control::Control()
{
	set_position( 0, 0, 0, 0 );
	Visible = FALSE;
	text	= NULL;	
	title 	= NULL;
	HasBorder = true;	
	Visible   = true;
	text_size = 14.0;
	border_roundness_x = 15.0;
	border_roundness_y = 15.0;
}

void Control::set_text( char* NewText, float TextSize )
{
// Sometimes we can't do a free()  ie for Static string constants!
// The 2nd call to this function causes a glibc error!
//	if (text) free(text);
//	text = (char*) malloc (strlen(NewText));

	text = new char[strlen(NewText)+1];
	strcpy (text,NewText);	
	if (TextSize!=-1)
		text_size = TextSize;		
}

void Control::set_text_size( float mTextSize )
{
	text_size = mTextSize;
}

void Control::set_text_color ( long int TextColor )
{
	text_color = TextColor;
}

void Control::set_title ( char* NewTitle )
{
//	if (title) free(title);
	title = new char[strlen(NewTitle)+1];
	strcpy (title,NewTitle);	
}


void Control::setPosition_left_of(Control* Sibling, int mwidth, int Top, int Bottom)
{
	left 	= Sibling->left - DefaultPadding - width;
//	left 	= right - width;
	width = mwidth;
	
	if (Top==-1)	height 	= Sibling->height;
	if (Bottom==-1)	bottom	= Sibling->bottom;
}

void Control::setPosition_right_of(Control* Sibling, int mwidth, int Top, int Bottom)
{
	left 	= Sibling->left + Sibling->width + DefaultPadding;
	width = mwidth;
	
	//right 	= left+width;
	if (Top==-1)	height 	= Sibling->height;
	if (Bottom==-1)	bottom	= Sibling->bottom;
}

void Control::set_position( int Left, int Right, int Top, int Bottom )
{
	left 	= Left;
	bottom 	= Bottom;
	width 	= Right-Left;
	height	= Top-Bottom;
}
void  Control::move_to( int Left, int Bottom )
{
	left = Left;
	bottom = Bottom;	
}

void Control::draw_box(  ) 
{
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);
	float alpha = 1.0;

	// HORIZONTAL:
	Fill(255, 0, 125, alpha);	
	float rw = 10;
	float rh = 10;
	Roundrect(left, bottom, width, height, rw, rh);
}

int Control::draw_text() 
{
	if (title==NULL) return -1;
	VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	VGfloat alpha = 1.0;
	int tenpercent = (float)(height)*margin_percent;

	Fill_l( text_color );
	TextMid  ( left+width/2.0, bottom+height+tenpercent, title, SerifTypeface, 
				height/strlen(title) );
	return TRUE;
}

int Control::draw() 
{
	if (Visible==FALSE)	 return 0;

	//Fill_l(BackgroundColor);	
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);
	if (HasBorder) 
		draw_border();

	draw_text	(  );
	return TRUE;
}

int Control::draw_border()
{
	Stroke(255, 255, 255, 1.0);
	Fill_l(BackgroundColor);
	Roundrect( left, bottom, width, height, 15.0, 15.0);
}

int	Control::HitTest(int x, int y)
{
	float right = left+width;
	float top = bottom+height;
	
	if ((x>left) && (x<right)  &&
	    (y>bottom) && (y<top))
	   return 1;
	// could add in a border check.  ie. if with 2 pixels of left or right, etc
	else	
		return 0;
}

int		Control::onClick()
{
	return -1; 
}

int		Control::onDoubleClick()
{
	return -1;	
}

int		Control::onReceiveFocus()
{
	return -1;
}


