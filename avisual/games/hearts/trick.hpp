#ifndef _TRICK_H_
#define _TRICK_H_

#include "control.hpp"
#include <list>

// Each Player's card will be drawn vertically first and later rotated.
// this simplifies the draw() a little.

class Trick : public CardPlayer
{
public:
	Trick( int MaxCardsInHand );

	bool	receive_card	( Card* mNewCard 	  );
	void	unload_on_player( CardPlayer* mPlayer );
	
private:

	
};


#endif
