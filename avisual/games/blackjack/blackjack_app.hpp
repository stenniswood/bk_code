#ifndef _BLACKJACK_APP2_
#define _BLACKJACK_APP2_

/**** These function handle the menu commands:   ****/

/***** END OF MENU COMMAND FUNCTIONS *****/


void init_blackjack( );


class BlackJackApp : public Application
{
public:
	BlackJackApp ();
	BlackJackApp ( Rectangle* mRect );
	~BlackJackApp();

	virtual void 	Initialize		(	);	// create all the objects here.
	int				onPlace			(	);
	virtual int		onCreate	  	(	);	// First Time application is run - load resources etc.

	virtual void	setup_main_menu ( );
	virtual void	setup_app_menu  ( );
	virtual void 	register_with_display_manager();	
	
	int				About			(	);
	int				Preferences		(	);
	int				Quit			(	);	

	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );

};

extern BlackJackApp* bj_app;

#endif


