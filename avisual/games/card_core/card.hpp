#ifndef _CARD_H_
#define _CARD_H_

#include "icon.hpp"


extern char const* e_card_names[];
class Deck;


/* Value:  [1..10,J,Q,K,A] 
   Suit :  [C,C,S,D]  { Hearts, Clubs, Spades, Diamonds }	*/
class Card : public IconView
{
	friend class Deck;
public:
	Card	( char* mFilename, char Value, char suit );
	Card	( int  mIndex );

	char	get_value	(	)			{ return value; };
	char	get_suit	(	)	  		{ return suit;  };
	void	flip  		(			   )  { is_face_down = !is_face_down;	};
	void	expose		( bool mExpose )  { is_face_down = !mExpose;   		};

	virtual int	draw  	( );

private:
	char	value;
	char	suit;

	VGImage	image;
	bool	is_face_down;

};

#endif
