#ifndef _CARD_PLAYER_CHIPS_HPP_
#define _CARD_PLAYER_CHIPS_HPP_


//#include "control.hpp"
#include "card_player.hpp"


class CardPlayerChips : public CardPlayer
{
public:
	CardPlayerChips( int MaxCardsInHand );

	void	 set_wager		( float mWager 			 );
	void	 increase_wager ( float mIncrement = 5.0 );
	void	 decrease_wager ( float mIncrement = 5.0 );

	void	 buy_in		( float mDollarAmount 	);

	virtual void	win	 ( );
	virtual void	lose( );

	virtual  int draw	( );
		
	float		in_hand;
	float		wager;

};


#endif
