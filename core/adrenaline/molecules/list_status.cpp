/*******************************************************
* Draws the Adrenaline Network Overview
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>

#include <ctype.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <fontinfo.h>
#include <shapes.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "icon.hpp"

#include "listbox.hpp"
#include "list_status.hpp"


StatusList::StatusList(  )
{

}

int	StatusList::populate_with_boards(	)
{
	return 0;
}
	
int	StatusList::draw_board		( )
{
	return 0;
}

int	StatusList::draw_board_info	( )
{
	return 0;
}

int	StatusList::draw  	( )
{
	return 0;
}
