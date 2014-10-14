#ifndef _DISPLAY_MANAGER_H_
#define _DISPLAY_MANAGER_H_

#include "control.hpp"






/* Packages Objects for display */
class DisplayManager
{
	friend class Control;
	
public:
	DisplayManager( int Left, int Right, int Top, int Bottom );
	DisplayManager( int mScreenWidth, int mScreenHeight 	 );

	void  set_title 			( char* Title 	  );
	void  set_background		( char* mFileName );
	void  set_background_color	( long int Color  );
		
//	void  setPosition_left_of (Control* mSibling, int width, int Top=-1, int Bottom=-1);
//	void  setPosition_right_of(Control* mSibling, int width, int Top=-1, int Bottom=-1);
	
	void	init_screen		(					  );
	void	start_screen	(					  );
	void	end_screen		(					  );	
	

	void  add_object		( Control* NewControl );
	void  remove_object		( Control* NewControl );
	void  remove_all_objects( );

	void  onClick			( int x, int y );
	Control* HitTest		( int x, int y );

	void  start_draw		(	);
	int   draw				(	);	
	int   draw_children		( 	);
	int   draw_text			( 	);
	int   draw_background	( 	);
	void  end_draw			(	);
	
protected:
	int 	left; 
	int		right;
	int		top;
	int		bottom;
	
	long int BackgroundColor;				// 
	char*	 BackgroundImageFileName;		// 
	
	// Linked List of objects.
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
