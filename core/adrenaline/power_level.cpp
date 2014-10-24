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


#define margin_percent 0.07

PowerLevel::PowerLevel(int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	Initialize();
}

PowerLevel::PowerLevel( int Width, int Height )
{
	left=0; bottom=0; 
	width  = Width;
	height = Height;
	Initialize();
}

PowerLevel::PowerLevel()
{
	set_position( 0, 0, 0, 0 );
	Initialize();
}

PowerLevel::~PowerLevel()
{
}

