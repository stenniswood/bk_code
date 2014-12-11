#ifndef _DISPLAY_MANAGER_H_
#define _DISPLAY_MANAGER_H_

#include "system_bar.hpp"
#include "control.hpp"
#include "icon.hpp"

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

	void  	set_title 			( char* Title 	  );
	void  	set_background		( char* mFileName );
	void  	set_background_color( long int Color  );

	void	init_screen		(					  );
	void	start_screen	(					  );
	void	end_screen		(					  );

	virtual int	onCreate	(  					  );	
	void	load_resources  (					  );

	float	get_width		(	)		{  return screen_width;	 };
	float 	get_height		(	)		{  return screen_height; };
	
	void	add_object			( Control* NewControl );
	void	remove_object		( Control* NewControl );
	void	remove_all_objects	( );

	//void 	onClick			( int x, int y, bool mouse_is_down=true );
	Control* HitTest		( int x, int y );

	void  start_draw		(	);
	int   draw				(	);	
	int   draw_children		( 	);
	int   draw_text			( 	);
	int   draw_background	( 	);
	void  end_draw			(	);

	void 	call_on_creates	( 	);  // maybe not needed

protected:
	int 	left; 
	int		right;
	int		top;
	int		bottom;
	
	long int BackgroundColor;				
	
	// Linked List of objects.
	SystemBar		sb;
	list<Control*>	controls;		// new way - not tested yet!
	Control* Head;
	Control* Tail;

	char*	text;
	char* 	title; 
	bool	ShowBox;

	int		screen_width;
	int 	screen_height;
};

extern DisplayManager MainDisplay;
 
#endif
