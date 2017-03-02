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
#include "adrenaline_windows.h"
#include "horizontal_menu.hpp"
#include "vertical_menu.hpp"


#define DEFAULT_PADDING 20
#define Debug 0

/******************************************************************
		MenuEntry	
 ******************************************************************/
MenuEntry::MenuEntry()
{
	memset(text,0,50);
	alt_key[0]=0;				// to activate from keyboard once menu is already active.
	short_cut_key[0]=0;		// to directly activate from keyboard 
	width = 30;
	sx=0.0;				// computed in calc_metrics()
	ex=10.0;				// computed in calc_metrics()	
	state=MENU_STATE_NORMAL;
	(callback)= NULL;		// called when the item is selected.
	text_size = 14.;
}
MenuEntry::~MenuEntry()
{
}	
enum eEntryType	MenuEntry::getEntryType()
{
	return REGULAR_ENTRY;
}


void	MenuEntry::set_text  ( const char* mText )
{
	strcpy(text, mText);
}
void	MenuEntry::calc_width( const char* mText )
{
	LineHeight = text_size*1.25;
	width = TextWidth( text, SerifTypeface, text_size );
}
int		MenuEntry::draw()
{
	float above_line_offset = (LineHeight-text_size)/2.0;
	Text( sx, Y+above_line_offset, (char*)text, SerifTypeface, text_size );
	return 1;
}

/******************************************************************
		CascadeMenuEntry
 ******************************************************************/
CascadeMenuEntry::CascadeMenuEntry()
:MenuEntry()
{
	submenu = NULL;
}
CascadeMenuEntry::~CascadeMenuEntry()
{
}
enum eEntryType	CascadeMenuEntry::getEntryType()
{
	return SUB_MENU_ENTRY;
}

void CascadeMenuEntry::calc_width( const char* mText )
{
	const int ARROW_WIDTH = 40;
	width = TextWidth( text, SerifTypeface, text_size );
	width += ARROW_WIDTH;
}
int	 CascadeMenuEntry::draw()
{
	MenuEntry::draw();
	return 1;
}

/******************************************************************
		Menu  base class
 ******************************************************************/
Menu::Menu()
{
	Initialize(); 
}
Menu::Menu ( int Left, int Right, int Top, int Bottom )
{
	Initialize(); 
}
Menu::Menu ( int Width, int Height 					 )
{
	Initialize(); 
}
Menu::~Menu()
{

}	
void Menu::Initialize	(	)
{
	Control::Initialize();
	strcpy (class_name, "HorizontalMenu");		
	background_color = 0xFFFFFFFF;
	m_selected_color = 0xFF007FBF;
	text_color 		 = 0xFF000000;
	text_size 		 = 14.;

	m_selection		 = -1 ;
	m_application	 = NULL;
	//printf("Colors: background=%4x; selected=%4x; text=%4x; mp=%6.1f\n", background_color, m_selected_color, text_color, m_menu_padding );
}

Menu*	Menu::set_parent	( Menu* mMenu  )
{
	return m_parent = mMenu;
	
}
int	Menu::add_sub_menu( const char* mMenuText, Menu* vm )
{
	vm->set_parent( this );		// back link here.
	MenuEntry me;
	me.set_text( mMenuText );
	m_entries.push_back( me );
	return 1;
}
int	Menu::add_entry	( MenuEntry mEntry 	  )
{
	m_entries.push_back( mEntry );
	calc_metrics();
	return 1;
}

int Menu::select 	( int mSelected 	) 
{
	m_selection    = mSelected;
	return 1;
}
bool	Menu::is_selection_valid( )
{
	if ((m_selection>=0) && (m_selection < m_entries.size()))
		return true;
	return false;
}

/*int	Menu::add_callback( int  mIndex, int (*callback)(void*, int)=NULL )
{}*/

void Menu::clear_all( 		  )
{
	m_entries.clear();
}

int	Menu::attach_at( float x, float y )
{
	move_to(x, y-height);
	return 1;	
}
int 	Menu::calc_metrics( )
{
	return 0;
}



/******************************************************************/

/******************************************************************/
HorizontalMenu::HorizontalMenu ()  
:Menu()
{
	Initialize(); 
}

HorizontalMenu::HorizontalMenu ( int Left, int Right, int Top, int Bottom )  
:Menu(Left, Right, Top, Bottom)
{ 
	Initialize();
}

HorizontalMenu::HorizontalMenu ( int Width, int Height )
:Menu( Width, Height )
{ 
	Initialize();
}

HorizontalMenu::~HorizontalMenu()  
{ 

}

void 	HorizontalMenu::Initialize (	)  
{ 
	Menu::Initialize();
	strcpy (class_name, "HorizontalMenu");		
	m_menu_padding	 = 50.;
	calc_metrics();
}
int 	HorizontalMenu::calc_metrics( )
{
	// Calc the widths for each entry:
	m_menu_padding	 = 20.;
	float sx = left; 
	for (size_t i=0; i<m_entries.size(); i++)
	{
		m_entries[i].width	= TextWidth( m_entries[i].text, SerifTypeface, text_size );
		//printf("%s %6.1f pad=%6.1f\n", m_entries[i]->text, m_entries[i]->width, m_menu_padding );
		m_entries[i].sx    = sx;
		m_entries[i].ex    = sx+m_entries[i].width + m_menu_padding;
		sx += (m_entries[i].width + m_menu_padding);
		
		// if its a CascadeMenuEntry, 
		Menu* m = m_entries[i].getSubMenu();
		if (m)
		{
			m->attach_at( m_entries[i].sx, bottom );
		}
	}
	return 0;
}

int		HorizontalMenu::add_entry_text( const char* mMenuText )
{
	MenuEntry me;
	me.set_text( mMenuText );
	m_entries.push_back( me );
	calc_metrics();
	return 1;
}
/*int		HorizontalMenu::add_sub_menu( const char* mMenuText, VerticalMenu* vm )
{ return 0;
}*/

/*int 	HorizontalMenu::add_entry( struct stMenuInfo* mEntry )
{
	m_entries.push_back( *mEntry );
	calc_metrics();
	return 1;
}*/


int HorizontalMenu::draw(	)
{ 
	// CENTER TEXT:
	size_t item_count = m_entries.size();
	float l = left;
	float c = (height-text_size) / 2.0 + bottom;

	// Draw Horizontal Bar:
	Stroke_l( background_color );
	Fill_l  ( background_color );
	Rect    ( left, bottom, width, height );
	if (Debug) print_positions();
	
	// Color the Selected Menu:
	if (m_selection>=0)
	{
		Stroke_l( m_selected_color );
		Fill_l  ( m_selected_color );
		Rect    ( m_entries[m_selection].sx, bottom, m_entries[m_selection].width, height );
		//printf("H menu drew rectangles!\n");
	}

	// Print text for each entry:
	for (size_t i=0; i<item_count; i++)
	{
		if (m_selection==i)
		{
			Stroke_l( background_color );
			Fill_l  ( background_color );
			//printf  ("MenuColor: txt=%4x\n", background_color );
			Menu* m = m_entries[i].getSubMenu();
			if (m) m->draw();
		} else {
			Stroke_l( text_color );
			Fill_l  ( text_color );
			//printf("MenuColor: txt=%4x\n", text_color );
		}
		l = m_entries[i].sx;
		//printf("%6.1f, %6.1f: %6.1f  %s\n", l, bottom, text_size, m_entries[i]->text );
		if (l>(left+width))
		{
			Stroke_l( 0xFF5f0000 );
			Fill_l  ( 0xFF5f0000 );
			float w = (width - m_entries[i-1].ex);
			Rect    ( m_entries[i-1].ex, bottom, w, height );			
		}
		Text( l, c, m_entries[i].text,  SerifTypeface, text_size );
	}
	if ( is_selection_valid() )  {
		//printf("H Menu selection is valid!\n");
		Menu* m = m_entries[m_selection].getSubMenu();
		if ((m) && (m->is_visible())) 
		{
			//printf("H Menu selection is a menu\n");					
			m->draw();
		}
	}
	return 1;	
}

int	HorizontalMenu::get_id(   )
{
	if (m_selection==(size_t)-1)				return -1;
	if (m_selection > m_entries.size()) return -1;
	//int result = (m_selection*1000);
	//m_entries[m_selection]->menu->get_id();
	return 1;
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
		l = m_entries[i].sx;		
		r = m_entries[i].ex;		
		if ((Mousex > l) && (Mousex < r) )
			return i;
	}	
	// if no items, 
	return -1;
}


Control* HorizontalMenu::HitTest( int x, int y )
{
	printf("HorizontalMenu::HitTest() \n" );
	Control* result = Control::HitTest(x,y);	// also checks the open selected menu too.

	if ( (result==NULL) && is_selection_valid() )
	{
		// Test the open submenu:
		printf("HorizontalMenu::HitTest() selection=%d\n", m_selection );
		Menu* m = m_entries[m_selection].getSubMenu();
		if (m)
			result = m->HitTest(x,y);
		printf("result==%p\n", result );		
	}
	//if (result)
	//	printf( "HorizontalMenu::HitTest() %d,%d =  %x\n", x,y,result );
	return result;
}

int	HorizontalMenu::onClick( int x, int y, bool mouse_is_down )
{
	int menu_index = get_hit_index( x,y );
	if (menu_index >= 0)
	{
		printf( "Mouse Click  Horiz Menu Item # %d\n", menu_index );	
		Menu* m = m_entries[menu_index].getSubMenu();
		if (m)
		{
			printf("Is a SubMenu. \n");			
			// hide any other visible menu
			if ( is_selection_valid() )  {
				Menu* om = m_entries[m_selection].getSubMenu();		
				if (om) {
					om->hide();
					Invalidate();
				}
			}
			m_selection = menu_index;
			m->set_z_order( ++MainDisplay.m_z_order_counter );
			m->show(true);
			m->draw();
			//Invalidate();			
		}
		return 1;		
	}	
	return 0;	
}

