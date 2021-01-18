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
#include "APP_InverseGUI.h"


char temp_str[20] = "\0";
char PRNDL = 0;
U8   InvalidRegions = 0xFF;


void display_statemachine_init()
{
	InvalidRegions = 0xFF;			// redraw all regions.
}

/*****************************************************************************
Name		:    InvalidateRegion()
Parameters	:	  		
Description	:    This function records if a portion of the display needs to be
				 redrawn.  New data in a graph for example.
*****************************************************************************/
void InvalidateRegion(U8 mField)
{
	InvalidRegions |= mField;	
}

/*****************************************************************************
Name:        	DrawTach
Parameters:  	origy		Line number of display	[0..7] 
				origx		column					[0..127]
			    *mIcon		Structure containing the dimensions & data of the 
							icon.				
				controller  [1,2, 3(both)]
Description:    This function sends data of the icon to LCD	
*****************************************************************************/
void DrawTach(U8 origx, U8 origy, U8 msetting)
{
	if (msetting > 11)	msetting = 11;;
	
	DD_DrawIcon(origx, origy, Tachs[msetting]);
}

/*****************************************************************************
Name:        	Show_Tach
Parameters:  	origy		Line number of display	[0..7] 
				origx		column					[0..127]
			    mSpeedIndex	
				mString		
				controller  [1,2, 3(both)]
Description:    This function sends data of the icon to LCD	
*****************************************************************************/
/*
*	Long division:
*
*	PWMFreq = (Clock Frequency=20000000) / (ISS1<<6)    
*   Surprisingly, 32 bit divide only takes 7.5us  ~21 cycles.
*/
void Show_Tach(U8 mLine, U8 mColumn, U8 mSpeedIndex, _far char* mString )
{
	U16 Freq=0;
	U16 char_index = 0;	
	U8 icon_index = ((255-mSpeedIndex)>>5);
	if (icon_index > 10)
	   icon_index = 10;
	
	// Draw Icon
	DrawTach(mLine, mColumn, icon_index);
	
	// Draw Text
	if (mSpeedIndex == 0)
	{
		sprintf(temp_str, "%s=xxxx",mString);
	}
	else
	{	
		Freq = 78431 / mSpeedIndex;					// Convert to RPM.  (20000000/255 = 78431Hz)
		sprintf (temp_str, "%s=%d", mString, Freq);	
	}
		
	GotoAddress(mLine+2, mColumn);
	Text_Out((_far char*)temp_str);	
}



void Display_TitleScreen()
{
	U8 sx=0;
	U8 MyAgeData=0x00;				// Temporary variable for testing RAM.

/*	GotoAddress(0,28);
	sprintf(temp_str,"X: %4x", X_Tilt);
	Text_Out((_far char*)temp_str);
	GotoAddress(1,28);
	sprintf(temp_str,"Y: %4x", Y_Tilt);
	Text_Out((_far char*)temp_str);
*/
	
	sx = dCenterLargeText(6,0,127);
	GotoAddress(0,sx);
	dText_Ln("BEYOND");
	sx = dCenterLargeText(8,0,127);
	GotoAddress(2,sx);
	dText_Ln("KINETICS");
	sx = dCenterLargeText(8,0,127);
	GotoAddress(4,sx);
	dText_Ln("AUTO-ARM");
	DD_DrawHorizontalLine( 50, 15, 112);
	Text_Ln(" ");
	Text_Ln("   Copyright 2008");	
}



/*****************************************************************************
Name		:    TitleScreen_Statemachine()
Parameters	:	returns TRUE when finished.  		
Description	:    
*****************************************************************************/
U8 TitleScreen_Statemachine()
{
	static U8 state = 0;
	switch (state)
	{
		case 0:		DD_DrawAllOff();
					if (!SELECT_BUTTON)			// wait for the release.
						state = 1;
					break;

		case 1:		
					Display_TitleScreen();
					//DD_DrawIcon(2,5, &AndroidIcon );
					if (SELECT_BUTTON)
						state = 99;
					break;
					
		case 99:	state=0;					// for next time.
					return TRUE;
					break;
		default:
					break;
	}
	return FALSE;
}


/*****************************************************************************
Name		:    Display_StateMachine()
Parameters	:	  		
Description	:    This is the top level state machine for the controller.
				 Each sub-state machine will be called from this.
*****************************************************************************/
void Display_StateMachine()
{
	static U8 delay = 0;
	static U8 state = 0;
	switch (state)
	{
		case 0:		//state = 3;
					if (TitleScreen_Statemachine())
						state = 1;
					break;

		case 1:		if (TeachPendant_Statemachine())
						state = 2;
					break;
					
		case 2:		if (InverseKinematic_Statemachine())
						state = 3;
					break;

		case 3:		if (Sequencer_Statemachine())
						state = 4;
					break;
					
		case 4:  	if (SensorStatus_Statemachine())
						state = 5;
					break;

		case 5:  	if (TiltSensorStatus_Statemachine())
						state = 6;
					break;

		case 6:		if (DrawAnimatedAndroid_Statemachine())
						state = 7;
 					break;

		case 7:		DisplayTestPage();
					if (SELECT_BUTTON)
						state=8;
					break;

		case 8:		DD_DrawAllOff();
					state = 0;
					break;
		
		case 9:
					//Display_Main();
					if (switches_Read_1())
						state = 0;
					break;
		
		case 10:	
					break;
		default:
					break;	
	}
}

