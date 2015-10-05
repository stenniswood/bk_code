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



CardPlayerChips::CardPlayerChips ( int MaxCardsInHand )
:CardPlayer( MaxCardsInHand )
{
	in_hand = 0.0;
	wager   = 0.0;
}
void	CardPlayerChips::increase_wager ( float mIncrement = 5.0 )
{
	wager += mIncrement;
	if (wager>1000) wager = 1000;	
}
void	CardPlayerChips::decrease_wager ( float mIncrement = 5.0 )
{
	wager -= mIncrement;
	if (wager<0) wager = 0;
}

void	CardPlayerChips::buy_in		( float mDollarAmount 	)
{
	in_hand = mDOllarAmount;
}
void	CardPlayerChips::lose		( )
{
	CardPlayer::lose();
	in_hand -= wager;
}
void	CardPlayerChips::win		( )
{
	CardPlayer::win();
	in_hand += wager;
}

int		CardPlayerChips::draw( )
{
	CardPlayer::draw( );
	
	// Now Draw  "Cash: $235"
				
}



/************************************************************************/

/************************************************************************/
