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
#include "vertical_menu.hpp"


#define Debug 0


VerticalMenu::VerticalMenu()
:ListBox()
{
	Initialize();
}

VerticalMenu::VerticalMenu( int Left, int Right, int Top, int Bottom )
:ListBox ( Left, Right, Top, Bottom )
{
	Initialize();
}

VerticalMenu::VerticalMenu( int Width, int Height )
:ListBox( Width, Height )
{
	Initialize();
}

VerticalMenu::~VerticalMenu()
{
}

void 	VerticalMenu::Initialize(	)
{
	ListBox::Initialize();
	
	set_odd_color ( 0xEFFFFFFF );
	set_even_color( 0xEFFFFFFF );

	m_horiz_parent	   = NULL;
	callback_all_items = NULL;
	has_scroller	= false;
	isTopDown  		= true;
	text_size 		= 15;
	text_color 		= 0xFF000000;	
}

int  VerticalMenu::calc_metrics()
{
	has_header = false;
	number_lines_visible = m_entries.size();
	//ListBox::adjust_height_for_num_visible_items(number_lines_visible);

	float tmp_width  	 = get_longest_line();	
	LineHeight           = 1.5 * text_size;
	float tmp_height 	 = (number_lines_visible*LineHeight)+5;
	set_width_height( tmp_width, tmp_height );

	ListBox::calc_metrics();
	return 1;
}

// New sjt - add to header!
int  VerticalMenu::create_std_file_menu()
{
	m_entries.clear();
	add_simple_command( "New"   		);
	add_simple_command( "Open"  		);
	add_simple_command( "Open Recent"	);	
	add_simple_command( "Save"  		);
	add_simple_command( "Save As" 		);
	return 1;	
}

int  VerticalMenu::add_simple_command( const char* mText, char* mShortcut )
{
	if (Debug) printf( "add_simple_command:: %s \n", mText );
	set_item( mText );

	struct stVertMenuInfo m;  
	strcpy (m.alt_key, 		 " ");
	strcpy (m.short_cut_key, " ");
	m.width 	= 0;
	m.state 	= MENU_STATE_NORMAL;
	m.callback 	= NULL;	
	m.menu 		= NULL;
	strcpy (m.text, mText 		);
	
	if (mShortcut)
		strcpy ( m.short_cut_key, mShortcut );		// to directly activate from keyboard 	
	m_entries.push_back( m );
	//printf  ( "add_simple_command:: 4 \n" );
	
	calc_metrics();
	return 1;	
	//printf  ( "add_simple_command:: 4 \n" );			
}

int	VerticalMenu::add_callback( int mIndex, int (*mcallback)(void*, int) )
{
	m_entries[mIndex].callback = mcallback;	
	return 1;	
}

int	VerticalMenu::add_callback_all_items( int (*callback)(void*, int, Application*) )
{
	callback_all_items = callback;
	return 1;	
}

int 	VerticalMenu::add_sub_menu		( char* mText, VerticalMenu* mSubMenu )
{
	return 1;	
}

int 	VerticalMenu::add_entry 		( stVertMenuInfo mEntry )
{
	return 1;
}

int		VerticalMenu::attach_at			( float x, float y )
{
//	left   =  x;
//	bottom = (y - height);
	move_to(x, y-height);
	return 1;	
}
	
int 	VerticalMenu::set_state ( int mState, int mIndex )
{
	m_entries[mIndex].state = mState;
	return 1;	
}

void 	VerticalMenu::draw_one_row( int mRow, float mY )
{
	float above_line_offset = (LineHeight-text_size)/2.0;
	mY += above_line_offset;
	Stroke_l( text_color );
	Fill_l  ( text_color );
	//printf("%6.1f %6.1f   %s \n", left, mY, (char*)get_item(mRow)->c_str() ); 
	Text( left, mY, (char*)get_item(mRow)->c_str(), SerifTypeface, text_size );

	if (m_entries[mRow].menu)
		draw_triangle( mRow, mY );

	// draw short cut info (if available)
	//TextEnd( left, mY, (char*)get_item(mRow)->c_str(), SerifTypeface, text_size );
}

// indicator of sub menu.
int	VerticalMenu::draw_triangle( int mRow, float mY )
{
	Stroke_l( text_color );
	Fill_l  ( text_color );
	float c = mY+text_size/2.;
	float b = mY + 2;
	float t = mY + text_size - 2;
	float x = left + width - 15;
	Line( x, b, x, t);
	Line( x, t, x+10, c);
	Line( x+10, c, x, b);
	return 1;	
}

int   	VerticalMenu::draw		 		( 	)
{
	//printf("VerticalMenu::draw	\n");
	//print_positions();
	ListBox::draw();
	return 1;	
}

int 	VerticalMenu::get_hit_index		( int Mousex, int Mousey )
{
	//printf("VerticalMenu::get_hit_index()\n");
	return ListBox::get_hit_index( Mousex, Mousey );
	
}

int	VerticalMenu::set_h_parent			( HorizontalMenu* mMenu )
{
	m_horiz_parent = mMenu;
	return 1;
}

/* We need to establish a capture mouse mechanism.  Maybe not for this though.
	Can we evaluate HitTest() for every object visible on the screen, fast enough to
	not slow the mouse noticeably?
	
	Let's assume if it's in here, the HitTest() has already return true for this object.
*/
int	VerticalMenu::onHover( int x, int y, bool mouse_is_down )
{
	int result = get_hit_index( x, y );
	if (result>=0)
	{	
		select( result );
		Invalidate();
	}	
	return result;
}

Application*	VerticalMenu::get_application(   )
{
	if (m_horiz_parent==NULL) return NULL;
	return  m_horiz_parent->m_application;
}

// int 	VerticalMenu::set_on_click_listener( void (void*) )	
int		VerticalMenu::onClick(int x, int y, bool mouse_is_down)
{
	int result = get_hit_index( x, y );
	if ((result < m_entries.size()) && (result >= 0))
	{
		if (Debug) printf("VerticalMenu:: Selected Item #%d: %s\n", result, m_entries[result].text );
		if (callback_all_items) {
			Application* app = get_application(   );
			callback_all_items( NULL, result, app );
		}	
		if (m_entries[result].callback)
			m_entries[result].callback( NULL, result );
	}
	return 1;	
}

