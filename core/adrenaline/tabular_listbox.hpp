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
	int		width;
	byte	alignment;		// left,center,right	
};

#define DEFAULT_SCROLL_WIDTH 10

class TabularListBox : public ListBox
{
public:
	TabularListBox();
	TabularListBox( int Left, int Bottom, int mWidth, int mNumber_items_shown, int mItem_height );
	TabularListBox( int Left, int Right, int Top, int Bottom  );
	~TabularListBox();
	
	virtual	void 	Initialize();

	virtual int   	draw 					(	);	
	int 			draw_line_backgrounds	(   );
	int 			draw_vertical_lines 	(   );
	int 			draw_header				(	);
	virtual void	draw_one_cell			( int mRow, int mCol, float mY );
	void		 	draw_one_row			( int mRow, float mY );
	int 			draw_text_top_down		(	);
	int 			draw_text_bottom_up		(	);
	
	void			print_tab_info			(   );
	void			print_client_rect		(   );		// body_height, scroll_bar width
	
	void			calc_metrics			(   );
	void		 	calc_widths_from_text	( int mNotToExceed=-1 );
	void 			calc_column_positions_from_widths( );
	virtual void  	move_to					( float Left, float Bottom );
	virtual void 	set_width_height   		( int Width, int Height );

	vector<string>*	get_line_data			( int mIndex );
	void			set_headings			( vector<struct HeaderItemInfo> *mHeaderTexts );
	void			set_column_width		( int mColumn, int mWidth );	
	void 			change_header_titles	( string mHeaderTexts, int column );

	void			add_column 				( struct HeaderItemInfo* mNewHeading );	// row data will be increased and blanked!
	void			update_col_data			( vector<string> *mNewColText, int mColumn );	// does not update the header!
	void			add_row   				( vector<string> *mData  );
	int				find_item 				( int index, int mcolumn );

	void			adjust_height_for_num_visible_items ( int mNumber_items_shown );
	
protected:
	void 			initialize();
	float			body_height;		// pixels
	float			header_height;		// pixels
	float			header_text_size;
	long int		header_text_color;
	long int		header_border_color;
	long int		header_background_color;

	vector<struct HeaderItemInfo> 	*Headings;
	vector< vector<string> >		LineData;
	
private:

	
};

 
#endif

