#ifndef _CLIENT_LIST_CONTROL_HPP_
#define _CLIENT_LIST_CONTROL_HPP_

#include "tabular_listbox.hpp"




class ClientList : public TabularListBox
{
public:
	ClientList();
	~ClientList();
	
	void		update_available_client_list();
	void		set_headings				();
	


};




#endif