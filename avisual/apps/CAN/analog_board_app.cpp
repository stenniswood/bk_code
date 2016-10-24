/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <fontinfo.h>
#include <shapes.h>
#include "Graphbase.hpp"
#include "adrenaline_windows.h"
#include "draw_app.hpp"
#include "draw_app2.hpp"
#include "CAN_base.h"
#include "can_window.hpp"
#include "analog_board_app.hpp"
#include "leveler.hpp"

#include "can_id_list.h"

#define Debug 1

/**** CHECKBOX LISTENER CALLBACKS ****/
void show_group_1_cb( void* mAnalogView )
{
	static bool visible=false;
	if (Debug) printf("show_group_1_cb\n\n");
	AnalogView* av = (AnalogView*)mAnalogView;
	av->calc_metrics();
	av->place_views();
	av->Invalidate();
	visible = !visible;
}
void show_group_2_cb( void* mAnalogView )
{
	static bool visible=false;
	if (Debug) printf("show_group_2_cb\n\n");
	AnalogView* av = (AnalogView*)mAnalogView;
	av->calc_metrics();
	av->place_views();	
	if (Debug) printf("show_group_2_cb show_group\n\n");
	av->Invalidate();
	visible = !visible;
}
void show_group_3_cb( void* mAnalogView )
{
	static bool visible=false;
	if (Debug) printf("show_group_3_cb\n\n");
	AnalogView* av = (AnalogView*)mAnalogView;
	av->calc_metrics();
	av->place_views();	
	av->Invalidate();
	visible = !visible;
}
void show_group_4_cb( void* mAnalogView )
{
	static bool visible=false;
	if (Debug) printf("show_group_4_cb\n\n");	
	AnalogView* av = (AnalogView*)mAnalogView;
	av->calc_metrics();
	av->place_views();
	av->Invalidate();
	visible = !visible;	
}
/**** END OF Listener CALLBACKS ****/

/****************************************************************
		AnalogView Class Implementation
****************************************************************/
AnalogView::AnalogView()
{
	Initialize();
}
AnalogView::AnalogView( int Left, int Right, int Top, int Bottom )
:Window(Left,Right,Top,Bottom)
{
	Initialize();
}
AnalogView::AnalogView( int Width, int Height 					 )
{
	left=0; bottom=0; 
	width  = Width;
	height = Height;
	Initialize();
}
AnalogView::~AnalogView()
{
}

void AnalogView::Initialize(	)
{
	Window::Initialize();
	printf("AnalogView::Initialize()\n");

	// Let user select which chips on the analog board to see:
	m_chip_enable1 = new CheckBox(-1, -1);
	m_chip_enable2 = new CheckBox(-1, -1);
	m_chip_enable3 = new CheckBox(-1, -1);
	m_chip_enable4 = new CheckBox(-1, -1);
	
	m_chip_enable1->set_text("Chip1",true);
	m_chip_enable2->set_text("Chip2",true);
	m_chip_enable3->set_text("Chip3",true);
	m_chip_enable4->set_text("Chip4",true);	
	m_chip_enable1->set_check();
	m_chip_enable2->set_check();	
	//m_chip_enable3->set_check();
	//m_chip_enable4->set_check();
	
	m_chip_enable1->set_on_click_listener( show_group_1_cb, this );
	m_chip_enable2->set_on_click_listener( show_group_2_cb, this );
	m_chip_enable3->set_on_click_listener( show_group_3_cb, this );
	m_chip_enable4->set_on_click_listener( show_group_4_cb, this );
	printf("AnalogView::textd()\n");	

	add_control( m_chip_enable1 );
	add_control( m_chip_enable2 );
	add_control( m_chip_enable3 );
	add_control( m_chip_enable4 );
	printf("AnalogView::Initialize() checkboxes added\n");
}

/*
	Assigns the index of the leveler controls based on 
	the check boxes for the 'banks' requested.	
*/
void AnalogView::configure_screen()
{	
	m_group_start_index[0] = 0;
	m_group_end_index  [0] = 7;

	m_group_start_index[1] = m_group_end_index[0]  +1;	//  8 
	m_group_end_index  [1] = m_group_start_index[1]+7;	// 16 

	m_group_start_index[2] = m_group_end_index[1]  +1;
	m_group_end_index  [2] = m_group_start_index[2]+7;

	m_group_start_index[3] = m_group_end_index[2]  +1;
	m_group_end_index  [3] = m_group_start_index[3]+7;
}

/* Creates the Levelers, and sizes them.
	Adds the text, and puts them in the vector list of indicators.
	AND adds the controls!
*/
void	AnalogView::create_group( int mChip )
{
	Leveler* l;
	char title[20];

	int NumberVisible = (m_group_end_index[mChip] - m_group_start_index[mChip]+1);
	printf("--- create_group:  Chip %d;  (%d levelers w=%d; h=%d)\n", mChip, NumberVisible, 
					m_col_increment, m_row_increment );	
	
	for (int i=0; i<NumberVisible; i++)
	{
		l = new Leveler(1023,0);	
		if (l != NULL) 
		{
			l->set_width_height( m_col_increment, m_row_increment*0.9 );
			l->set_level_percent( 50 );
			if (i==0)
				sprintf ( title, "IC%d, A%d", mChip, i );
			else
				sprintf ( title, "A%d", i );		

			l->set_text(title);			// NAME
			m_indicators.push_back( l );	
			add_control( l );	
		}
	}
}

/* Places them on the screen.  Just 1 group (ie. a row - chip 2)
*/
void	AnalogView::place_group( int mGroup, float mBottom )
{
	// Place Levelers:	
	float Left=m_col_start;
	for (int i=m_group_start_index[mGroup]; i<=m_group_end_index[mGroup]; i++)
	{
		m_indicators[i]->set_width_height( m_col_increment, m_row_increment*0.9 );
		m_indicators[i]->move_to( Left, mBottom );
		Left+=m_col_increment;
	}
}

void	AnalogView::create_groups()
{
		create_group( 0 );	
		create_group( 1 );
		create_group( 2 );
		create_group( 3 );
}

void	AnalogView::place_views()
{
	long size = m_indicators.size();
	printf("AnalogView::place_views() %ld\n",size);

	m_chip_enable1->move_to(10, bottom+height/2);
	m_chip_enable2->set_position_below( m_chip_enable1, true, 40.0 );
	m_chip_enable3->set_position_below( m_chip_enable2, true, 40.0 );
	m_chip_enable4->set_position_below( m_chip_enable3, true, 40.0 );	

	float b = bottom + m_row_start;
	if (m_chip_enable1->is_checked()) {
		place_group( 0, b );
		show_group( 0, true );
		b+= m_row_increment;		
	}
	if (m_chip_enable2->is_checked()) {
		place_group( 1, b );
		show_group( 1, true );
		b+= m_row_increment;		
	}

	if (m_chip_enable3->is_checked())	{
		place_group( 2, b );
		show_group( 2, true );
		b+= m_row_increment;		
	}
	if (m_chip_enable4->is_checked())	{
		place_group( 3, b );
		show_group( 3, true );
		b+= m_row_increment;
	}

	return ;
}

void AnalogView::show_group( int mGroup, bool mVisible )
{	
	for (int i=m_group_start_index[mGroup]; i <= m_group_end_index[mGroup]; i++)
		m_indicators[i]->show( mVisible );
	Invalidate();
}

int	AnalogView::count_rows_showing()
{
	int num = 0;
	if (m_chip_enable1->is_checked() )	{ num++;	};
	if (m_chip_enable2->is_checked() )	{ num++;	};
	if (m_chip_enable3->is_checked() )	{ num++;	};
	if (m_chip_enable4->is_checked() )	{ num++;	};
	printf("count_rows_showing() = %d \n", num );
	return num;
}

int	AnalogView::calc_metrics()
{
	m_col_start = m_chip_enable1->get_right() + 10;
	float height_margined = height * 0.90;
	int number_visible = count_rows_showing();
	
	m_row_start = 10;
	m_col_increment = (int)(width  / 9.);
	m_row_increment = (int)(height_margined / (number_visible));

	//printf("\tAnalogView::calc_metrics()  width,height=%6.2f,%6.2f  :  w,h= %d %d\n", width, height, m_col_increment, m_row_increment );
	return 1;
}

int	AnalogView::handle_incoming_msg	( struct sCAN* msg )
{
	int instance;
	unsigned int channel;
	static int value;
	if (msg==NULL) { printf("CAN msg NULL!\n"); return 0; };
	
	if (msg->id.group.id == ID_ANALOG_MEASUREMENT)
	{	
		instance = msg->id.group.instance;
		channel  = msg->data[0];
				
		// Map Chip 3 to upper 8 levelers.
		if ((channel>=0x20) && (channel<0x2F))
			channel = (channel&0x0F) + 8;		// map 0x20-0x27 to 8 thru 15. 
		
		value = (msg->data[1]<<8) + msg->data[2];		
		//printf("analog %d %d\n", channel, value );
		if ((channel>=0) && (channel<16)) 
		{
			if ((m_indicators.size()) && (channel < m_indicators.size()))
			{
				//printf("handle_incoming_msg() channel=%d of %d\n", channel, m_indicators.size() );
				if (m_indicators[channel])
				{	
					//printf("[%d]->set_level(%d)\n", channel, value );
					m_indicators[channel]->set_level( value );
					m_indicators[channel]->Invalidate();
				}
			}
		}
		return 1;
	}			
	return 0;
}

int		AnalogView::setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms )
{
	return 0;
}
int		AnalogView::setup_triggered_response_msg( struct sCAN* mTriggerMsg, struct sCAN* mResponse )
{
	return 0;
}
void	AnalogView::fill_phony_msgs()
{
}
int	AnalogView::onCreate	  (  )	// chance to load resources, call functions which use fonts
{
	if (created)	return 0;
	printf("---AnalogView::onCreate---\n");

	calc_metrics();
	configure_screen();
	create_groups();
	place_views();
	printf("---done with place views\n");	
	int retval = Window::onCreate();	
	return retval;
}
int		AnalogView::get_hit_index	( int Mousex, int Mousey )
{
	return 0;
}
int	AnalogView::onClick				( int x, int y, bool mouse_is_down)
{
	return Window::onClick(x,y,mouse_is_down);
}
int AnalogView::draw		 		(				)
{
	return Window::draw();
}


