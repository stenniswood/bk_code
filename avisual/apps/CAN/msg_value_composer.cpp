#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "msg_value_composer.hpp"

 

MsgValueComposer::MsgValueComposer()
{
	Initialize();
}

void 	MsgValueComposer::Initialize(	)
{
	Window::Initialize();
	
	m_cancel.set_text("Cancel", true );
	m_update.set_text("Update", true );
	 
	//m_msg_id_.set_width_height( 50, 30  );
	//m_instance_.set_width_height( 50, 30  );
	//m_byte_length_.set_width_height( 50, 30 );
	//m_byte_start_index_.set_width_height( 50, 30 );
	m_add.set_text("+", true);
	m_remove.set_text("-", true);
	
	m_msg_id_.set_text			("ID_MOTOR_VALUE",true);
	m_instance_.set_text		("31",true);
	m_byte_length_.set_text		("16",true);
	m_byte_start_index_.set_text("2 ",true);

	//
	struct HeaderItemInfo head;
	head.width = -1;
/*{
	string	text;
	float 	start_x;
	float 	end_x;
	float	width;
	float	layout_weight;	
	float	min_width;		// 
	float	max_width;		// 
	byte	alignment;		// left,center,right 
} */

	vector<struct HeaderItemInfo> headings;
	head.text = "#";
	headings.push_back(head);
	head.text = "id";
	headings.push_back(head);
	head.text = "instance";
	headings.push_back(head);
	head.text = "byte index";
	headings.push_back(head);
	head.text = "size";
	headings.push_back(head);	
	m_list.set_headings( &headings );
	
	add_control( &m_cancel );			
	add_control( &m_update );	
	add_control( &m_add    );	
	add_control( &m_remove );
	add_control( &m_msg_id_ );	
	add_control( &m_instance_ );	
	add_control( &m_byte_length_ );	
	add_control( &m_byte_start_index_ );
	//add_control( &m_list );
}

int		MsgValueComposer::calc_metrics()
{
	return 0;
}

int		MsgValueComposer::place_views()
{
	printf(" MsgValueComposer::place_views()   \n");
	m_add.move_to( left+20, bottom+10 );
	m_remove.set_position_right_of( &m_add );
	
	m_cancel.set_position_right_of( &m_remove, true, 50 );
	m_update.set_position_right_of( &m_cancel, true, 50 );
	
	m_msg_id_.move_to( left+20, bottom+150 );
	m_instance_.set_position_below( &m_msg_id_, true, 10 );
	m_byte_length_.set_position_below( &m_instance_, true, 10 );
	m_byte_start_index_.set_position_below( &m_byte_length_, true, 10 );	

	//m_list.set_width_height(  ,   );
	//m_list.move_to( left+width-m_list.get_width()-10, bottom+height-10 );
	
/*	m_cancel.print_positions();
	m_update.print_positions();
	m_msg_id_.print_positions();
	m_byte_start_index_.print_positions();
	m_byte_length_.print_positions();		*/
	return 1;	
}

