/*************************************************************************
NAME	:	test_layouts.cpp
DESCRIPTION:

	This file is used to test the primitive objects in the Adrenaline
	windowing package.

	For combinations of objects see:  test_combo_layouts.cpp
	
DATE	: November 20, 2014
AUTHOR	: Steve Tenniswood
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"
#include "display.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"

#include "frame_window.hpp"
#include <vector>
#include "test_layouts.hpp"

#include "visual_memory.h"
#include "audio_memory.h"
#include "fft.h"
#include "test_combo_layouts.hpp"
#include "audio_app.hpp"
#include "card_games.h"





/*Card card1(3);
Card card2(4);
Card card3(5);
Card* c6 = new Card(6);
Card* c7 = new Card(7);
Card* c8 = new Card(8);

CardPlayer player1(4);
CardPlayer player2(4);
//Deck deck();

*/
void init_cards_only( )
{
	const int PADDING = 20;
	int l = 100+PADDING; 
	MainDisplay.remove_all_objects(	);
			
	//  HEARTS.   h c d s 
	Card* cardsH[13];				// first suit.
	for (int i=0; i<13; i++)
	{
		cardsH[i] = new Card(i);
		cardsH[i]->load_resources();
		cardsH[i]->match_image_size();
		cardsH[i]->move_to( l, 200 );
		cardsH[i]->print_positions();		
		l += cardsH[i]->get_width() + PADDING;
		MainDisplay.add_object	( cardsH[i] );
	}
	
	//  HEARTS.   h c d s 
	Card* cardsC[13];				// first suit.
	l = 100+PADDING;
	for (int i=0; i<13; i++)
	{
		cardsC[i] = new Card(i+13);
		cardsC[i]->load_resources();
		cardsC[i]->match_image_size();
		cardsC[i]->move_to( l, 300 );
		cardsC[i]->print_positions();		
		l += cardsC[i]->get_width() + PADDING;
		MainDisplay.add_object	( cardsC[i] );
	}

	//  HEARTS.   h c d s 
	Card* cardsD[13];				// first suit.
	l = 100+PADDING;
	for (int i=0; i<13; i++)
	{
		cardsD[i] = new Card(i+26);
		cardsD[i]->load_resources();
		cardsD[i]->match_image_size();
		cardsD[i]->move_to( l, 400 );
		cardsD[i]->print_positions();		
		l += cardsD[i]->get_width() + PADDING;
		MainDisplay.add_object	( cardsD[i] );
	}

	//  HEARTS.   h c d s 
	Card* cardsS[13];				// first suit.
	l = 100+PADDING;
	for (int i=0; i<13; i++)
	{
		cardsS[i] = new Card(i+39);
		cardsS[i]->load_resources();
		cardsS[i]->match_image_size();
		cardsS[i]->move_to( l, 500 );
		cardsS[i]->print_positions();		
		l += cardsS[i]->get_width() + PADDING;
		MainDisplay.add_object	( cardsS[i] );
	}	
}

void init_card( )
{
	printf("init_card()\n");	
/*	card1.move_to( 200, 200 );	card1.load_resources();		card1.match_image_size();
	card2.move_to( 200, 300 );	card2.load_resources();		card2.match_image_size();
	card3.move_to( 200, 400 );	card3.load_resources();		card3.match_image_size();

	c6->move_to( 200, 200 );	c6->load_resources();		c6->match_image_size();
	c7->move_to( 200, 300 );	c7->load_resources();		c7->match_image_size();
	c8->move_to( 200, 400 );	c8->load_resources();		c8->match_image_size();

	hit.set_width_height 	( 100, 100 );
	stay.set_width_height	( 100, 100 );
	hit.move_to 		 	( 100, 100 );
	stay.move_to		 	( 220, 100 );

	player1.move_to			( 300, 200  );
	player1.set_width_height( 70*5, 100 );
	player1.receive_card	( c6, true );
	player1.receive_card	( c7, true );
	player1.receive_card	( c8, true );

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( &card1 );
	MainDisplay.add_object	( &card2 );
	MainDisplay.add_object	( &card3 );
*/	
/*	MainDisplay.add_object	( c6 );
	MainDisplay.add_object	( c7 );
	MainDisplay.add_object	( c8 ); */

//	MainDisplay.add_object	( &player1 );
}

BlackJack  bj(3);
HeartsGame hearts(3);

void init_blackjack( )
{
	printf("init_blackjack()\n");
	float wide = MainDisplay.get_width();
	float high = MainDisplay.get_height();
	bj.set_width_height( wide*(1-0.125), 3.*high/4. );
	bj.move_to(0.0625*wide, high*(1./8.) );
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( &bj 	);
}

void init_hearts( )
{
	printf("init_hearts()\n");	
	float wide = MainDisplay.get_width();
	float high = MainDisplay.get_height();
	hearts.set_width_height( wide*(1-0.125), high*(3./4.) );
	hearts.move_to(0.0625*wide, high*(1./8.) );
	printf("Done setting up\n");

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object		  ( &hearts );
}

void init_reversi( )
{
/*
	printf("init_reversi()\n");	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( &hit  );
	MainDisplay.add_object	( &stay );	
*/	
	// black jack knows how to add all it's graphic objects.
//	bj.add_to_display_manager( &MainDisplay );	
}

void init_battleships( )
{
/*
	printf("init_reversi()\n");	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( &hit  );
	MainDisplay.add_object	( &stay );	
*/	
// black jack knows how to add all it's graphic objects.
//	bj.add_to_display_manager( &MainDisplay );	
}


