#ifndef _TABULAR_LISTBOX_
#define _TABULAR_LISTBOX_

#include "control.hpp" 
#include "scroll_bar.hpp"
#include "scroll_control.hpp"
#include "listbox.hpp"

#include <vector>
#include <list>
#include <string>

using namespace std;

#define HEADER_ALIGN_LEFT    0x01
#define HEADER_ALIGN_CENTER  0x02
#define HEADER_ALIGN_RIGHT   0x04

// Make a list of these.  
struct HeaderItemInfo
{
	string	text;
	float 	start_x;
	float 	end_x;
	float	width;
	float	layout_weight;	
	float	min_width;		// 
	float	max_width;		// 
	byte	alignment;		// left,center,right 
};

#define DEFAULT_SCROLL_WIDTH 10

/*  Tabular List Box.
	Add capabillities for:
		Stretching.  Widths weights for proportional expansion.
		Min/Max for widths
		Scrolling tabs (when more than visible in the control width)
		Automatic Padding between columns when possible.
*/

class TabularListBox : public ListBox
{
public:
	TabularListBox( );
	TabularListBox( int Left, int Bottom, int mWidth, int mNumber_items_shown, int mItem_height );
	TabularListBox( int Left, int Right, int Top, int Bottom  );
	TabularListBox( int mWidth, int mHeight );
	~TabularListBox();

	virtual	void 	Initialize();

	virtual int   	draw 					(	);
	//int 			draw_line_backgrounds	(   );
	int 			draw_vertical_lines 	(   );
	int 			draw_header				(	);
	virtual void	draw_one_cell			( int mRow, int mCol, float mY );
	void		 	draw_one_row			( int mRow, float mY 		   );

	void			print_tab_info			(   );
	void			print_client_rect		(   );		// body_height, scroll_bar width
	void			clear_items				( );
	
	virtual int		onCreate	  			(  	);	// after loading resources, call functions which use fonts (already loaded before this call) etc.	
	void			calc_metrics			(   );
	void		 	calc_widths_from_text	( int mNotToExceed=-1 );
	void 			calc_column_positions_from_widths( );
	virtual void  	move_to					( float Left, float Bottom );
	virtual void 	set_width_height   		( int Width, int Height );

	virtual int		get_total_lines			( 	)	{ /*printf("tabularListbox::gettotal_lines\n");*/ return LineData.size();  };
	vector<string>*	get_line_data			( int mIndex );
	void			set_headings			( vector<struct HeaderItemInfo> *mHeaderTexts );
	void			set_column_width		( int mColumn, int mWidth );	
	void 			change_header_titles	( string mHeaderTexts, int column );
	void			set_row_col_text		( string mNewText, int row, int col );

	// Does not copy the structure !!
	void			add_column 				( struct HeaderItemInfo* mNewHeading );		// row data will be increased and blanked!
	void			update_col_data			( vector<string> *mNewColText, int mColumn );	// does not update the header!
	void			add_row   				( vector<string> *mData  );
	int				find_item 				( int index, int mcolumn );

	virtual void	select					( int mIndex );
	void			adjust_height_for_num_visible_items ( int mNumber_items_shown );

protected : 
	vector<struct HeaderItemInfo> 	Headings;
	vector< vector<string> >		LineData;

	
};

 
#endif

