#ifndef _APPLICATION_
#define _APPLICATION_

#include "control.hpp"
#include "horizontal_menu.hpp"
#include "rectangle.hpp"


extern HorizontalMenu draw_menu;


#define MENU_STATE_NORMAL 		1
#define MENU_STATE_GRAYED 		2
#define MENU_STATE_SELECTED 	3
#define MENU_STATE_CIRCLED 		4
#define MENU_STATE_EMPHASIZED 	5

/* All the Display Manager should have to do is new an instance of the derived Application,
	add it to the list of controls, call it's onCreate().
	
	This onCreate then sets the menus, sidebars, and status, etc.
*/
class Application 
{
public:
	Application ();
	Application ( Rectangle* mRect );
	~Application();

	virtual void 	Initialize		(	);	// create all the objects here.
	int				calc_metrics	(	);
	int				onPlace			(	);

	virtual void	setup_sidebar	(	);  // derived class adds these here
	virtual void	setup_menu		(	);  // derived class adds these here
	virtual void	setup_app_menu	(	);  // Name of App, About, Preferences, Quit.
	
	virtual void 	register_with_display_manager();

	// chance to load resources, call functions which use fonts
	virtual int		onCreate	  	(	);	// First Time application is run - load resources etc.
											// send them to the display_manager here!
	int				onActivate		(	);	// Future reactivation - set menus, sidebars, status, etc.

	// Standard Menu Handlers:	
	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );
			void	file_quit		( );
			
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	virtual int   	draw		 	(	);
	
protected:
	HorizontalMenu			m_hMenu;	
	vector<Control*>		m_sidebar_controls;	
	string					m_welcome_status;
	Control*				m_main_window;
	
};

 
#endif
