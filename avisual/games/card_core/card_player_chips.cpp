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


CardPlayerChips::CardPlayerChips()
:CardPlayer()
{

}

/* User has purchased so many of each chips:  */
void	CardPlayerChips::buy_in( int mNumber_of_1000s, int mNumber_of_100s, int mNumber_of_10s, int mNumber_of_1s )
{
	number_of_1000s	= mNumber_of_1000s;
	number_of_100s  = mNumber_of_100s;
	number_of_10s   = mNumber_of_10s;
	number_of_1s    = mNumber_of_1s;
}

void	CardPlayerChips::cash_out( int* mNumber_of_1000s, int* mNumber_of_100s, int* mNumber_of_10s, int* mNumber_of_1s )
{
	mNumber_of_1000s = number_of_1000s;
	mNumber_of_100s  = number_of_100s;
	mNumber_of_10s   = number_of_10s;
	mNumber_of_1s    = number_of_1s;
}

void	CardPlayerChips::bet( int mNumber_of_1000s, int mNumber_of_100s, int mNumber_of_10s, int mNumber_of_1s )
{
	number_of_1000s	-= mNumber_of_1000s;
	number_of_100s  -= mNumber_of_100s;
	number_of_10s   -= mNumber_of_10s;
	number_of_1s    -= mNumber_of_1s;
}

int		CardPlayerChips::draw  	 ( )
{

}
