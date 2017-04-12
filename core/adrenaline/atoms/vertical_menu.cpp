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

#include "control.hpp"
#include "listbox.hpp"
#include "display.h"
#include "vertical_menu.hpp"


#define Debug 0


VerticalMenu::VerticalMenu()
:Menu()
{
	Initialize();
}

VerticalMenu::VerticalMenu( int Left, int Right, int Top, int Bottom )
:Menu ( Left, Right, Top, Bottom )
{
	Initialize();
}

VerticalMenu::VerticalMenu( int Width, int Height )
:Menu( Width, Height )
{
	Initialize();
}

VerticalMenu::~VerticalMenu()
{
}

void 	VerticalMenu::Initialize(	)
{
	Menu::Initialize();

//	set_odd_color ( 0xEFFFFFFF );
//	set_even_color( 0xEFFFFFFF );

	m_parent	   = NULL;
	callback_all_items = NULL;
	has_scroller	= false;
//	isTopDown  		= true;
	text_size 		= 19;
	text_color 		= 0xFF000000;	
	m_disabled_color = 0xFF7f7f7f;
}

int  VerticalMenu::calc_metrics()
{
	//has_header = false;
	//number_lines_visible = m_entries.size();
	//Menu::adjust_height_for_num_visible_items(number_lines_visible);

	float tmp_width  	 = get_longest_line();	
	LineHeight     = 1.5 * text_size;
	float tmp_height 	 = (m_entries.size()*LineHeight)+5;
	set_width_height( tmp_width, tmp_height );

	Menu::calc_metrics();
	return 1;
}

int  VerticalMenu::onCreate	(  )
{
	if (Debug) printf( "VerticalMenu::onCreate()\n" );
	int retval = Menu::onCreate();
	m_underneath = vgCreateImage(VG_sABGR_8888, width, height, 
								 VG_IMAGE_QUALITY_BETTER);
	save_pixels();
	return retval;
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
	//set_item( mText );

	//struct stMenuInfo m;  
	MenuEntry m;
	strcpy (m.alt_key, 		 " ");
	strcpy (m.short_cut_key, " ");
	m.width 	= 0;
	m.state 	= MENU_STATE_NORMAL;
	m.callback 	= NULL;	
	m.set_text(mText);
	
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

/*int 	VerticalMenu::add_sub_menu		( char* mText, VerticalMenu* mSubMenu )
{
	return 1;	
}

int 	VerticalMenu::add_entry 		( stVertMenuInfo mEntry )
{
	return 1;
}*/

	
int 	VerticalMenu::set_state ( enum eMenuState mState, int mIndex )
{
	m_entries[mIndex].state = mState;
	return 1;	
}

void 	VerticalMenu::draw_one_row( int mRow, float mY )
{
	float above_line_offset = (LineHeight-text_size)/2.0;
	mY += above_line_offset;

	if (m_entries[mRow].state == MENU_STATE_NORMAL)
	{
		Stroke_l( text_color );
		Fill_l  ( text_color );
	}
	else if (m_entries[mRow].state == MENU_STATE_GRAYED)
	{
		Stroke_l( m_disabled_color );
		Fill_l  ( m_disabled_color );	
	}
	
	//printf("%6.1f %6.1f   %s \n", left, mY, (char*)get_item(mRow)->c_str() ); 
	Text( left, mY, (char*)m_entries[mRow].text, SerifTypeface, text_size );

	Menu* m = m_entries[mRow].getSubMenu();
	if (m)
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
	Menu::draw();
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

void VerticalMenu::save_pixels	()
{	
	vgGetPixels(m_underneath, 0, 0, left, bottom, width, height);
}
void VerticalMenu::restore_pixels()
{
	vgSetPixels(left, bottom, m_underneath, 0, 0, width, height );	
}

void VerticalMenu::close_menu( )
{
	hide();
	printf("VerticalMenu::close_menu( )\n");
	//restore_pixels();
	Invalidate();
}	

Application* VerticalMenu::get_application(   )
{
	if (m_parent==NULL) return NULL;
	return  m_parent->m_application;
}

int		VerticalMenu::onClick(int x, int y, bool mouse_is_down)
{
	size_t result = get_hit_index( x, y );
	if (((result >= 0) && (result < m_entries.size())) &&
			(m_entries[result].state != MENU_STATE_GRAYED))
	{
		printf("VerticalMenu:: Selected Item #%d: %s\n", result, m_entries[result].text );
		// 1 Callback for entire menu :
		if (callback_all_items) {
			Application* app = get_application(   );
			callback_all_items( NULL, result, app );
		}	
		// Individual Callbacks :
		if (m_entries[result].callback)
			m_entries[result].callback( NULL, result );
		close_menu();
	}
	return 1;	
}

/*	This does not count the header!
	Lines within the box only!

	VisibleIndex means the index which is visible on the screen. 
	This is different from the item index (lines may not be visible)
*/
float VerticalMenu::get_line_bottom( int mVisibleIndex )
{
	float y; 
	//if (isTopDown)
	if (1)	
	{
		y = bottom + height - LineHeight * (mVisibleIndex+1);
		return y;
	} else {
		y = bottom + LineHeight * (mVisibleIndex);
		return y;
	}
}

int VerticalMenu::get_hit_index(int mx, int my)
{
	int   start	 = 0;
	int   end  	 = m_entries.size();
	int	  retval = -1;
	float y;

	for ( int i=start; i<end; i++ )
	{
		y = get_line_bottom( i-start ); 
		if ( (my>y) && (my<(y+LineHeight)) )
			retval = i;
	}
	//string str = *get_item(retval);
	string str =m_entries[retval].text;
//	if (Debug) printf("Selected Item # %d : start=%d, end=%d  %s\n", retval, first_visible_line, end, str.c_str() );
	return retval; 
}

// result in pixels : 
float VerticalMenu::get_longest_line(  )
{
	float max_w = 0;
	for (size_t i=0; i<m_entries.size(); i++)
	{
		float w = TextWidth( (char*)m_entries[i].text, SerifTypeface, text_size );
		if (w > max_w)
			max_w = w;
	}
	//printf("ListBox::get_longest_line() max_w=%6.1f\n", max_w );
	return max_w;
}

/*int		VerticalMenu::attach_at			( float x, float y )
{
	move_to(x, y-height);
	return 1;	
}*/
