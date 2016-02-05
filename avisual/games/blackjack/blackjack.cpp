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

#include "card_player_chips.hpp"

#define MAX_PLAYERS 10

//using namespace std;
const int TITLE_HEIGHT = 32;
const int CARD_WIDTH = 62;

#define Debug 1


/* CALLBACK FOR BUTTONS: */
void hit_cb( void* mBlackJack )
{
	if (Debug) printf("Hit button pushed\n");
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
	if (Debug) printf("Stay button pushed\n");
	// House gets one : 
	((BlackJack*)mBlackJack)->next_player();
}

void play_again_cb( void* mBlackJack )
{
	if (Debug) printf("Play Again button pushed\n");
	((BlackJack*)mBlackJack)->start_new_round();	
}

/***********************************************************/

/***********************************************************/
BlackJack::BlackJack( int mNumber_of_m_players )
: Control(), hit(-1,-1), stay(-1,-1), play_again(-1,-1)
{
	number_of_m_players = mNumber_of_m_players;
	
	hit.set_text	( "Hit" );
	stay.set_text   ( "Stay");	
	
	hit.set_on_click_listener( hit_cb, this );
	stay.set_on_click_listener( stay_cb, this );

	play_again.set_on_click_listener( play_again_cb, this );
	play_again.set_width_height( 100, 30 );
	play_again.move_to( 400, 240 );
	play_again.set_text("Play Again");
	play_again.hide();

	// SETUP THE HOUSE : 
	house  = new CardPlayerChips(4);
	house->set_width_height( 4*CARD_WIDTH, 100 );

	// SETUP THE PLAYERS : 
	CardPlayerChips* cp;
	for (int i=0; i<mNumber_of_m_players; i++)
	{
		cp = new CardPlayerChips(4);
		cp->set_width_height( 4*CARD_WIDTH, 100 );
		m_players.push_back( cp );
	} 

	// SETUP THE DECKS : 
	Deck* tmp = new Deck();
	deck.push_back(tmp);
	// If multiple decks are desired, add here.
	whos_turn_is_it = 0;
}
	
CardPlayerChips*	BlackJack::next_player( )	
{
	whos_turn_is_it++;
	printf("NEXT PLAYER:  %d\n", whos_turn_is_it );
	if (whos_turn_is_it == number_of_m_players) 	// dealers turn	
	{
		dealer_play();  
		evaluate_winners();
		play_again.show();
		whos_turn_is_it = 0;
		Invalidate();
	} else {
		place_buttons();
		Invalidate();
	}
	return m_players[whos_turn_is_it];
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
	house->expose_all_cards( true );	
	hit.hide();
	stay.hide();
	Invalidate();
	printf("Dealer play done!\n"); 
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
	
CardPlayerChips*	BlackJack::get_player( int mPlayerIndex )
{
	int index = mPlayerIndex;
	if (index == -1)
		index = whos_turn_is_it;
	if (index >= m_players.size())
		index = 0;
	return m_players[index];
}

void BlackJack::start_new_round (   )
{
	collect_cards();
	setup_game();
	play_again.hide();
	hit.show();
	stay.show();
	Invalidate();	
}
void BlackJack::collect_cards(	)
{
	vector<CardPlayerChips*>::iterator	iter = m_players.begin();
	for ( ; iter != m_players.end(); iter++ )
		(*iter)->discard_all_cards();
	house->discard_all_cards();
}

void BlackJack::arrange_players_cards(	)
{
	float card_spacing = CARD_WIDTH + 10;	
	house->arrange_cards( card_spacing );	

	// Arrange each m_players cards:
	vector<CardPlayerChips*>::iterator iter = m_players.begin();
	for ( ; iter!=m_players.end(); iter++ )
	{
		(*iter)->arrange_cards( card_spacing );
	}
}

void BlackJack::setup_game(	)
{
	// play_again.hide();
	// PLACE PLAYERS AND DEAL:
	place_m_players( 100. );	

	deal();
	
	arrange_players_cards();
	whos_turn_is_it = 0;
	place_buttons( whos_turn_is_it );
}

int	BlackJack::onCreate(  )
{
	printf("\nBJ\n");
	if (Debug) printf("BlackJack::onCreate\n");
	set_graphic_center();
	float sx = m_cx - play_again.get_width() /2.;
	float sy = m_cy - play_again.get_height()/2.;
	printf("PLAY_AGAIN MOVE_TO: %6.1f, %6.1f \n", sx, sy);
	play_again.move_to( sx, sy );

	if (Debug) printf("BlackJack::onCreate - deck\n");
	deck[0]->onCreate();
	if (Debug) printf("BlackJack::onCreate - setup\n");	
	setup_game( );	
}

// The start of a new game.
void	BlackJack::deal()
{
	Card* card;
	vector<CardPlayerChips*>::iterator	iter;
	bool face_up = false;

	for (int i=0; i<2; i++)
	{
		// House gets one
		card = draw_one();
		house->receive_card( card, face_up );
		face_up = true;
		
		// Disperse to each player 1 at a time	
		iter = m_players.begin();
		for ( ; iter!=m_players.end(); iter++ )
		{
			if (Debug) printf("dealing player : card # %d\n", i);	
			card = draw_one();
			(*iter)->receive_card( card, true );
		}
	}	
}

	
void BlackJack::set_graphic_center( )
{
	m_cx = width /2. + left;
	m_cy = height/2. + bottom;
	if (Debug) printf(" m_cx,m_cy = %6.1f,%6.1f \n", m_cx, m_cy );	
}

void BlackJack::onPlace( )	
{
	// This window has already been place by the Application object.	
	set_graphic_center();
	place_m_players	( 100. );
	place_buttons	( -1   );
	
}

void BlackJack::place_m_players( float radius )		// place players around the game's center point.
{
	if (Debug) {
		printf("place_m_players()  %d  ", number_of_m_players );
		print_positions();
	}

	float title_bottom = bottom + height - 1.5*TITLE_HEIGHT;
	float angle_between_players = 360.0 / (number_of_m_players+1);
		
	// PLACE THE HOUSE:
	float house_x = m_cx - house->get_width()/2.;
	float house_y = (title_bottom - m_cy - house->get_height() )/2. + m_cy;
	house->move_to( house_x, house_y );
	if (Debug) house->print_positions( );

	// PLACE 1ST PLAYER:
	float w,yp;
	std::vector<CardPlayerChips*>::iterator  iter = m_players.begin();
	if (number_of_m_players > 0)
	{	
		w = (*iter)->get_width()/2.;
		yp = bottom + (m_cy-bottom-(*iter)->get_height() )/2. ;
		(*iter)->move_to( m_cx-w, yp );				// bottom
		printf("Just placed Player #1 :\t");
		(*iter)->print_positions();
	}
	
	// PLACE 2ND PLAYER:
	if (number_of_m_players>1) 
	{
		iter++;
		w = (*iter)->get_width();
		// was:  m_cx-3*radius-w
		(*iter)->move_to( 10, m_cy-(*iter)->get_height()/2. );		// goes left
		printf("Just placed Player #2 :\t");
		(*iter)->print_positions();

	}
	// PLACE 3RD PLAYER:
	if (number_of_m_players>2) {
		iter++;		
		// was m_cx+3*radius
		w = (*iter)->get_width();
		(*iter)->move_to( width-w-10, m_cy-(*iter)->get_height()/2 );			// goes right
		printf("Just placed Player #3 :\t");
		(*iter)->print_positions();
	}
	// anything else is circle configuration.
	//if (number_of_m_players>3)  // circle configuration.
	//	return;		// Not implemented yet.
}

void BlackJack::place_buttons( int mPlayerIndex )	
{
	float sx;
	float sy;
	const float below = 50;
	int index = mPlayerIndex;
	if (mPlayerIndex == -1)
		index = whos_turn_is_it;

	hit.set_width_height ( 75, 30 );
	stay.set_width_height( 75, 30 );
	//play_again.show();

	// BUTTONS ARE RELATIVE TO THE CARD PLAYERS POSITIONING:
	CardPlayerChips* cp = get_player(index);
	sx = cp->get_left();
	sy = cp->get_bottom() - below;
	if (Debug) printf("place_buttons: sx,sy = %d,%d\n", sx, sy );
	hit.move_to( sx, sy );
	stay.set_position_right_of( &hit, true, 10 );	
}

void	BlackJack::evaluate_winners()
{
	printf("EVALUATE WINNERS!!!\n");
	
	int scores[MAX_PLAYERS];
	int pi = 0;
	int house_score = 0;
	house_score = house->get_best_black_jack_score();
	bool house_bust = (house_score > 21);

	// PAY OUT EACH PLAYER:
	float house_winnings = 0.0;
	vector<CardPlayerChips*>::iterator  iter = m_players.begin();
	for ( ; iter!=m_players.end(); iter++, pi++ )
	{
		scores[pi] = (*iter)->get_best_black_jack_score();
		bool player_bust = (scores[pi] > 21);
		bool win = house_bust && (player_bust==false);
		if (win==false)
			win = (scores[pi] > house_score) && (player_bust==false);
		
		if (win)
		{
			printf("Player #%d won.\n", pi);
			(*iter)->win();		
			house_winnings -= (*iter)->wager;
		} else {
			printf("Player #%d won.\n", pi);		
			(*iter)->lose();
			house_winnings += (*iter)->wager;
		}
	}
	house->in_hand += house_winnings;
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


int	BlackJack::draw_score_text( CardPlayerChips* mcp )
{
	const int TEXT_HEIGHT=24;

	char  score_text[40];
	int   score = mcp->get_best_black_jack_score();
	float sx = mcp->get_left();
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
	Text( sx, sy, score_text, SerifTypeface, 16 );	
}

int BlackJack::draw()
{
	if (Debug) printf("\tBlackjack draw.\n");
	//print_positions();
	Control::draw();

	// Draw title
	Stroke_l(0xFFFFFFFF);
	Fill_l(0xFFFFFF00);
	float leftx = left + 15;
	float centery = height + bottom- 1.5*TITLE_HEIGHT;
	Text(leftx, centery, "Black", SerifTypeface, TITLE_HEIGHT );	
	leftx = left + width-15;
	TextEnd(leftx, centery, "Jack", SerifTypeface, TITLE_HEIGHT );
	
	// Draw house + m_players
	house->draw(  );
	if (whos_turn_is_it>=number_of_m_players)
		draw_score_text( house );
	
	int i=0;
	// Display Players : 
	vector<CardPlayerChips*>::iterator	iter = m_players.begin();
	while (  iter != m_players.end()  )
	{
		(*iter)->draw( );
		iter++;
	}
	// Draw Wagers 

	// Draw Scores:	
	iter = m_players.begin();
	while ( iter != m_players.end() )
	{
		draw_score_text( *iter );
		iter++;	
	}
	
	// Draw Hit/stay buttons:
	hit.draw();
	stay.draw();	
	play_again.draw();
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
