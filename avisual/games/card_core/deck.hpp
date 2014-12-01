#ifndef _DECK_H_
#define _DECK_H_

#include <vector>
#include "card.hpp"


class Deck
{
public:
	Deck( );
	
	void	load_resources();	

	Card*	draw_one();
	void	discard	( Card* mCard );

	void 	reuse_discarded();		// call when action is required. (ie not a state)
	
	void	shuffle();
	void	print_order();	
	void 	order();
		
private:
	std::vector<Card*>	theDeck;
	std::vector<int>	shuffled_indices;	// these are indices into theDeck.

	
};


#endif
