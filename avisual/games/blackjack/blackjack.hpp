#ifndef _BLACKJACK_H_
#define _BLACKJACK_H_

#include <vector>
#include "control.hpp"
#include "button.hpp"
#include "deck.hpp"
#include "card_player.hpp"
#include "display_manager.hpp"


class BlackJack : public Control
{
public:
	BlackJack( int mNumber_of_players );
	
	void		deal			(	);
	void		score			(	);
	int			get_score		( int mPlayerIndex );

	CardPlayer*	get_player		( int mPlayerIndex=-1 );
	CardPlayer*	next_player		( 	);

	void		setup_game		(	);		
	void		evaluate_winners(	);	
	void		pay_out			(	);
	int			dealer_hits		(	);
	int			dealer_play		(	);
		
	void 		set_graphic_center( );	
	void 		place_buttons	( int mPlayerIndex =-1	);
	void		place_players	( float radius 		);	// place places around the game's center point.
														// house at top.  square first, then circle.
	void		add_to_display_manager( DisplayManager* dm );	
	void		load_resources	(	) { deck[0]->load_resources();  };	
	Card*		draw_one		(	);		

	int			draw_score_text	( CardPlayer* mcp );
	virtual int	draw  	 		(	);
	virtual int		onCreate  	(	);
	int 			onClick( int x, int y, bool mouse_is_down );

private:
	std::vector<Deck*>			deck;
	std::vector<CardPlayer*>	players;
	CardPlayer*					house;
	Button hit ;
	Button stay;
	Button play_again;

	int		whos_turn_is_it;		// player index
	int 	deck_index;
	int 	number_of_players;
	int		number_of_decks;

	float 	cx;
	float	cy;
};


#endif
