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
#include "file_browser.hpp"
#include "complete_file_browser.hpp"
#include "test_layouts.hpp"


 
#define Debug 1


FavoritesPane::FavoritesPane()
{

}

int FavoritesPane::onCreate	(  )
{
	printf("FavoritesPane::onCreate	(  )\n");	
	set_odd_color( 0xFF00207F );
	set_even_color( 0xFF00207F );
	add_item("home");				m_paths.push_back("/home/pi/");
	add_item("root");				m_paths.push_back("/");
	add_item("Applications");		m_paths.push_back("/home/pi/avisual/applications/");
	add_item("Documents");			m_paths.push_back("/home/pi/avisual/documents/");
	add_item("Pictures");			m_paths.push_back("/home/pi/avisual/pictures/");
	add_item("Movies");				m_paths.push_back("/home/pi/avisual/movies/");
	add_item("Music");				m_paths.push_back("/home/pi/avisual/music/");
	add_item("Sounds");				m_paths.push_back("/home/pi/avisual/sounds/");
	add_item("Downloads");			m_paths.push_back("/home/pi/avisual/downloads/");
	add_item("Recently Accessed");	m_paths.push_back("/home/pi/avisual/");
	add_item("Trash");				m_paths.push_back("/home/pi/avisual/trash/");
	return ListBox::onCreate();
}

/*************************************************************/
ClippingsPane::ClippingsPane()
{

}
int	ClippingsPane::onCreate(  )
{
	printf("ClippingsPane::onCreate	(  )\n");
	//set_odd_color ( 0xFF7F2020 );
	//set_even_color( 0xFF7F2020 );
	//clear_items();
	add_item("junk");
	return ListBox::onCreate();
}
/*************************************************************/

/*************************************************************/
CompleteFileBrowser::CompleteFileBrowser()
{
	Initialize( );
}
CompleteFileBrowser::CompleteFileBrowser( int Left, int Right,  int Top, int Bottom  )
: Window(  Left, Right, Top, Bottom  )
{
	Initialize( );
}
CompleteFileBrowser::CompleteFileBrowser( int Width, int Height )
: Window( Width, Height )
{
	Initialize( );
} 
CompleteFileBrowser::~CompleteFileBrowser()
{

}
void CompleteFileBrowser::Initialize( )
{
	if (Debug) printf("\n\tCompleteFileBrowser::Initialize()::\n");
	m_clippings_pane = new ClippingsPane();
	m_clippings_pane->set_width_height( 100, 50 );
	if (Debug) printf("\tCompleteFileBrowser::Initialize()::done!\n");
}

void CompleteFileBrowser::set_base_path( char* mBasePath )
{
	//if (Debug)  printf("Set base_path( %s )\n", mBasePath );	
	m_browser_pane.set_base_path( mBasePath );	
}

int CompleteFileBrowser::onPlace	(  )	// chance to place children.
{
	printf("CompleteFileBrowser::onPlace(  )\n");
	m_favorites_pane.set_width_height( 120, height );	
	m_favorites_pane.move_to		 ( left, bottom );
	
	printf("  m_clippings_pane (  )\n");
	//m_clippings_pane.set_width_height( 100, 50 );
	printf("  m_clippings_pane::set_width_height (  )\n");	
	float clippings_left = left+width - m_clippings_pane->get_width();
	//m_clippings_pane->move_to		 ( clippings_left, bottom );
	
	printf("  m_browser_pane ( )\n");	
	float b_width = clippings_left - m_favorites_pane.get_width() + left;
	m_browser_pane.set_width_height	 ( b_width, height );
	m_browser_pane.move_to			 ( m_favorites_pane.get_width()+left, bottom);	
	printf("****** CompleteFileBrowser::onPlace\n"); m_browser_pane.print_positions();	
	return 1;
}

int	CompleteFileBrowser::onCreate(  )
{
	printf("CompleteFileBrowser::onCreate(  )\n");
	register_child( &m_favorites_pane);
	register_child( &m_browser_pane	 );
	//register_child( m_clippings_pane);
	
	onPlace();
	
	//m_clippings_pane.clear_items();
	//printf("CompleteFileBrowser::onCreate(  )\n");		
	return Control::onCreate();
}


int CompleteFileBrowser::onClick( int Mousex, int Mousey, bool mouse_is_down )
{
	int retval = m_favorites_pane.onClick( Mousex, Mousey, mouse_is_down );	
	if (retval)
	{
		int selection = m_favorites_pane.get_selection( );
		//printf("CompleteFileBrowser::onClick() %d\n", selection ); 
		if (selection>=0) {
			m_browser_pane.collapse_to_level(0);
			set_base_path( m_favorites_pane.m_paths[selection].c_str() );
			return 1;
		}
	}	
	else
		retval = Window::onClick(Mousex, Mousey);

	return retval;
}

// Need to draw a > marker on each list box's selected item.
