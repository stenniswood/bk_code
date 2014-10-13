// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "CAN_Interface.h"
#include "line_graph.hpp"
#include "dataset.hpp"
#include "histogram.hpp"
#include "line_graph.hpp"
#include "leveler.hpp"
#include "display.h"
#include "stick_leg.hpp"
#include "display_manager.hpp"
#include "button.hpp"
#include "listbox.hpp"
#include "checkbox.hpp"
#include "progressbar.hpp"
#include "button_array_gui.hpp"
#include "packer_motor.h"
#include "can_txbuff.h"
#include "motor.hpp"
#include "motor_pack.hpp"
#include "window_layouts.hpp"
#include "robot.hpp"


#define Debug 0

int PrevMotorSelectionButton=0;
int PrevPushedButton=0;

ButtonArray::ButtonArray( int Left, int Right, int Top, int Bottom )
:Control( Left, Right, Top, Bottom )
{
	float BWidth  = (Right-Left) / 4.0;	// 4 cols
	float BHeight = (Top-Bottom) / 5.0;	// 5 rows
	
	a[0] = new Button( Left, 		  Left+1*BWidth, 	Top, 			Top-BHeight );
	a[1] = new Button( Left+1*BWidth, Left+2*BWidth,	Top, 			Top-BHeight );
	a[2] = new Button( Left+2*BWidth, Left+3*BWidth, 	Top, 			Top-BHeight );
	a[3] = new Button( Left+3*BWidth, Left+4*BWidth, 	Top, 			Top-BHeight );
	a[1]->set_text( "Hip"  , 10.0 );
	a[2]->set_text( "Knee" , 10.0 );
	a[3]->set_text( "Ankle", 10.0 );

	b[0] = new Button( Left, 		  Left+1*BWidth, 	Top-BHeight, 	Top-2*BHeight );
	b[1] = new Button( Left+1*BWidth, Left+2*BWidth, 	Top-BHeight, 	Top-2*BHeight );	
	b[2] = new Button( Left+2*BWidth, Left+3*BWidth, 	Top-BHeight, 	Top-2*BHeight );	
	b[3] = new Button( Left+3*BWidth, Left+4*BWidth, 	Top-BHeight, 	Top-2*BHeight );	
	b[1]->set_text( "Hip", 10.0   );
	b[2]->set_text( "Knee", 10.0  );
	b[3]->set_text( "Ankle", 10.0 );

	c[0] = new Button( Left, 		  Left+1*BWidth, 	Top-2*BHeight, 	Top-3*BHeight );
	c[1] = new Button( Left+1*BWidth, Left+2*BWidth, 	Top-2*BHeight, 	Top-3*BHeight );
	c[2] = new Button( Left+2*BWidth, Left+3*BWidth, 	Top-2*BHeight, 	Top-3*BHeight );
	c[3] = new Button( Left+3*BWidth, Left+4*BWidth, 	Top-2*BHeight, 	Top-3*BHeight );
	c[0]->set_text( "<<<", 10.0  );
	c[1]->set_text( "Stop 1", 10.0 );
	c[2]->set_text( "Stop 2", 10.0 );
	c[3]->set_text( ">>>", 10.0 );


	d[0] = new Button( Left, 		  Left+1*BWidth, 	Top-3*BHeight, Top-4*BHeight );
	d[1] = new Button( Left+1*BWidth, Left+2*BWidth, 	Top-3*BHeight, Top-4*BHeight );
	d[2] = new Button( Left+2*BWidth, Left+3*BWidth, 	Top-3*BHeight, Top-4*BHeight );
	d[3] = new Button( Left+3*BWidth, Left+4*BWidth, 	Top-3*BHeight, Top-4*BHeight );
	d[0]->set_text( "<<" );
	d[1]->set_text( "<"  );
	d[2]->set_text( ">"  );
	d[3]->set_text( ">>" );	

	e[0] = new Button( Left, 		  Left+1*BWidth, 	Top-4*BHeight, Top-5*BHeight );
	e[1] = new Button( Left+1*BWidth, Left+2*BWidth, 	Top-4*BHeight, Top-5*BHeight );
	e[2] = new Button( Left+2*BWidth, Left+3*BWidth, 	Top-4*BHeight, Top-5*BHeight );
	e[3] = new Button( Left+3*BWidth, Left+4*BWidth, 	Top-4*BHeight, Top-5*BHeight );
	e[0]->set_text( "Screen 1", 8.0 );
	e[1]->set_text( "Levels 1", 8.0 );
	e[2]->set_text( "Graph",    8.0 );
	e[3]->set_text( "Quit",     8.0 );
	motor = NULL;
}

ButtonArray::ButtonArray()
{
	motor = NULL;
}

byte ButtonArray::setMotor(byte Row, byte Col)
{
	if (Row==1)
	{
		a[Col-1]->setState( true );			// hip knee ankle leg1
		motor = Onesimus.get_left_leg()->get_motor( Col-2 );
	}
	if (Row==2)
	{
		b[Col-1]->setState( true );			// hip knee ankle leg1
		motor = Onesimus.get_right_leg()->get_motor( Col-2 );
	}	
}

void ButtonArray::MarkStop(byte StopNum)
{
	// Read Motor Position
	// User input for the angle
	// Store both here
	// Package and send to BigMotor board.
	float angle;
	word value;
}

#define CCW 0
#define CW  1

void ButtonArray::jog( byte Direction, byte Speed )
{
	float speed = 0.0;
	if (Speed==1)
		speed = 25.0;
	else if (Speed==2)
		speed = 50.0;
	else if (Speed==3)
		speed = 100.0;
	if (Direction == CCW)
		speed = -speed;		

	printf("Sending Motor Move speed=%6.2f!\n", speed);
	motor->move_speed( speed );
}

void ButtonArray::release_prev_MotorSelection( int id )
{
	if (row1(id))
		a[id-4]->setState(false);		
	else if (row2(id))
		b[id-8]->setState(false);	
}

void ButtonArray::button_pushed( int B_ID )
{
	PrevPushedButton= B_ID;
	int col=0;
	if (row1(B_ID))				// Top Row
	{
		col = B_ID - 4;
		release_prev_MotorSelection( PrevMotorSelectionButton );
		PrevMotorSelectionButton = B_ID;
		setMotor( 1, B_ID-3);		// send row,cols
	}
	if (row2(B_ID))
	{
		col = B_ID - 8;
		release_prev_MotorSelection( PrevMotorSelectionButton );		
		PrevMotorSelectionButton = B_ID;
		setMotor( 2, B_ID-7);		// send row,col	
	}
	if (row3(B_ID))
	{
		col = B_ID - 12;
		if (B_ID==12)
			jog(CCW, 3);		// 16,17 Speed 100%
		if (B_ID==15)
			jog(CW, 3);			// 16,17 Speed 100%

		if (B_ID==13)
		{
			//motor->MarkStop(1,   0.0, motor->getValue() );		// We need an angle too!
			motor->setStopValue( 1, motor->getValue() );
			
		} else if (B_ID==14) {
			//motor->MarkStop(2, 180.0, motor->getValue() );		// We need an angle too!
			motor->setStopValue( 2, motor->getValue() );
		}
		c[col]->setState( true );		// hip knee ankle leg1 
	}
	if (row4(B_ID))			// Bottom Row
	{
		col = B_ID - 16;
		if (B_ID==16)
			jog(CCW, 2);		// 16,17 Speed of 1 or 2 
		if (B_ID==17)
			jog(CCW, 1);		// 16,17 Speed of 1 or 2 			
		if (B_ID>=18)
			jog(CW, 1);		// 18,19 Speed of 1 or 2 
		if (B_ID>=19)
			jog(CW, 2);		// 18,19 Speed of 1 or 2 
		d[col]->setState( true );		// 
	}
	if (frontRow(B_ID))		// Front Edge
	{
		col = B_ID - 0;
		e[col]->setState( true );		// Screen
		//if (B_ID==2)
		//	motor->adjustZeroOffset( motor->getValue() );
	}
}

void ButtonArray::button_released()
{
	//printf("button_released() PrevPushed=%d\n", PrevPushedButton);
	// if a motor selection button, restore the state to unsunken.
	if (row4(PrevPushedButton))			// Stop Jog.	
	{
		d[PrevPushedButton-16]->setState(false);
		jog(CCW, 0);		// Speed of 0, 1, or 2
	}			
}
#define Red 0xFFFF0000


int ButtonArray::draw (	)
{
	if (Debug) printf("Start of ButtonArray draw() ts=%6.1f\n", text_size );	
	for (int i=0; i<4; i++)
	{
		a[i]->draw();
		b[i]->draw();
		c[i]->draw();
		d[i]->draw();
		e[i]->draw(); 
	}
	draw_motor_info( );	
	if (Debug) 	printf("Done with ButtonArray draw()\n");
}


int ButtonArray::process_CAN_msg( struct sCAN* mMsg )
{
	if (mMsg->id.group.instance != Instance)  return 0;	// Only deal with this board's message!
	int result,retval;
	int NumButtonPushed;

	switch(mMsg->id.group.id)
	{
	case ID_BUTTON_PRESSED:
			NumButtonPushed = mMsg->data[0];
			for (int i=0; i<NumButtonPushed; i++)
			   Keys[i] = mMsg->data[i+1];
			if (NumButtonPushed)
				button_pushed( Keys[0] );
			else 
				button_released( );
			break;
			
	default: 
			break;
	}
}


