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
	default:
			break;
	}
}
