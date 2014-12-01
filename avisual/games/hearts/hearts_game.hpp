#ifndef _HEARTS_H_
#define _HEARTS_H_

#include "card.hpp"

class HeartsGame
{
public:
	HeartsGame ( int mNumber_of_players =4 );

	void	deal();
	void	score();
	void	evaluate_winners();	
	void 	set_graphic_center( float cx, float cy );	
	void	place_players( float radius );		// place places around the game's center point.
	void	add_to_display_manager( DisplayManager* dm );
	
	Card*	draw_one();

private:
	int		number_of_players;
//	int		;

};


#endif
