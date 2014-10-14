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
#include "listbox.hpp"
#include "display.h"

#define margin_percent 0.07
#define Debug 0

ListBox::ListBox(int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	text		= NULL;
	title 		= NULL;
	LineHeight = 20.0;
	TextSize   = 12.0;
	numItems = 0;
}

ListBox::ListBox()
{
	set_position( 0, 0, 0, 0 );
	Visible = FALSE;
	text	= NULL;	
	title 	= NULL;
	LineHeight = 20.0;
	TextSize   = 12.0;
	numItems = 0;

}

void Fill_mainline()
{
	Fill(255, 255, 255, 1.0);				   // Big blue marble	
}
void Fill_alternateline()
{
	Fill(88, 154, 232, 1.0);				   // Big blue marble	
}

void ListBox::setAlpha( float  mAlpha )
{
	unsigned long int a = mAlpha*255;
	EvenColor &= 0x00FFFFFF;
	OddColor  &= 0x00FFFFFF;
	EvenColor |= a;
	OddColor |= a;
}
	
int ListBox::draw_text_bottom_up()
{
	float text_size = 14.0;
	float number_lines = (height / LineHeight);
	int i=0;

	Fill_l(0xFF000000);
	float y=bottom;
	for (int i=0; i<numItems; i++, y+=LineHeight)
		Text( left, y+(LineHeight-TextSize)/2.0, Linetexts[i], SerifTypeface, TextSize );
}

int ListBox::draw_text_top_down()
{
	float text_size = 14.0;
	float number_lines = (height/LineHeight);
	int i=0;

	Fill_l(0xFF000000);
	float y=bottom+height;
	for (int i=0; i<numItems; i++, y-=LineHeight)
		Text( left, y-(LineHeight-TextSize)/2.0, Linetexts[i], SerifTypeface, TextSize );
}

int ListBox::draw()
{	
	Control::draw();
	
	if (Debug) printf("ListBox draw(): bottom=%6.1f; height=%6.1f\n", bottom, height);
	StrokeWidth(3.0);
	
	float number_lines = (height / LineHeight);
	int i=0;
	float y=bottom;
	for (y=bottom; y<(bottom+height); y+=LineHeight, i++)
	{
		if ((i%2)==0)	
			Fill_mainline();	
		else 
			Fill_alternateline();	
		Rect( left, y, width, LineHeight );	
	}
	draw_text_top_down();
}

void ListBox::ClearItems( )  
{
	for (int i=0; i<numItems; i++) {
		free(Linetexts[i]);
		Linetexts[i] = NULL;
	}
	numItems=0;
}
void ListBox::setItem( char* mString)  
{ 
		char* tmp = new char[strlen(mString)+1];
		strcpy(tmp, mString ); 
		Linetexts[numItems++] = tmp;
};
	

