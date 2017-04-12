#ifndef _TABLE_WINDOW_
#define _TABLE_WINDOW_

#include "window.hpp"
#include "scroll_bar.hpp"
#include <list>
#include <string>
using namespace std;


#define DEFAULT_VSCROLL_WIDTH 10
#define DEFAULT_HSCROLL_HEIGHT 10 

/**************************************
This is an abstract class which has a scroll bar
built in.  Used in classes such as TextView,
ListBox, etc.  Which often need scroll capability.
This offers is readily builtin.
***************************************/

class TableWindow : public Window 
{
public:
	TableWindow();
	TableWindow( int Left, int Right, int Top, int Bottom );
	~TableWindow();
	
	virtual void	Initialize		 (   );	
	virtual int   	draw 			 (	 );
	void			calc_metrics	 (   );

	void			set_column_width( int mWidth  );
	void			set_row_height	( int mHeight );

	void			set_rows		( int mRows )	{ rows = mRows; }
	void			set_cols		( int mCols )	{ cols = mCols; }
	Control*		HitTest			( int x, int y );
	
	// 	

	int				add_control_local_coords	( Control* mControl, int mRow, int mCol );	
	int				add_control_absolute_coords ( Control* mControl, int mRow, int mCol );	

protected:
	int				rows;
	int				cols;
	int				col_width;
	int				row_height;

private:

};

 
#endif
