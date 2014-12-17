// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
#include "horizontal_menu.hpp"
#include "vertical_menu.hpp"


#define DEFAULT_PADDING 20


HorizontalMenu::HorizontalMenu ()  
:Control()
{
	Initialize(); 
}

HorizontalMenu::HorizontalMenu ( int Left, int Right, int Top, int Bottom )  
:Control(Left, Right, Top, Bottom)
{ 
	Initialize();
}

HorizontalMenu::HorizontalMenu ( int Width, int Height )
:Control( Width, Height )
{ 
	Initialize();
}

HorizontalMenu::~HorizontalMenu()  
{ 
}

void 	HorizontalMenu::Initialize (	)  
{ 
	Control::Initialize();
	background_color = 0xFFFFFFFF;
	m_selected_color = 0xFF007FBF;
	text_color 		 = 0xFF000000;
	text_size 		 = 14.;
	m_menu_padding	 = 50.;
	m_selection		 = -1 ;
	printf("Colors: background=%4x; selected=%4x; text=%4x; mp=%6.1f\n", background_color, m_selected_color, text_color, m_menu_padding );
}

int 	HorizontalMenu::calc_metrics( )
{
	// Calc the widths for each entry:
	m_menu_padding	 = 20.;
	float sx = left; 
	for (int i=0; i<m_entries.size(); i++)
	{
		m_entries[i]->width	= TextWidth( m_entries[i]->text, SerifTypeface, text_size );
		//printf("%6.1f pad=%6.1f\n", m_entries[i]->width, m_menu_padding );
		m_entries[i]->sx    = sx;
		m_entries[i]->ex    = sx+m_entries[i]->width + m_menu_padding;
		sx += (m_entries[i]->width + m_menu_padding);
		
		VerticalMenu* vm = m_entries[i]->menu;
		if (vm)
			vm->attach_at( m_entries[i]->sx, bottom );
	}
	return 0;
}

int		HorizontalMenu::add_entry_text( char* mMenuText )
{
	stHorizMenuInfo* ptr = new struct stHorizMenuInfo;
	int len = strlen(mMenuText);
	ptr->text = new char[len];
	ptr->menu = NULL;
	strcpy(ptr->text, mMenuText);
	
	m_entries.push_back( ptr );
	calc_metrics();
}

int		HorizontalMenu::add_sub_menu( char* mMenuText, VerticalMenu* vm )
{
	stHorizMenuInfo* ptr = new struct stHorizMenuInfo;
	int len   = strlen(mMenuText);
	ptr->text = new char[len];
	ptr->menu = vm;
	strcpy( ptr->text, mMenuText );

	m_entries.push_back( ptr );
	calc_metrics();
}

int 	HorizontalMenu::add_entry( stHorizMenuInfo* mEntry )
{
	m_entries.push_back( mEntry );
	calc_metrics();
}

int 	HorizontalMenu::select 	( int mSelected 	) 
{
	m_selection    = mSelected;
}

int HorizontalMenu::draw(	)
{ 
	//printf("H Menu Draw()\n");
	int item_count = m_entries.size();
	float l = left;
	float c = (height-text_size) / 2.0 + bottom;

	// Draw Horizontal Bar:
	Stroke_l( background_color );
	Fill_l  ( background_color );
	Rect    ( left, bottom, width, height );
	print_positions();	
	//printf("HORIZONTAL_MENU::  left=%6.1f; bottom=%6.1f; width=%6.1f; height=%6.1f\n", left, bottom, width, height);
	
	// Color the Selected Menu:
	if (m_selection>=0)
	{
		Stroke_l( m_selected_color );
		Fill_l  ( m_selected_color );
		Rect    ( m_entries[m_selection]->sx, bottom, m_entries[m_selection]->width, height );
		//printf("H menu drew rectangles!\n");
	}

	// Print text for each entry:
	for (int i=0; i<item_count; i++)
	{
		if (m_selection==i)
		{
			Stroke_l( background_color );
			Fill_l  ( background_color );
			//printf  ("MenuColor: txt=%4x\n", background_color );
			if (m_entries[i]->menu)
				m_entries[i]->menu->draw();
		} else {
			Stroke_l( text_color );
			Fill_l  ( text_color );
			//printf("MenuColor: txt=%4x\n", text_color );
		}
		l = m_entries[i]->sx;
		//printf("%6.1f, %6.1f: %6.1f  %s\n", l, bottom, text_size, m_entries[i]->text );
		Text( l, c, m_entries[i]->text,  SerifTypeface, text_size );
	}
	if ( is_selection_valid() )  {
		//printf("H Menu selection is valid!\n");
		if (m_entries[m_selection]->menu) {
			//printf("H Menu selection is a menu\n");		
			m_entries[m_selection]->menu->draw();
		}
	}
}

int	HorizontalMenu::get_id(   )
{
	if (m_selection==-1)				return -1;
	if (m_selection > m_entries.size()) return -1;
	int result = (m_selection*1000);
	//m_entries[m_selection]->menu->get_id();
}

int 	HorizontalMenu::get_hit_index( int Mousex, int Mousey ) 
{
	if (Mousey < bottom) 			return -1;
	if (Mousey > (bottom+height))	return -1;	
	if (Mousex < left)	 			return -1;	
	if (Mousex > (left+width))	 	return -1;		
	
	int item_count = m_entries.size();
	float l=0;
	float r=0;

	for (int i=0; i<item_count; i++)
	{
		l = m_entries[i]->sx;		
		r = m_entries[i]->ex;		
		if ((Mousex > l) && (Mousex < r) )
			return i;
	}	
}

bool	HorizontalMenu::is_selection_valid( )
{
	if ((m_selection>=0) && (m_selection < m_entries.size()))
		return true;
	return false;
}

Control* HorizontalMenu::HitTest( int x, int y )
{
	// Test this Horizontal Menu first.	
	Control* result = Control::HitTest(x,y);	
	
	if ((result==NULL) && is_selection_valid()) 
	{
		// Test the open submenu:
		result = m_entries[m_selection]->menu->HitTest(x,y);
	}
	if (result)
		printf( "HorizontalMenu::HitTest() %d,%d =  %x\n", x,y,result );
	return result;
}

int		HorizontalMenu::onClick 	( int x, int y, bool mouse_is_down )  
{
	int menu_index = get_hit_index( x,y );
	printf( "Mouse Click  Horiz Menu Item # %d\n", menu_index );
	if (menu_index > 0)
	{
		if (m_entries[menu_index]->menu)
		{
			printf("Is a SubMenu: \n");
			// hide any other visible menu
			if ( is_selection_valid() )  {
				if (m_entries[m_selection]->menu)
					m_entries[m_selection]->menu->show(false);
			}
			m_selection = menu_index;
			m_entries[menu_index]->menu->show(true);
			//m_entries[menu_index]->menu->draw();
			Invalidate();
		}
	}	
}

