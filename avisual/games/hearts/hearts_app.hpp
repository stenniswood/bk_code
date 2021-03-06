#ifndef _HEARTS_APP_
#define _HEARTS_APP_

/**** These function handle the menu commands:   ****/

/***** END OF MENU COMMAND FUNCTIONS *****/


void init_hearts( );


class HeartsApp : public Application
{
public:
	HeartsApp ( );
	HeartsApp ( Rectangle* mRect );
	virtual ~HeartsApp( );

	virtual void 	Initialize		(	);	// create all the objects here.
	virtual int		onCreate	  	(	);	// First Time application is run - load resources etc.

	virtual void	setup_main_menu ( );
	virtual void	setup_app_menu  ( );
	
	void			About			(	);
	void			Preferences		(	);
	int				Quit			(	);	

	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );
};

extern HeartsApp* hearts_app;

#endif


