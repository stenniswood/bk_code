#ifndef PACKER_LCD_H
#define PACKER_LCD_H


/*********** LCD TEST CODE - THE BIGMOTOR PACKAGES DATA UP TO TEST THE LCD **************/
// FOR DEBUG, SEND POT VALUE: 
#define LCD_CMD_NO_ACTION			0x00
#define LCD_CMD_CURSOR_TO			0x01		// Change location where the next DrawText will go.
#define LCD_CMD_SELECT_FONT			0x02		// Change location where the next DrawText will go.
#define LCD_CMD_CLEAR_SCREEN		0x03		// Clear entire LCD screen
#define LCD_CMD_ALL_ON_SCREEN		0x04		// Set all pixels on the LCD screen
#define LCD_CMD_CLEAR_EOL			0x05		// Clear a text row [0..7] from specified start & end columns.
#define LCD_CMD_DRAW_GRAPH			0x06		// Draw a 2D graph of the accumulated data (see ID_COLLECT_GRAPH_DATA)
#define LCD_CMD_DRAW_POLAR			0x07		// 
#define LCD_CMD_MONITOR_CAN			0x08
#define LCD_CMD_DRAW_LINE			0x09		// Draw a 2D line any end points.  Not implemented yet.
#define LCD_CMD_DRAW_HORIZONTAL		0x0A		// Specify the Row, Start & End Columns
#define LCD_CMD_DRAW_VERTICAL		0x0B		// Specify the Column, Start & End Rows
#define LCD_CMD_DRAW_ARC			0x0C		// Not Implemented yet
#define LCD_CMD_DRAW_CIRCLE			0x0D		// Not Implemented yet
#define LCD_CMD_DRAW_BOX			0x0E		// Specify (Start & End) Row; Start & End Column
#define LCD_CMD_INVERT_REGION		0x0F		// Invert Region
#define LCD_CMD_SET_WINDOW			0x10
#define LCD_CMD_BEEP				0x20		// Specify On & Off time in milliseconds
#define LCD_CMD_BACKLIGHT			0x30		// Specify the percent * 10 in word.
#define LCD_CMD_CONTRAST			0x31		// Specify the percent * 10 in word.


void pack_lcd_write_text	( struct sCAN* mMsg, byte mDestInstance, char* mText );
void pack_lcd_cursor_to		( struct sCAN* mMsg, byte mDestInstance, byte mLine, byte mColumn, byte mFont );						
void pack_lcd_clear_screen	( struct sCAN* mMsg, byte mDestInstance );
void pack_lcd_all_on_screen	( struct sCAN* mMsg, byte mDestInstance );
void pack_lcd_clear_eol		( struct sCAN* mMsg, byte mDestInstance, 
								byte Line, byte StartCol, byte EndCol );
void pack_lcd_draw_graph	(struct sCAN* mMsg, byte mDestInstance );
void pack_lcd_draw_polar	(struct sCAN* mMsg, byte mDestInstance );

#define NO_HISTORY_OP_MODE 		0
#define NORMAL_HISTORY_OP_MODE 	1
#define MAILBOX_HISTORY_OP_MODE 2	
void pack_lcd_monitor_can	(struct sCAN* mMsg, byte OperationalMode, byte mDestInstance);
void pack_lcd_draw_line	 	(struct sCAN* mMsg, byte mDestInstance );
void pack_lcd_draw_horizontal(struct sCAN* mMsg, byte mDestInstance, byte x_row, byte StartCol, byte EndCol);
void pack_lcd_draw_vertical	(struct sCAN* mMsg, byte mDestInstance,  byte y_col, byte StartRow, byte EndRow);
void pack_lcd_draw_arc		(struct sCAN* mMsg, byte mDestInstance );
void pack_lcd_draw_circle	(struct sCAN* mMsg, byte mDestInstance );
void pack_lcd_draw_box		(struct sCAN* mMsg, byte mDestInstance, byte RowStart,
									byte RowEnd, byte ColumnStart,  byte ColumnEnd);
void pack_lcd_invert_region	(struct sCAN* mMsg, byte mDestInstance );
void pack_lcd_set_window	(struct sCAN* mMsg, byte mDestInstance );
void pack_lcd_beep			(struct sCAN* mMsg, byte mDestInstance, byte On_ms, byte Off_ms, byte NumBeeps);
void pack_lcd_backlight		(struct sCAN* mMsg, byte mDestInstance, word mPercentinTenths );
void pack_lcd_contrast		(struct sCAN* mMsg, byte mDestInstance, word mPercentinTenths );
















						




						





#endif
