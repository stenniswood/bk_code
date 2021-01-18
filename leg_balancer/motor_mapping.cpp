#include <string.h>
#include <stdio.h>

#include "drive_five.h"
#include "motor_mapping.h"



struct stMap body_joints[TOTAL_BODY_JOINTS];


void set_one_entry( int mIndex, const char* limb, const char* joint, const char* board_name, const char* device_name, char mLetter )
{
	strcpy(body_joints[mIndex].limb,        limb  );	
	strcpy(body_joints[mIndex].joint,       joint );
	strcpy(body_joints[mIndex].board_name,  board_name );
	strcpy(body_joints[mIndex].device_name, device_name   ); 		
	body_joints[mIndex].assigned_motor    = mLetter;
}


void init_map()
{
	set_one_entry( 0, "head", "tilt", "head", "", 'v');
	set_one_entry( 1, "head", "pan",  "head", "", 'w');

	set_one_entry( 2, "left arm", "pit", 	  		"arms", "", 'w');
	set_one_entry( 3, "left arm", "shoulder", 	  	"arms", "", 'x');
	set_one_entry( 4, "left arm", "elbow",    	  	"right arm", "", 'v');
	set_one_entry( 5, "left arm", "wrist", 		  	"right arm", "", 'w');
	set_one_entry( 6, "left arm", "wrist_rotate", 	"right arm", "", 'z');

	set_one_entry( 7, "right arm", "pit", 	  		"arms", "", 'v');
	set_one_entry( 8, "right arm", "shoulder", 	  	"arms", "", 'y');
	set_one_entry( 9, "right arm", "elbow",    	  	"right arm", "", 'x');
	set_one_entry( 10, "right arm", "wrist", 	  	"right arm", "", 'y');
	set_one_entry( 11, "right arm", "wrist_rotate", "right arm", "", 'z');

	set_one_entry( 12, "left leg", "swivel", 	  	"left leg", "", 'v');
	set_one_entry( 13, "left leg", "swing", 	  	"left leg", "", 'w');
	set_one_entry( 14, "left leg", "knee", 	  		"left leg", "", 'x');
	set_one_entry( 15, "left leg", "ankle", 	  	"left leg", "", 'v');
	set_one_entry( 16, "left leg", "ankle_tilt", 	"left leg", "", 'z');

	set_one_entry( 17, "right leg", "swivel", 	  	"right leg", "", 'v');
	set_one_entry( 18, "right leg", "swing", 	  	"right leg", "", 'w');
	set_one_entry( 19, "right leg", "knee", 	  	"right leg", "", 'x');
	set_one_entry( 20, "right leg", "ankle", 	  	"right leg", "", 'w');
	set_one_entry( 21, "right leg", "ankle_tilt", 	"right leg", "", 'z');
}

void print_map()
{
	for (int i=0; i<TOTAL_BODY_JOINTS; i++)
	{
		printf("%2d %20s %20s %20s %s %c\n", i,
			body_joints[i].limb,
			body_joints[i].joint,
			body_joints[i].board_name,
			body_joints[i].device_name,	
			body_joints[i].assigned_motor );
	}	
}

// Convert Limb name into the alias :
char* alias_lookup( char* mAlias )
{
	static char full_name[20];
	if (mAlias[0]=='h')
	{
		strcpy (full_name, "head");
	}
	if (mAlias[0]=='l')
	{
		strcpy(full_name, "left");
		if (mAlias[1]=='a')
			strcat( full_name, " arm");
		else
			strcat( full_name, " leg");	
	}
	if (mAlias[0]=='r')
	{
		strcpy(full_name, "right");
		if (mAlias[1]=='a')
			strcat( full_name, " arm");
		else
			strcat( full_name, " leg");				
	}
	return full_name;
}

char* to_alias( char* mLimb )
{
	static char alias[3];
	int result1 = strcmp(mLimb, "head")==0;
	int result2 = strcmp(mLimb, "left arm")==0;
	int result3 = strcmp(mLimb, "right arm")==0;
	int result4 = strcmp(mLimb, "left leg")==0;
	int result5 = strcmp(mLimb, "right leg")==0;	
				
	if (result1)	{	strcpy( alias, "h");	}
	if (result2)	{	strcpy( alias, "la");	}
	if (result3)	{	strcpy( alias, "ra");	}
	if (result4)	{	strcpy( alias, "ll");	}
	if (result5)	{	strcpy( alias, "rl");	}
	return alias;
}

/* Look up in our map and find the device name */
char* get_device_name( char* limb, char* joint )
{
	static char dev_name[100];
	for (int i=0; i<TOTAL_BODY_JOINTS; i++)
	{
		int lc = strcmp(body_joints[i].limb, limb)==0;
		int jc = strcmp(body_joints[i].joint, joint)==0;
		if (lc && jc) 
		{
			strcpy(dev_name,body_joints[i].device_name);
			return dev_name;
		}
	}
	strcpy(dev_name,"not found");
	return dev_name;
}

/* Look up in our map and find the assigned motor */
char get_assigned_motor( char* limb, char* joint )
{
	for (int i=0; i<TOTAL_BODY_JOINTS; i++)
	{
		int lc = strcmp(body_joints[i].limb, limb)==0;
		int jc = strcmp(body_joints[i].joint, joint)==0;
		if (lc && jc)
			return body_joints[i].assigned_motor;
	}
	return -1;
}

void fill_in_device_name( const char* mBoardName, const char* mDevName, DriveFive* mDF )
{
	for (int i=0; i<TOTAL_BODY_JOINTS; i++)
	{
		int compare = strcmp( body_joints[i].board_name, mBoardName );
		if (compare==0)
		{
			strcpy( body_joints[i].device_name, mDevName );
			body_joints[i].df = mDF;
			
		}
	}
}



/* = {
{ .limb="head", .joint="tilt", 		.board_name="head", .device_name="", .assigned_motor='v' },
{ .limb="head", .joint="pan", 		.board_name="head", .device_name="", .assigned_motor='w' },
{ .limb="left arm", .joint="arm pit", 		.board_name="arm", 		.device_name="", .assigned_motor='v' },
{ .limb="left arm", .joint="shoulder", 		.board_name="arm", 		.device_name="", .assigned_motor='w'},
{ .limb="left arm", .joint="elbow", 		.board_name="left_arm", .device_name="", .assigned_motor='x'},
{ .limb="left arm", .joint="wrist", 		.board_name="left_arm", .device_name="", .assigned_motor='y'},
{ .limb="left arm", .joint="wrist rotate",  .board_name="left_arm", .device_name="", .assigned_motor='z'},
{ .limb="right arm", .joint="arm pit",  	.board_name="arm", 		 .device_name="", .assigned_motor='x'},
{ .limb="right arm", .joint="shoulder", 	.board_name="arm", 		 .device_name="", .assigned_motor='y'},
{ .limb="right arm", .joint="elbow", 	  	.board_name="right_arm", .device_name="", .assigned_motor='x'},
{ .limb="right arm", .joint="wrist", 	  	.board_name="right_arm", .device_name="", .assigned_motor='y'},
{ .limb="right arm", .joint="wrist rotate", .board_name="right_arm", .device_name="", .assigned_motor='z'},
{ .limb="left leg", .joint="swivel", 	 .board_name="left_leg", 	.device_name="", .assigned_motor='v'},
{ .limb="left leg", .joint="swing", 	 .board_name="left_leg", 	.device_name="", .assigned_motor='w'},
{ .limb="left leg", .joint="knee", 		 .board_name="left_leg", 	.device_name="", .assigned_motor='x'},
{ .limb="left leg", .joint="ankle", 	 .board_name="left_leg", 	.device_name="", .assigned_motor='y'},
{ .limb="left leg", .joint="ankle tilt", .board_name="left_leg", 	.device_name="", .assigned_motor='z'},
{ .limb="right leg", .joint="swivel", 	 .board_name="right_leg", 	.device_name="", .assigned_motor='v'},
{ .limb="right leg", .joint="swing", 	 .board_name="right_leg", 	.device_name="", .assigned_motor='w'},
{ .limb="right leg", .joint="knee", 	 .board_name="right_leg", 	.device_name="", .assigned_motor='x'},
{ .limb="right leg", .joint="ankle", 	 .board_name="right_leg", 	.device_name="", .assigned_motor='y'},
{ .limb="right leg", .joint="ankle tilt", .board_name="right_leg", 	.device_name="", .assigned_motor='z'}
}; */