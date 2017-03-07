// line graph OpenVG program
// Steve Tenniswood
#include <shapes.h>
#include <fontinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include <vector>
#include <string>
#include "CAN_Interface.h"
#include "dataset.hpp"
#include "adrenaline_windows.h"
#include "display_manager.hpp"
//#include "visual_memory.h"
#include "file_browser.hpp"
#include "complete_file_browser.hpp"
#include "test_layouts.hpp"


#define Debug 1


IconBar::IconBar()
{
	m_horizontal_buttons = true;	
}
	
void IconBar::wrap_to_icons    (          )
{

}
void IconBar::reserve_num_icons( int mNum )
{

}

int IconBar::onPlace(  )	
{
	int num_icons = m_child_controls.size();
	float left_edge = left;
	for (int i=0; i<num_icons; i++)
	{
		m_child_controls[i]->move_left_to  ( left_edge );
		m_child_controls[i]->move_bottom_to( bottom    );
		left_edge += left;
	}	
	return 1;
}

