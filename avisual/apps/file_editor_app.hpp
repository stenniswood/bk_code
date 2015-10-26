#ifndef _FILE_EDITOR_APP_
#define _FILE_EDITOR_APP_


/**** These function handle the menu commands:   ****/

/***** END OF MENU COMMAND FUNCTIONS *****/

void init_file_editor_app ( );


class FileEditorApp : public Application
{
public:
	FileEditorApp ();
	FileEditorApp ( Rectangle* mRect );
	~FileEditorApp();

	virtual void 	Initialize		(	);	// create all the objects here.
	int				onPlace			(	);

	virtual void	setup_main_menu  	(  );
	virtual void	setup_app_menu  (  );
	virtual void 	register_with_display_manager();	
	
	void			About			(	);
	void			Preferences		(	);
	//int				Quit		(	);	

	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );

	void 			show_grid		( );

	void 			zoom_in			( );
	void 			zoom_out		( );


};

extern FileEditorApp* file_app;

#endif


