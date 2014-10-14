#ifndef _ListBox_
#define _ListBox_

#include "control.hpp"


class ListBox : public Control 
{
public:
	ListBox();
	ListBox( int Left, int Right, int Top, int Bottom );

	virtual int   	draw (	);	
	int 			draw_text_top_down();
	int 			draw_text_bottom_up();
	
	void 			ClearItems( );
	void			setItem   ( char* mString );

	void			setOddColor   ( long int mColor ) { OddColor =mColor; };
	void			setEvenColor  ( long int mColor ) { EvenColor=mColor; };
	void			setAlpha	  ( float  mAlpha );
	
//	void	removeItem( char* mString );
//	void	FindItemPtr( char* mString );
//	void	FindTextContaining( char* mString );
//virtual int		onClick();
	
private:
	long int EvenColor;
	long int OddColor;
	
	float LineHeight;
	float TextSize ;	
	char* Linetexts[255];
	int	  numItems;
};

 
#endif
