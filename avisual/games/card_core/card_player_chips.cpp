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
#include "card_player_chips.hpp"



CardPlayerChips::CardPlayerChips ( int MaxCardsInHand )
:CardPlayer( MaxCardsInHand )
{
	in_hand = 100.0;
	wager   = 5.0;
}
void	CardPlayerChips::increase_wager ( float mIncrement )
{
	wager += mIncrement;
	if (wager>1000) wager = 1000;	
}
void	CardPlayerChips::decrease_wager ( float mIncrement )
{
	wager -= mIncrement;
	if (wager<0) wager = 0;
}

void	CardPlayerChips::buy_in		( float mDollarAmount 	)
{
	in_hand = mDollarAmount;
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

	float sx = get_width()  + get_left();
	float sy = get_bottom() - SCORE_TEXT_HEIGHT;
	char  cash_text[40];	
	
	// CASH
	Stroke_l(0xFF00FF00);
	Fill_l  (0xFF7FFF00);
	sprintf(cash_text, "$%6.0f", in_hand );	
	TextEnd ( sx, sy, cash_text, SerifTypeface, 16 );	

	// WAGER:
	Stroke_l(0xFFFFFF00);
	Fill_l  (0xFFFFFF00);
	sprintf(cash_text, "Bet $%6.0f", wager );	
	TextEnd ( sx, sy-SCORE_TEXT_HEIGHT, cash_text, SerifTypeface, 14 );	
}



/************************************************************************/

/************************************************************************/
