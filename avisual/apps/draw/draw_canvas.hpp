#ifndef _DRAWING_APP_
#define _DRAWING_APP_

#include "window.hpp"
#include "adren_network_view.hpp"
#include "can_msg_view.hpp"


extern HorizontalMenu draw_menu;

void init_main_draw_menu();

#define MENU_STATE_NORMAL 		1
#define MENU_STATE_GRAYED 		2
#define MENU_STATE_SELECTED 	3
#define MENU_STATE_CIRCLED 		4
#define MENU_STATE_EMPHASIZED 	5

struct stPathPoint
{
	VGfloat x;
	VGfloat y;
};

struct stPath
{
	int					finger_index;		// only used while capturing!
	vector<VGubyte> 	commands;
	vector<stPathPoint>	path_points;	
};

class Drawing2D : public Window
{
public:
	Drawing2D ();
	Drawing2D ( int Width, int Height  );
	~Drawing2D();

	virtual void 	Initialize(	);
	int				calc_metrics();
	int				place_views();
	 	
	void			file_new();
	 
	void			accept_poly_line(  );
	virtual int		onCreate	    (  );	// chance to load resources, call functions which use fonts
	
	int				create_cmd_list 	( struct stPath mlines );
	int				get_hit_index		( int Mousex, int Mousey );
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	int   			draw_poly_line		( int mIndex	);
	virtual int   	draw		 		(				);
	
	bool			append_new_captured	( );
	virtual Control*	HitTest		  ( int x, int y 	);
	Control*		Capture 			( int x, int y, int finger 	);
	
protected:

	vector<struct stPath>	m_points_in_making;			// this creates the paths for each finger pressed.
	// Only added to m_lines after all fingers are released.
	// If a finger is released and pressed again while others fingers are still held, 
	// then we create a new entry into the above.  So the above lines could be more than the 10 fingers.
	
	
	vector<struct stPath>	m_lines;	
	
	Button*					m_new_poly_line;
	Button*					m_color_chooser;	
	SpinnerControl*			m_stroke_width;
			
	int						m_width;
	bool					has_scroller;
};

 
#endif
