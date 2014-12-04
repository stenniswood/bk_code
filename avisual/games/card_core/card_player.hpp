#ifndef _CARD_PLAYER_
#define _CARD_PLAYER_


#include "control.hpp"
#include <list>


// Each Player's card will be drawn vertically first and later rotated.
// this simplifies the draw() a little.

class CardPlayer : public Control
{
public:
	CardPlayer(int MaxCardsInHand );

	void	receive_card( Card* mNewCard, bool mExposed = false );
	void	flip_card	( Card* mNewCard 						);
	void	expose_card	( Card* mNewCard, bool mExposed = false );
	void	expose_all_cards( bool mExposed = false );
	
	float	determine_card_spacing();
	int		get_total_value	( );
	int		get_best_black_jack_score( );
	bool	is_ace_in_hand	( );	
	void	set_winner		( bool mWon )	{ winner = mWon;  };
	bool	is_winner		( )	{ return winner;  };

	float	get_one_card_width ();
	float	get_one_card_height();
	
	int		arrange_cards(  float mCardSpacing=-1  );
	virtual int	draw  	 ( );

protected:
	std::list<Card*>	cards;
	float		card_spacing;
	int			max_number_cards_in_hand;
	int			number_cards_in_hand;
	int			number_exposed_cards;
	bool		winner;
	
};


#endif
