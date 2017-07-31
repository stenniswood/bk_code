// 
//  ThreeD_object_protocol.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/20/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include "ThreeD_object_protocol.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <time.h>
#include <string>

#include "protocol.h"
#include "devices.h"
#include "GENERAL_protocol.hpp"
#include "nlp_extraction.hpp"
#include "client_memory.hpp"
#include "nlp_sentence.hpp"
#include "client_to_socket.hpp"
#include "alias.hpp"
#include "simulator_memory.h"
#include "parameter.hpp"
#include "ThreeD_object_protocol.hpp"
#include "positioner_2d.hpp"
#include "positioner_3d.hpp"
#include "color_protocol.hpp"
#include "sql_device.hpp"
#include "sql_users.hpp"
 
#define Debug 1

void Init_UserDevs_Protocol  ( )
{

}

std::map<string,string> dev_info_map;

std::map<string,string> extract_dev_info( SuperString& minfo )
{
	std::map<string,string> map;	
	int len = minfo.m_split_words.size()-1;		// last one is a blank!
	for (int i=1; i<len; i++)
	{
		minfo.m_split_words[i].split(':');
		minfo.m_split_words[i].trim_spaces_in_splits();		
		//Dprintf("map[%s] = %s\n", minfo.m_split_words[i].m_split_words[0].c_str(),
		//						 minfo.m_split_words[i].m_split_words[1].c_str() );

		map[minfo.m_split_words[i].m_split_words[0]] = minfo.m_split_words[i].m_split_words[1].c_str();
	
		// EXTRACT : 
/*		if (info.m_split_words[i].m_split_words[0].compare("hostname")==0)
			h->m_login_hostname = info.m_split_words[i].m_split_words[1]; 		
	 */
	}
	return map;
}

SuperString info;
// return : true => just added.
static bool register_device_if_not_already(ServerHandler* h, char* mDeviceInfo )
{

	return 0;
}		

int  Parse_UserDevs_Statement( Sentence& mSentence, ServerHandler* mh )
{
    int retval =-1;
    mSentence.restore_reduced();

    int foundA = mSentence.regex_find( "^Device Info" );
	if (foundA)
	{
		if (mh->m_credentials_validated==true) {
			printf("Device Info RECEIVED:\n" );
			// Split up each part of Device Info:
			info = mSentence.m_sentence;
			info.split(';');
			info.trim_spaces_in_splits();
	
			// EXTRACT ALL FIELD'S INFO : 
			dev_info_map = extract_dev_info( info );
			bool just_added=false;
			if (mh->m_device_id == -1)		// means device was not found when logging in.
				// ADD TO DATABASE :
				just_added = (sql_devices.sql_add_if_not_already( mh->m_user_id, 
							 						  dev_info_map  )>0);	
			//printf("just_added = %d\n", just_added );
			//bool just_added = register_device_if_not_already(mh, mSentence.m_sentence.c_str() );
			if (just_added==false) {
				sql_devices.sql_bump_login_count( mh->m_login_hostname, mh->m_user_id );			
				//mh->form_response("Device Info acknowledged.");
			} //else 
				//mh->form_response("Device Info registered!");
		} else 
			mh->form_response("Not logged in!");			
	}
	foundA = mSentence.regex_find( "Device Capabilities" );
	if ((foundA) && (mh->m_credentials_validated==true))
	{
		if (mh->m_credentials_validated==true) 
			printf("Device Capabilities RECEIVED:\n" );
		else 
			mh->form_response("Not logged in!");		
	}
	
    foundA = mSentence.regex_find( "change password" ) ;
	if (foundA)
	{
		
		return 1;
	}

    foundA = mSentence.regex_find( "forgot my password" ) ;
	if (foundA)
	{
		
		return 1;
	}

    foundA = mSentence.regex_find( "forgot my username" ) ;
	if (foundA)
	{
		string name;
		string new_passwd;
		sql_users.sql_update_password( name, new_passwd );			
		return 1;
	}

    foundA = mSentence.regex_find( "how many users are there" );
    int foundB = mSentence.regex_find( "how many users are online" );    
	if (foundA)
	{
		
		return 1;
	}

    foundA = mSentence.regex_find( "how many devices (do I have|are registered)" );
    foundB = mSentence.regex_find( "how many devices are (online|active|can I connect to)" );    
	if (foundA)
	{
		
		return 1;
	}

    if (retval>-1)  printf( "Parse_UserDevs_Statement done\n" );
    return retval;
}





