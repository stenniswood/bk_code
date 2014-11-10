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


#define margin_percent 0.07


StereoPowerLevels::StereoPowerLevels(int Left, int Right, int Top, int Bottom )
: Control(Left,Right,Top,Bottom)
{
	Initialize();
}

StereoPowerLevels::StereoPowerLevels( int Width, int Height )
: Control (0,0,0,0)
{
	Initialize();
	width  = Width;
	height = Height;
}

StereoPowerLevels::StereoPowerLevels()
{
	set_position( 0, 0, 0, 0 );
	Initialize();
}

void StereoPowerLevels::Initialize()
{
	left.set_level (10);
	right.set_level(10);	
	
	left.set_max( 100. );
	left.set_min(   0. );
	
	right.set_max( 100.);
	right.set_min(   0.);
} 

StereoPowerLevels::~StereoPowerLevels()
{
}



int StereoPowerLevels::draw( )
{
	Control::draw();
	left.draw();
	right.draw();		
}

void StereoPowerLevels::set_max  ( float mMax )	
{
	left.set_max (mMax);
	right.set_max(mMax);	
}

void StereoPowerLevels::set_min  ( float mMin )
{
	left.set_min (mMin);
	right.set_min(mMin);	
}

