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



CardPlayer::CardPlayer()
{

}

void CardPlayer::receive_card( Card* mNewCard, bool mExposed )
{
	cards.push_back(mNewCard);
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
	int l = left; 
	const int PADDING = 20;
	printf(" arrange_cards() \n" ); 
	
	std::list<Card*>::iterator	iter = cards.begin();
	while (iter != cards.end())
	{
		(*iter)->move_to( l, bottom );
		l += (*iter)->get_width() + PADDING;
		printf(" %d \t", l );
		iter++;
	}
	printf("\n");
}

int	CardPlayer::draw( )
{
	arrange_cards( );
	
	std::list<Card*>::iterator	iter = cards.begin();
	// FIRST ARRANGE CARDS:
	while (iter != cards.end())
	{		
		(*iter)->draw();
		iter++;
	}
}

