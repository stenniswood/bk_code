/********************************************************
	Though this app is for all CAN traffic.

	Here are details for the Analog board specifically.
	Chip Layout:
		1	3
		4	2			
	The robot wiring harness has "teach pendant" potentiometers
	wired to the upper 4 bits of the chip.  
	Cable left is plugged into chip 1
	Cable right is plugged into chip 3
				
********************************************************/
#ifndef _CAN_APP2_
#define _CAN_APP2_

#include "adren_network_view.hpp"
#include "can_msg_view.hpp"
#include "gyro_window.h"
#include "analog_board_app.hpp"
#include "can_window.hpp"


/**** These function handle the menu commands:   ****/
void draw_zoom_in			();
void draw_zoom_out			();
/***** END OF MENU COMMAND FUNCTIONS *****/


class CANApp : public Application
{
public:
	CANApp ( );
	CANApp ( Rectangle* mRect );
	~CANApp( );

	virtual void 	Initialize		(	);	// create all the objects here.

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

	void 			add_watch		(	);	
	void 			line_graph		(	);	
	void 			histogram		(	);	
	void 			timing			(	);		

	void 			show_messages	( );	// CAN Messages view.
	void 			show_network	( );	// Adrenaline Network.
	void 			show_sequencer	( );	// Robot sequencer
	void 			show_gyro		( );	// CAN Messages view.
	void 			show_analogs	( );	// CAN Messages view.	

	void 			zoom_in			();
	void 			zoom_out		();

	int				m_rx_tail;
	int				m_rx_tail_laps;
	
	// List of all boards present
	NetworkView*			m_board_view;		// in ../core/adrenaline/molecules/adren_network_view.cpp
	CANMessageView*			m_msg_view;			// in ../core/adrenaline/atoms/
	CANMessages*			m_msgs;				// in ../core/adrenaline/molecules/can_window.cpp
	GyroOverview*			m_gyro;				// in apps/CAN/gyro_window.h
	AnalogView*				m_analog;			// in apps/CAN/analog_board_app.cpp
};

extern CANApp* can_app;

#endif


