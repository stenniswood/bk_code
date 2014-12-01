#ifndef _BLACKJACK_H_
#define _BLACKJACK_H_

#include <vector>
#include "deck.hpp"
#include "card_player.hpp"
#include "display_manager.hpp"

class BlackJack
{
public:
	BlackJack( int mNumber_of_players );
	
	void	deal();
	void	score();
	void	evaluate_winners();	
	void	pay_out();

	void 	set_graphic_center( float cx, float cy );	
	void	place_players( float radius );		// place places around the game's center point.
												// house at top.  square first, then circle.
	void	add_to_display_manager( DisplayManager* dm );	
	void	load_resources() { deck[0]->load_resources();  };	
	Card*	draw_one();

private:
	std::vector<Deck*>			deck;
	std::vector<CardPlayer*>	players;
	CardPlayer*					house;

	int 	number_of_players;
	int		number_of_decks;

	float 	cx;
	float	cy;
};


#endif
