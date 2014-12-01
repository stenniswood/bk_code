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

	int		get_value	(	);
	char	get_run		(	)			  { return value; };
	char	get_suit	(	)	  		  { return suit;  };
	void	flip  		(			   )  { is_face_down = !is_face_down;	};
	void	expose		( bool mExpose )  { is_face_down = !mExpose;   		};
	bool	is_exposed	( )  			  { return !is_face_down;			};
	
	virtual int	draw  	( );

private:
	char	value;
	char	suit;

	bool	is_face_down;

};

#endif
