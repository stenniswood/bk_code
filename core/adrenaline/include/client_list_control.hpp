#ifndef _CLIENT_LIST_CONTROL_HPP_
#define _CLIENT_LIST_CONTROL_HPP_

#include "tabular_listbox.hpp"
#include "window.hpp"
#include "button.hpp"


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
	
	virtual int		 onCreate   ( );
	virtual void	 place_views( );
	
	virtual Control* HitTest( int x, int y );
	virtual int		 onClick( int x, int y, bool mouse_is_down=true );

	
	ClientList* 	m_clients;	
	Button			m_connect;		
};


#endif
