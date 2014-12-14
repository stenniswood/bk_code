#ifndef _BASICCARD_H_
#define _BASICCARD_H_

#include <vector>
#include "control.hpp"
#include "button.hpp"
#include "deck.hpp"
#include "card_player.hpp"
#include "display_manager.hpp"


extern const int CARD_WIDTH;

class BasicCardGame : public Control
{
public:
	BasicCardGame( int mNumber_of_players );
	~BasicCardGame();

	virtual int		number_of_cards_to_start( int mNumber_of_players);
	void			deal				( int n_Cards );
	virtual void	score				(	);
	int				get_score			( int mPlayerIndex );

	void		setup_game				(	);		
	void		evaluate_winners		(	);	
	void		pay_out					(	);

	void 		place_buttons	( int mPlayerIndex 	);
	void		place_players	( float radius 		);	// place places around the game's center point.
														// house at top.  square first, then circle.
	void		load_resources	(	) { deck[0]->load_resources();  };	
	Card*		draw_one		(	);		
	virtual int	draw  	 		(	);
	virtual int	onCreate  		(	);

protected:
	char*						NameOfTheGame;
	Button 						hit ;
	Button 						stay;
	std::vector<Deck*>			deck;
	std::vector<CardPlayer*>	players;
	CardPlayer*					house;
	
	int		whos_turn_is_it;		// player index
	int 	deck_index;
	int 	number_of_players;
	int		number_of_decks;

	float 	cx;
	float	cy;
};


#endif
