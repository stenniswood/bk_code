/*******************************************************
* Draws the Adrenaline Network Overview
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <string>
#include <fontinfo.h>
#include <shapes.h>

#include "adrenaline_windows.h"
#include "display.h"
#include "adren_network_view.hpp"
#include "CAN_base.h"
#include "can_id_list.h"


#define Debug 0


NetworkView::NetworkView( )
:TabularListBox()
{ 
	Initialize();
}

NetworkView::NetworkView ( int Left, int Right, int Top, int Bottom )
:TabularListBox(Left, Right, Top, Bottom)
{
	Initialize();
}

NetworkView::NetworkView ( int Width, int Height )
:TabularListBox(Width, Height)
{
	Initialize();
}

NetworkView::~NetworkView(  )
{
	Initialize();
}

int	NetworkView::setup_headers()
{
	static struct HeaderItemInfo mNewHeading;	// = new struct HeaderItemInfo();
	//mNewHeading.width	  = -1;
	mNewHeading.text 		= "Model";		// BigMotor, Tilt, Button, etc.
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;
	add_column( &mNewHeading );

	mNewHeading.text 		= "Instance";		// [0..255]
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;
	add_column( &mNewHeading );

	mNewHeading.text 		= "Status";		// Failure, Fault condition, Okay
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;
	add_column( &mNewHeading );

	mNewHeading.text 		= "Firmware";		// Version
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;
	add_column( &mNewHeading );

	mNewHeading.text 		= "Hardware";		// Revision
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;
	add_column( &mNewHeading );

	mNewHeading.text 		= "Serial Number";		// Revision
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;
	add_column( &mNewHeading );

	mNewHeading.text 		= "Description";		// Revision
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;
	mNewHeading.width      = 300;
	add_column( &mNewHeading );

/*	mNewHeading.text 		= "Manufacturer";		// Revision
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;
	add_column( &mNewHeading ); */
}

void NetworkView::reset()
{
	m_board_info.clear();
	m_board_info_text.clear();
	clear_items();
}

struct stBoardInfoText* NetworkView::convert_to_text( struct stBoardInfo2* mInfo )
{
	static struct stBoardInfoText Txt;	
	sprintf( Txt.model, 			"%d", mInfo->model    		);
	sprintf( Txt.instance, 			"%d", mInfo->instance 		);
	sprintf( Txt.status, 			"%d", mInfo->status   		);
	sprintf( Txt.Software_Version, 	"%d.%d", mInfo->Software_Major, mInfo->Software_Minor );
	sprintf( Txt.Hardware_Revision, "%c", mInfo->Hardware_Revision );
	sprintf( Txt.BKSerialNumber, 	"%4x", mInfo->BKSerialNumber );
	sprintf( Txt.Description, 		"%s", mInfo->Description    );
//	sprintf( Txt.Manufacturer, 		"%d", mInfo->Manufacturer   );
	return &Txt;
}

int	NetworkView::formulate_line	( struct stBoardInfoText* mTxt )
{
	vector<string> data;
	data.push_back( mTxt->model );
	data.push_back( mTxt->instance );
	data.push_back( mTxt->status );	
	data.push_back( mTxt->Software_Version );
	data.push_back( mTxt->Hardware_Revision );
	data.push_back( mTxt->BKSerialNumber );
	data.push_back( mTxt->Description );
	//data.push_back( mTxt->Manufacturer );
	add_row( &data );
}

int	NetworkView::add_board( struct stBoardInfo2* mBInfo )
{
	if (Debug) printf("NetworkView::add_board() \n");
	m_board_info.push_back( *mBInfo );
	struct stBoardInfoText* Txt = convert_to_text( mBInfo );
	m_board_info_text.push_back( *Txt );
	formulate_line( Txt );
}

void can_parse_board_presence_msg( sCAN* mMsg, struct stBoardInfo2* mOut )
{
    mOut->model    = mMsg->data[0];
    mOut->instance = mMsg->data[1];
    mOut->status   = mMsg->data[2];
   // mOut->block    = mMsg->data[3];
}

int	NetworkView::handle_incoming_msg( struct sCAN* msg )
{
	static struct stBoardInfo2 bi;
	switch(msg->id.group.id)
	{
	case ID_BOARD_PRESENCE_BROADCAST:
		can_parse_board_presence_msg( msg, &bi );
		add_board( &bi );		 
		break;
	case ID_BOARD_REVISION:
		break;
	case ID_BOARD_SERIAL_NUMBER:
		break;
	case ID_BOARD_DESCRIPTION:
		break;							
	default: break;
	}
}

void 	NetworkView::Initialize  (	) 
{ 
	TabularListBox::Initialize();
	setup_headers();	
	populate_with_boards();
}

int	NetworkView::populate_with_boards(	)
{
        struct stBoardInfo2 bi;
        bi.model = 21;
        bi.instance = 31;
        bi.status = 0;
        bi.Software_Major = 3;
        bi.Software_Minor = 5;
        bi.Hardware_Revision = 'C';
        bi.Manufacturer = 1;
        bi.BKSerialNumber = 0x00001001;
        strcpy (bi.Description, "BigMotor Board");              
        add_board ( &bi );
        bi.instance = 32;
        add_board ( &bi );
        bi.instance = 33;
        add_board ( &bi );
        bi.instance = 41;       
        add_board               ( &bi );        
        bi.instance = 42;       
        add_board               ( &bi );        
        bi.instance = 43;
        add_board               ( &bi );
}

int		NetworkView::calc_metrics(	) 
{

}
	
int	NetworkView::draw_board( )
{

}


int	NetworkView::draw_board_info( )
{

}

int	NetworkView::draw  	( )
{
	TabularListBox::draw();
}

void can_parse_board_revision_msg( sCAN* mMsg, struct stBoardInfo2* mOut )
{
    mOut->Hardware_Revision = mMsg->data[0];
    mOut->Software_Major = mMsg->data[1];
    mOut->Software_Minor = mMsg->data[2];
    mOut->Manufacturer   = (mMsg->data[4]<<8) + mMsg->data[3];
}

void can_parse_serial_number_msg( sCAN* mMsg, struct stBoardInfo2* mOut )
{
    mOut->BKSerialNumber =  ((long)mMsg->data[0]<<24) + (mMsg->data[1]<<16) + 
    						(mMsg->data[2]<<8) + (mMsg->data[3]);
}

void can_parse_board_description_msg( sCAN* mMsg, struct stBoardInfo2* mOut )
{
	int i;
    byte Starting_Index = mMsg->data[0];

   	for (i=0; i<8; i++)
	    mOut->Description[i] = mMsg->data[i+1];
 }


