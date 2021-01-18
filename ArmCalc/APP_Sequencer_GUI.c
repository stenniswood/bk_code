/*********************************************************************
 *											   						 *
 *    DESCRIPTION:  Arm sequencing GUI - Fixed positions			 *
 *					GUI to setup a sequence, save it to EEPROM,		 *
 *					And invoke it.									 *
 *											  						 *
 *	  CATEGORY	:  Low level APP									 *
 *											  						 *
 *    AUTHOR	:  Stephen Tenniswood, Embedded Engineer 	 		 *
 *											   						 *
 *    PURPOSE	:  	 												 *
 *																	 *
 *	  DATE		:  Oct 27,2008										 *
 *********************************************************************/
#include <stdio.h>
#include "skp_bsp.h"
#include "lcd_icons.h"
#include "lcd_Driver.h"
#include "LCD_display.h"
#include "MID_FilterServo.h"
#include "app_sequencer.h"
#include "APP_ArmPositionStorage.h"
#include "DEV_RF_Receive.h"
#include "APP_Robot_Display.h"
#include "UI_RemoteUserInput.h"
#include "APP_TeachPendant.h"
#include "DEV_switches.h"


struct stSequence RAM_Sequence;

/****************************************************************************
Name		:   SEQ_DrawCurrentSequence()
Parameters	:	mStartStop		- First stop shown
Description	:   Draws as much of the sequence as will fit on the page.
*****************************************************************************/
void SEQ_DrawCurrentSequence(stSequencePtr mPtr, U8 mStartStop)
{
	U8 iStop = mStartStop;			// current stop
	
	GotoAddress(0, 10);
	Text_Ln("SEQUENCE - SETUP");
	
	GotoAddress(1, 0);
	sprintf(temp_str, "Stops in Seq: %3d", mPtr->NumOfPositions);
	Text_Ln((_far U8*)temp_str);

	GotoAddress(2, 0);
	Text_Ln((_far U8*)"St Pos dT PAUSE");
	
	GotoAddress(3, 0);
	while (  (iStop < mPtr->NumOfPositions) &&  ((iStop-mStartStop) < 6)  )
	{
		sprintf(temp_str, "%2d %3d %4x", iStop, mPtr->IndexArray[iStop], mPtr->Time[iStop] );
		Text_Ln((_far U8*)temp_str);
		iStop++;
	}
}

/****************************************************************************
Name		:   Sequencer_Statemachine()
Parameters	:	none
Description	:   
*****************************************************************************/
U8 Sequencer_Statemachine()
{
	static U8 state = 0;
	static struct stSequence* Ptr=0;
	int DlgResult=0;
	
	switch (state)
	{
		case 0:  DD_DrawAllOff();
				 if (!SELECT_BUTTON)			// wait for the release.
					state = 1;
				 break;

		case 1:									// Show Current Sequence
				 //Ptr = SEQ_GetEEPROMLocation(0);
				 SEQ_DrawCurrentSequence(Ptr,0);
				 if (SELECT_BUTTON)			// next screen
					state = 99;
				 if (TRIANGLE_BUTTON)
				 	state = 2;
				 break;
		case 2:
				 if (!TRIANGLE_BUTTON)
				 {
					 if (R1)					// Erase
				 		state = 3;
					 else if (R2)
					 	state = 4;				// Recall
					 else
					 	state = 5;				// Save
				 }			
				 break;	

		case 3:		// ERASE
					DlgResult = DoNonModal_YesNo_Dialog(EraseAllQuestion);
					if (DlgResult == DLG_RESULT_YES)
					{
						DD_DrawAllOff();
						SEQ_EraseAll();
						SEQ_NumStoredSequences = 0;
						state = 0;
					}
					else if (DlgResult == DLG_RESULT_NO)
					{						
						
						DD_DrawAllOff();
						state = 0;
					}
				 break;

		case 4:	 	// RECALL
				 //SEQ_Recall();
				 state = 0;
				 break;	
				 
		case 5:		// STORE
				if (DoNonModal_YesNo_Dialog(StoreCoordsQuestion) == DLG_RESULT_YES)
				{
					DD_DrawAllOff();						// YES
					SEQ_Store( &RAM_Sequence, 1);
					state = 1;
				}
				else if (DoNonModal_YesNo_Dialog(StoreCoordsQuestion) == DLG_RESULT_NO)
				{
					DD_DrawAllOff();						// NO
					state = 1;
				}
				break;
								
		case 99:
				state = 0;
				return TRUE;
				break;		
		default:
				 break;	
	};
	return FALSE;
}

