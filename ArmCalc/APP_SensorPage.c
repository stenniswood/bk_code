/*********************************************************************
 *											   						 *
 *    DESCRIPTION:  Sonar Polar Plot & Cds Level displays			 *
 *											  						 *
 *	  CATEGORY	:  APP 												 *
 *											  						 *
 *    AUTHOR	:  Stephen Tenniswood, Embedded Engineer 	 		 *
 *											   						 *
 *    PURPOSE	:  Draw the sonar plot and CdS sensor	 			 *
 *																	 *
 *	  DATE		:  Oct 30,2008										 *
 *********************************************************************/
#include <stdio.h>
#include "skp_bsp.h"
#include "lcd_icons.h"
#include "LCD_Driver.h"
#include "LCD_Display.h"
#include "DEV_OCpwm.h"
#include "DEV_switches.h"
#include "SIM_statemachine_hand.h"
#include "DEV_rf_receive.h"
#include "DEV_sonar.h"
#include "LCD_Polar.h"
#include "APP_Robot_Display.h"
#include "APP_AndroidAnim.h"
#include "LCD_Graph.h"
#include "DEV_ADC_Driver.h"


/*****************************************************************************
Name		:   DisplaySonarPage()
Parameters	:	none
Description	:    Draws a graph of the Sonar "image", and CdS levels.
*****************************************************************************/
void DisplaySonarPage()
{
	//static FirstTime = TRUE;
	if (InvalidRegions)
		DD_DrawAllOff();

	// When enough data points have been accrued, clear display and redraw the graph
	if (InvalidRegions & 0x01)
	{
		POLAR_ScaleDataPoints();
		POLAR_Draw_Graph();
		POLAR_ResetData();
	}
	
	if ((InvalidRegions & 0x02) & (0))
	{
		GotoAddress(0,0);
		Text_Ln( "Sensor Status:" );
		Text_Ln(" ");		
		Text_Ln("Light Level:  ");
	}		
	
	/*sprintf(temp_str,"Sonar %4d", LatestSonarReading);
	GotoAddress(3,0);
	Text_Ln( (_far char*)temp_str );*/
	InvalidRegions = 0x00;
}

/*****************************************************************************
Name		:   DisplaySensorPage()
Parameters	:	none
Description	:   Draws a graph of the Sonar "image", and CdS levels.
*****************************************************************************/
void DisplaySensorPage()
{
	//static FirstTime = TRUE;
	if (InvalidRegions)
		DD_DrawAllOff();

	// When enough data points have been accrued, clear display and redraw the graph
	if (InvalidRegions & 0x01)
	{
		POLAR_ScaleDataPoints();
		POLAR_Draw_Graph();
		POLAR_ResetData();
	}
	
	if ((InvalidRegions & 0x02) & (0))
	{
		GotoAddress(0,0);
		Text_Ln( "Sensor Status:" );
		Text_Ln(" ");		
		Text_Ln("Light Level:  ");
	}		
	
	/*sprintf(temp_str,"Sonar %4d", LatestSonarReading);
	GotoAddress(3,0);
	Text_Ln( (_far char*)temp_str );*/
	InvalidRegions = 0x00;
}

/*****************************************************************************
Name		:   DisplayTestPage()
Parameters	:	returns TRUE when finished.  		
Description	:    Draws a polar graph of the sonar data.
*****************************************************************************/
void DisplayTestPage()
{
	enum Button_States Button_1;
	enum Button_States Button_2;
	
	Button_1 = switches_Read_1();
	Button_2 = switches_Read_2();
	
	GotoAddress(1,0);
	sprintf(temp_str,"Button1 = %4d", Button_1);
	Text_Ln( (_far char*)temp_str );
	GotoAddress(2,0);
	sprintf(temp_str,"Button2 = %4d", Button_2);
	Text_Ln( (_far char*)temp_str );

	GotoAddress(4,0);
	sprintf(temp_str,"Cds1 = %4d", ADC_Values[0]);
	Text_Ln( (_far char*)temp_str );
	GotoAddress(5,0);
	sprintf(temp_str,"Cds2 = %4d", ADC_Values[1]);
	Text_Ln( (_far char*)temp_str );
}

/*****************************************************************************
Name		:   SensorStatus_Statemachine()
Parameters	:	returns TRUE when finished.  		
Description	:    Draws a polar graph of the sonar data.
*****************************************************************************/
U8 SensorStatus_Statemachine()
{
	static U8 state = 0;
	switch (state)
	{
		case 0:		DD_DrawAllOff();
					POLAR_ClearData();
					POLAR_SetGraphPosition( 1, 127, 1, 63 );
					POLAR_Set_XScaleMinMax( MAX_SONAR_RANGE_TENTH_INCHES );
					if (!SELECT_BUTTON)			// wait for the release.
						state = 1;
					break;

		case 1:		DisplaySensorPage();
					if (SELECT_BUTTON)
						state = 99;
					if (X_BUTTON)
						state = 2;
					break;
		case 2:		
					//if (DD_DrawYesNo_Box(StoreCoordsQuestion, 1))
					state = 99;
					break;
		
		case 3:		
					break;

		case 99:	state=0;			// for next time.
					return TRUE;
		default:
					break;
	}
	return FALSE;
}


/*****************************************************************************
Name		:   Draw_AccelXY()
Parameters	:	returns TRUE when finished.  		
Description	:    Draws a polar graph of the sonar data.
*****************************************************************************/
void Draw_AccelXY()
{
	U8 i;
	DD_DrawHorizontalLine( 20, 0, 64 );
	DD_DrawVerticalLine  ( 0, 32, 32  );

//	for (i=0; i < NumDataPoints; i++)
	{	
//		DD_SetPixel( yBottom - yDataPoint[i], xLeft+i  );
	}
}

/*****************************************************************************
Name		:   Draw_AccelXY()
Parameters	:	Returns TRUE when finished.
Description	:   Draws a polar graph of the sonar data.
*****************************************************************************/
void Draw_VelocityXY()
{
/*	U8 i;
	DD_DrawHorizontalLine( 20, 0, 64  );
	DD_DrawVerticalLine  ( 0, 32, 32  );

	//for (i=0; i < NumDataPoints; i++)
	{	
		DD_SetPixel( yBottom - yDataPoint[i], xLeft+i  );
	}	*/
}

/*****************************************************************************
Name		:   Draw_AccelXY()
Parameters	:	returns TRUE when finished.  		
Description	:    Draws a polar graph of the sonar data.
*****************************************************************************/
void TiltSensorStatus_Statemachine()
{
	
}


