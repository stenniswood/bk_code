#ifndef _HEARTS_H_
#define _HEARTS_H_

#include "card.hpp"
#include "card_player.hpp"
#include "display_manager.hpp"

/* Aces are high.  2's are low.  

*/


class HeartsGame
{
public:
	HeartsGame ( int mNumber_of_players =4 );

	// Deals cards, places one in the center.
	void	setup					(						);
		
	int		number_of_cards_to_start( int mNumber_of_players);
	void	deal					( int n					);
	void	discard_one				(						);
	int		remove_cards			(						);
		
	void	score					(						);
	void	evaluate_winners		(						);	
	void 	set_graphic_center		( float cx, float cy 	);	
	void	place_players			( float radius 			);		// place places around the game's center point.
	void	add_to_display_manager	( DisplayManager* dm 	);	
	Card*	draw_one				(						);

private:
	std::vector<Deck*>			deck;
	std::vector<CardPlayer*>	players;
	CardPlayer					discard_pile;



	int		whos_turn_is_it;
	int 	number_of_players;
	int		number_of_decks;

	float 	cx;
	float	cy;
};


#endif
