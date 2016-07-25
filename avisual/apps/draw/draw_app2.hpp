#ifndef _DRAW_APP2_LAYOUTS_
#define _DRAW_APP2_LAYOUTS_


/**** These function handle the menu commands:   ****/
void draw_zoom_in			();
void draw_zoom_out			();


/***** END OF MENU COMMAND FUNCTIONS *****/

void init_drawing_app( );


class DrawApp : public Application
{
public:
	DrawApp ();
	DrawApp ( Rectangle* mRect );
	virtual ~DrawApp();

	virtual void 	Initialize		(	);	// create all the objects here.

	virtual void	setup_main_menu	(	);  // The horizontal menu. derived class adds these here
	virtual void	setup_app_menu  ( );
	
	void				About			(	);
	void				Preferences		(	);
	//int				Quit			(	);	

	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );

	void 			show_grid		( );

	void 			zoom_in			();
	void 			zoom_out		();


};

extern DrawApp* draw_app;

#endif


