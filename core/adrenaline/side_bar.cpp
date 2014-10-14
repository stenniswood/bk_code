// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "listbox.hpp"
#include "display.h"
#include "side_bar.hpp"


#define Debug 0


SideBarControl::SideBarControl()
{

}
SideBarControl::SideBarControl( int Left, int Right, int Top, int Bottom )

{

}
SideBarControl::~SideBarControl()
{

}

void SideBarControl::Initialize()
{
	Control::Initialize();	
	background_color = 0x8F000000;
	
}
void SideBarControl::calc_metrics	 	(   )
{
	
}

int		SideBarControl::add_control( Control* mControl, char* mText=NULL )
{
	// Need to set it's location first:
	// This control proportions them out:
	int num_controls = controls.size();
	if (num_controls > MAX_CONTROLS)
		return -1;	
	
	if (Alignment==SIDEBAR_ALIGN_BOTTOM)
	{

	} 
	else if (Alignment==SIDEBAR_ALIGN_CENTER)
	{
	
	} 
	else if (Alignment==SIDEBAR_ALIGN_TOP)
	{
	
	}

	controls.push_back( mControl );
	paired_text.push_back( mText );
	return 1;
}
void	SideBarControl::hide			 ( bool mHide = true)
{	
	isHidden = mHide;
	Invalidate();
}
void	SideBarControl::auto_hide		 ( bool mAutoHide = true)
{	
	AutoHideEnabled = mAutoHide;
	// need a timer after 5 seconds hide.
}

void SideBarControl::set_alignment( byte mAlignment = SIDEBAR_ALIGN_CENTER )
{
	Alignment = mAlignment;
}

int SideBarControl::draw()
{
	if (isHidden)	return;

	Control::draw();
	list<Control*>::iterator	iter = controls.begin();

	for (int i=0; iter!=controls.end(); i++ )
	{
		(*iter)->draw();
	}	
}


int	SideBarControl::onClick(int x, int y, bool mouse_is_down)
{	
	draw();
	return -1;
}
