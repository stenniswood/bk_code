#ifndef _SPINNER_H_
#define _SPINNER_H_

#include "control.hpp"
#include "button.hpp"

#define BUTTON_WIDTH 40

#define ALIGN_LEFT   1
#define ALIGN_RIGHT  2
#define ALIGN_CENTER 3

/************************************************************
This SPINNER control implements a standard integer.
	with buttons for increasing and decreasing.
This offers is readily builtin.
************************************************************/
class SpinnerControl : public Control
{
public:
	SpinnerControl();
	SpinnerControl( int Left, int Right, int Top, int Bottom );
	SpinnerControl( int Width, int Height 					 );
	~SpinnerControl();
	
	virtual void 	Initialize(	);
	int				calc_metrics();
	
	void			set_value		( int mValue )	{ m_value = mValue; };
	int				get_value		( )				{ return m_value;   };
	void			increment		( )				{ m_value++; };
	void			decrement		( )				{ m_value--; };

	void			set_max			( int mMax ) { m_max_value=mMax; };
	void			set_min			( int mMin ) { m_min_value=mMin; };	

	virtual void 	set_width_height( int Width, int Height 		 );
	virtual void  	move_to	  		( float Left,   float  Bottom	 );

	virtual int   	draw 				(	);
	
	int				get_hit_index		( int Mousex, int Mousey );
	virtual int		onClick				( int x, int y, bool mouse_is_down=true );
	int				onCreate			();


protected:
	byte			m_alignment;
	int				m_value;
	int				m_max_value;
	int				m_min_value;	
	float			margin;

	Button			m_up;
	Button			m_down;
};

 
#endif
