#ifndef _LEGEND_H_
#define _LEGEND_H_

#include <vector>
#include <string>

#include "bk_system_defs.h"
#include "dataset.hpp"
#include "control.hpp"




class Legend : public Control
{
public:
	Legend( int Left, int Right, int Top, int Bottom );
	Legend();
	~Legend();
	
	virtual void  Initialize	(					);
	void  	set_data_series		( long mColor, char* mLabel );
	void	add_data_series	  	( DataSet* NewData );
	void	remove_data_series	( int index /*DataSet* OldData*/ );
	void	reset				( );
	
	void  calc_metrics			(					  );

	// This links the Datasets together!
	virtual int   draw			(	);		// Draw calls all the sub draw routines.


protected:
	std::vector<long>	m_color;
	std::vector<string>	m_names;

	
};

#endif
