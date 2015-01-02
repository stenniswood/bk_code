#ifndef _SYSTEM_BAR_
#define _SYSTEM_BAR_



#include "horizontal_menu.hpp"



class SystemBar : public Control 
{
public:
	SystemBar ( );
	SystemBar ( int mScreenWidth, int mScreenHeight );
	~SystemBar( );
	
	virtual void 	Initialize	(	);
	void			onPlace		(	);

	void 				set_menu		 ( HorizontalMenu* mMenu 		 );
	virtual void 		set_width_height ( int Width, int Height 		 );
	virtual void  		move_to	  		 ( float Left,   float  Bottom	 );

	virtual Control*	HitTest		  ( int x, int y 	);
	virtual int			onClick		  ( int x, int y, bool mouse_is_down=true );
	virtual int			onCreate	  (  );	
	virtual int   		draw 		  (	 );

	virtual int			onHover		  ( int x, int y	);
	virtual int			onDoubleClick ( 				);

	HorizontalMenu*		m_Menu;	
private:
	float			m_MenuStart_x;
	float			m_MenuEnd_x;

	Button			m_show_taskbar;
	Button			m_show_sidebar;
	
	Button			m_wifi;
	Button			m_volume;		
	
//	vector<Control*>    m_icons;		// use something more than control* later.
//	Video				m_Time;	
//	Camera				m_Time;	
//	Intercom			m_intercom;	
//  VoiceCommands		m_voice_dialog;  on/off
			
//	void (*on_click_callback)(void*);
//	void* on_click_context;				// often this pointer to a Control*
};

 
#endif

