#ifndef _CONTROL_H_
#define _CONTROL_H_
  
#include <vector>
#include "../core/can/bk_system_defs.h"

#define DefaultPadding 15
class Window;

/* Abstract Base Class for all graphs */
class Control
{
	friend class Window;
public:
	Control( int Left,  int Right, int Top, int Bottom );
	Control( int Width, int Height 					   );
	Control();
	~Control();

	virtual void 	Initialize			  (					);	
	virtual void  	set_text_size		  ( float TextSize	);
	void  			set_text  			  ( const char* NewText	);
	void  			set_text_color 	  	  ( long int TextColor 	);
	void  			set_border_color      ( long mColor 	) { border_color= mColor; 	  }
	void  			set_background_color  ( long mColor 	) { background_color= mColor; }

	// set_position should call move_to() and set_width_height() both are virtual.  so this need not be!
	virtual void 	wrap_content	  	  ( 				);

	void		 	set_position	  	  ( int Left, int Right, int Top, int Bottom );
	virtual void 	set_width_height  	  ( int Width, int Height 		 )  { width = Width; height=Height; };
	virtual void  	move_to	  		  	  ( float Left,   float  Bottom	 );
	void  			get_xy	  		  	  ( float* mLeft, float* mBottom )  { *mLeft=left; *mBottom=bottom; };
	float			get_width			  (			)	{return width; 	 };
	float			get_height			  (			)	{return height;  };
	float 			get_left	  		  ( 		)	{ return left;   };
	float 			get_bottom	  		  ( 		)	{ return bottom; };
	inline bool		is_created			  ( 		)   { return created; };
	
	void 		 	move_bottom_to  	  ( float  mNewBottom	);
	void 		 	move_left_to  	 	  ( float  mNewLeft		);
	virtual void  	move_by	  		  	  ( int DeltaX, int  DeltaY	) { left+=DeltaX; bottom+=DeltaY; };

	void  			set_position_left_of  ( Control* Sibling, bool mCopyVert=true, float mPadding=DefaultPadding );
	void  			set_position_right_of ( Control* Sibling, bool mCopyVert=true, float mPadding=DefaultPadding );
	void 		 	set_position_above    ( Control* Sibling, bool mCopyHoriz=true, float mPadding=DefaultPadding );
	void  			set_position_below	  ( Control* Sibling, bool mCopyHoriz=true, float mPadding=DefaultPadding ); 
	void  			copy_position_horiz	  ( Control* Sibling );	
	void  			copy_position_vert	  ( Control* Sibling );

	void  			print_positions	  	  ( 				 );
	void  			print_color_info  	  ( 				 );

	void  			show				  ( ) 	{ Visible = TRUE;  };
	void  			hide				  ( ) 	{ Visible = FALSE; };
	void  			show_border 		  ( bool Show=true ) { HasBorder = Show; };
	
	virtual void 	load_resources		  (	);
	/*At some point, want to add a param: Canvas canvas to the draw functions.
		so that we can draw into any memoryDC or the screen.  Switching "on
		the fly".
	*/
	virtual int   	draw				(	);
	int   			draw_border			(   );

	void			Invalidate			( )			{ invalidated = true; };
	// Once registered, will be tested in the HitTest & OnClick()
	void			register_child		( Control* mNewChild );
	void			unregister_child	( Control* mNewChild );	

	/* Display manager calls HitTest on a window, then all it's child controls.
	if the mouse click is on this control (ie. if the function below returns true), 
	then the display manager will call OnClick() subsequently.  
	*/
			Control*	ChildrenHitTest( int x, int y 	);

	virtual Control*	HitTest		  ( int x, int y 	);
	virtual int			onHover		  (  			 	) { return 0; };
	virtual int			onClick		  ( int x, int y, bool mouse_is_down=true );
	virtual int			onDoubleClick ( 				);
	virtual int			onReceiveFocus( 				);
	int					isTabStop	  (					)	{ return 0; };
	virtual int			onCreate	  (  );	// chance to load resources, call functions which use fonts (already loaded before this call) etc.

protected:
	std::vector<Control*>	m_child_controls;
	
	bool		invalidated;		// forces a redraw in display manager.  not implemented yet.
	bool		created;
	
	float 		left;
	float 		bottom;
	float 		width;
	float 		height;

	float 		border_roundness_x;
	float 		border_roundness_y;
	float 		text_size;
	long int 	text_color;

	char*		text;
	bool		Visible;
	bool		HasBorder;
	long int 	background_color;
	long int 	border_color;
};
 
#endif
