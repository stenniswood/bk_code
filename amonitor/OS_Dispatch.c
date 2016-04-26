/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Low Level Routines RTOS					 	 	 *
 *											  						 *
 *	  CATEGORY:   BIOS Driver										 *
 *											   						 *
 *    PURPOSE: Use Timer 0 for a 1ms tic.  Interrupt handler to 	 *
 *           set update counters, then dispatch any tasks.			 *
 *																	 *
 *	  HARDWARE RESOURCES:									   		 *
 *			 Atmega16m1 Timer 0 is set up as an 8bit PWM	 		 *
 *																	 *
 *	  PIN ASSIGNMENTS:												 *
 *           No external pins 						 				 * 
 *											   						 *
 *    AUTHOR: Stephen Tenniswood, Robotics Engineer 	 			 *
 *		  	  December 2007						 				 	 *
 *		  	  Revised for atmel Sept 2013						 	 *
 *********************************************************************/
#include "bk_system_defs.h"

#include "CAN_base.h"
#include "can_eid.h"
#include "OS_timers.h"
#include "can_board_msg.h"
#include "can_txbuff.h"
#include "leds.h"


/*****************************************************************************
Name		:  OS_Dispatch()   
Parameters	:  none
Returns		:  nothing      
Description	:  Dispatches the tasks when their time is due
******************************************************************************/
void OS_Dispatch()
{	
	if ((OS_Event_TIC_Counter % 5) == 0)	// 5ms tasks
	{
	}
	if ((OS_Event_TIC_Counter % 10) == 0) 	// 10ms tasks
	{	
		can_tx_timeslice();	
	}
	if ((OS_Event_TIC_Counter % 33) == 0)	// 20ms tasks
	{	
	}
	if ((OS_Event_TIC_Counter % 50) == 0)	// 50ms tasks
	{
	}
	if ((OS_Event_TIC_Counter % 100) == 0)	// 100ms tasks
	{
	}
}
