// line graph OpenVG program 
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "display.h"

#define margin_percent 0.07


Control::Control(int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	Initialize();
}

Control::Control( int Width, int Height )
{
	left=0; bottom=0; 
	width  = Width;
	height = Height;
	Initialize();
}

Control::Control()
{
	set_position( 0, 0, 0, 0 );
	Initialize();	
}

Control::~Control()
{
}

void Control::Initialize() 
{
	created		= false;
	text		= NULL;
	HasBorder 	= true;	
	Visible   	= true;
	text_size 	= 14.0;
	border_roundness_x = 15.0;
	border_roundness_y = 15.0;	
	background_color = 0xCF202020;
	border_color	 = 0xFFFFFFFF;
	text_color		 = 0xFFFFFFFF;
	//printf("Ctrl: border_color=%8x; background_color=%8x\n", border_color, background_color );
}

void Control::print_positions( )
{
	printf("Left=%5.1f; Bottom=%6.1f; width=%6.1f;  height=%6.1f;  \n", left, bottom, width, height );
}

void Control::print_color_info( )
{
	printf("Control:  border_color=%6x; text_color=%6x; background_color=%6x; \n", 
					  border_color,     text_color,     background_color );
}

// Chance to load resources, call functions which use fonts
//  (already loaded before this call) etc.
int	Control::onCreate(  )
{	
	created = true;
	wrap_content();
}

void Control::wrap_content( )
{ 	
	if (created==false)  return;	//if fonts are not yet loaded...
	VGfloat text_width;
	if (((width==-1.) || (height==-1.)) && (text))
	{
		text_width = TextWidth( (char*)text, SerifTypeface, (int)text_size );
		if (width == -1.)
			width  = text_width*1.2;
		if (height== -1.)
			height = text_size*1.5;
		//printf("\t\tControl::wrap_content() Got Called! w=%6.1f h=%6.1f\n", width, height);			
	}
}

void Control::set_text( const char* NewText )
{
// Sometimes we can't do a free()  ie for Static string constants!
// The 2nd call to this function causes a glibc error!
//	if (text) free(text);
//	text = (char*) malloc (strlen(NewText));
	int len = strlen(NewText)+1;
	text = new char[len];		
	strcpy(text, NewText);	
}

void Control::set_text_size( float mTextSize )
{
	text_size = mTextSize;
}

void Control::set_text_color ( long int TextColor )
{
	text_color = TextColor;
}

void Control::set_position_left_of( Control* Sibling, bool mCopyVert, float mPadding )
{
	float Left 	= Sibling->left - mPadding - width;
	move_to ( Left, bottom );
	if (mCopyVert)
		copy_position_vert(Sibling);
}

void Control::set_position_right_of(Control* Sibling, bool mCopyVert, float mPadding )
{
	float Left 	= Sibling->left + Sibling->width + mPadding;
	move_to ( Left, bottom );
	if (mCopyVert)
		copy_position_vert(Sibling);
}

void Control::set_position_above( Control* Sibling, bool mCopyHoriz, float mPadding )
{
	float Bottom  = Sibling->bottom + Sibling->height + DefaultPadding;
	move_to( left, Bottom );
	if (mCopyHoriz)
		copy_position_horiz(Sibling);
}

// The control should already have a height & width.
// mleft, mwidth are optional.
void Control::set_position_below( Control* Sibling, bool mCopyHoriz, float mPadding )
{ 
	float Bottom  = Sibling->bottom - DefaultPadding - height;
	move_to( left, Bottom );
	if (mCopyHoriz) 
		copy_position_horiz(Sibling);
}

void Control::copy_position_horiz	  ( Control* Sibling )
{
	set_width_height( Sibling->width, height );
	move_to			( Sibling->left,  bottom );		
}
void Control::copy_position_vert	  ( Control* Sibling )
{
	set_width_height( width, Sibling->height );
	move_to			( left,  Sibling->bottom );	
}

void Control::set_position( int Left, int Right, int Top, int Bottom )
{
	move_to			( Left,Bottom );
	set_width_height( Right-Left, Top-Bottom );
}

void  Control::move_bottom_to  	 ( float  mNewBottom	)
{
	move_to ( left, mNewBottom );
}
void  Control::move_left_to  	 ( float  mNewLeft	)
{
	move_to ( mNewLeft, bottom );
}

// virtual function (for classes such as scroll view which 
// have to update the scroll bar as well)
void  Control::move_to( float Left, float Bottom )
{
	left   = Left;
	bottom = Bottom;	
}
void Control::load_resources( )
{
}


int Control::draw() 
{
	//printf("Control::draw() \n");
	if (Visible==false)	 return 0;

	StrokeWidth(2);
	if (HasBorder) 
		draw_border();

	return TRUE;
}

int Control::draw_border()
{
	//printf("Control::draw_border() HasBorder!\n");
	
	Stroke_l ( border_color );
	Fill_l   ( background_color );
	Roundrect( left, bottom, width, height, 15.0, 15.0);
}

Control* Control::HitTest(int x, int y)
{
	float right = left+width;
	float top = bottom+height;
	
	if ((x>left) && (x<right)  &&
	    (y>bottom) && (y<top))
	   return this;
	// could add in a border check.  ie. if with 2 pixels of left or right, etc
	else	
		return NULL;
}

int		Control::onClick(int x, int y, bool mouse_is_down)
{
	return -1; 
}

int		Control::onDoubleClick()
{
	return -1;	
}

int		Control::onReceiveFocus()
{
	return -1;
}

void Control::register_child( Control* mNewChild )
{
	m_child_controls.push_back( mNewChild );
}

