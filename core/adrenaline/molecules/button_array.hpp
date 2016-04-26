#ifndef _BUTTON_ARRAY_HPP_
#define _BUTTON_ARRAY_HPP_

#include <vector>
#include <string>

#include "control.hpp"


#define frontRow(B_ID) ((B_ID >= 0) && (B_ID < 4))
#define row1(B_ID) ((B_ID > 3) && (B_ID < 8))
#define row2(B_ID) ((B_ID > 7) && (B_ID < 12))
#define row3(B_ID) ((B_ID > 11) && (B_ID < 16))
#define row4(B_ID) ((B_ID > 15) && (B_ID < 20))

class ButtonRC : public Button
{
public:
	ButtonRC();
	ButtonRC( int Left, int Right, int Top, int Bottom );
	
	int m_row;
	int m_col;
};

/* 		
	Let's make more generic.  So can use as number pad also.	
*/
class ButtonArray : public Control
{
public:
	ButtonArray( );
	ButtonArray( int rows, int cols );
	ButtonArray( int Left, int Right, int Top, int Bottom );

	void			set_rows_cols 		( int rows, int  mCols );
	void			create		  		( );
	void			place		  		( );
	void			set_button_text_row ( int row, std::vector<std::string>&  mTexts );
	
	virtual Control*	HitTest		  ( int x, int y 	);
	
	void			set_callback	 	( void (*callback)(int,int) );
	virtual int			onCreate	  (  );	// chance to load resources, call functions which use fonts (already loaded before this call) etc.
	int				onClick				( int x, int y, bool mouse_is_down=true );
		
	int 			process_CAN_msg		( struct sCAN* mMsg );
	void			button_pushed		( int ButtonNumber );
	virtual int   	draw				( );

	void (*m_callback)(int,int);	
private:
	int	m_rows;
	int	m_cols;
	std::vector<std::vector<Button*> >	m_buttons;
};

 
#endif
