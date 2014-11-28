
#include "control.hpp"
#include <list>


// Each Player's card will be drawn vertically first and later rotated.
// this simplifies the draw() a little.

class CardPlayer : public Control
{
public:
	CardPlayer();

	void	receive_card( Card* mNewCard, bool mExposed = false );
	void	flip_card	( Card* mNewCard 						);
	void	expose_card	( Card* mNewCard, bool mExposed = false );

	int		arrange_cards( );
	virtual int	draw  	 ( );
		
private:
	std::list<Card*>	cards;
	
	int		number_cards_in_hand;
	int		number_exposed_cards;

};
