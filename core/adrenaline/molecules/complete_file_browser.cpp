// line graph OpenVG program
// Steve Tenniswood
#include <shapes.h>
#include <fontinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"
#include "dataset.hpp"
#include "adrenaline_windows.h"
#include "display_manager.hpp"
#include "visual_memory.h"
#include <vector>
#include "test_layouts.hpp"
#include "file_browser.hpp"

#define Debug 1


CompleteCompleteFileBrowser::CompleteFileBrowser()
: Control()
{
	Initialize( );
}
CompleteFileBrowser::CompleteFileBrowser( int Left, int Right,  int Top, int Bottom  )
: Control(  Left, Right, Top, Bottom  )
{
	Initialize( );
}
CompleteFileBrowser::CompleteFileBrowser( int Width, int Height )
: Control( Width, Height )
{
	Initialize( );
}
CompleteFileBrowser::~CompleteFileBrowser()
{
}

void CompleteFileBrowser::Initialize( )
{
	if (Debug) printf("\n\tCompleteFileBrowser::Initialize()::\n");
	//Control::Initialize();
	path_descriptor   = NULL;
	show_hidden_files = false;
	if (Debug) printf("\tCompleteFileBrowser::Initialize()::done!\n");
}

void CompleteFileBrowser::set_base_path( char* mBasePath )
{
	base_path = mBasePath;
}


int CompleteFileBrowser::onPlace	(  )	// chance to place children.
{
	m_favorites_pane.set_width_height( 200, height);	
	m_favorites_pane.move_to		 ( left, bottom );

	m_clippings_pane.set_width_height( 200, height );
	float clippings_left = left+width-m_clippings_pane.get_width();
	m_clippings_pane.move_to		 ( clippings_left, bottom);
	
	float b_width = clippings_left - m_favorites_pane.get_width() + left;
	m_browser_pane.move_to			( m_favorites_pane.get_width() + left, bottom);
	m_browser_pane.set_width_height	( b_width, height );
	
	return 1;	
}

int	CompleteFileBrowser::onCreate(  )
{
	register_child( &m_favorites_pane);
	register_child( &m_browser_pane	 );
	register_child( &m_clippings_pane);
	
	return Control::onCreate();
}


int CompleteFileBrowser::onClick( int Mousex, int Mousey, bool mouse_is_down )
{
	// Now click : 
	//selected_listbox->onClick(Mousex, Mousey);
	Invalidate();
}

// Need to draw a > marker on each list box's selected item.
