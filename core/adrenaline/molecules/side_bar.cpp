// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string> 
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

SideBar::SideBar()
:Control()
{
	Initialize();
}
SideBar::SideBar( int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{
	Initialize();
}
SideBar::~SideBar()
{

}

void SideBar::Initialize()
{
	Control::Initialize();
	background_color = 0x8F000000;	
	calc_metrics();

	m_state=0;
	Alignment 		=SIDEBAR_ALIGN_CENTER;
	isHidden		=false;
	AutoHideEnabled	=false;
}

void SideBar::calc_metrics	 	(   )
{
	m_bottom_margin = ( height* 0.01 );
}

void SideBar::clear( bool mDelete )
{
	if (mDelete)
		for(int i=0; i<m_child_controls.size(); i++)
			delete m_child_controls[i];
			
	m_child_controls.clear();	
}

void SideBar::onPlace()
{
	for(int i=0; i<m_child_controls.size(); i++)
	{
		float c_left;
		if (Alignment == SIDEBAR_ALIGN_LEFT)
			c_left = left;
		else if (Alignment == SIDEBAR_ALIGN_CENTER)
			c_left = left + width/2. - (m_child_controls[i]->get_width()/2.);
		else if (Alignment == SIDEBAR_ALIGN_RIGHT)
			c_left = left + width - (m_child_controls[i]->get_width());

		m_child_controls[i]->move_to( c_left, get_bottom(i) );
	}
}
void SideBar::load_resources	 ( 	 )
{
	for(int i=0; i<m_child_controls.size(); i++)
		m_child_controls[i]->load_resources();
	calc_metrics();
	onPlace();
}
float SideBar::get_short_width	 ( )	// for minimized view (ie. just icons)
{
	return ICON_WIDTH;
}

float SideBar::get_expanded_width( )	// for minimized view (ie. just icons)
{
	return EXPANDED_WIDTH;
}

float SideBar::get_bottom( int mIndex )
{
	float spacing = (height / 6.);
	return (spacing *(float)mIndex) + bottom + m_bottom_margin;
}

void	SideBar::unload_controls ( )	// does not delete
{
	m_child_controls.clear();
}
void	SideBar::load_controls	 ( vector<Control*>* mSidebar_controls )
{
	vector<Control*>::iterator iter = mSidebar_controls->begin();
	while (iter != mSidebar_controls->end() )
		register_child( (*iter) );
}

int		SideBar::add_control( Control* mControl, char* mText )
{
	// Need to set it's location first  :
	// This control proportions them out:
	int num_tabs = m_child_controls.size();
	if (num_tabs > MAX_CONTROLS)
		return -1;	

	register_child( mControl );
	onPlace( );
	return 1;
}

void	SideBar::hide			 ( bool mHide )
{	
	isHidden = mHide;
	Invalidate();
}
void	SideBar::auto_hide		 ( bool mAutoHide )
{	
	AutoHideEnabled = mAutoHide;
	// need a timer after 5 seconds hide.
}

void SideBar::set_alignment( byte mAlignment  )
{
	Alignment = mAlignment;
}

int SideBar::draw()
{
	//printf("\n\nSideBar::draw()\n");
	if (isHidden)	return 0; 

	Control::draw();
	return 1;
}

int	SideBar::onClick(int x, int y, bool mouse_is_down)
{	
	Control::onClick(x,y,mouse_is_down);
	return -1;
}
