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
#include "CAN_Interface.h"
//#include "dataset.hpp"
//#include "adrenaline_windows.h"
//#include "display_manager.hpp"
//#include "visual_memory.hpp"
#include <vector>
#include "listbox_icon.hpp"

#define Debug 1




IconListBox::IconListBox()
{
}
IconListBox::IconListBox( int Left, int Right,  int Top,    int Bottom  )
{
}
IconListBox::IconListBox( int Width, int Height )
{
}
IconListBox::~IconListBox()
{
}

void IconListBox::Initialize			( )
{
	strcpy (class_name, "IconListBox");		
}
int	IconListBox::onPlace(  )	// chance to place children.
{ return 1;
}
int	IconListBox::onCreate(  )	// chance to load resources, 
{ return 1;
}

int	IconListBox::onClick( int x, int y, bool mouse_is_down )
{ return 1;
}
