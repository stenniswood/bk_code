/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "text_view.hpp" 











TaskBar::TaskBar ( ) 
{ }
TaskBar::TaskBar ( int Left, int Right, int Top, int Bottom ) 
{ }
TaskBar::~TaskBar( ) 
{ }

void	TaskBar::Initialize		 (   ) 
{ }	
int   	TaskBar::draw 			 (	 ) 
{ }
void	TaskBar::calc_metrics	 (   ) 
{ }

int		TaskBar::add_control	 ( Control* mControl, char* mText=NULL ) 
{ }
void	TaskBar::hide			 ( bool mHide = true) 
{ }
void	TaskBar::auto_hide		 ( bool mAutoHide = true) 
{ }
void	TaskBar::set_alignment	 ( byte mAlignment = SIDEBAR_ALIGN_CENTER ) 
{ }
int		TaskBar::onClick(int x, int y, bool mouse_is_down=true) 
{ }

