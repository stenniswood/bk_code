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
#include "basic_card_game.hpp"

#define MAX_PLAYERS 10

using namespace std;

const int TITLE_HEIGHT = 32;
const int CARD_WIDTH = 62;

/* CALLBACK FOR BUTTONS: */
void hearts_hit_cb( void* mHearts )
{
}
void hearts_stay_cb( void* mHearts )
{
}


BasicCardGame::BasicCardGame( int mNumber_of_players )
: Control(), hit(-1,-1), stay(-1,-1)
{
	NameOfTheGame = new char[10];
	strcpy(NameOfTheGame, "Hearts");

	hit.set_text	( "Hit", true );
	stay.set_text   ( "Stay", true);		
	hit.move_to 		 ( 100, 100 );
	stay.move_to		 ( 220, 100 );	
	hit.set_on_click_listener( hearts_hit_cb, this );
	stay.set_on_click_listener( hearts_stay_cb, this );

	house  = new CardPlayerChips(4);
	house->set_width_height( 4*CARD_WIDTH, 100 );

	// CREATE THE PLAYERS:
	CardPlayer* cp;	
	for (int i=0; i<mNumber_of_players; i++)
	{
		cp = new CardPlayer ( 13 );
		cp->set_width_height( 6*CARD_WIDTH, 100 );
		players.push_back( cp );
	}

	// Add 3 decks:
	Deck* tmp = new Deck();
	deck.push_back(tmp);
	
	whos_turn_is_it = 0;
}

BasicCardGame::~BasicCardGame()
{
	delete NameOfTheGame;
	NameOfTheGame = NULL;
}

void BasicCardGame::load_resources	(	) 
{
	deck[0]->load_resources();  
}

void BasicCardGame::setup_game(	)
{
	place_players( 100. );
	int starting_card_count = number_of_cards_to_start( players.size() );
	printf("BasicCardGame::onCreate() start with %d cards\n", starting_card_count );
	deal( starting_card_count );	
	printf("BasicCardGame::onCreate() cards dealt\n" );

	whos_turn_is_it = 0;
}

int	BasicCardGame::onCreate  (  )
{
	printf("BasicCardGame::onCreate()\n" );
	deck[0]->onCreate();
	setup_game();
	
	//float card_spacing = CARD_WIDTH + 10; 		// this should be done in the card player, not here!
	vector<CardPlayer*>::iterator	iter = players.begin();
	for ( ; iter!=players.end(); iter++ )
	{
		(*iter)->arrange_cards( );
	}
	//printf("BasicCardGame::onCreate() players cards arranged\n" );

	place_buttons( whos_turn_is_it );
	printf("BasicCardGame::onCreate\n" );
}

int	BasicCardGame::number_of_cards_to_start( int mNumber_of_players )
{
	return 1;
}
void	BasicCardGame::score		( )
{

}

// The start of a new game.
void	BasicCardGame::deal( int n_Cards )
{
	printf("Dealing %d card to each %d player\n", n_Cards, players.size() );
	
	Card* card;
	for (int i=0; i<n_Cards; i++)
	{
		// House gets one:
		if (house)
		{	
			card = draw_one();
			house->receive_card( card, true );
		}
	
		// Disperse to each player 1 at a time	
		for (int p=0; p<players.size(); p++)
		{
			printf("deal card player %d\n", p);
			card = draw_one();
			players[p]->receive_card( card, true );
		}
	}
}


void BasicCardGame::place_buttons( int mPlayerIndex )	
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

void BasicCardGame::place_players( float radius )		// place places around the game's center point.
{
	printf("place_players()  %d\n", players.size());
	cx = width/2. + left;
	cy = height/2. + bottom;

	if (house)
	{
		float house_x = cx - house->get_width()/2. ;
		float title_bottom = bottom + height - 1.5*TITLE_HEIGHT;
		float house_y = (title_bottom - cy - house->get_height() )/2. + cy;
		house->move_to( house_x, house_y );
		//house->print_positions( );
	}

	// place hit, stay buttons here.
	if (players.size()>4)
	{
		// circle configuration.
		return;
	}
	float w,yp;
	std::vector<CardPlayer*>::iterator  iter = players.begin();
	if (players.size()>0)
	{	
		w = (*iter)->get_width()/2.;
		yp = bottom + (cy-bottom-(*iter)->get_height() )/2. ;
		(*iter)->move_to( cx-w, yp );				// bottom
		(*iter)->print_positions();
	}
	if (players.size()>1) 
	{
		iter++;
		w = (*iter)->get_width();
		yp = cy-(*iter)->get_height()/2.;
		(*iter)->move_to( cx-3.*radius-w, yp );		// goes left
		printf("player 2 done.\n");
		(*iter)->print_positions();		
	}
	if (players.size()>2) 
	{
		iter++;		
		yp = cy-(*iter)->get_height()/2.;		
		(*iter)->move_to( cx+3.*radius, yp );			// goes right
		printf("player 3 done.\n");
		(*iter)->print_positions();		
	}
	// anything else is circle configuration.
}

void	BasicCardGame::evaluate_winners()
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
			(*iter)->win();
		else 
			(*iter)->lose();
	}
}

// closest to 21 without going over.	
// add them all up.
void	BasicCardGame::pay_out()
{
}

Card*	BasicCardGame::draw_one()
{
	printf("draw_one()\n");
	return deck[0]->draw_one();
}

#define TEXT_HEIGHT 24

int		BasicCardGame::draw()
{
	char score_text[40];
	
	printf("Basic Card draw.\n");
	Control::draw();

	// Draw title
	Stroke_l(0xFFFFFFFF);
	Fill_l  (0xFFFFFF00);
	float centerx = width/8. + left;
	float centery = height + bottom- 1.5*TITLE_HEIGHT;
	TextMid(centerx, centery, NameOfTheGame, SerifTypeface, TITLE_HEIGHT );
	printf("Basic Card draw.  %s\n", NameOfTheGame );
	
	// Draw house + players
	/*if (house)
	{
		house->draw(  );
		
		int score = house->get_best_black_jack_score();
		float sx = house->get_width()/2.  + house->get_left();
		float sy = house->get_bottom() - TEXT_HEIGHT;
		sprintf ( score_text, "Score: %d", score  );
		printf  ( "sx=%6.1f;  sy=%6.1f\n", sx, sy ); 
		TextMid ( sx, sy, score_text, SerifTypeface, 16 );
	}*/
	
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

/*	float sx,sy;
	iter = players.begin();
	while ( iter != players.end() )
	{
		sx = (*iter)->get_width()/2.  + (*iter)->get_left();
		sy = (*iter)->get_bottom() - TEXT_HEIGHT;
		int score = (*iter)->get_best_black_jack_score();
		sprintf(score_text, "Score: %d", score );
		printf("sx=%6.1f;  sy=%6.1f\n", sx, sy ); 
		TextMid(sx, sy, score_text, SerifTypeface, 16 );
		iter++;	
	} */

	// Draw Hit/stay buttons:
//	hit.draw();
//	stay.draw();	
	return TRUE;
}

