#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "bcm_host.h"
#include <fontinfo.h>
#include <shapes.h>
#include "bk_system_defs.h"
#include "display.h"
#include "control.hpp"
#include "card.hpp"
#include "card_player.hpp"

#include "display.h"
#include "display_manager.hpp"
#include "blackjack.hpp"

#define MAX_PLAYERS 10

using namespace std;


BlackJack::BlackJack( int mNumber_of_players )
{
	house		   = new CardPlayer(4);
	//house->set_width_height( 2*62, 100 );

	CardPlayer* cp;
	for (int i=0; i<mNumber_of_players; i++)
	{
		cp = new CardPlayer(4);
		//cp->set_width_height( 2*62, 100 );
		players.push_back( cp );
		number_of_players++;
	}

	// Add 3 decks:
	Deck* tmp = new Deck();
	//tmp->load_resources();	
	deck.push_back(tmp);
//	tmp = new Deck();
//	deck.push_back(tmp);	
//	tmp = new Deck();
//	deck.push_back(tmp);

	set_graphic_center( 750, 350 );
	place_players( 100. );	
}

void BlackJack::add_to_display_manager( DisplayManager* dm )
{	
	dm->add_object( house );
	int i=0;
	// Display Manager Players : 
	vector<CardPlayer*>::iterator	iter = players.begin();
	while (  iter != players.end()  )
	{
		dm->add_object( (*iter) );
		iter++;
	}
}

// The start of a new game.
void	BlackJack::deal()
{
	Card* card;
	
	// House gets one.
	card = draw_one();
	house->receive_card( card, true );
	
	// Disperse to each player 1 at a time	
	vector<CardPlayer*>::iterator	iter = players.begin();
	for ( ; iter!=players.end(); iter++ )
	{
		card = draw_one();
		(*iter)->receive_card( card, true );
	}

	// House gets one.
	card = draw_one();
	house->receive_card( card, true );

	// Disperse to each player 1 at a time	
	iter = players.begin();
	for ( ; iter!=players.end(); iter++ )
	{
		printf("dealing player : 2nd card\n");	
		card = draw_one();
		(*iter)->receive_card( card, true );
	}
}

void BlackJack::set_graphic_center( float mcx, float mcy )
{
	cx = mcx;
	cy = mcy;
}

void BlackJack::place_players( float radius )		// place places around the game's center point.
{
	printf("place_players()  %d\n", number_of_players);
	
	house->move_to( cx-70, 500 );
	house->print_positions();
	
	// place hit, stay buttons here.
	if (number_of_players>3)
	{
		// circle configuration.
		return;
	}
	std::vector<CardPlayer*>::iterator  iter = players.begin();
	if (number_of_players>0)
	{	
		(*iter)->move_to( cx-70, 200 );				// bottom
	}
	if (number_of_players>1) 
	{
		iter++;
		(*iter)->move_to( cx-3*radius-70, cy );		// goes left
	} 
	if (number_of_players>2) {
		iter++;
		(*iter)->move_to( cx+3*radius, cy );		// goes right
	}
	// anything else is circle configuration.	
}

void	BlackJack::evaluate_winners()
{
	int scores[MAX_PLAYERS];
	int pi = 0;
	int house_score = 0;
	house_score = house->get_best_black_jack_score();
		
	// Add them all up.
	vector<CardPlayer*>::iterator  iter = players.begin();
	for ( ; iter!=players.end(); iter++, pi++ )
	{
		scores[pi] = (*iter)->get_best_black_jack_score();
		if (scores[pi] > house_score)
			(*iter)->set_winner(true);		
		else 
			(*iter)->set_winner(false);
	}
		
}
// closest to 21 without going over.	
// add them all up.

void	BlackJack::pay_out()
{
	
}

Card*	BlackJack::draw_one()
{
	return deck[0]->draw_one();
}

