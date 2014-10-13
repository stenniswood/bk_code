#include "bk_system_defs.h"
#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include "can_eid.h"
#include "can.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "packer.h"
#include "packer_lcd.h"


/* String may be upto 8 characters only! */
void pack_lcd_write_text( struct sCAN* mMsg, byte mDestInstance, 
						  char* mText )
{
	mMsg->id = create_CAN_eid( ID_LCD_DRAW_TEXT, mDestInstance );
	mMsg->header.DLC = 8;
	mMsg->header.rtr = 0;	

	byte length = strlen(mText);
	if (length > 8) length = 8;
	for (int i=0; i<length; i++)
		mMsg->data[i] = mText[i];
}

/* INPUTS:
	 mFont :  0 => no change;  1=small letters; 2=Big letters;	 
*/
void pack_lcd_cursor_to( struct sCAN* mMsg, byte mDestInstance, 
						byte mLine, byte mColumn, byte mFont )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 4;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_CURSOR_TO;
	mMsg->data[1] = mLine;
	mMsg->data[2] = mColumn;
	mMsg->data[3] = mFont;
}
void pack_lcd_clear_screen(struct sCAN* mMsg, byte mDestInstance )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 1;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_CLEAR_SCREEN;
}
void pack_lcd_all_on_screen(struct sCAN* mMsg, byte mDestInstance )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 1;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_ALL_ON_SCREEN;
}
void pack_lcd_clear_eol(struct sCAN* mMsg, byte mDestInstance, 
						byte Line, byte StartCol, byte EndCol )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 4;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_CLEAR_EOL;	
	mMsg->data[1] = Line;
	mMsg->data[2] = StartCol;
	mMsg->data[3] = EndCol;
}
void pack_lcd_draw_graph(struct sCAN* mMsg, byte mDestInstance )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 1;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_DRAW_GRAPH;
	// Not implemented yet (on lcd)	
}
void pack_lcd_draw_polar(struct sCAN* mMsg, byte mDestInstance )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 1;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_DRAW_POLAR;
	// Not implemented yet (on lcd)	
}
// defined in can_buff.c
/* Sender to provide either:   */
#define NO_HISTORY_OP_MODE 		0
#define NORMAL_HISTORY_OP_MODE 	1
#define MAILBOX_HISTORY_OP_MODE 2	
void pack_lcd_monitor_can(struct sCAN* mMsg, byte OperationalMode, byte mDestInstance)
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 1;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_MONITOR_CAN;
	mMsg->data[1] = OperationalMode;
}
void pack_lcd_draw_line(struct sCAN* mMsg, byte mDestInstance )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 8;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_DRAW_LINE;
	// Not implemented in lcd yet
}
void pack_lcd_draw_horizontal(struct sCAN* mMsg, byte mDestInstance, byte x_row, byte StartCol, byte EndCol)
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 8;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_DRAW_HORIZONTAL;
	mMsg->data[1] = x_row;			// [0..63]
	mMsg->data[2] = StartCol;		// [0..127]
	mMsg->data[3] = EndCol;			// [0..127]	
}

void pack_lcd_draw_vertical(struct sCAN* mMsg, byte mDestInstance, byte y_col, byte StartRow, byte EndRow)
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 8;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_DRAW_VERTICAL;
	mMsg->data[1] = y_col;
	mMsg->data[2] = StartRow;
	mMsg->data[3] = EndRow;
}
void pack_lcd_draw_arc(struct sCAN* mMsg, byte mDestInstance )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 1;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_DRAW_ARC;
	// not implemented
}
void pack_lcd_draw_circle(struct sCAN* mMsg, byte mDestInstance )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 1;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_DRAW_CIRCLE;
	// not implemented
}
void pack_lcd_draw_box(struct sCAN* mMsg, byte mDestInstance, byte RowStart,
						byte RowEnd, byte ColumnStart, byte ColumnEnd)
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 5;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_DRAW_BOX;
	mMsg->data[1] = RowStart;		// [0..63]
	mMsg->data[2] = RowEnd;			// [0..63]
	mMsg->data[3] = ColumnStart;	// [0..127]	
	mMsg->data[4] = ColumnEnd;		// [0..127]

}
void pack_lcd_invert_region(struct sCAN* mMsg, byte mDestInstance )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 8;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_INVERT_REGION;
}
void pack_lcd_set_window(struct sCAN* mMsg, byte mDestInstance )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 1;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_SET_WINDOW;
}
void pack_lcd_beep(struct sCAN* mMsg, byte mDestInstance, byte On_ms, byte Off_ms, byte NumBeeps)
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 4;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_BEEP;
	mMsg->data[1] = On_ms;		// [0..63]
	mMsg->data[2] = Off_ms;		// [0..63]
	mMsg->data[3] = NumBeeps;	// number of repeat times
}
void pack_lcd_backlight(struct sCAN* mMsg, byte mDestInstance, word mPercentinTenths )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 3;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_BACKLIGHT;
	mMsg->data[1] = hi(mPercentinTenths);		// Percent * 10  (ie 100.0 % integer representation)
	mMsg->data[2] = lo(mPercentinTenths);		// 	
}
void pack_lcd_contrast(struct sCAN* mMsg, byte mDestInstance, word mPercentinTenths )
{
	mMsg->id = create_CAN_eid( ID_LCD_COMMAND, mDestInstance );
	mMsg->header.DLC = 3;
	mMsg->header.rtr = 0;	
	mMsg->data[0] = LCD_CMD_CONTRAST;
	mMsg->data[1] = hi(mPercentinTenths);		// Percent * 10  (ie 100.0 % integer representation)
	mMsg->data[2] = lo(mPercentinTenths);		// 	
}

/***********************************SYSTEM CONFIGURE **************************/
/*void pack_configure_lcd( struct sCAN* mMsg, byte mDestInstance, byte mIndex, byte mMask, byte mValue )
{
//	byte Masks[4] = { 0,0,0,0 };
//	byte Values[4] = { 0,0,0,0 };
	pack_configure( mMsg, mDestInstance, mIndex, mMask, mValue );
}*/


