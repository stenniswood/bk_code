#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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

#define CARD_SPACE_PADDING 15


CardPlayer::CardPlayer( int MaxCardsInHand ) 
:Control()
{
	max_number_cards_in_hand = MaxCardsInHand;
	//set_width_height( MaxCardsInHand*(22+5), 100 );
}

void CardPlayer::receive_card( Card* mNewCard, bool mExposed )
{
	//printf("%x received %x 1 card\n",this,  mNewCard );
	mNewCard->expose( mExposed );
	cards.push_back(mNewCard);
	arrange_cards( card_spacing );
}

float	CardPlayer::get_one_card_width ()
{	return 62.;	}

float	CardPlayer::get_one_card_height()
{	return 81.;	}

float CardPlayer::determine_card_spacing(  )
{
	printf(" max_number_cards_in_hand=%d;  width=%6.1f\n", max_number_cards_in_hand, width);
	float edge_width = (float)width - get_one_card_width();
	float spacing = ( edge_width / (float)max_number_cards_in_hand  );
	return spacing;
}

int	CardPlayer::get_total_value( )
{
	int total_value=0;
	std::list<Card*>::iterator	iter = cards.begin();
	while (iter != cards.end())
	{
		total_value += (*iter)->get_value();
		//printf(" %d \t", l );
		iter++;
	}
	return total_value;
}

bool CardPlayer::is_ace_in_hand( )
{
	std::list<Card*>::iterator	iter = cards.begin();
	while (iter != cards.end())
	{
		if ( (*iter)->get_card() == 'A' ) {
			printf("ACE is in Hand!\n");
			return true;
		}
		iter++;
	}
	return false;
}

int CardPlayer::get_best_black_jack_score( )
{
	int total = get_total_value();
	if (total > 21)
	{
		if (is_ace_in_hand()==true)
			return (total-10);
	}
	return total;
}

void	CardPlayer::flip_card	( Card* mNewCard )
{
//	mNewCard->
}

void CardPlayer::expose_all_cards( bool mExposed )
{
	std::list<Card*>::iterator	iter = cards.begin();
	while (iter != cards.end())
	{
		(*iter)->expose( mExposed );
		iter++;
	}	
}

void CardPlayer::expose_card	( Card* mNewCard, bool mExposed )
{
	cards.push_back(mNewCard);	
}

int	CardPlayer::arrange_cards( float mCardSpacing )
{
	const int PADDING = 20;
	const float CARD_HEIGHT = 81;
	
	
	int l = left+PADDING; 
	float b = (height-CARD_HEIGHT)/2 + bottom;
	//printf("CardPlayer::arrange_cards() \n" ); 
	if (mCardSpacing==-1)
		mCardSpacing = determine_card_spacing();
	
	std::list<Card*>::iterator	iter = cards.begin();
	while (iter != cards.end())
	{
		(*iter)->move_to( l, b );
		l += mCardSpacing;
		iter++;
	}
	//printf("\n");
} 

int	CardPlayer::draw( )
{
	Control::draw();
	//float spacing = determine_card_spacing();

	//printf("CardPlayer::draw()  %x \n", this );
	//Control::print_positions();
	
	std::list<Card*>::iterator	iter = cards.begin();
	// FIRST ARRANGE CARDS:
	while (iter != cards.end())
	{		
		printf("drawing %d %c\n", (*iter)->get_value(), (*iter)->get_suit() );
		//(*iter)->print_positions();
		(*iter)->draw();
		iter++;
	}
}

