#ifndef _SYSTEM_BAR_
#define _SYSTEM_BAR_


#include "control.hpp"
#include "horizontal_menu.hpp"
#include "system_bar.hpp"


class SystemBar : public Control 
{
public:
	SystemBar ( );
	SystemBar ( int mScreenWidth, int mScreenHeight );
	~SystemBar( );
	
	virtual void 	Initialize	(	);
	void			place_items	(	);

	virtual void 	set_width_height  	  ( int Width, int Height 		 );
	virtual void  	move_to	  		  	  ( float Left,   float  Bottom	 );

	virtual int   	draw 		(	);

	virtual Control*	HitTest		  ( int x, int y 	);
	virtual int			onClick		  ( int x, int y, bool mouse_is_down=true );
	virtual int			onHover		  (  			 	);
	virtual int			onDoubleClick ( 				);

	HorizontalMenu		m_Menu;	
private:
	float			m_MenuStart_x;
	float			m_MenuEnd_x;

//	Date				m_Date;
//	Time				m_Time;
//	vector<Control*>    m_icons;		// use something more than control* later.
//	Volume				m_Time;
//	Video				m_Time;	
//	Camera				m_Time;	
//	Connections			m_Time;
			
//	void (*on_click_callback)(void*);
//	void* on_click_context;				// often this pointer to a Control*
};

 
#endif

