// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <string>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "listbox.hpp"
#include "display.h"
#include "tab_control.hpp"


#define Debug 0


TabControl::TabControl()
{

}
TabControl::TabControl( int Left, int Right, int Top, int Bottom )
{

}

TabControl::~TabControl()
{

}

void TabControl::Initialize()
{
	Control::Initialize();
}

void TabControl::calc_metrics	 (   )
{

}
	
/* Scrolling on tab control is different than a ScrollControl */
void TabControl::enable_scroll_buttons( bool mEnable )
{
	if (mEnable)
	{
		// add buttons  <  and >  
 	} else {
 	}
}

int TabControl::draw()
{
	Control::draw();
}
