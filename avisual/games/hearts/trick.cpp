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
#include "trick.hpp"


#define CARD_SPACE_PADDING 15


Trick::Trick( int MaxCardsInHand ) 
:CardPlayer(MaxCardsInHand)
{ 
	
}

bool Trick::receive_card( Card* mNewCard )
{
	std::list<Card*>::iterator iter;
	if (cards.size() == 0)
	{
		CardPlayer::receive_card( mNewCard, true );
	}
	else 
	{
		iter = cards.begin();
		if ( mNewCard->get_suit() == (*iter)->get_suit() )
		{
			CardPlayer::receive_card( mNewCard, true );
			return true;
		} else {
			return false;		// can't accept, it doesn't follow suit.
		}			
	}	
}

void Trick::unload_on_player( CardPlayer* mPlayer )
{
	Card* card;

	std::list<Card*>::iterator	iter = cards.begin();
	while (iter != cards.end())
	{	
		mPlayer->receive_card( (*iter), true );
		cards.erase( iter );
		iter++;		
	}
}


