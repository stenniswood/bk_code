#ifndef _MSG_VALUE_COMPOSER_H_
#define _MSG_VALUE_COMPOSER_H_


#include "button.hpp"
#include "edit_box.hpp"
#include "window.hpp"
#include "tabular_listbox.hpp"



/*
	This dialog box will allow someone to specify which CAN msg, and bytes
	should be used to extract the 8 or 16 byte value for graphing.
	ie it indicates first of all the 
			CAN message ID, 
			CAN instance, 
			Size of the value (8 or 16, 32)
			byte index to start at (within the CAN msg)
			
*/

class MsgValueComposer : public Window
{
public:
	MsgValueComposer();
	
	virtual void 	Initialize(	);
	int				calc_metrics();
	virtual int		place_views();

	unsigned short 	m_instance;
	unsigned short 	m_id;
	int				m_num_bytes;
	int				m_start_byte_index;		// within the CAN msg.


	Button			m_update;
	Button			m_cancel;
	Button			m_add;
	Button			m_remove;
	
	EditBox			m_msg_id_;
	EditBox			m_instance_;
	EditBox			m_byte_length_;
	EditBox			m_byte_start_index_;
	TabularListBox	m_list;
	
};


#endif
