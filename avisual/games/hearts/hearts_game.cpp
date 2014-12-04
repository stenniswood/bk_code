#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include <math.h>
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <fontinfo.h>
#include <shapes.h>

#include "bcm_host.h"
#include "bk_system_defs.h"
#include "display.h"
#include "control.hpp"
#include "card.hpp"
#include "card_player.hpp"
#include "display.h"
#include "display_manager.hpp"
#include "card_games.h"



HeartsGame::HeartsGame( int mNumber_of_players )
: BasicCardGame(mNumber_of_players), trick(mNumber_of_players)
{
	house = NULL;
	CardPlayer* cp;
	int cards_in_hand = number_of_cards_to_start( mNumber_of_players );	

	// 
	for (int i=0; i<mNumber_of_players; i++)	{
	//	players[i]->set_width_height( 4*CARD_WIDTH, 100 );
	}
	
}

void	HeartsGame::score					( )
{

}
void	HeartsGame::evaluate_winners		( )
{

}

void	HeartsGame::setup(		)
{
	place_players(100.);
	deck[0]->load_resources();
	
	int n_to_deal = number_of_cards_to_start(number_of_players);
	deal		( n_to_deal );
	
	// No discard.  the first person discards to set the suit.
	//discard_one	(			);
	whos_turn_is_it = 0;
}

/*void	HeartsGame::deal( int n	)
{
	printf("Dealing %d cards to each of %d players\n", n, number_of_players);
	Card* card;
	for (int i=0; i<n; i++)
	{
		printf("%d\n", i);
		for (int p=0; p<number_of_players; p++)
		{
			card = draw_one();
			players[p]->receive_card( card, true );
		}
	}
} */

void	HeartsGame::discard_one(	)
{
	Card* card = draw_one();
	if (card)
		trick.receive_card( card );
}

int		HeartsGame::number_of_cards_to_start( int mNumber_of_players )
{
	switch(mNumber_of_players)
	{
	case 3:	return 17;
	case 4: return 13;
	case 5: return 10;
	case 6: return 8;
	case 7:	return 7;
	default: break;
	}
}

int		HeartsGame::remove_cards(	)
{
	Card* card;
	switch(number_of_players)
	{
	case 3:	card = deck[0]->find( '2', 'D' );	// the 2 of diamonds is removed 
			deck[0]->discard( card );
	case 4: ;	// none
	case 5: // two of diamonds and two of clubs are removed;
			card = deck[0]->find( '2', 'D' );		deck[0]->discard( card );
			card = deck[0]->find( '2', 'C' );		deck[0]->discard( card );
			break;
	case 6: ;	// two and three of diamonds and the three and four of clubs are removed
			card = deck[0]->find( '2', 'D' );		deck[0]->discard( card );
			card = deck[0]->find( '3', 'D' );		deck[0]->discard( card );	
			card = deck[0]->find( '3', 'C' );		deck[0]->discard( card );
			card = deck[0]->find( '4', 'C' );		deck[0]->discard( card );
			break;
	case 7:	;	// two and three of diamonds and the three of clubs are removed
			card = deck[0]->find( '2', 'D' );		deck[0]->discard( card );
			card = deck[0]->find( '3', 'D' );		deck[0]->discard( card );	
			card = deck[0]->find( '3', 'C' );		deck[0]->discard( card );
			break;
	default: break;
	}
}
	
// place places around the game's center point.
/*void	HeartsGame::place_players( float radius )
{
	float width = 7*61;
	
	trick.set_width_height( 61*3, 100 );
	trick.move_to( cx-90, cy );
		
	std::vector<CardPlayer*>::iterator  iter = players.begin();
	if (number_of_players>0)
	{	
		(*iter)->set_width_height( 61*7, 100 );		// bottom
		(*iter)->move_to( cx-width/2., 200 );		// bottom
	}
	if (number_of_players>1) 
	{
		iter++;
		(*iter)->set_width_height( 61*7, 100 );		// goes left
		(*iter)->move_to( cx/2-50, cy );			// goes left
	} 
	if (number_of_players>2) {
		iter++;
		(*iter)->set_width_height( 61*7, 100 );		// goes right		
		(*iter)->move_to( 3*cx/2, cy );				// goes right
	}	
	if (number_of_players>3) {
		iter++;
		(*iter)->set_width_height( 61*7, 100 );		// goes right		
		(*iter)->move_to( cx - width/2, 500 );		// goes right
	}
}*/

void	HeartsGame::add_to_display_manager( DisplayManager* dm )
{
	dm->add_object( &trick );
	int i=0;

	// Display Manager Players : 
	vector<CardPlayer*>::iterator	iter = players.begin();
	while (  iter != players.end()  )
	{
		dm->add_object( (*iter) );
		iter++;
	}	
}

Card*	HeartsGame::draw_one				( )
{
	return deck[0]->draw_one();
}


