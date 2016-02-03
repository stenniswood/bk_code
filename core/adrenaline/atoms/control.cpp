// line graph OpenVG program 
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
//#include "../core/can/bk_system_defs.h"
#include "Graphbase.hpp"
#include "rectangle.hpp"
#include "control.hpp"
#include "display.h"


#define margin_percent 0.07
#define Debug 0

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
	invalidated 	 = true;		// force a draw on init.
	z_order          = 0;
	//printf("Ctrl: border_color=%8x; background_color=%8x\n", border_color, background_color );
}

void Control::Revalidate( )
{
	invalidated = false;
	// AND Update all child controls:
	std::vector<Control*>::iterator iter = m_child_controls.begin();
	while ( iter != m_child_controls.end() )
	{ 	
		(*iter)->Revalidate();
		iter++; 
	}		
}
bool Control::is_invalid( )
{
	if (invalidated) return true;
	std::vector<Control*>::iterator iter = m_child_controls.begin();
	while ( iter != m_child_controls.end() )
	{
		if ((*iter)->is_invalid())
			return true;		
		iter++;
	}	
	return false;
}

bool  Control::is_visible( )
{
	return Visible;  
}
void Control::hide ( ) 					
{
	Visible = false; 
}

void Control::show( bool mShow ) 	
{
	 Visible = mShow; 
};


float Control::get_left_( 		)	
{ 
	return left;   
}
float Control::get_bottom( 	)
{
 return bottom; 
}

void Control::print_positions( )
{
	printf("Left=%6.1f; Bottom=%6.1f;  width=%6.1f; height=%6.1f;  \n", 
					left, bottom, width, height );
}

void Control::print_color_info( )
{
	printf("Control:  border_color=%6x; text_color=%6x; background_color=%6x; \n", 
					  border_color,     text_color,     background_color );
}

// Chance to load resources, call functions which use fonts
//  (already loaded before this call) etc.
// Return:		1 => Action was performed
//				0 => already created
int	Control::onCreate(  )
{	
	if (created)	return 0;
	
	created = true;
	int count=0;

	// Create All Children : 
	std::vector<Control*>::iterator iter = m_child_controls.begin();
	while ( iter != m_child_controls.end() )
	{
		if (Debug) printf("Control::onCreate() Child Create: %d %x\n", count++, *iter );
		(*iter)->onCreate();
		iter++; 
	}	
	return 1;  
}

void Control::wrap_content( ) 
{ 	
	//if (created==false)  return;	//if fonts are not yet loaded...
	VGfloat text_width;
	//if (((width==-1.) || (height==-1.)) && (text))
	if (text)
	{
		text_width = TextWidth( (char*)text, SerifTypeface, (int)text_size );
		//if (width == -1.)
			width  = text_width*1.2;
		//if (height== -1.)
			height = text_size*1.5;
		//printf("\t\tControl::wrap_content() Got Called! w=%6.1f h=%6.1f\n", width, height);			
	}
}

// Allocates and copies!
void Control::set_text( const char* NewText, bool mWrapContent )
{
// Sometimes we can't do a free()  ie for Static string constants!
// The 2nd call to this function causes a glibc error!
//	text = (char*) malloc (strlen(NewText));
	if (text) free(text);
	int len = strlen(NewText)+1;
	text = new char[len];		
	strcpy(text, NewText);	
	if (mWrapContent)
		wrap_content();
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
	float Bottom  = Sibling->bottom + Sibling->height + mPadding;
	move_to( left, Bottom );
	if (mCopyHoriz)
		copy_position_horiz(Sibling);
}

// The control should already have a height & width.
// mleft, mwidth are optional.
void Control::set_position_below( Control* Sibling, bool mCopyHoriz, float mPadding )
{ 
	float Bottom  = Sibling->bottom - mPadding - height;
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

void Control::set_position( Rectangle* mRect )
{
	move_to( mRect->get_left(), mRect->get_bottom() ); 
	float w = mRect->get_width();  // - mRect->get_left();
	float h = mRect->get_height(); // - mRect->get_bottom();
	set_width_height( w, h );	
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

void Control::set_width_height ( int Width, int Height )
{
	width = Width; 
	height=Height; 
}

// virtual function (for classes such as scroll view which 
// have to update the scroll bar as well)
void  Control::move_to( float Left, float Bottom )
{ 
	float deltaX = left   - Left;
	float deltaY = bottom - Bottom;
	
	left   = Left;
	bottom = Bottom;	

	Control* retval = NULL;
	std::vector<Control*>::iterator iter = m_child_controls.begin();
	while ( iter != m_child_controls.end() )
	{
		(*iter)->move_to( (*iter)->get_left()+deltaX, (*iter)->get_bottom()+deltaY );
		iter++;
	}
}

void Control::move_by( int DeltaX, int  DeltaY	) 
{
	left+=DeltaX; bottom+=DeltaY; 
}

void Control::load_resources( )
{
}

bool Z_compare (Control* i,Control* j) 
{
 	if (Debug)  printf("%d %d ", i, i->z_order); 
	return (i->z_order < j->z_order); 
}

void Control::set_z_order( long int& mZorder )
{
	z_order = mZorder;
	
	// How should we do this?  child controls go in front always?  
	// in the file browser, we wanted them at lower level so that the clicks came to 
	// the parent FileBrowser objects.
	std::vector<Control*>::iterator iter = m_child_controls.begin();
	while ( iter != m_child_controls.end() )
	{
		(*iter)->set_z_order( mZorder );
		iter++;
	}
}

void Control::sort_z_order() 
{
	std::sort( m_child_controls.begin(), m_child_controls.end(), Z_compare );
}

int Control::draw() 
{
	if (Visible==false)	 return 0;
	if (invalidated)
		erase();
	
	StrokeWidth(2);
	if (HasBorder) 
		draw_border();

	// Draw Any Children:
	std::vector<Control*>::iterator iter = m_child_controls.begin();
	while ( iter != m_child_controls.end() )
	{ 	
		//printf("drawing child control..\t");
		//(*iter)->print_positions();
		(*iter)->draw();
		iter++; 
	};
	return TRUE;
}

int Control::erase()
{
	/* Erase background of the control */
	Stroke_l ( 0x00000000 );
	Fill_l   ( 0x00000000 );
	Rect( left, bottom, width, height);
}

int Control::draw_border()
{
	//printf("Control::draw_border() HasBorder!\n");
	
	Stroke_l ( border_color );
	Fill_l   ( background_color );
	Roundrect( left, bottom, width, height, 15.0, 15.0);
}

Control* Control::ChildrenHitTest( int x, int y )
{
	if (Visible==false) return NULL;

	std::vector<Control*> hit_objects;
	Control* retval = NULL;
	
	std::vector<Control*>::iterator iter = m_child_controls.begin();
	while ( iter != m_child_controls.end() )
	{
		retval = (*iter)->HitTest( x,y );		
		if (retval) {
			//printf("Hit: %s\t", retval);
			(*iter)->print_positions();
			hit_objects.push_back( retval );
		}
		iter++;
	}	
	return FindHighestZOrder( hit_objects );
}

Control* Control::FindHighestZOrder( std::vector<Control*> &mObjects )
{
	Control* highest_control = NULL;
	long int highest = -1;
	for (int i=0; i<mObjects.size(); i++)
		if (mObjects[i]->z_order > highest)
		{
			highest = mObjects[i]->z_order;
			highest_control = mObjects[i];
		}
	if (highest_control)
		if (Debug) printf("overlapping objects=%d, highest Z order: %x \n", mObjects.size(), highest_control );
	return highest_control;
}

Control* Control::HitTest(int x, int y)
{
	if (Visible==false) return NULL;

	Control* result = ChildrenHitTest( x, y );
	if (result) {
		return result;
	}

	float right = left+width;
	float top   = bottom+height;
	
	if ((x>left) && (x<right) && (y>bottom) && (y<top))
	{
	   return this;
	   // could add in a border check.  ie. if with 2 pixels of left or right, etc
	} else	
		return NULL;
}
int	Control::onHover ( int x, int y	) 
{
	return 0;
};

int	Control::onClick(int x, int y, bool mouse_is_down)
{
	Control* result = ChildrenHitTest(x,y);
	if (result)
	{
		if (Debug) printf("Control::onClick()  Clicked a registered child.\n");
		result->onClick(x,y,mouse_is_down);
		return 1;
	}
	return -1;
}
int	Control::onRelease( int x, int y ) 
{
	return 0; 
}

int	Control::onKey( char mKey )
{
	return 0; 
}

int	Control::onDoubleClick()
{
	return -1;	
}

int	Control::onReceiveFocus()
{
	return -1;
}

void Control::register_child( Control* mNewChild )
{
	m_child_controls.push_back( mNewChild );
}

void Control::print_children( )
{
	std::vector<Control*>::iterator iter = m_child_controls.begin();
	while ( iter != m_child_controls.end() )  
	{ 
		if (Debug) printf(" Children:  %x \t", *iter ); 
		(*iter)->print_positions();
		iter++; 
	};		
}

void Control::unregister_child	( Control* mNewChild )
{
	if (Debug) printf("\t\tControl::unregister_child( %x )\n", mNewChild );	

	std::vector<Control*>::iterator iter = m_child_controls.begin();
	while (( *iter != mNewChild ) && (iter != m_child_controls.end()))
	{ iter++; };

	if (Debug) printf("Unregistering_child( %x )\n", *iter );
	m_child_controls.erase( iter );
}

