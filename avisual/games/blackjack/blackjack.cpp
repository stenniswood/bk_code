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

const int CARD_WIDTH = 62;

BlackJack::BlackJack( int mNumber_of_players )
: Control(), hit(-1,-1), stay(-1,-1)
{
	hit.set_text	( "Hit" );
	stay.set_text   ( "Stay");	
	hit.move_to 		 ( 100, 100 );
	stay.move_to		 ( 220, 100 );	
	hit.set_width_height ( 75, 30 );
	stay.set_width_height( 75, 30 );

	house		   = new CardPlayer(4);
	house->set_width_height( 2*CARD_WIDTH, 100 );

	CardPlayer* cp;
	for (int i=0; i<mNumber_of_players; i++)
	{
		cp = new CardPlayer(4);
		cp->set_width_height( 4*CARD_WIDTH, 100 );
		players.push_back( cp );
		number_of_players++;
	} 

	// Add 3 decks:
	Deck* tmp = new Deck();
	deck.push_back(tmp);
	//tmp->load_resources();	
//	tmp = new Deck();
//	deck.push_back(tmp);	
//	tmp = new Deck();
//	deck.push_back(tmp);

	whos_turn_is_it = 0;
//	set_graphic_center( 750, 350 );
}

int	BlackJack::onCreate  (  )
{
	deck[0]->onCreate();
	place_players( 100. );	
	deal();	
	float card_spacing = CARD_WIDTH + 10;	
	vector<CardPlayer*>::iterator	iter = players.begin();
	for ( ; iter!=players.end(); iter++ )
	{
		(*iter)->arrange_cards( card_spacing );
	}
	set_graphic_center();
	place_buttons( whos_turn_is_it );
}

// The start of a new game.
void	BlackJack::deal()
{
	Card* card;
	vector<CardPlayer*>::iterator	iter;

	for (int i=0; i<2; i++)
	{
		// House gets one
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
}

void BlackJack::set_graphic_center( )
{
	cx = width /2 + left;
	cy = height/2 + bottom;
}

void BlackJack::place_buttons( int mPlayerIndex )	
{
	float sx;
	float sy;
	const float below = 75;
	switch (mPlayerIndex)
	{
	case 0 : sx = players[mPlayerIndex]->get_left();
			 sy = players[mPlayerIndex]->get_bottom() - below ;
			 break;
	case 1 : sx = players[mPlayerIndex]->get_left() ;
			 sy = players[mPlayerIndex]->get_bottom() - below ;
			 break;
	case 2 : sx = players[mPlayerIndex]->get_left() ;
			 sy = players[mPlayerIndex]->get_bottom() - below ;
			 break;
	case 3 : sx = players[mPlayerIndex]->get_left() ;
			 sy = players[mPlayerIndex]->get_bottom() - below ;
			 break;
	default: break;
	};
	hit.move_to( sx, sy );
	stay.set_position_right_of( &hit, true, 10 );
}

void BlackJack::place_players( float radius )		// place places around the game's center point.
{
	printf("place_players()  %d\n", number_of_players);
	set_graphic_center();
	
	house->move_to( cx - house->get_width()/2, 400 );
	house->print_positions( );

	// place hit, stay buttons here.
	if (number_of_players>3)
	{
		// circle configuration.
		return;
	}
	float w;
	std::vector<CardPlayer*>::iterator  iter = players.begin();
	if (number_of_players>0)
	{	
		w = (*iter)->get_width()/2.;
		(*iter)->move_to( cx-w, 150 );				// bottom
	}
	if (number_of_players>1) 
	{
		iter++;
		w = (*iter)->get_width();
		(*iter)->move_to( cx-3*radius-w, cy-(*iter)->get_height()/2 );		// goes left
	}
	if (number_of_players>2) {
		iter++;
		
		(*iter)->move_to( cx+3*radius, cy-(*iter)->get_height()/2 );			// goes right
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

int		BlackJack::draw()
{
	printf("\tBlackjack draw.\n");
	print_positions();
	Control::draw();
	
	// Draw title
	Stroke_l(0xFFFFFFFF);
	Fill_l(0xFFFFFF00);
	float centerx = width/2 + left;
	float centery = height + bottom- 36;
	TextMid(centerx, centery, "Black Jack", SerifTypeface, 32 );

#define TEXT_HEIGHT 24
	
	// Draw house + players
	house->draw(  );
		char score_text[40];
		int score = house->get_best_black_jack_score();
		float sx = house->get_width()/2.  + house->get_left();
		float sy = house->get_bottom() - TEXT_HEIGHT;
		sprintf ( score_text, "Score: %d", score  );
		printf  ( "sx=%6.1f;  sy=%6.1f\n", sx, sy ); 
		TextMid ( sx, sy, score_text, SerifTypeface, 16 );
	
	int i=0;
	// Display Manager Players : 
	vector<CardPlayer*>::iterator	iter = players.begin();
	while (  iter != players.end()  )
	{
		(*iter)->draw( );
		iter++;
	}
	
	// Draw Wagers 

	// Draw Scores:	
	Stroke_l(0xFFFFFFFF);
	Fill_l  (0xFFFFFFFF);


	iter = players.begin();
	while ( iter != players.end() )
	{
		sx = (*iter)->get_width()/2.  + (*iter)->get_left();
		sy = (*iter)->get_bottom() - TEXT_HEIGHT;
		score = (*iter)->get_best_black_jack_score();
		sprintf(score_text, "Score: %d", score );
		printf("sx=%6.1f;  sy=%6.1f\n", sx, sy ); 
		TextMid(sx, sy, score_text, SerifTypeface, 16 );
		iter++;	
	} 
	
	// Draw Hit/stay buttons:
	hit.draw();
	stay.draw();	
	return TRUE;
}

