#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "bk_system_defs.h"
#include "CAN_Interface.h"
#include "can_txbuff.h"

#include "packer.h"
#include "board_list_oop.hpp"
#include "can_id_list.h"
#include "vector_math.h"
#include "cmd_process.h"
#include "parser_tilt.h"
#include "buttons.h"
#include "fuse_ag.h"
#include "fuse_accel.h"
#include "fuse_magnet.h"
#include "fuse_gyro.h"
#include "fuse_log.h"
#include "dataset.hpp"
#include "leveler.hpp"
#include "display.h"
#include "line_graph.hpp"
#include "leveler.hpp"
#include "histogram.hpp"
#include "callbacks.hpp"
#include "CAN_dispatcher.hpp"
//#include "robot.hpp"


using namespace std;

typedef BOOL (*tCallback)(struct sCAN*);

vector<void*>  m_callbacks;

#define Debug 0


void CAN_add_rx_callback( BOOL (*mCallback)(struct sCAN*) )
{
	m_callbacks.push_back( (void*)mCallback );
}


void dispath_to_all_can_receivers( struct sCAN* mPtr )
{
	tCallback tmp = NULL;
	if (m_callbacks.empty())  return;

	vector<void*>::iterator iter = m_callbacks.begin();	
	while (iter != m_callbacks.end() )
	{
		tmp = (tCallback)*iter;
		BOOL result = tmp( mPtr );			// Call each callback
		iter++;
	}
}

