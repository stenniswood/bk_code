#ifndef _BUTTON_ARRAY_HPP_
#define _BUTTON_ARRAY_HPP_

#include "control.hpp"
#include "motor.hpp"

#define frontRow(B_ID) ((B_ID >= 0) && (B_ID < 4))
#define row1(B_ID) ((B_ID > 3) && (B_ID < 8))
#define row2(B_ID) ((B_ID > 7) && (B_ID < 12))
#define row3(B_ID) ((B_ID > 11) && (B_ID < 16))
#define row4(B_ID) ((B_ID > 15) && (B_ID < 20))


/* 		
	Let's make more generic.  So can use as number pad also.	
*/
class ButtonArray : public Control
{
public:
	ButtonArray();
	ButtonArray( int Left, int Right, int Top, int Bottom );

	int 			process_CAN_msg( struct sCAN* mMsg );		
	void			button_pushed( int ButtonNumber );
	void			button_released();

	virtual int   	draw( );

private:
	int 		ActiveMotor;
	Button* 	a[4];
	Button* 	b[4];
	Button* 	c[4];
	Button* 	d[4];
	Button* 	e[4];	

	Motor*		motor;
};

 
#endif
