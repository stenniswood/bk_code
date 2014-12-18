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
//	m_width		= -1.;
	has_scroller= false;
	is_visible	= true;
	set_odd_color ( 0xEFFFFFFF );
	set_even_color( 0xEFFFFFFF );
	isTopDown  = true;
	text_color = 0xFF000000;
}

int  VerticalMenu::calc_metrics()
{
	//ListBox::calc_metrics();
	LineHeight           = text_size * 1.5;
	number_lines_visible = m_entries.size();
	printf  ( "VerticalMenu::calc_metrics() \n" );			
	float tmp_width  = get_longest_line();
	printf  ( "get_longest_line \n" );		
	float tmp_height = (number_lines_visible*LineHeight);
	set_width_height( tmp_width, tmp_height );
}

int  VerticalMenu::add_simple_command( char* mText, char* mShortcut )
{
	set_item( mText );
	printf  ( "add_simple_command:: %s \n", mText );
	
	struct stVertMenuInfo* m = new struct stVertMenuInfo();
	strcpy ( m->text, mText );  
	if (mShortcut)
		strcpy ( m->short_cut_key, mShortcut );		// to directly activate from keyboard 	
	m->state = MENU_STATE_NORMAL;
	m_entries.push_back( m );
	printf  ( "add_simple_command:: 3 \n" );		
	calc_metrics();
	printf  ( "add_simple_command:: 4 \n" );			
}

int	VerticalMenu::add_callback( int mIndex, int (*mcallback)(void*, int) )
{
	m_entries[mIndex]->callback = mcallback;
	
}

int 	VerticalMenu::add_sub_menu		( char* mText, VerticalMenu* mSubMenu )
{
	
}

int 	VerticalMenu::add_entry 		( stVertMenuInfo mEntry )
{

}

int		VerticalMenu::attach_at			( float x, float y )
{
//	left   =  x;
//	bottom = (y - height);
	move_to(x, y-height);
}
	
int 	VerticalMenu::set_state ( int mState, int mIndex )
{
	m_entries[mIndex]->state = mState;
}

void 	VerticalMenu::draw_one_row( int mRow, float mY )
{
	float above_line_offset = (LineHeight-text_size)/2.0;
	mY += above_line_offset;
	Stroke_l( text_color );
	Fill_l  ( text_color );
	//printf("%6.1f %6.1f   %s \n", left, mY, (char*)get_item(mRow)->c_str() ); 
	Text( left, mY, (char*)get_item(mRow)->c_str(), SerifTypeface, text_size );

	// draw short cut info (if available)
	//TextEnd( left, mY, (char*)get_item(mRow)->c_str(), SerifTypeface, text_size );
}

int   	VerticalMenu::draw		 		( 	)
{
	//printf("VerticalMenu::draw	\n");
	//print_positions();
	ListBox::draw();
}

int 	VerticalMenu::get_hit_index		( int Mousex, int Mousey )
{
	printf("VerticalMenu::get_hit_index()\n");
	return ListBox::get_hit_index( Mousex, Mousey );
}

int	VerticalMenu::set_h_parent			( HorizontalMenu* mMenu )
{
	m_horiz_parent = mMenu;
}

// int 	VerticalMenu::set_on_click_listener( void (void*) )	
int		VerticalMenu::onClick(int x, int y, bool mouse_is_down)
{
	int result = get_hit_index( x, y );
	if ((result < m_entries.size()) && (result >= 0))
	{
		printf("VerticalMenu:: Selected Item #%d: %s\n", result, m_entries[result]->text );
		if (m_entries[result]->callback)
			m_entries[result]->callback( NULL, result );
	}
}

int   VerticalMenu::show( bool mVisible )
{
	is_visible = mVisible;
}

