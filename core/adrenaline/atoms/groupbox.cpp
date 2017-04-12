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
#include "global.h"

#include "control.hpp"
#include "button.hpp"
#include "groupbox.hpp"


#define margin_percent 0.07

GroupBox::GroupBox( int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	text		= NULL;
}

GroupBox::GroupBox()
{
	set_position( 0, 0, 0, 0 );
	Visible = FALSE;
	text	= NULL;	

}

int GroupBox::draw()
{	
	Control::draw();
	float text_size = 14.0;
	
	Roundrect( left, bottom, left+width, bottom+height, 3.0, 3.0 );
	TextMid  ( left+width/2.0, bottom+height/2.0, text, SerifTypeface, 
				text_size );
	return 1;
}


