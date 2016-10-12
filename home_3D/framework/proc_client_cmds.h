//
//  proc_client_cmds.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/6/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#ifndef __home3D__proc_client_cmds__
#define __home3D__proc_client_cmds__

//#include <stdio.h>
#include "verbal_object.h"


// ROBOT COMMANDS :
const int ROBOT_CMD_LIFT_LEG = 1;
const int ROBOT_CMD_LIFT_ARM = 2;
const int ROBOT_CMD_BEND_LEG = 3;
const int ROBOT_CMD_BEND_ARM = 4;
const int ROBOT_CMD_MOVE_OBJ = 5;


// COMMANDS :
const int CMD_MOVE_OBJ          = 1;
const int CMD_CLEAN_SLATE       = 2;
const int CMD_ONLY_SHOW         = 3;
const int CMD_ROBOT_CMD         = 4;
const int CMD_CREATE_SHAPE      = 5;
const int CMD_CREATE_FILE_OBJ   = 6;


/*const int QUERY_GET_OBJECT_ID       = 0x201;
const int QUERY_GET_OBJECT_COORD    = 0x202;
const int QUERY_COORDINATE          = 0x203;

const int ACTION_OPEN_DOOR      = 0x301;
const int ACTION_LIFT_OBJECT    = 0x302;
const int ACTION_PUSH_OBJECT    = 0x303; */


// Looks to IPC sim memory.
bool command_waiting        ( );
void process_robot_commands ( );
void process_commands       ( );


void proc_sim_command       ( );

void respond_to_client_timeslice();


#endif /* defined(__home3D__proc_client_cmds__) */
