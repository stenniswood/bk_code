#ifndef _USER_LIST_HPP_
#define _USER_LIST_HPP_

#include <vector>
#include <string> 
#include <list>
#include "server_handler.hpp"


/********* This struct and vector will group all of a users devices */
struct stUsers {
	std::string login_name;
	std::list<ServerHandler*> connections;

};

int		add_user		 ( std::string mName );
int		find_user		 ( std::string mName );
void	add_connection	 ( std::string mName, ServerHandler* mSH );
bool	remove_connection( std::string mName, ServerHandler* mSH );
bool	remove_all_connections( std::string mName );
void	print_user_list	 (  );

/*****************/

extern std::vector<struct stUsers>  m_user_list;	// holds all ServerHandler* for a single user.

void test_user_list();

#endif
