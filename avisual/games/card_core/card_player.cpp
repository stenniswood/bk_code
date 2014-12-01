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



CardPlayer::CardPlayer( int MaxCardsInHand ) 
:Control()
{
	set_width_height( MaxCardsInHand*(62+20), 100 );
}

void CardPlayer::receive_card( Card* mNewCard, bool mExposed )
{
	printf("%x received %x 1 card\n",this,  mNewCard );
	mNewCard->expose( mExposed );
	cards.push_back(mNewCard);
}
#define CARD_SPACE_PADDING 15

int	CardPlayer::get_total_value( )
{
	float l=left;
	std::list<Card*>::iterator	iter = cards.begin();
	while (iter != cards.end())
	{
		l += (*iter)->get_value() + CARD_SPACE_PADDING;
		printf(" %d \t", l );
		iter++;
	}
}

int CardPlayer::is_ace_in_hand( )
{
//	return ;
}

int CardPlayer::get_best_black_jack_score( )
{
	int total = get_total_value();
	if (total > 21)
	{
		
	}
}

void	CardPlayer::flip_card	( Card* mNewCard )
{
//	mNewCard->
}

void	CardPlayer::expose_card	( Card* mNewCard, bool mExposed )
{
	cards.push_back(mNewCard);
	
}

int	CardPlayer::arrange_cards( )
{
	const int PADDING = 20;
	int l = left+PADDING; 
	//printf("CardPlayer::arrange_cards() \n" ); 
	
	std::list<Card*>::iterator	iter = cards.begin();
	while (iter != cards.end())
	{
		(*iter)->move_to( l, bottom );
		l += (*iter)->get_width() + PADDING;
		//printf(" %d \t", l );
		iter++;
	}
	//printf("\n");
} 

int	CardPlayer::draw( )
{
	Control::draw();
	arrange_cards();
	printf("CardPlayer::draw()  %x \n", this );
	Control::print_positions();
	
	std::list<Card*>::iterator	iter = cards.begin();
	// FIRST ARRANGE CARDS:
	while (iter != cards.end())
	{		
		printf("drawing %d %c\n", (*iter)->get_value(), (*iter)->get_suit() );
		(*iter)->print_positions();
		(*iter)->draw();
		iter++;
	}
}

