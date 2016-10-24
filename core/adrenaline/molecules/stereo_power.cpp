// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "display.h"
#include "power_level.hpp"
#include "stereo_power.hpp"
#include "global_funcs.h"


#define margin_percent 0.07

#define Debug 0


StereoPowerLevels::StereoPowerLevels(int Left, int Right, int Top, int Bottom )
: Control(Left,Right,Top,Bottom)
{
	Initialize();
}

StereoPowerLevels::StereoPowerLevels( int Width, int Height )
: Control (Width,Height)
{
	Initialize();
}

StereoPowerLevels::StereoPowerLevels()
{
	set_position( 0, 0, 0, 0 );
	Initialize();
}

void StereoPowerLevels::Initialize()
{
	left_ch.set_level (10);
	right_ch.set_level(10);	

	set_max ( 100.);
	set_min (   0.);
}

void StereoPowerLevels::set_width_height ( int Width,  int Height )
{
	Control::set_width_height( Width, Height );
	
	float half_w = (Width/2.0);
	left_ch.set_width_height ( half_w, height );
	left_ch.move_to			 ( left,   bottom );
	right_ch.set_width_height( half_w, height );	
	left_ch.set_position_right_of(&left_ch, true, 4);	
}

void StereoPowerLevels::move_to( float Left, float Bottom )
{
	Control::move_to( Left, Bottom );

	float half_w = (width/2.0);
	left_ch.move_to ( left, 		 bottom );
	right_ch.move_to( left+half_w+2, bottom );
}

StereoPowerLevels::~StereoPowerLevels()
{

}

int StereoPowerLevels::draw( )
{
	Control::draw();
	Dprintf("StereoPowerLevels::draw()\n");
	left_ch.draw();
	right_ch.draw();
	return 1;
}

void StereoPowerLevels::set_max  ( float mMax )	
{
	left_ch.set_max (mMax);
	right_ch.set_max(mMax);	
}

void StereoPowerLevels::set_min  ( float mMin )
{
	left_ch.set_min (mMin);
	right_ch.set_min(mMin);
}

void StereoPowerLevels::set_level_left ( float mLevel )	
{
	left_ch.set_level(mLevel);  
}
void StereoPowerLevels::set_level_right( float mLevel )	
{
	right_ch.set_level(mLevel); 
}
void StereoPowerLevels::set_number_boxes( int mBoxes )
{
	left_ch.set_number_boxes(mBoxes); 
	right_ch.set_number_boxes(mBoxes); 
}

