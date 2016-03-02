#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <algorithm>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "text_view.hpp"
#include "drop_down_box.hpp"



DropDownBox::DropDownBox()
{

}

void DropDownBox::Initialize(	)
{
	strcpy (class_name, "DropDownBox");		
}

int	DropDownBox::calc_metrics()
{

}

int DropDownBox::onKey	  ( char mKey )
{

}

bool alpha_compare( std::string m1, std::string m2 )
{
	int result = m1.compare( m2 );
	if (result<0)
		return true;
}

void DropDownBox::alphabetize( )
{
	std::sort( m_dd_list.begin(), m_dd_list.end(), alpha_compare );
			
}


