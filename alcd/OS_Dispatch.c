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
#include "can_eid.h"
//#include "can.h"
#include "OS_timers.h"
#include "can_board_msg.h"
#include "leds.h"


/*****************************************************************************
Name		:  OS_Dispatch()   
Parameters	:  none
Returns		:  nothing      
Description	:  Dispatches the tasks when their time is due
******************************************************************************/
int t=0;
void OS_Dispatch()
{	
	if ((OS_Event_TIC_Counter % 5) == 0)	// 5ms tasks
	{
	}
	if ((OS_Event_TIC_Counter % 10) == 0) 	// 10ms tasks
	{	
	}
	if ((OS_Event_TIC_Counter % 33) == 0)	// 20ms tasks
	{	
		RESET_LED_1();
		if (t==0) 	 {
		} else if (t==1) {
		} else if (t==2) {
		} else if (t==3) {
		} else t=-1;
		t++;		
	}
	if ((OS_Event_TIC_Counter % 50) == 0)	// 50ms tasks
	{
	}
	if ((OS_Event_TIC_Counter % 100) == 0)	// 100ms tasks
	{
	}
	
	/******************************************************************************
	****   PHASE DELAYED OS TASKS:
	******************************************************************************/
//	if (((OS_Event_TIC_Counter+SONAR_PHASE_DELAY) % 50) == 0)	// 50ms tasks
	{
		// This will occur every 25ms behind the normal 50ms task
		// The sonar servo motor must first be moved before we start our ping.
		// This 25ms delay gives the motor sometime to respond.  
		// The motor will move again in the next 25ms.  By then we should have our reading.
		// and any further echo beyond max distance may occur with out disrupting any software
		// If the ping happens to be picked up, so much the better.  If it is not detected,
		// it is beyond the max specified anyway.
	}
}
