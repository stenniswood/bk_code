#ifndef _DRAWING_APP_
#define _DRAWING_APP_

#include "control.hpp"
#include "adren_network_view.hpp"
#include "can_msg_view.hpp"


extern HorizontalMenu draw_menu;

void init_drawing_app();
void init_main_draw_menu();


#define MENU_STATE_NORMAL 		1
#define MENU_STATE_GRAYED 		2
#define MENU_STATE_SELECTED 	3
#define MENU_STATE_CIRCLED 		4
#define MENU_STATE_EMPHASIZED 	5


class Drawing2D : public Control
{
public:
	Drawing2D ();
	Drawing2D ( Rectangle* mRect );
	Drawing2D ( int Width, int Height 					 );
	~Drawing2D();

	virtual void 	Initialize(	);
	int				calc_metrics();
	int				place_views();
	 	
	void			file_new();
	 
	void			accept_poly_line();
	virtual int		onCreate	  (  );	// chance to load resources, call functions which use fonts
	
	int				get_hit_index		( int Mousex, int Mousey );
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	int   			draw_poly_line		( int mIndex	);
	virtual int   	draw		 		(				);
	
protected:
	vector<float>			m_points;	
	vector<vector<float> >	m_all_lines;	
	
	Button*					m_new_poly_line;
	Button*					m_color_chooser;	
	SpinnerControl*			m_stroke_width;
			
	int						m_width;
	bool					has_scroller;
};

 
#endif
