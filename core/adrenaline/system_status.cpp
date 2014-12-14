








SystemStatusBar ( ) 
{ }
SystemStatusBar ( int Left, int Right, int Top, int Bottom ) 
{ }
~SystemStatusBar( ) 
{ }

void SystemStatusBar::Initialize	 (   ) 
{ }	
int  SystemStatusBar::draw 			 (	 ) 
{ }
void SystemStatusBar::calc_metrics	 (   ) 
{ }

int	 SystemStatusBar::add_control	 ( Control* mControl, char* mText=NULL ) 
{ }
void SystemStatusBar::hide			 ( bool mHide ) 
{ }
void SystemStatusBar::auto_hide		 ( bool mAutoHide ) 
{ }
void SystemStatusBar::set_alignment	 ( byte mAlignment  ) 
{ }
int		onClick(int x, int y, bool mouse_is_down=true) 
{ }
