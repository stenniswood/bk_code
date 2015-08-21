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
#include "display.h"
#include "CAN_base.h"
#include "horizontal_menu.hpp"
#include "can_msg_view.hpp"

#include "can_id_list.h"

#define Debug 0


CANMessageView::CANMessageView ()
:TabularListBox()
{
	Initialize(	);
}
CANMessageView::CANMessageView ( int Left, int Right, int Top, int Bottom )
:TabularListBox(Left, Right, Top, Bottom)
{
	Initialize(	);
}
CANMessageView::CANMessageView ( int Width, int Height )
:TabularListBox(Width, Height)
{
	Initialize(	);
}

CANMessageView::~CANMessageView() 
{
   std::vector<sCAN*>::iterator iter = m_msgs.begin();
   while ( iter != m_msgs.end() )
   {	   
	   delete *iter;
   }   
}

void 	CANMessageView::Initialize(	) 
{
 	TabularListBox::Initialize();
 	setup_headers();
}
int		CANMessageView::calc_metrics() 
{
 
}

//struct sCAN cant;

int	CANMessageView::handle_incoming_msg	( struct sCAN* msg )
{
	int instance;
	int channel;
	int value;
	int in_filter = 1;
	static int count = 0;
//		if (msg->id.group.id == ID_ANALOG_MEASUREMENT)
	
	if (in_filter)
	{	
		if (count++ < 50)
		{	
			if (msg->header.DLC > 8)
				printf("DLC=%d\n", msg->header.DLC );
				
			msg->header.DLC 		  = 8;
			
			add_message( msg );
			printf("CANMessageView:: %d\n", count );			
			Invalidate();
		}

	}		
	return 0;	
}

int		CANMessageView::add_message ( struct sCAN* msg )
{ 
   //std::vector<std::string>*  row = new std::vector<std::string>;
   
   // Make a copy on the heap : 
   struct sCAN* tmpC = new struct sCAN();
   memcpy( tmpC, msg, sizeof(struct sCAN) );
   m_msgs.push_back( tmpC );
   
   std::string 				 str;
   std::vector<std::string>  row;   

	// NOW CONVERT Data to Text, and add to the table : 
	// ID Column:
   char str2[50];
   sprintf( str2, "%2x", msg->id.group.id );
   row.push_back( str2 ); 

	// MSG ID NAME :
   strcpy( str2 , getID_Text(msg->id.group.id) );
   //strcpy( str2 , "unknown" );   
   row.push_back( str2 );

	// INSTANCE:   
   sprintf( str2, "%2x", msg->id.group.instance );
   row.push_back( str2 );
	// DLC
   sprintf( str2, "%d", msg->header.DLC );
   row.push_back( str2 );
   // DATA[0..8]
	str2[0] = 0;
	for (int i=0; i<msg->header.DLC; i++)
	{
		sprintf( str2, "%2x", msg->data[i] );
		row.push_back( str2 );
	} 
	add_row( &row ); 
}

int	CANMessageView::setup_headers()
{
	static struct HeaderItemInfo mNewHeading;	// = new struct HeaderItemInfo();
	//mNewHeading.width	  = -1;
	mNewHeading.text 		= "ID";
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;
	add_column( &mNewHeading );

	mNewHeading.text 		= "Name";
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;
	add_column( &mNewHeading );

	mNewHeading.text 		= "Instance";
	mNewHeading.alignment  = HEADER_ALIGN_CENTER;	
	add_column( &mNewHeading );

	mNewHeading.text 		= "DLC";
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;	
	add_column( &mNewHeading );

	mNewHeading.text 		= "[0]";		// for now all 8 bytes at once.
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;	
	add_column( &mNewHeading );

	mNewHeading.text 		= "[1]";		// for now all 8 bytes at once.
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;	
	add_column( &mNewHeading );

	mNewHeading.text 		= "[2]";		// for now all 8 bytes at once.
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;	
	add_column( &mNewHeading );

	mNewHeading.text 		= "[3]";		// for now all 8 bytes at once.
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;	
	add_column( &mNewHeading );

	mNewHeading.text 		= "[4]";		// for now all 8 bytes at once.
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;	
	add_column( &mNewHeading );

	mNewHeading.text 		= "[5]";		// for now all 8 bytes at once.
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;	
	add_column( &mNewHeading );

	mNewHeading.text 		= "[6]";		// for now all 8 bytes at once.
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;	
	add_column( &mNewHeading );

	mNewHeading.text 		= "[7]";		// for now all 8 bytes at once.
	mNewHeading.alignment  = HEADER_ALIGN_LEFT;	
	add_column( &mNewHeading );
}

int 	CANMessageView::order_by_time		() 
{
 
}
bool pred_name( vector<string> m1, vector<string> m2 )
{
	if (m1[1] < m2[1])
		return true;
	return false;
}
int 	CANMessageView::order_by_msg_id		() 
{
	// defined in Tabular Base  vector< vector<string> >	LineData;
	//std::sort( LineData.begin(), LineData.end(), pred_name());	
}
int   	CANMessageView::draw_entry 			( int mState	) 
{
 
}	
int   	CANMessageView::draw		 		(				) 
{
 	TabularListBox::draw();
}

int		CANMessageView::get_hit_index		( int Mousex, int Mousey ) 
{
 
}
int		CANMessageView::onClick( int x, int y, bool mouse_is_down ) 
{ 

}
