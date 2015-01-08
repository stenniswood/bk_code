#ifndef _CAN_APP2_
#define _CAN_APP2_


#include "adren_network_view.hpp"
#include "can_msg_view.hpp"


/**** These function handle the menu commands:   ****/
void draw_zoom_in			();
void draw_zoom_out			();
/***** END OF MENU COMMAND FUNCTIONS *****/

void configure_wave_views	( int mChannels, short* mDataCh1, short* mDataCh2 );

class CANApp : public Application
{
public:
	CANApp ();
	CANApp ( Rectangle* mRect );
	~CANApp();

	virtual void 	Initialize		(	);	// create all the objects here.
	int				onPlace			(	);

	virtual void	setup_sidebar	(	);  // derived class adds these here
	virtual void	setup_menu  	( );
	virtual void	setup_app_menu  ( );
	virtual void 	register_with_display_manager();	
	
	int				About			(	);
	int				Preferences		(	);
	//int				Quit			(	);	

	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );

	void 			add_watch		(	);	
	void 			line_graph		(	);	
	void 			histogram		(	);	
	void 			timing			(	);		

	void 			show_messages	( );	// CAN Messages view.
	void 			show_network	( );	// Adrenaline Network.
	void 			show_sequencer	( );	// Robot sequencer

	void 			zoom_in			();
	void 			zoom_out		();

	
	NetworkView*			m_board_view;		// List of all boards present
	CANMessageView*			m_msg_view;
//	CANMessageView			m_msg_view;	
};

extern CANApp* can_app;

#endif


