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
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "CAN_Interface.h"
#include "line_graph.hpp"
#include "dataset.hpp"
#include "histogram.hpp"
#include "line_graph.hpp"
#include "leveler.hpp"
#include "display.h"
#include "display_manager.hpp"
#include "button.hpp"
#include "listbox.hpp"
#include "checkbox.hpp"
#include "progressbar.hpp"



IncomingCmd::IncomingCmd	( int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{

}

IncomingCmd::IncomingCmd	(		)
:Control()
{

}

virtual int   IncomingCmd::draw(	)		// Draw calls all the sub draw routines.
{
	Fill(255, 255, 255, 1);				   // White text
	
}

