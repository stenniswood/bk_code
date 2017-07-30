#include <regex>

#include <iostream>
#include "GENERAL_protocol.hpp"
#include "route_protocol.hpp"
#include "user_list.hpp"
#include "sql_device.hpp"
#include "sql_users.hpp"

void Init_Routing_Protocol  ( )
{
    
}

void form_device_regex(int mUserIndex, std::string& device_regex)
{
	device_regex = "(";
	std::list<ServerHandler*>::iterator iter = (m_user_list[mUserIndex].connections.begin());
	while (iter != m_user_list[mUserIndex].connections.end())
	{
		device_regex += (*iter)->m_login_hostname;		
		if (*iter != m_user_list[mUserIndex].connections.back())
			device_regex += "|";
		else device_regex += ")";		
		iter++;
	}
	printf("Devices available regex = %s\n", device_regex.c_str() );
}

int route_to_which( Sentence& mSentence, ServerHandler* mh )
{
	int retval = -1;
	std::string dev_regex;
	sql_devices.get_all_device_names_regex( mh->m_user_id, dev_regex );		
	int result = mSentence.regex_find( dev_regex );
	if (result) {
		std::string match1 = mSentence.m_sentence.regex_matches.str(0);
		printf("Device select = %s\n", match1.c_str() );
	
		// now convert personal_name to hostname :
		std::string matching_hostname = "unknown";
		matching_hostname = sql_devices.sql_find_preferred_name( mh->m_user_id, match1 );
		
		// Now look up in list of actives:
		int user_index = find_user( mh->m_login_name );
		ServerHandler* nsh = find_device( user_index, matching_hostname );
		if (nsh==NULL)
		{
			mh->form_response( "Sorry that device is not online." );			
			retval = 0;
		} else {			
			std::string respond = "Okay routing all text to ";
			respond += match1;
			mh->form_response( respond.c_str() );
			//nsh->form_response("you have an incoming transmission...");
			
			printf("Route to connfd = %d\n", nsh->connfd );
			mh->establish_route( nsh );		        
			retval = 0;
		}
	}  else {
			mh->form_response( "Sorry i don't know any device by that name." );			
			retval = 0;	
	}      
	return retval;
}
int cancel_route_to_which( Sentence& mSentence, ServerHandler* mh )
{
	int retval = -1;
	std::string dev_regex;
	sql_devices.get_all_device_names_regex( mh->m_user_id, dev_regex );		
	int result = mSentence.regex_find( dev_regex );
	if (result) {
		std::string match1 = mSentence.m_sentence.regex_matches.str(0);
		printf("Device select = %s\n", match1.c_str() );
	
		// now convert personal_name to hostname :
		std::string matching_hostname = "unknown";
		matching_hostname = sql_devices.sql_find_preferred_name( mh->m_user_id, match1 );
		
		// Now look up in list of actives:
		int user_index = find_user( mh->m_login_name );
		ServerHandler* nsh = find_device( user_index, matching_hostname );
		if (nsh==NULL)
		{
			mh->form_response( "You are not connected to that device." );			
			retval = 0;
		} else {			
			std::string respond = "Okay cancelling the connection ";
			respond += match1;
			mh->form_response( respond.c_str() );
			//nsh->form_response("incoming transmission terminating...");
			
			printf("Cancel route to connfd = %d\n", nsh->connfd );
			mh->remove_route( nsh );
			retval = 0;
		}
	}  else {
			mh->form_response( "Sorry i don't know any device by that name." );			
			retval = 0;	
	}      
	return retval;
}

int  Parse_Routing_Statement( Sentence& mSentence, ServerHandler* mh )
{
    int retval =-1;
    mSentence.restore_reduced();

    int foundA = mSentence.regex_find( "(what )?routes? (are )?established" ) ;
    if (foundA)
    {
    	std::string routes = "Established routes are ";
		std::list<ServerHandler*>::iterator iter = mh->routes.begin();
		int count = 0;
		while (iter != mh->routes.end())
		{

			routes += (*iter)->m_login_hostname;
			iter++;
			count++;
        }
        if (count==0) {
        	routes = "No routing has been established.";
	        mh->form_response( routes.c_str() );
	    } else 
			mh->form_response( routes.c_str() );
		printf(" %s\n", routes.c_str() );
		retval = 1;        		
    } 

    foundA = mSentence.regex_find( "(what )?(devices|connections)? (are )?(available|online|connected)" ) ;
    if (foundA)
    {
    	std::string devices = "Online Devices are ";    	
    	std::list<ServerHandler*>::iterator iter = (m_user_list[mh->m_user_index].connections.begin());
		while (iter != m_user_list[mh->m_user_index].connections.end())
		{ 
			int dev_is_this_dev = mh->m_login_hostname.compare( (*iter)->m_login_hostname );
			if (dev_is_this_dev)
				devices += "this device";
			else 
				devices += (*iter)->m_dev_preferred_name;

			if (*iter != m_user_list[mh->m_user_index].connections.back())
				devices += ", ";
			else devices += ".";			
			iter++;
        }
        
		mh->form_response( devices.c_str() );				
		printf("Devices available for routing = %s\n", devices.c_str() );
		retval = 0;			
    }
    foundA = mSentence.regex_find( "(what )?(devices|robots)? (are )?(registered|do i have)" ) ;
    if (foundA)
    {
    	std::string devices = "Here's a list of all your devices:";
    	std::string list;
		sql_devices.get_all_device_names( mh->m_user_id, list );
		devices += list;
		mh->form_response( devices.c_str() );				
		printf("Devices available for routing = %s\n", list.c_str() );
		retval = 0;	
    }
    
    foundA = mSentence.regex_find( "(cancel|stop|drop)" ) ;
    int foundB = mSentence.is_found_in_sentence( "route" ) ;
    if (foundA && foundB)
    {
		retval = cancel_route_to_which(mSentence, mh);
    }
    else if (foundB)
    {
		retval = route_to_which(mSentence, mh);				
    }
    
    if (retval>-1)  printf( "Parse_Routing_Statement done\n" );
    return retval;
}
