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

void	HeartsGame::onPlace			(	)
{
	
}

void	HeartsGame::register_with_display_manager(  )
{
	MainDisplay.add_object( &trick );
	int i=0;

	// Display Manager Players : 
	vector<CardPlayer*>::iterator	iter = players.begin();
	while (  iter != players.end()  )
	{
		MainDisplay.add_object( (*iter) );
		iter++;
	}	
}

Card*	HeartsGame::draw_one				( )
{
	return deck[0]->draw_one();
}


