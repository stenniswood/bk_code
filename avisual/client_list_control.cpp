
#include <shapes.h>
#include <fontinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
       
#include "CAN_Interface.h"
#include "dataset.hpp"
#include "display.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "display_manager.hpp"
#include "client_memory.hpp"
#include "client_list_control.hpp"

#include "protocol.h"
#include "devices.h"
#include "CAN_memory.h"
//#include "CAN_util.h"
//#include "GENERAL_protocol.h"
//#include "CAMERA_device.h"
//#include "thread_control.h"



#define Debug 1


ClientList::ClientList()
{
}
ClientList::ClientList( int Left, int Right, int Top, int Bottom  )
: TabularListBox( Left, Right, Top, Bottom)
{
}
ClientList::~ClientList()
{
}

void ClientList::set_headings()
{
	static struct HeaderItemInfo hdr_info;
	hdr_info.start_x  = 10.;
	hdr_info.end_x	  = 10.;

	// Column #1 : 
	hdr_info.text  = "Name";
	hdr_info.width = -1;
	hdr_info.alignment = HEADER_ALIGN_LEFT;	
	add_column( &hdr_info );

	// Column #2 :
	hdr_info.text  = "Network";
	hdr_info.width = -1;
	hdr_info.alignment = HEADER_ALIGN_LEFT;		
	add_column( &hdr_info );

	hdr_info.text  = "Machine_Type";
	hdr_info.width = -1;
	hdr_info.alignment = HEADER_ALIGN_LEFT;
	add_column( &hdr_info );

	// Column #3:
	//hdr_info = new struct HeaderItemInfo();
	hdr_info.text  = "Address";
	hdr_info.width = -1;
	hdr_info.alignment = HEADER_ALIGN_LEFT;			
	add_column( &hdr_info );	

	hdr_info.text  = "Status        ";
	hdr_info.width = -1;
	hdr_info.alignment = HEADER_ALIGN_LEFT;
	add_column( &hdr_info );

}

void ClientList::set_row( struct stClientData* mDatum )
{
	if (Debug) printf("CLient:%s\t%s\t%s\t%s\n", mDatum->name, "tcp/ip", mDatum->machine, mDatum->address );
	
	vector<string> *data = new vector<string>;	
	data->push_back(mDatum->name);
	data->push_back("TCP/IP");
	data->push_back(mDatum->machine);
	data->push_back(mDatum->address);
	data->push_back(mDatum->status );
	add_row (data);
}

void ClientList::update_available_client_list()
{
	if (Debug) printf("====== List of Available Clients: ==================\n");
	if (ipc_memory_client==NULL)  {
		printf(" Error Peer List is not available!\n");
		return;
	}
	cli_print_clients();
	clear_items();
	//if (Debug) printf("===================================================\n");
		
	for (int i=0; i<ipc_memory_client->NumberClients; i++)
		set_row( &(ipc_memory_client->ClientArray[i]) );
	if (Debug) printf("===================================================\n");
}

/************************************************************************
	Client List Panel - has button for connecting, etc.
 ************************************************************************/
ClientListPanel::ClientListPanel()
{
	
}
ClientListPanel::ClientListPanel( int Left, int Right, int Top, int Bottom  )
:Window( Left, Right, Top, Bottom )
{

}

ClientListPanel::~ClientListPanel()
{

}	

/* CALLBACK :   */
void connect_button_cb( void* mControl )
{
	ClientListPanel* panel = (ClientListPanel*) mControl;
	
	// CONNECT TO  SEQUENCING RPI : 
	char working_buffer[127];
	int index = cli_find_name( "sequencer" );
	if ((index==-1) || (index>MAX_CLIENTS)) return;

	strcpy( working_buffer, "connect " );
	strcat( working_buffer, ipc_memory_client->ClientArray[index].address );

	// Where do we get the real IP Address? 
	printf("\n\nConnect command:  %s\n", working_buffer );
	cli_ipc_write_sentence( working_buffer );		// This is working!

	// Need to wait for acknowledgement here.
	cli_wait_for_ack_update();
	printf("back from cli_wait_for_ack_update\n" );
	
	const int STATUS_COL =4;
	panel->m_clients->set_row_col_text( "connected", index, STATUS_COL );
	
}

int	ClientListPanel::onCreate(  )
{
	m_clients = new ClientList(left, left+width-50, bottom+height, bottom );
	m_clients->calc_metrics();
	m_clients->set_headings();
	m_clients->onCreate();
	m_clients->update_available_client_list();

	m_connect.set_on_click_listener( connect_button_cb, this );
	add_control( m_clients );
	add_control( &m_connect );
		
	Window::onCreate();

	// Create a Timer for updating the list box,  No!
	// Should have a callback from the udp_transponder receive thread.
	// then on new client callback will update.	
	//		No again!  udp_transponder is in Instant, not avisual!
	//      So... we poll cli_is_new_client() instead.
}

int ClientListPanel::place_views( )
{
	m_connect.set_text( "connect", true );
	m_connect.set_position_right_of( m_clients );	
}

Control* ClientListPanel::HitTest( int x, int y )
{
	return Window::HitTest(x,y);
}

int		 ClientListPanel::onClick( int x, int y, bool mouse_is_down=true )
{
	return Window::onClick(x,y, mouse_is_down);
}

