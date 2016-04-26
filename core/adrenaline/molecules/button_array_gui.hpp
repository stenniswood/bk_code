#ifndef _BUTTON_ARRAY_GUIHPP_
#define _BUTTON_ARRAY_GUIHPP_

#include "control.hpp"
//#include "motor.hpp"


#define frontRow(B_ID) ((B_ID >= 0) && (B_ID < 4))
#define row1(B_ID) ((B_ID > 3) && (B_ID < 8))
#define row2(B_ID) ((B_ID > 7) && (B_ID < 12))
#define row3(B_ID) ((B_ID > 11) && (B_ID < 16))
#define row4(B_ID) ((B_ID > 15) && (B_ID < 20))


/* Designed specifically for the motor control
	So some buttons act like radio controls.
	
	Let's make more generic.  So can use as number pad also.
	
*/
class ButtonArrayMot : public Control
{
public:
	ButtonArrayMot();
	ButtonArrayMot( int Left, int Right, int Top, int Bottom );

	void 			jog(byte Direction, byte Speed);
	void			set_button_pushed( int ButtonNumber );
	void			button_released();
	virtual int   	draw( );
	int 			draw_motor_info();
	void 			MarkStop(byte StopNum);

protected:
	byte 		setMotor(byte Row, byte Col);
	void 		release_prev_MotorSelection( int id );

private:
	int 		ActiveMotor;
	Button* 	a[4];
	Button* 	b[4];
	Button* 	c[4];
	Button* 	d[4];
	Button* 	e[4];	

//	Motor*		motor;
};

 
#endif
