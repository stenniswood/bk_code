#ifndef _LABEL_EDIT_TEXT_H_
#define _LABEL_EDIT_TEXT_H_

#include "text_view.hpp"
#include "edit_box.hpp"


/*
	This dialog box will allow someone to specify which CAN msg, and bytes
	should be used to extract the 8 or 16 byte value for graphing.
	ie it indicates first of all the 
			CAN message ID, 
			CAN instance, 
			Size of the value (8 or 16, 32)
			byte index to start at (within the CAN msg)			
*/

class LabelEditBox : public TextView
{
public:
	LabelEditBox();
	
	virtual void 	Initialize(	);
	int				calc_metrics();
	virtual int		onKey	  ( char mKey );
	
	TextView		m_label;
	EditBox			m_edit;	
};



#endif
