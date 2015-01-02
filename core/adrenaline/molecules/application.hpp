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
class Application : public Control
{
public:
	Application ();
	Application ( Rectangle* mRect );
	Application ( int Width, int Height );
	~Application();

	virtual void 	Initialize		(	);
	int				calc_metrics	(	);
	int				onPlace			(	);

	// chance to load resources, call functions which use fonts
	virtual int		onCreate	  	(	);	// First Time application is run - load resources etc.
	int				onActivate		(	);	// Future reactivation - set menus, sidebars, status, etc.

	// Standard Menu Handlers:	
	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );
			void	file_quit		( );
			
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	virtual int   	draw		 		(	);
	
protected:
	HorizontalMenu			m_hMenu;	
	vector<Control*>		m_sidebar_controls;	
	string					m_welcome_status;
	Control*				m_main_window;
	
};

 
#endif
