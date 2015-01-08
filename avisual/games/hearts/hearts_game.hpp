#ifndef _HEARTS_H_
#define _HEARTS_H_

#include "card.hpp"
#include "card_player.hpp"
#include "basic_card_game.hpp"
#include "display_manager.hpp"
#include "trick.hpp"

/* Aces are high.  2's are low.  

*/

class HeartsGame : public BasicCardGame
{
public:
	HeartsGame ( int mNumber_of_players=4 );

	// Deals cards, places one in the center.
	void	setup					(						);
	void	onPlace					(						);
	
	virtual int		number_of_cards_to_start( int mNumber_of_players);
	void	discard_one				(						);
	int		remove_cards			(						);
		
	void	score					(						);
	void	evaluate_winners		(						);	
	//void 	set_graphic_center		( float cx, float cy 	);	
	//void	place_players			( float radius 			);		// place places around the game's center point.
	void	register_with_display_manager( 					);	
	Card*	draw_one				(						);

protected:
	Trick	trick;

};


#endif
