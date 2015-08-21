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


AnalogView::AnalogView()
{
	set_width_height( 1200., 900 );	
	move_to		    ( 0, 100   );
	Initialize();
}
AnalogView::AnalogView( int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
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
	m_chip_enable1->set_text("Chip1");
	m_chip_enable2->set_text("Chip2");
	m_chip_enable3->set_text("Chip3");
	m_chip_enable4->set_text("Chip4");	
	m_chip_enable1->set_check();
	m_chip_enable3->set_check();
	printf("AnalogView::textd()\n");	

	add_control( m_chip_enable1 );
	add_control( m_chip_enable2 );
	add_control( m_chip_enable3 );
	add_control( m_chip_enable4 );
	printf("AnalogView::Initialize() checkboxes added\n");
	
	configure_screen();
	//show_group( 0, 8 );
	//show_group( 2, 8 );	// chip 3
	printf("done with show groups\n");
	
	place_views();
	printf("done with place views\n");	
}

void AnalogView::configure_screen()
{	
	int last_used_index = 0;
	float b = bottom+50;
	if (m_chip_enable1->is_checked())
	{
		m_group_start_index[0] = 0;
		m_group_end_index  [0] = 7;
		show_group ( 0, 8 );
		place_group( last_used_index, last_used_index+8, b );
		last_used_index += 8;
		b+= 200;
	}
	if (m_chip_enable2->is_checked())
	{
		m_group_start_index[1] = m_group_end_index[0]  +1;
		m_group_end_index  [1] = m_group_start_index[1]+8;
		show_group( 1, 8 );
		place_group( last_used_index, last_used_index+8, b );
		last_used_index += 8;
		b+= 200;		
	}
	if (m_chip_enable3->is_checked())
	{
		m_group_start_index[2] = m_group_end_index[1]+1;
		m_group_end_index  [2] = m_group_start_index[2]+8;
		show_group( 2, 8 );
		place_group( last_used_index, last_used_index+8, b );
		last_used_index += 8;
		b+= 200;		
	}
	if (m_chip_enable4->is_checked())
	{
		m_group_start_index[3] = m_group_end_index[2]+1;
		m_group_end_index  [3] = m_group_start_index[3]+8;
		show_group( 3, 8 );
		place_group( last_used_index, last_used_index+8, b );
		last_used_index += 8;
		b+= 200;		
	}
}

void	AnalogView::show_group( int mChip, int mNumberVisible )
{
	Leveler* l;
	char title[20];
	for (int i=0; i<mNumberVisible; i++)
	{
		l = new Leveler(1023,0);	
		if (l != NULL) {
			l->set_width_height( 100., 150. );
			l->set_level_percent( 50 );
			if (i==0)
				sprintf ( title, "Chip %d, A%d", mChip, i );
			else
				sprintf ( title, "A%d", i );			
			l->set_text(title);			// NAME
			m_indicators.push_back( l );	
			add_control( l );	
		}
	}
}
void	AnalogView::place_group( int mStartIndex, int mEndIndex, float mBottom )
{
	float Left=100;
	for (int i=mStartIndex; i<mEndIndex; i++)
	{
		m_indicators[i]->move_to( Left, mBottom );
		Left+=125;		
	}
}

int		AnalogView::place_views()
{
	long size = m_indicators.size();
	printf("AnalogView::place_views() %d\n",size);

	m_chip_enable1->move_to(25, bottom+850);
	m_chip_enable2->set_position_below( m_chip_enable1, true, 20.0 );
	m_chip_enable3->set_position_below( m_chip_enable2, true, 20.0 );
	m_chip_enable4->set_position_below( m_chip_enable3, true, 20.0 );	

	// these are done in configure_screen() now.
	//place_group( 0, 8,  bottom+40 );
	//place_group( 8, 16, bottom+400 );
}

int		AnalogView::calc_metrics()
{
}


int	AnalogView::handle_incoming_msg	( struct sCAN* msg )
{
	int instance;
	int channel;
	static int value;
	if (msg->id.group.id == ID_ANALOG_MEASUREMENT)
	{	
		instance = msg->id.group.instance;
		channel  = msg->data[0];
		// Map Chip 3 to upper 8 levelers.
		if ((channel>=0x20) && (channel<0x2F))
			channel = (channel&0x0F) + 8;		// map 0x20-0x27 to 8 thru 15. 
			
		value = (msg->data[1]<<8) + msg->data[2];		
		//printf("analog %d %d\n", channel, value );
		if ((channel>=0) && (channel<16)) {
			m_indicators[channel]->set_level( value );
			//m_indicators[channel]->Invalidate();
		}
		Invalidate();
	}			
}

int		AnalogView::setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms )
{
}
int		AnalogView::setup_triggered_response_msg( struct sCAN* mTriggerMsg, struct sCAN* mResponse )
{
}
void	AnalogView::fill_phony_msgs()
{
}
int	AnalogView::onCreate	  (  )	// chance to load resources, call functions which use fonts
{
	return Window::onCreate();
}
int		AnalogView::get_hit_index	( int Mousex, int Mousey )
{
}
int	AnalogView::onClick				( int x, int y, bool mouse_is_down)
{
}
int AnalogView::draw		 		(				)
{
	return Window::draw();
}


