

#include "GENERAL_protocol.hpp"
#include "route_protocol.hpp"
#include "user_list.hpp"


void Init_Routing_Protocol  ( )
{
    
}

void form_device_regex(int mUserIndex, std::string& device_regex)
{
	device_regex = "(";
	std::list<ServerHandler*>::iterator iter = (m_user_list[mUserIndex].connections.begin());
	while (iter != m_user_list[mUserIndex].connections.end())
	{
		device_regex += (*iter)->m_login_devicename;		
		if (*iter != m_user_list[mUserIndex].connections.back())
			device_regex += "|";
		else device_regex += ")";		
		iter++;
	}
	printf("Devices available regex = %s\n", device_regex.c_str() );
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
		while (iter != mh->routes.end())
		{
			routes += (*iter)->m_login_devicename;
			iter++;
        }
		mh->form_response( routes.c_str() );
		printf("Established Routes : %s\n", routes.c_str() );
		retval = 1;        
    }

    foundA = mSentence.regex_find( "(what )?(devices|connections)? (are )?available" ) ;
    if (foundA)
    {
    	std::string devices = "Devices for routing are ";
    	std::list<ServerHandler*>::iterator iter = (m_user_list[mh->m_user_index].connections.begin());
		while (iter != m_user_list[mh->m_user_index].connections.end())
		{
			devices += (*iter)->m_login_devicename;
			if (*iter != m_user_list[mh->m_user_index].connections.back())
				devices += ", ";
			else devices += ".";
			
			iter++;
        }
		mh->form_response( devices.c_str() );				
		printf("Devices available for routing = %s\n", devices.c_str() );
		retval = 0;			
    }

    foundA = mSentence.is_found_in_sentence( "route" ) ;
    if (foundA)
    {
    	std::string dev_regex;
    	form_device_regex( mh->m_user_index, dev_regex );
		int result = mSentence.regex_find( dev_regex );
		int msize  = mSentence.m_sentence.regex_matches.size();
		printf("matches=%d Device select = %s\n", msize, mSentence.m_sentence.regex_matches[1] );
		
        mh->form_response( "Okay routing all text to your device." );
        		
		ServerHandler* hndlr = (m_user_list[mh->m_user_index].connections.back());
		printf("Route to connfd = %d\n", hndlr->connfd );
		mh->establish_route( hndlr );		        
        retval = 0;
    }
    
    if (retval>-1)  printf( "Parse_Routing_Statement done\n" );
    return retval;
}
