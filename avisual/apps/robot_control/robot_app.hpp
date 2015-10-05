#ifndef _ROBOT_APP_
#define _ROBOT_APP_

/**** These function handle the menu commands:   ****/
void draw_zoom_in			();
void draw_zoom_out			();

/***** END OF MENU COMMAND FUNCTIONS *****/
void configure_wave_views	( int mChannels, short* mDataCh1, short* mDataCh2 );


void init_robot_app();


class RobotApp : public Application
{
public:
	RobotApp ();
	RobotApp ( Rectangle* mRect );
	~RobotApp();

	virtual void 	Initialize		(	);	// create all the objects here.
	int				onPlace			(	);

	virtual void	setup_main_menu ( );
	virtual void 	register_with_display_manager();	

	int				About			(	);
	int				Preferences		(	);
	int				Quit			(	);	

	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );

	void 			show_grid		( );
	
	void 			zoom_in			( );
	void 			zoom_out		( );
	
};

extern DrawApp* draw_app;

#endif
