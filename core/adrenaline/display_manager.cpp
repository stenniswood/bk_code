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
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "../core/can/bk_system_defs.h"
#include "display_manager.hpp"


#define old_way 1

// Offer one instance for whole app;
DisplayManager MainDisplay(1920, 1080);
#define Debug 1

DisplayManager::DisplayManager(int Left, int Right, int Top, int Bottom )
: IconView(  Left,  Right,  Top,  Bottom, NULL)
{
	background_color = 0x800000B0;

	screen_width   = Right-Left;
	screen_height  = Bottom-Top;
	init_screen();
}

DisplayManager::DisplayManager( int mScreenWidth, int mScreenHeight )
: IconView( 0,  mScreenWidth,  mScreenHeight,  0, NULL )
{
	background_color = 0x800000B0;

	screen_width   = mScreenWidth;
	screen_height  = mScreenHeight;
	init_screen();
}

// Graphics initialization
void  DisplayManager::init_screen()
{
	printf("DisplayManager::\tscreen_width=%d;\tscreen_height=%d\n", screen_width, screen_height );
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

void DisplayManager::load_resources( )
{
	if (Filename)
	{
		read_from_jpeg_file( );
	}
	printf("Loading child resources\n");

#ifdef old_way
		int count = 0;
		Control* obj = Head;
		while (obj)
		{
			if (Debug) printf("load child resources %d  \n", count++);
			obj->load_resources();
			obj = obj->getNext();
		}
#else
		list<Control*>::iterator	iter = controls.begin();
		for (int i=0; iter!=controls.end(); i++ )
		{
			(*iter)->load_resources();
		}
#endif
}

void  DisplayManager::set_title( char* Title )
{
}
//	A more preferred way of doing the background:
//void  DisplayManager::set_background( void* Image 	  )
void  DisplayManager::set_background( char* mFileName )
{
	Filename = mFileName;
}
// "/home/pi/openvg/client/desert0.jpg";

void  DisplayManager::set_background_color( long int Color )
{
	background_color = Color;
}

void  DisplayManager::add_object( Control* NewControl )
{
#ifdef old_way
	// Add to list
	NewControl->setPrev( Tail );	// attach to end of list
	NewControl->setNext( NULL );

	if (Head == NULL) {
		Head = NewControl;
	}
	if (Tail != NULL)
		Tail->setNext( NewControl );		// this one is now end

	Tail = NewControl;						// this one is now end
#else
	controls.push_back( NewControl );	
#endif

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
	controls.clear();
	
}

void  DisplayManager::start_draw(	)
{
	start_screen();
}

int   DisplayManager::draw(	)
{
	if (Debug) printf("\ndraw display manager\tstart:\n" );
	start_draw();
	draw_background();
	draw_children();	
	end_draw();	
		if (Debug) printf("draw display manager\tdone!\n\n" );
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
	Control* result = NULL;
	Control* obj = Head;
	while (obj)
	{
		result = obj->HitTest(x,y);
		if (result)
			return obj;
		obj = obj->getNext();
	}
	return NULL;
}

int   DisplayManager::draw_children( )
{
#ifdef old_way

	int count = 0;
	Control* obj = Head;
	while (obj)
	{
		if (Debug) printf("draw child %d\n", count++);
		obj->draw();
		obj = obj->getNext();
	}
	return -1;
	
#else
	list<Control*>::iterator	iter = controls.begin();
	for (int i=0; iter!=controls.end(); i++ )
	{
		(*iter)->draw();
	}
#endif
	
}

int   DisplayManager::draw_text( 	)
{
	return -1;
}

int   DisplayManager::draw_background( 	)
{
	if (file_loaded)
		IconView::draw();	
	else if(1) {
		// TEST PATTERN:
		// [intensity, Red, Green, Blue, Alpha]
		VGfloat stops[] = {
		0.0, 0.0, 0.0, 1.0, 1.0,
		0.5, 0.0, 0.0, 0.5, 1.0,
		1.0, 0.0, 0.0, 0.0, 1.0
		};	
		float x1 = 0.0;				float y1 = 0.0;
		float x2 = screen_width;	float y2 = screen_height+1;
		FillLinearGradient(x1, y2, x1, y1, stops, 3);
		Rect(x1, y1, x2-x1, y2-y1);
	} else {
		Background( //(background_color & 0xFF000000)>>24,
				(background_color & 0x00FF0000)>>16,
				(background_color & 0x0000FF00)>>8,
				(background_color & 0x000000FF) );
	}
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
