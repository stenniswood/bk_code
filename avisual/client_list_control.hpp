#ifndef _CLIENT_LIST_CONTROL_HPP_
#define _CLIENT_LIST_CONTROL_HPP_

#include "tabular_listbox.hpp"
#include "window.hpp"


class ClientList : public TabularListBox
{
public:
	ClientList();
	ClientList( int Left, int Right, int Top, int Bottom  );	
	~ClientList();
	
	void		update_available_client_list();
	void		set_headings				();
	void		set_row( struct stClientData* mDatum );

};


class ClientListPanel : public Window
{
public:
	ClientListPanel();
	ClientListPanel( int Left, int Right, int Top, int Bottom  );	
	~ClientListPanel();
	
	void		update_available_client_list();
	void		set_headings				();
	void		set_row( struct stClientData* mDatum );
	
};



#endif
