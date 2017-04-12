#ifndef _DROP_DOWN_H_
#define _DROP_DOWN_H_

#include <vector>
#include <string>
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

class DropDownBox : public EditBox
{
public:
	DropDownBox();
	
	virtual void 	Initialize  (  );
	int				calc_metrics(  );

	virtual int		onKey	   ( char mKey );
	void			alphabetize( );

	std::vector<std::string>	m_dd_list;
};



#endif
