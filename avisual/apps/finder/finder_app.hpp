#ifndef _FINDER_APP2_
#define _FINDER_APP2_

#include "adren_network_view.hpp"
#include "complete_file_browser.hpp"


void init_finder_app( );

/**** These function handle the menu commands:   ****/
void draw_zoom_in			();
void draw_zoom_out			();
/***** END OF MENU COMMAND FUNCTIONS *****/


class FinderApp : public Application
{
public:
	FinderApp ( );
	FinderApp ( Rectangle* mRect );
	virtual ~FinderApp( );

	virtual void 	Initialize		(	);	// create all the objects here.
	virtual void	setup_sidebar	(	);  // derived class adds these here
	virtual void	setup_main_menu ( );
	virtual void	setup_app_menu  ( );
	
	void			About			(	);
	void			Preferences		(	);
	//int			Quit			(	);	
	virtual int		background_time_slice(	);

	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );
	
	void			edit_cut		( );
	void			edit_copy		( );
	void			edit_paste		( );

	void 			show_preview	( );
	void 			show_toolbar	( );
	void 			show_history	( );
	
	CompleteFileBrowser* m_finder_view;		// List of all boards present
};

extern CANApp* can_app;

#endif


//	void 			pie_graph		( );	
//	void 			histogram		( );	
//	void 			timing			( );	
