/***********************************************************************
* DisplayManager
*
* 
* Steve Tenniswood
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "Graphbase.hpp"
#include "../core/can/bk_system_defs.h"
#include "display_manager.hpp"


// Offer one instance for whole app;
DisplayManager MainDisplay(1280, 760);
#define Debug 1

DisplayManager::DisplayManager(int Left, int Right, int Top, int Bottom )
{
	//BackgroundColor = 0xFFFFFFFF;
	BackgroundImageFileName = NULL;
	BackgroundColor = 0x20202020;
	BackgroundColor = 0x800000B0;

	screen_width   = Right-Left;
	screen_height  = Bottom-Top;
	init_screen();
}

DisplayManager::DisplayManager( int mScreenWidth, int mScreenHeight )
{
	BackgroundColor = 0x800000B0;
	BackgroundImageFileName = NULL;

	screen_width   = mScreenWidth;
	screen_height  = mScreenHeight;
	init_screen();
}

// Graphics initialization
void  DisplayManager::init_screen()
{
	init(&screen_width, &screen_height);	// Graphics initialization
}
// Screen initialization
void  DisplayManager::start_screen()
{
	Start(screen_width, screen_height);		// Start the picture
}
void  DisplayManager::end_screen( )
{
	End();
}

void  DisplayManager::set_title( char* Title )
{
}
//	A more preferred way of doing the background:
//void  DisplayManager::set_background( void* Image 	 )
void  DisplayManager::set_background( char* mFileName )
{
	BackgroundImageFileName = mFileName;	
}
// "/home/pi/openvg/client/desert0.jpg";

void  DisplayManager::set_background_color( long int Color )
{
	BackgroundColor = Color;
}

void  DisplayManager::add_object( Control* NewControl )
{
	// Add to list
	NewControl->setPrev( Tail );	// attach to end of list
	NewControl->setNext( NULL );

	if (Head == NULL) {
		Head = NewControl;
	}
	if (Tail != NULL)
		Tail->setNext( NewControl );		// this one is now end

	Tail = NewControl;					// this one is now end	
}


// This removes the object from the list.  Does not delete or free memory.
// because some may be static.  Leave that to the caller.
void  DisplayManager::remove_object( Control* NewControl )
{
	Control* prev = NewControl->getPrev();
	Control* next = NewControl->getNext();
	
	if (prev==NULL)
		// head of list
		Head = next;
	else
		prev->setNext( next );
		
	if (next==NULL)
		// end of list
		Tail = prev;
	else
		next->setPrev( prev );
}
void  DisplayManager::remove_all_objects(  )
{
	// not going to actually delete/free the objects since some are static!
	Head = NULL;
	Tail = NULL;
}

void  DisplayManager::start_draw(	)
{
	start_screen();
}

int   DisplayManager::draw(	)
{
	if (Debug) printf("draw display manager  \n" );
	start_draw();
	Background( //(BackgroundColor & 0xFF000000)>>24,
				(BackgroundColor & 0x00FF0000)>>16,
				(BackgroundColor & 0x0000FF00)>>8,
				(BackgroundColor & 0x000000FF) );
	draw_background();

	draw_children();
	end_draw();	
		if (Debug) printf("draw display manager  done!\n" );
}

void  DisplayManager::end_draw(	)
{
	end_screen();	   // End the picture
}


void  DisplayManager::onClick( int x, int y )
{
	Control* obj = HitTest(x,y);
	obj->onClick();
}

Control* DisplayManager::HitTest( int x, int y )
{
	int result = -1;
	Control* obj = Head;
	while (obj)
	{
		result = obj->HitTest(x,y);
		if (result>0)
			return obj;
		obj = obj->getNext();
	}
	return NULL;
}

int   DisplayManager::draw_children( )
{
	int count = 0;
	Control* obj = Head;
	while (obj)
	{
			if (Debug) printf("draw child %d  \n", count++);
		obj->draw();
		obj = obj->getNext();
	}
	return -1;

}
int   DisplayManager::draw_text( 	)
{
	return -1;
}

int   DisplayManager::draw_background( 	)
{
	if (BackgroundImageFileName==NULL)
		return 0;

	Image(0, 0, screen_width, screen_height, BackgroundImageFileName);
	//Image(200, 200, imgw, imgh, "/home/pi/openvg/client/desert0.jpg");
	return 1;
}


/*Control*  DisplayManager::Find_object( Control* NewControl )
{
	Control* ctrl = Head;
	while (ctrl)
	{
		if (ctrl->id==NewControl->id)
			return ctrl;
		ctrl = ctrl->getNext();
	}
	return NULL;
}*/
