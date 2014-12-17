#ifndef _DISPLAY_MANAGER_H_
#define _DISPLAY_MANAGER_H_

#include "system_bar.hpp"
#include "control.hpp"
#include "icon.hpp"

#include "rectangle.hpp"
#include "system_status.hpp"
#include "side_bar.hpp"
#include "task_bar.hpp"

#include <list>
#include <string>
using namespace std;


/* Packages Objects for display */
class DisplayManager : public IconView
{
	friend class Control;
	
public:
	DisplayManager( int Left, int Right, int Top, int Bottom );
	DisplayManager( int mScreenWidth, int mScreenHeight );

	virtual void	Initialize();
	
	void  	set_background		( char* mFileName );

	void	init_screen		(					  );
	void	start_screen	(					  );
	void	end_screen		(					  );

	virtual int	onCreate	(  					  );	
	void	load_resources  (					  );

	float	get_width			(	)		{  return screen_width;	 };
	float 	get_height			(	)		{  return screen_height; };
	Rectangle*	get_useable_rect( );	// max client coordinates to avoid the sidebars, status and system bars.
	
	void	add_object			( Control* NewControl );
	void	remove_object		( Control* NewControl );
	void	remove_all_objects	( );

	//void 	onClick			( int x, int y, bool mouse_is_down=true );
	Control* HitTest		( int x, int y );

	void  start_draw		(	);
	int   draw				(	);	
	int   draw_children		( 	);
	int   draw_background	( 	);
	void  end_draw			(	);

	void 	call_on_creates	( 	);  // maybe not needed

	// Linked List of objects.
	SystemBar		m_sb;			// Top
	TaskBar			m_task_bar;		// Left side
	SideBar			m_soft_side;	// Right side
	SystemStatusBar	m_status;		// Bottom

protected:
	//list<Control*>	controls;		// new way - not tested yet!

	int		screen_width;
	int 	screen_height;
};

extern DisplayManager MainDisplay;
 
#endif
