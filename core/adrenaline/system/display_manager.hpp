#ifndef _DISPLAY_MANAGER_H_ 
#define _DISPLAY_MANAGER_H_

//#include "adrenaline_windows.h"
#include "bk_system_defs.h"
#include "control.hpp"
#include "system_bar.hpp"
#include "side_bar.hpp"
#include "system_status.hpp"
#include "icon.hpp"

#include "rectangle.hpp"
#include "task_bar.hpp"
#include "application.hpp"
#include "keyboard.hpp"
//using namespace std;


/* Packages Objects for display */
class DisplayManager : public IconView
{
	//friend class Control;
	
public:
	DisplayManager( int Left, int Right, int Top, int Bottom );
	DisplayManager( int mScreenWidth, int mScreenHeight );

	virtual void	Initialize();
	
	void  	set_background		( char* mFileName );

	void	init_screen			(					  );
	void	start_screen		(					  );
	void	end_screen			(					  );

	int 	any_invalid_children();
	int		update_invalidated	(  					  );
	int		onPlace				(  					  );
	virtual int	onCreate		(  					  );	
	void	load_resources  	(					  );
	void	set_menu	    	( HorizontalMenu* mHMenu = NULL );
	void	start_app			( Application* mApp	  );
	void	set_main_window		( Control* mNewWindow );
	
	void	close_app			( Application* mApp	  );
	void	idle_tasks			(   );

	float		get_aspect_ratio (  );		// width / height  ratio 1080 = 16/9	
	float		get_width		 (	)		{  return screen_width;	 };
	float 		get_height		 (	)		{  return screen_height; };
	Rectangle*	get_useable_rect ( );	// max client coordinates to avoid the sidebars, status and system bars.
	Rectangle*	get_centered_rect( float width, float height );	// centers the window within the useable area.
	void		center_window    ( Window* mWin				 );	
	
	void		add_object			( Control* NewControl );
	void		remove_object		( Control* NewControl );
	void		remove_all_objects	( 					  );

	//void 	onClick			( int x, int y, bool mouse_is_down=true );
	Control* HitTest		( int x, int y );

	//void  start_draw		(	);
	int	  draw_invalid_children( );
	int   draw				(	);	
	int   draw_children		( 	);
	int   draw_background	( 	);
	void  end_draw			(	);

	void 	call_on_creates	( 	);  // maybe not needed

	void	show_keyboard	(   );
	void	hide_keyboard	(   );
	
	void	show_calendar	(   );
	void	hide_calendar	(   );
	
	bool	relay_mouse		(   );
	bool	relay_key		( char mKey );
	void	grab_focus		( Control* mKey );
		
	std::vector<Application*>	m_running_apps;
	unsigned int				m_current_running_app;
	
	// Linked List of objects.
	SystemBar		m_sb;			// Top
	TaskBar			m_task_bar;		// Left side
	SideBar			m_side;			// Right side
	SystemStatusBar	m_status;		// Bottom

	Control*		m_focus;		// for directing keyboard input.
	
	long int	m_z_order_counter;	// highest Z_order
	int		screen_width;
	int 	screen_height;
	
	bool	mouse_capturing;
	Control* mouse_capture_control;
	bool	 openvg_initialized;
};

extern DisplayManager MainDisplay;
 
#endif
