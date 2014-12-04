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

void hit_cb( void* mBlackJack )
{
	printf("Hit button pushed\n");
	// House gets one
	Card* card = ((BlackJack*)mBlackJack)->draw_one();
	CardPlayer* cp = ((BlackJack*)mBlackJack)->get_player(  );
	cp->receive_card( card, true );
	cp->arrange_cards( );
	int score = cp->get_best_black_jack_score();
	//if (score>21)
		// skip to next player.
	cp->draw();
}

void stay_cb( void* mBlackJack )
{
	printf("Stay button pushed\n");
	// House gets one : 
	((BlackJack*)mBlackJack)->next_player();
}

void play_again( void* mBlackJack )
{
	printf("Play Again button pushed\n");
	((BlackJack*)mBlackJack)->setup_game();
}

BlackJack::BlackJack( int mNumber_of_players )
: Control(), hit(-1,-1), stay(-1,-1), play_again(-1,-1)
{
	hit.set_text	( "Hit" );
	stay.set_text   ( "Stay");	
	hit.move_to 		 ( 100, 100 );
	stay.move_to		 ( 220, 100 );	
	hit.set_width_height ( 75, 30 );
	stay.set_width_height( 75, 30 );
	
	hit.set_on_click_listener( hit_cb, this );
	stay.set_on_click_listener( stay_cb, this );
	play_again.set_text("Play Again");
	play_again.hide();

	house		   = new CardPlayer(4);
	house->set_width_height( 4*CARD_WIDTH, 100 );

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
}
	
CardPlayer*	BlackJack::next_player( )	
{
	whos_turn_is_it++;
	if (whos_turn_is_it == number_of_players) 	// dealers turn	
		dealer_play();
	place_buttons();
	Invalidate();
	return players[whos_turn_is_it];
}

int	BlackJack::dealer_play(	)
{	
	int decision = dealer_hits( );
	while (decision)
	{
		Card* card = draw_one( );
		house->receive_card  ( card, true );
		house->arrange_cards ( );
		house->draw();
		decision = dealer_hits( );
	}
	hit.hide();
	stay.hide();
	play_again.show();
	Invalidate();
}
/*
return	0 - stay
		1 - hit
*/
int	BlackJack::dealer_hits( )
{
	int score = house->get_best_black_jack_score();
	if (score<17)
		return 1;
		
	// Some casinos hit on soft 17
	if (score==17)
	{
		// (ie. A+6, A+2+4, A+5+A, etc)
		if (house->is_ace_in_hand())
			return 1;
	}
	return 0;	
}
	
CardPlayer*	BlackJack::get_player( int mPlayerIndex )
{
	int index = mPlayerIndex;
	if (index == -1)
		index = whos_turn_is_it;
	if (index >= players.size())
		index = 0;
	return players[index];
}

void BlackJack::setup_game(	)
{
	play_again.hide();
	place_players( 100. );	
	deal();	
	float card_spacing = CARD_WIDTH + 10;	
	vector<CardPlayer*>::iterator	iter = players.begin();
	for ( ; iter!=players.end(); iter++ )
	{
		(*iter)->arrange_cards( card_spacing );
	}
	whos_turn_is_it = 0;
	place_buttons( whos_turn_is_it );
}

int	BlackJack::onCreate  (  )
{
	printf("onCreate\n");
	float sx = width/2. + left   - play_again.get_width()/2.;
	float sy = height/2. + bottom - play_again.get_height()/2.;	
	play_again.move_to( sx, sy );

	printf("onCreate - deck\n");
	deck[0]->onCreate();
	printf("onCreate - setup\n");	
	setup_game( );	
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
	int index = mPlayerIndex;
	if (mPlayerIndex == -1)
		index = whos_turn_is_it;

	CardPlayer* cp = get_player(index);
	sx = cp->get_left();
	sy = cp->get_bottom() - below;
	printf("place_buttons:  sx,sy = %d,%d\n", sx, sy );	
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


int	BlackJack::draw_score_text( CardPlayer* mcp )
{
	const int TEXT_HEIGHT=24;

	char  score_text[40];
	int   score = mcp->get_best_black_jack_score();
	float sx = mcp->get_width()/2.  + mcp->get_left();
	float sy = mcp->get_bottom() - TEXT_HEIGHT;

	if (score > 21)
	{
		Stroke_l(0xFFFF0000);
		Fill_l  (0xFFFF0000);		
		sprintf ( score_text, "Bust!" );
	}
	else {
		Stroke_l(0xFFFFFFFF);
		Fill_l  (0xFFFFFFFF);
		sprintf(score_text, "Score: %d", score );
	}
	TextMid ( sx, sy, score_text, SerifTypeface, 16 );	
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

	// Draw house + players
	house->draw(  );
	if (whos_turn_is_it>=number_of_players)
		draw_score_text( house );
	
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
	iter = players.begin();
	while ( iter != players.end() )
	{
		draw_score_text( *iter );
		iter++;	
	}
	
	// Draw Hit/stay buttons:
	hit.draw();
	stay.draw();	
	return TRUE;
}

int BlackJack::onClick( int x, int y, bool mouse_is_down )
{
	Control* result = NULL;
	result = hit.HitTest(x,y);
	if (result) {
		hit.onClick(x,y,mouse_is_down);
		//printf ("DisplayManager::HitTest:  found a hit. child number %d\n", i );		
	}

	result = stay.HitTest(x,y);
	if (result)
		stay.onClick(x,y,mouse_is_down);

	result = play_again.HitTest(x,y);
	if (result)
		play_again.onClick( x, y, mouse_is_down );
		
	return -1; 
}
