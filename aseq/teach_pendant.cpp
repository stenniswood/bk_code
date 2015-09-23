#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <vector> 
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <time.h>
#include <string>

#include "bk_system_defs.h"
#include "can_eid.h"
#include "CAN_Interface.h"
#include "can_instance.h"
#include "can_txbuff.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "packer.h"
#include "packer_motor.h"
#include "packer_lcd.h"
//#include "config_file.h"
#include "seq_init.hpp"
#include "teach_pendant.hpp"




TeachPendant::TeachPendant()
{
	Initialize();
}

void TeachPendant::Initialize( )
{
	m_Instance = -1;
	m_is_connected = true;
	m_CAN_msg_id = ID_ANALOG_MEASUREMENT;
	m_Instance = 7;
}

bool	TeachPendant::read_config_data( Preferences& mprefs, int mIndex )	// reads indices, names, 
{

}

int TeachPendant::matches_any_dial( int mIndex )
{
	for (int i=0; i<m_dials.size(); i++)
		if (m_dials[i].index == mIndex)
			return i;
	return -1;
}

int TeachPendant::handle_CAN_message( struct sCAN* mMsg )	// handles incoming msg
{
	if (mMsg->id.group.instance != m_Instance)	return 0;
				
	if (mMsg->id.group.id == m_CAN_msg_id)
	{
		if (m_CAN_msg_id == ID_ANALOG_MEASUREMENT)
		{
			int dial_index = matches_any_dial(mMsg->data[0]);
			if (dial_index>=0)
			{
				m_dials[dial_index].PrevCount = m_dials[dial_index].CurrCount;
				m_dials[dial_index].CurrCount = (mMsg->data[1]<<8) + mMsg->data[2];
				return 1;
			}
		}
		else if (m_CAN_msg_id == ID_MOTOR_VALUE)
		{
/*			int dial_index = matches_any_dial(mMsg->data[0]);
				struct stMotorStateInfo tmp;
				can_parse_motor_value( mMsg, &(tmp.PotValue), 
											 &(tmp.CurrentTimesTen), 
									 (short*)&(tmp.SpeedTimesTen) );
				PrevCount = CurrCount;
				CurrCount = tmp.PotValue;
				update_position();
				print_state(); */
				return 1;
		}
	} 
	return 0;	
}

void	TeachPendant::add_dial( int mIndex, char* mName )
{
	struct DialInfo dial;
	if (mName)
		strcpy (dial.Name, mName);
		
	dial.index = mIndex;
	m_dials.push_back(dial);
}

void TeachPendant::print()
{
	printf("TEACH PENDANT:\n");
	for (int i=0; i<m_dials.size(); i++)
		printf("  %d %s = %d", i, m_dials[i].Name, m_dials[i].CurrCount );

}

