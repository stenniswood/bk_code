/*********************************************************************
Product of Beyond Kinetics, Inc
This code handles the buffering of can messages.

There are 2 modes of operation:
A) History buffer (remembering previous n msgs sequentially)

B) ID Based buffer (filing msgs into mailboxes based on ID; remembers the last n distinct IDs received)
					
DATE 	:  10/16/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdio.h>
#include <string.h>
#include "inttypes.h"
#include "bk_system_defs.h"
#include "can_eid.h"
#include "can_id_list.h"


char* getID_Text(word mID)
{
	switch( mID )
	{
	case ID_BOARD_PRESENCE_REQUEST:		return "ID_BOARD_PRESENCE_REQUEST";
			break;
	case ID_BOARD_PRESENCE_BROADCAST:	return "ID_BOARD_PRESENCE_BROADCAST";
			break;
	case ID_BOARD_REVISION:				return "ID_BOARD_REVISION";
			break;
	case ID_BOARD_SERIAL_NUMBER:		return "ID_BOARD_SERIAL_NUMBER";
			break;
	case ID_BOARD_DESCRIPTION:			return "ID_BOARD_DESCRIPTION";
			break;
	case ID_INSTANCE_CLAIM:				return "ID_INSTANCE_CLAIM";
			break;
	case ID_INSTANCE_QUERY:				return "ID_INSTANCE_QUERY";
			break;
	case ID_INSTANCE_ASSIGN:			return "ID_INSTANCE_ASSIGN";
			break;
	case ID_INSTANCE_RESTART:			return "ID_INSTANCE_RESTART";
			break;
	case ID_SYSTEM_BAUD_CHANGE:			return "ID_SYSTEM_BAUD_CHANGE";
			break;
	case ID_SYSTEM_SHUT_DOWN:			return "ID_SYSTEM_SHUT_DOWN";
			break;
	case ID_SYSTEM_CONFIGURE:			return "ID_SYSTEM_CONFIGURE";
			break;
	case ID_SYSTEM_CONFIGURE_REPORT:		return "ID_SYSTEM_CONFIGURE_REPORT";
			break;
	case ID_SYSTEM_LED_REQUEST:			return "ID_SYSTEM_LED_REQUEST";
			break;
	case ID_BUTTON_PRESSED:				return "ID_BUTTON_PRESSED";
			break;
	case ID_BUTTON_ROSTER:				return "ID_BUTTON_ROSTER";
			break;
	case ID_MARK_MOTOR_STOP:			return "ID_MARK_MOTOR_STOP";
			break;
	case ID_MOVE_TO_ANGLE:				return "ID_MOVE_TO_ANGLE";
			break;
	case ID_MOVE_SPEED:					return "ID_MOVE_SPEED";
			break;
	case ID_SET_MAX_MOTOR_ACCEL:		return "ID_SET_MAX_MOTOR_ACCEL";
			break;
	case ID_CALIBRATED_STOP:			return "ID_CALIBRATED_STOP";
			break;
	case ID_MOTOR_ANGLE:				return "ID_MOTOR_ANGLE";
			break;
	case ID_MOTOR_VALUE:				return "ID_MOTOR_VALUE";
			break;
	case ID_MOTOR_SPEED:				return "ID_MOTOR_SPEED";
			break;
	case ID_MOVE_DUAL_SPEED:			return "ID_MOVE_DUAL_SPEED";
			break;
	case ID_MOTOR_ACCEL:				return "ID_MOTOR_ACCEL";
			break;
	case ID_MOTOR_STATUS:				return "ID_MOTOR_STATUS";
			break;
	case ID_LCD_COMMAND:				return "ID_LCD_COMMAND";
			break;
	case ID_LCD_DRAW_TEXT:				return "ID_LCD_DRAW_TEXT";
			break;
	case ID_COLLECT_GRAPH_DATA:			return "ID_COLLECT_GRAPH_DATA";
			break;
	case ID_TILT_CHANGE_SENSITIVITY:	return "ID_TILT_CHANGE_SENSITIVITY";
			break;
	case ID_ACCEL_XYZ:					return "ID_ACCEL_XYZ";
			break;
	case ID_GYRO_XYZ:					return "ID_GYRO_XYZ";
			break;
	case ID_MAGNET_XYZ:					return "ID_MAGNET_XYZ";
			break;
	case ID_POSITION_XYZ:				return "ID_POSITION_XYZ";
			break;
	case ID_POSITION_ABC:				return "ID_POSITION_ABC";
			break;
	case ID_VELOCITY_XYZ:				return "ID_VELOCITY_XYZ";
			break;
	case ID_VELOCITY_ABC:				return "ID_VELOCITY_ABC";
			break;
	case ID_ACCELERATION_XYZ:			return "ID_ACCELERATION_XYZ";
			break;
	case ID_ACCELERATION_ABC:			return "ID_ACCELERATION_ABC";
			break;
	case ID_PWM_CHANNEL_UPDATE:			return "ID_PWM_CHANNEL_UPDATE";
			break;
	case ID_CORR_FACTOR_UPDATE:			return "ID_CORR_FACTOR_UPDATE";
			break;
	case ID_THRUST_UPDATE:				return "ID_THRUST_UPDATE";
			break;
	case ID_ANALOG_MEASUREMENT:			return "ID_ANALOG_MEASUREMENT";
			break;			
	case ID_STOP_MOTOR:					return "ID_STOP_MOTOR";
			break;			

	default: return "unknown message ID";
			break;
	}
}


int getID(char* mName)
{
	if (strcmp(mName, "ID_BOARD_PRESENCE_REQUEST")==0)
		return ID_BOARD_PRESENCE_REQUEST;	
	if (strcmp(mName, "ID_BOARD_PRESENCE_BROADCAST")==0)
		return ID_BOARD_PRESENCE_BROADCAST;
	if (strcmp(mName, "ID_BOARD_REVISION")==0)
		return ID_BOARD_REVISION;				
	if (strcmp(mName, "ID_BOARD_SERIAL_NUMBER")==0)
		return ID_BOARD_SERIAL_NUMBER;		
	if (strcmp(mName, "ID_BOARD_DESCRIPTION")==0)
		return ID_BOARD_DESCRIPTION;			
	if (strcmp(mName, "ID_INSTANCE_CLAIM")==0)
		return ID_INSTANCE_CLAIM;				
	if (strcmp(mName, "ID_INSTANCE_QUERY")==0)
		return ID_INSTANCE_QUERY;				
	if (strcmp(mName, "ID_INSTANCE_ASSIGN")==0)
		return ID_INSTANCE_ASSIGN;			
	if (strcmp(mName, "ID_INSTANCE_RESTART")==0)
		return ID_INSTANCE_RESTART;			
	if (strcmp(mName, "ID_SYSTEM_BAUD_CHANGE")==0)
		return ID_SYSTEM_BAUD_CHANGE;			
	if (strcmp(mName, "ID_SYSTEM_SHUT_DOWN")==0)
		return ID_SYSTEM_SHUT_DOWN;			
	if (strcmp(mName, "ID_SYSTEM_CONFIGURE")==0)
		return ID_SYSTEM_CONFIGURE;			
	if (strcmp(mName, "ID_SYSTEM_CONFIGURE_REPORT")==0)
		return ID_SYSTEM_CONFIGURE_REPORT;		
	if (strcmp(mName, "ID_SYSTEM_LED_REQUEST")==0)
		return ID_SYSTEM_LED_REQUEST;			
	if (strcmp(mName, "ID_BUTTON_PRESSED")==0)
		return ID_BUTTON_PRESSED;				
	if (strcmp(mName, "ID_BUTTON_ROSTER")==0)
		return ID_BUTTON_ROSTER;				
	if (strcmp(mName, "ID_MARK_MOTOR_STOP")==0)
		return ID_MARK_MOTOR_STOP;			
	if (strcmp(mName, "ID_MOVE_TO_ANGLE")==0)
		return ID_MOVE_TO_ANGLE;				
	if (strcmp(mName, "ID_MOVE_SPEED")==0)
		return ID_MOVE_SPEED;					
	if (strcmp(mName, "ID_SET_MAX_MOTOR_ACCEL")==0)
		return ID_SET_MAX_MOTOR_ACCEL;		
	if (strcmp(mName, "ID_CALIBRATED_STOP")==0)
		return ID_CALIBRATED_STOP;			
	if (strcmp(mName, "ID_MOTOR_ANGLE")==0)
		return ID_MOTOR_ANGLE;				
	if (strcmp(mName, "ID_MOTOR_VALUE")==0)
		return ID_MOTOR_VALUE;				
	if (strcmp(mName, "ID_MOTOR_SPEED")==0)
		return ID_MOTOR_SPEED;				
	if (strcmp(mName, "ID_MOVE_DUAL_SPEED")==0)
		return ID_MOVE_DUAL_SPEED;			
	if (strcmp(mName, "ID_MOTOR_ACCEL")==0)
		return ID_MOTOR_ACCEL;				
	if (strcmp(mName, "ID_MOTOR_STATUS")==0)
		return ID_MOTOR_STATUS;				
	if (strcmp(mName, "ID_LCD_COMMAND")==0)
		return ID_LCD_COMMAND;				
	if (strcmp(mName, "ID_LCD_DRAW_TEXT")==0)
		return ID_LCD_DRAW_TEXT;				
	if (strcmp(mName, "ID_COLLECT_GRAPH_DATA")==0)
		return ID_COLLECT_GRAPH_DATA;			
	if (strcmp(mName, "ID_TILT_CHANGE_SENSITIVITY")==0)
		return ID_TILT_CHANGE_SENSITIVITY;	
	if (strcmp(mName, "ID_ACCEL_XYZ")==0)
		return ID_ACCEL_XYZ;					
	if (strcmp(mName, "ID_GYRO_XYZ")==0)
		return ID_GYRO_XYZ;					
	if (strcmp(mName, "ID_MAGNET_XYZ")==0)
		return ID_MAGNET_XYZ;					
	if (strcmp(mName, "ID_POSITION_XYZ")==0)
		return ID_POSITION_XYZ;				
	if (strcmp(mName, "ID_POSITION_ABC")==0)
		return ID_POSITION_ABC;				
	if (strcmp(mName, "ID_VELOCITY_XYZ")==0)
		return ID_VELOCITY_XYZ;				
	if (strcmp(mName, "ID_VELOCITY_ABC")==0)
		return ID_VELOCITY_ABC;				
	if (strcmp(mName, "ID_ACCELERATION_XYZ")==0)
		return ID_ACCELERATION_XYZ;			
	if (strcmp(mName, "ID_ACCELERATION_ABC")==0)
		return ID_ACCELERATION_ABC;			
	if (strcmp(mName, "ID_PWM_CHANNEL_UPDATE")==0)
		return ID_PWM_CHANNEL_UPDATE;			
	if (strcmp(mName, "ID_CORR_FACTOR_UPDATE")==0)
		return ID_CORR_FACTOR_UPDATE;			
	if (strcmp(mName, "ID_THRUST_UPDATE")==0)
		return ID_THRUST_UPDATE;				
	if (strcmp(mName, "ID_ANALOG_MEASUREMENT")==0)
		return ID_ANALOG_MEASUREMENT;			
	if (strcmp(mName, "ID_STOP_MOTOR")==0)
		return ID_STOP_MOTOR;

	return -1;

}
