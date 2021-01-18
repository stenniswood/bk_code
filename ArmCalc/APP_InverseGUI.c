/*********************************************************************
 *											   						 *
 *    DESCRIPTION:  Robot LCD Screen displays					 	 *
 *											  						 *
 *	  CATEGORY	:  BIOS Driver										 *
 *											  						 *
 *    AUTHOR	:  Stephen Tenniswood, Embedded Engineer 	 		 *
 *											   						 *
 *    PURPOSE	:  Setup an ADC channel for the Cds light sensor	 *
 *			   	   and Thermistor.									 *
 *																	 *
 *	  DATE		:  Sept 25,2008										 *
 *********************************************************************/
#include <stdio.h>
#include "skp_bsp.h"
#include "lcd_icons.h"
#include "LCD_Driver.h"
#include "LCD_Text_12x16_chars.h"
#include "LCD_Display.h"
#include "DEV_OCpwm.h"
#include "MID_FilterServo.h"
#include "DEV_switches.h"
#include "SIM_statemachine_hand.h"
#include "DEV_rf_receive.h"
#include "DEV_sonar.h"
#include "LCD_Graph.h"
#include "APP_Robot_Display.h"
#include "APP_AndroidAnim.h"
#include "APP_Sensorpage.h"
#include "DEV_RAM.h"
#include "DEV_tiltsensor.h"
#include "LCD_Text_6x8_chars.h"
#include "UI_RemoteUserInput.h"
#include "APP_PositionDescriptions.h"
#include "APP_CoordinateTransform.h"
#include "APP_InverseCoordinates.h"

/************** CONSTANT DEFINITIONS ************************/
#define MAX_MODES 	 3

#define MAX_X_COORD 18.0
#define MAX_Y_COORD 18.0
#define MAX_Z_COORD 18.0
#define DRAW_ANGLES 0


/************** VARIABLE DEFINITIONS ************************/
float X_Coord = 0.0;
float Y_Coord = 0.0;
float Z_Coord = 0.0;
U8 Approach_Mode = 0;


/****************************************************************************
Name		:   Draw_Servos()
Parameters	:	returns TRUE when finished
Description	:   Draw the servos Angle or Positions.
*****************************************************************************/
void Draw_Servos()
{
	float tmp1,tmp2,tmp3;
	
	if (DRAW_ANGLES)
	{
		// Now show the servo angles:
		GotoAddress( 5,0 );
		sprintf ( temp_str, "%4x %4x %4x", Servo1, Servo2, Servo3 );
		//sprintf ( temp_str, "%6.1f %6.1f", Servo1, Servo2 );
		Text_Out( temp_str );

		GotoAddress( 6,0 );
		sprintf ( temp_str, "%4x %4x %4x", Servo4, Servo5, Servo6 );
		Text_Out( temp_str );

		GotoAddress( 7,0 );
		sprintf ( temp_str, "%4x %4x ", Servo7, Servo8 );
		Text_Out( temp_str );
	}
	else
	{
		// Now show the servo angles:
		tmp1 = CT_Convert_Servo_to_Degrees(Servo1, 1);
		tmp2 = CT_Convert_Servo_to_Degrees(Servo2, 2);
		tmp3 = CT_Convert_Servo_to_Degrees(Servo3, 3);
		GotoAddress( 5,0 );
		sprintf ( temp_str, "%5.1f %5.1f %5.1f", tmp1, tmp2, tmp3 );
		Text_Out( temp_str );

		tmp1 = CT_Convert_Servo_to_Degrees(Servo4, 4);
		tmp2 = CT_Convert_Servo_to_Degrees(Servo5, 5);
		tmp3 = CT_Convert_Servo_to_Degrees(Servo6, 6);
		GotoAddress( 6,0 );
		sprintf ( temp_str, "%5.1f %5.1f %5.1f", tmp1, tmp2, tmp3 );
		Text_Out( temp_str );

		tmp1 = CT_Convert_Servo_to_Degrees(Servo7, 7);
		tmp2 = CT_Convert_Servo_to_Degrees(Servo8, 8);
		GotoAddress( 7,0 );
		sprintf ( temp_str, "%5.1f %5.1f", tmp1, tmp2 );
		Text_Out( temp_str );
	}
}

/****************************************************************************
Name		:   DrawInverseKinematicScreen()
Parameters	:	returns TRUE when finished
Description	:   Show the requested XYZ coordinate and approach.
*****************************************************************************/
void DrawInverseKinematicScreen()
{
	U8 start_col = CenterText (13);
	
	GotoAddress(0, start_col );
	Text_Ln( "XYZ - Inverse");
	Text_Out(" ");
	
	// Display XYZ 
	GotoAddress(2, 0);
	sprintf (temp_str, "X:%6.1f", X_Coord );
	Text_Out((_far U8*)temp_str);

	GotoAddress(3, 0 );
	sprintf ( temp_str, "Y:%6.1f", Y_Coord );
	Text_Out( temp_str );

	GotoAddress(4, 0 );
	sprintf ( temp_str, "Z:%6.1f", Z_Coord );
	Text_Out( temp_str );

	// OUT OF REACH WARNING:
	if (Feasibility > 0)
	{
		GotoAddress(2, 75 );
		Text_Out("LIMIT!");
	}
	else
	{
		GotoAddress(2, 75 );
		Text_Out("      ");
	}

	// APPROACH:
	GotoAddress(3, 75 );
	Text_Out("From:");
	switch (Approach_Mode)
	{
		case 0: GotoAddress(4, 81 );
				Text_Out("Front");
				break;				
		case 1: GotoAddress(4, 81 );
				Text_Out("Top");
				break;		
		case 2: GotoAddress(4, 81 );				
				Text_Out("Bottom");
				break;
		default:
				GotoAddress(4, 81 );				
				Text_Out("Unknown");
				break;	
	}
	
	// Draw Servo Angles:
	Draw_Servos();
}

/*****************************************************************************
Name		:   Draw_InvKin_Help_Screen()
Parameters	:	returns TRUE when finished.  		
Description	:    
*****************************************************************************/
void Draw_InvKin_Help_Screen()
{
	U8 start_col;
	
	// Display XYZ:
	//start_col = CenterText(22);
	GotoAddress(0, 0);
	Text_Ln( "HELP XYZ-Inverse 1/2");
	Text_Ln( "" );
	Text_Ln( "Specify XYZ location.");
	Text_Ln( "Servo positions are  ");
	Text_Ln( "calculated. Use right");
	Text_Ln( "pod to move XY. Use  ");
	Text_Ln( "left-up/down moves Z ");
}

/*****************************************************************************
Name		:   Draw_InvKin_Help_Screen2()
Parameters	:	returns TRUE when finished.  		
Description	:   
*****************************************************************************/
void Draw_InvKin_Help_Screen2()
{
	U8 start_col;
	
	// Display XYZ:
	//start_col = CenterText(22);
	GotoAddress(0, 0);
	Text_Ln( "HELP XYZ-Inverse 2/2");
	Text_Ln( "" );
	Text_Ln( "Unlock: L1->X L2->Y");
	Text_Ln( "Use arrows for small");
	Text_Ln( "XY moves. ");
	
	DD_DrawIcon(5, 0, &SquareIcon);
	Text_Ln( "  changes approach.");
	Text_Ln( "      ");	
}

/*****************************************************************************
Name		:   Bound_XYZ()
Parameters	:	returns TRUE when finished.  		
Description	:   The analog pods will provide the velocity for movements.
*****************************************************************************/
inline void Bound_XYZ()
{
	if (X_Coord > MAX_X_COORD)
		X_Coord = MAX_X_COORD;
	if (Y_Coord > MAX_Y_COORD)
		Y_Coord = MAX_Y_COORD;
	if (Z_Coord > MAX_Z_COORD)
		Z_Coord = MAX_Z_COORD;
}

/*****************************************************************************
Name		:   Bound_RadiusHeight()
Parameters	:	returns TRUE when finished.  		
Description	:   The analog pods will provide the velocity for movements.
*****************************************************************************/
void Bound_RadiusHeight(U8 mx_vel, U8 my_vel, U8 mz_vel)
{
	if (X_Coord > MAX_X_COORD)
		X_Coord = MAX_X_COORD;
	if (X_Coord > MAX_X_COORD)
		X_Coord = MAX_X_COORD;
	if (X_Coord > MAX_X_COORD)
		X_Coord = MAX_X_COORD;
}

/*****************************************************************************
Name		:   Calc_ServosPos()
Parameters	:	returns TRUE when finished.  		
Description	:   
******************************************************************************/
void Calc_ServosPos()
{
	switch (Approach_Mode)
	{
		case 0:
			POS_FromFront_Approach(X_Coord, Y_Coord, Z_Coord);
			break;
		case 1:
			POS_FromTop_Approach  (X_Coord, Y_Coord, Z_Coord);
			break;
		case 2:
			POS_FromBottom_Approach(X_Coord, Y_Coord, Z_Coord);				
			break;
		default:
			break;
	}	
}

/*****************************************************************************
Name		:   Adjust_XYZ_Timeslice()
Parameters	:	returns TRUE when finished.  		
Description	:   The analog pods will provide the velocity for movements.
			:   Call from OS timed task 50ms.
			:	Max rate = 0.1" per 0.05sec	==>  20*0.1 per second = 2"/second
			:
*****************************************************************************/
void Adjust_XYZ_Timeslice()
{
	float delta_x,delta_y,delta_z;

	// Find delta Inches to move:
	delta_x = (float)(RIGHT__LEFT_RIGHT - 0x7F)/(float)1280.0;		// 128/1280 = 0.1" per timeslice max rate.
	delta_y = (float)(RIGHT__UP_DOWN    - 0x7F)/(float)1280.0;		// 
	delta_z = (float)(LEFT__LEFT_RIGHT  - 0x7F)/(float)1280.0;		// 
	
	if (R1) X_Coord += delta_x;
	if (R2) Y_Coord += delta_y;
	if (L1) Z_Coord += delta_z;
	
	Bound_XYZ();
	
	Calc_ServosPos();
	// Displayed XYZ should always show the current arm position.
	// If out adjusted position designated by X_Coord, etc should revert to the last closest position.	
	
//	if (Feasibility == 0)
//		INV_Actuate_Position();				// okay
//	else	
	{
		/*X_Coord = ActiveCoord.x;			// error, back up.
		Y_Coord = ActiveCoord.y;
		Z_Coord = ActiveCoord.z;*/
	}
}


/*****************************************************************************
Name		:   TitleScreen_Statemachine()
Parameters	:	returns TRUE when finished.  		
Description	:   
*****************************************************************************/
U8 InverseKinematic_Statemachine()
{
	static U8 state = 0;
	static U8 count = 0;
	switch (state)
	{
		case 0:		DD_DrawAllOff();
					UI_Mode = UI_INVERSEKINETICS;
					if (!SELECT_BUTTON)			// wait for the release
						state = 1;
					break;

		case 1:		DrawInverseKinematicScreen();
					if (SELECT_BUTTON)
						state = 99;			// Next Screen
//					if (TRIANGLE_BUTTON)
//						state = 2;			// EEPROM 
					if (CIRCLE_BUTTON)
						state = 90;			// HELP
					if (SQUARE_BUTTON)
						state = 20;			// SWITCH APPROACH
					break;

		/*------------------ CHANGE APPROACH ---------------------*/
		case 20:	if (!SQUARE_BUTTON)		// Wait for release
						state = 21;			
					break;
		case 21:	Approach_Mode++;
					if (Approach_Mode >= MAX_MODES)
						Approach_Mode = 0;
					state = 0;			// SWITCH APPROACH
					break;
					
		/*------------------ ADJUST POSITION ---------------------*/
		case 30:	
					break;
		case 31:	
					break;
		
		/*------------------------ HELP --------------------------*/
		case 90:	DD_DrawAllOff();
					state = 91;
					count = 0;
					break;

		case 91:	Draw_InvKin_Help_Screen();
					if (count++ > 200)
						state = 92;
					break;

		case 92:	if (CIRCLE_BUTTON)
						state = 93;			// button Pressed
					break;
					
		case 93:	DD_DrawAllOff();
					if (!CIRCLE_BUTTON)
						state = 94;			// button Released
					break;
		case 94:	Draw_InvKin_Help_Screen2();
					state = 95;
					break;
		case 95:	if (CIRCLE_BUTTON)
						state = 96;			// button Pressed
					break;
		case 96:	DD_DrawAllOff();
					if (!CIRCLE_BUTTON)
						state = 0;			// button Released
					break;
		/*------------------ END STATEMACHINE --------------------*/
		case 99:	state=0;					// for next time.
					return TRUE;
					break;
		default:
					break;
	}
	return FALSE;
}
