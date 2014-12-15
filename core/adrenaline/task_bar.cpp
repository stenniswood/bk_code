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
#include "adrenaline_windows.h"
#include "display.h"
#include "task_bar.hpp"




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

int		TaskBar::add_control	 ( Control* mControl, char* mText) 
{ }
void	TaskBar::hide			 ( bool mHide ) 
{ }
void	TaskBar::auto_hide		 ( bool mAutoHide) 
{ }
void	TaskBar::set_alignment	 ( byte mAlignment ) 
{ }
int		TaskBar::onClick(int x, int y, bool mouse_is_down) 
{ }

