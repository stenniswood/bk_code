
#include "user_list.hpp"




/*************** USER LIST **************************/
std::vector<struct stUsers>  m_user_list;	// holds all ServerHandler* for a single user.

int add_user ( std::string mName )
{
	struct stUsers st;
	st.login_name = mName;
	m_user_list.push_back( st );
	return m_user_list.size()-1;
}

int		find_user		 ( std::string mName )
{
	size_t size = m_user_list.size();
	for (int i=0; i<size; i++)
	{
		int result = m_user_list[i].login_name.compare( mName );
		if (result==0)
		{
			return i;
		}
	}
	return -1;
}
void	add_connection	 ( std::string mName, ServerHandler* mSH )
{
	int user_index = find_user( mName );
	if (user_index >=0)
	{
		m_user_list[user_index].connections.push_back( mSH );
	}
}
bool	remove_connection( std::string mName, ServerHandler* mSH )
{
	int user_index = find_user( mName );
	if (user_index >=0)
	{
		m_user_list[user_index].connections.remove( mSH );
	}
}
bool	remove_all_connections( std::string mName )
{
	int user_index = find_user( mName );
	if (user_index >=0)
	{
		m_user_list[user_index].connections.clear( );
	}
}
void	print_user_list( )
{
	printf("m_user_list[]:\n");
	size_t size = m_user_list.size();
	for (int i=0; i<size; i++)
		printf("[%d] : name=%s;\t connections=%d\n", i, 
				m_user_list[i].login_name.c_str(),
				m_user_list[i].connections.size() );
}

ServerHandler*	find_device( int user_index, std::string mDeviceName )
{	 
	printf("LOOKING FOR dev=%s\n", mDeviceName.c_str() );
	std::list<ServerHandler*>::iterator iter = m_user_list[user_index].connections.begin();	
	while (iter != m_user_list[user_index].connections.end())
	{
		int comparison = ((*iter)->m_login_hostname.compare(mDeviceName));
		printf( "dev_name=%s; \n", (*iter)->m_login_hostname.c_str() );
		if (comparison==0)
			return *iter;
		iter++;
	}
	return NULL;
}



void test_user_list()
{
	add_user( "mtenniswood" );
	add_user( "stenniswood" );
	add_user( "ltenniswood" );
	print_user_list();
	
	int index = find_user( "stenniswood" );	
	printf("found user = %d\n", index);

}



/*std::list<struct stRoute>::iterator  find_route( int mIndex, ServerHandler* mSrc, ServerHandler* mDest )
{
	std::list<struct stRoute>::iterator iter = m_user_list[mIndex].routes.begin();
	while (iter != m_user_list[mIndex].routes.end())
	{
		bool result = compare_route(*iter, mSrc, mDest);
		if (result)
			return iter;
		iter++;
	}	
}

bool remove_route ( std::string mName, ServerHandler* mSrc, ServerHandler* mDest )
{
	int index = find_user( mName );
	if (index>=0)
	{
		struct stRoute route;
		route.src  = mSrc;
		route.dest = mDest;
		std::list<struct stRoute>::iterator iter = find_route( index, mSrc, mDest );
		m_user_list[index].routes.remove( *iter );	
	}
}*/

