#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "../core/can/bk_system_defs.h"
#define DefaultPadding 15

/* Abstract Base Class for all graphs */
class Control
{
//	friend class DisplayManager;
public:
	Control( int Left, int Right, int Top, int Bottom );
	Control();

	void  set_title 		  ( char* NewTitle );
	void  set_text  		  ( char* NewText, float TextSize=-1.0 );
	void  set_text_size		  ( float TextSize = 18.0 			   );
	void  set_text_color	  ( long int TextColor = 0xFFFFFFFF	   );
	
	void  print_positions	  ( ) {
		printf("Left=%5.1f; Bottom=%6.1f; width=%6.1f;  height=%6.1f;  \n", left, bottom, width, height );
	};
	virtual void  set_position		  ( int Left, int Right, int Top, int Bottom );
	void  move_to	  		  ( int Left, int Right 					 );
	void  setPosition_left_of ( Control* Sibling, int width, int Top=-1, int Bottom=-1);
	void  setPosition_right_of( Control* Sibling, int width, int Top=-1, int Bottom=-1);

	void  set_background_color  ( long mColor ) { BackgroundColor= mColor; }
	void  show				  ( ) 			  { Visible = TRUE;  };
	void  hide				  ( ) 			  { Visible = FALSE; };

	virtual int   	draw		(	);
	int   			draw_text	( 	);
	void  			draw_box	(   );
	int   			draw_border	(   );
	void  			show_border ( bool Show  ) { HasBorder = Show; };
//	virtual int	  draw_body		(	)=0;	// Must implement in derived class! Contains the graph body.

	// Datasets may be linked:
	void		setNext(Control* next)	{ Next = next; };
	void		setPrev(Control* prev)	{ Prev = prev; };
	Control*	getNext()				{ return Next; };
	Control*	getPrev()				{ return Prev; };

	/* Display manager calls HitTest on a window, then all it's child controls.
	if the mouse click is on this control (ie. if the function below returns true), 
	then the display manager will call OnClick() subsequently.  
	*/
	int				HitTest(int x, int y);
	virtual int		onHover() {return 0;};
	virtual int		onClick() ;
	virtual int		onDoubleClick();
	virtual int		onReceiveFocus();
	int				isTabStop()			{ return 0; };

protected:
	float 	left;
	float 	bottom;
	float 	width;
	float 	height;	

	float 	border_roundness_x;
	float 	border_roundness_y;	
	float 	text_size;
	long int text_color;
	
	char*	text;
	char* 	title;
	bool	Visible;
	bool	HasBorder;
	long int BackgroundColor;

	// Display manager will link objects together.
	Control* Next;
	Control* Prev;	
};

 
#endif
