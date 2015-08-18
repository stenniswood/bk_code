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
	set_width_height( 1200., 700 );	
	move_to		 ( 0, 100   );
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
	for (int i=0; i<8; i++)
	{
		Leveler* l = new Leveler(1023,0);		
		m_indicators.push_back( l );	
		add_control( l );		
		//register_child( l  );		

		//MainDisplay.add_object( &ParentWindowF );	
	}
	place_views();
}
int		AnalogView::calc_metrics()
{
}
int		AnalogView::place_views()
{
	long size = m_indicators.size();
	printf("AnalogView::place_views() %d\n",size);
	float Left=100;
	
	for (int i=0; i<8; i++)
	{
		m_indicators[i]->set_width_height( 100., 200. );
		m_indicators[i]->set_level_percent( 50 );
		m_indicators[i]->move_to		 ( Left, bottom+40  );
		Left+=125;		
	}
	m_indicators[0]->set_text("A0");
	m_indicators[1]->set_text("A1");
	m_indicators[2]->set_text("A2");
	m_indicators[3]->set_text("A3");

	m_indicators[4]->set_text("A4");
	m_indicators[5]->set_text("A5");
	m_indicators[6]->set_text("A6");
	m_indicators[7]->set_text("A7");
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
		value = (msg->data[1]<<8) + msg->data[2];		
		//printf("analog %d %d\n", channel, value );
		if ((channel>=0) && (channel<8)) {
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


