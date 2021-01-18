#ifndef _SKP_BSP_H
#define _SKP_BSP_H

/*****************************************************************************
*
*   File Name:   SKP16C26 Board Support Package
*			     skp_bsp.h, October 2003                                     
*                                                                  
*   Content:    This header file contains useful macros for use with the 
*				RTA SKP's.               
*                                                                  
*   Copyright (c) 2003 Renesas Technology America , Inc.                  
*   All rights reserved
*=============================================================================
*   $Log:$
*===========================================================================*/

#include "sfr29.h"  		// M16C/26 special function register definitions
//#include "skp_lcd.h"  		// SKP LCD function definitions

/* Xin Freq */
#define	XIN_FREQ		20e6		// 20MHz 

/* Switches */
#define SWITCH_SU  		p6_4
#define SWITCH_SD  		p6_5
#define SWITCH_SU_DDR   pd6_4
#define SWITCH_SD_DDR   pd6_5
// Aliases
#define	S1 				p6_4 
#define S2 				p6_5 
#define S1_DDR			pd6_4
#define S2_DDR			pd6_5
#define HIGH 1
#define LOW  0


/********************************************************************************/
/* Macro Definitions 															*/
/********************************************************************************/
#define LED_ON      	0
#define LED_OFF     	1
#ifndef TRUE
#define TRUE    		1
#endif
#ifndef FALSE
#define FALSE   		0
#endif

#define ENABLE_IRQ   	{_asm(" FSET I");}
#define DISABLE_IRQ		{_asm(" FCLR I");}


// Use these macros to control the LEDs.  
#define LED(led, state) ((led) = !state)
#define ENABLE_LEDS {RED_LED = LED_OFF; YLW_LED = LED_OFF; GRN_LED = LED_OFF; RED_DDR = 1; YLW_DDR = 1; GRN_DDR = 1; }

#define INPUT 0
#define OUTPUT 1

/**********************************************************************************/
/* Type definitions 															  */
/**********************************************************************************/
typedef unsigned char  Bool;
typedef signed char    S08;
typedef signed short   S16;
typedef signed long    S32;
typedef unsigned char  U8;
typedef unsigned char  BYTE;
typedef unsigned short U16;
typedef unsigned short WORD;
typedef unsigned long  U32; 
typedef unsigned long  QUAD; 


#define INCHES_PER_METER 39.3700787 
#define PI 3.14159265


#endif
