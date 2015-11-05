#ifndef _FINDER_APP2_
#define _FINDER_APP2_

#include "adren_network_view.hpp"
#include "can_msg_view.hpp"
#include "gyro_window.h"
#include "analog_board_app.hpp"
#include "can_window.hpp"


/**** These function handle the menu commands:   ****/
void draw_zoom_in			();
void draw_zoom_out			();
/***** END OF MENU COMMAND FUNCTIONS *****/


class FinderApp : public Application
{
public:
	FinderApp ( );
	FinderApp ( Rectangle* mRect );
	~FinderApp( );

	virtual void 	Initialize		(	);	// create all the objects here.
	int				onPlace			(	);

	virtual void	setup_sidebar	(	);  // derived class adds these here
	virtual void	setup_main_menu ( );
	virtual void	setup_app_menu  ( );
	virtual void 	register_with_display_manager();	
	
	void				About			(	);
	void				Preferences		(	);
	//int			Quit			(	);	
	virtual int		background_time_slice(	);
	
	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );

	void 			pie_graph		( );	
	void 			histogram		( );	
	void 			timing			( );	

	void 			show_details	( );	// CAN Messages view.
	void 			show_files		( );	// Adrenaline Network.
	void 			show_icon		( );	// Robot sequencer

	void 			zoom_in			();
	void 			zoom_out		();

	int				m_rx_tail;
	int				m_rx_tail_laps;
	
	CompleteFileBrowser* m_finder_view;		// List of all boards present
};

extern CANApp* can_app;

#endif


