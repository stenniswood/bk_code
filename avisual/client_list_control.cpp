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
}

void ClientList::set_row( struct stClientData* mDatum )
{
	if (Debug) printf("CLient:%s\t%s\t%s\t%s\n", mDatum->name, "tcp/ip", mDatum->machine, mDatum->address );
	
	vector<string> *data = new vector<string>;	
	data->push_back(mDatum->name);
	data->push_back("TCP/IP");
	data->push_back(mDatum->machine);
	data->push_back(mDatum->address);
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
	for (int i=0; i<ipc_memory_client->NumberClients; i++)
		set_row( &(ipc_memory_client->ClientArray[i]) );
	if (Debug) printf("===================================================\n");
}



