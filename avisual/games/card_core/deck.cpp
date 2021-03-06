#include <sys/timeb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <jpeglib.h>
#include "bcm_host.h"
#include <shapes.h>
#include <fontinfo.h>
#include "bk_system_defs.h"
#include "display.h"
#include "control.hpp"
#include <vector>
#include <list>
#include "deck.hpp"


#define Debug 0


Deck::Deck()
{
	Card* tmp = NULL;
	// Load up all the cards:
	for (int i=0; i<52; i++)
	{
		tmp = new Card(i);
		theDeck.push_back( tmp );
	}	
	shuffle();	
}
	
int	Deck::onCreate(  )
{
	for (int i=0; i<52; i++)
	{
		theDeck[i]->onCreate();
	}
}

void Deck::load_resources()
{
	Card* tmp = NULL;
	// Load up all the cards:
/*	for (int i=0; i<52; i++)
	{
		theDeck[i]->load_resources();
		theDeck[i]->match_image_size();
	} */
}

Card* Deck::draw_one()
{
	if (shuffled_indices.empty()) 
		return NULL;
		
	int index = shuffled_indices.back();
	if ((index>theDeck.size()) || (index<0))
		return NULL;
	Card* tmp = theDeck[index];
	shuffled_indices.pop_back();		
	return tmp;	
}

void Deck::discard( Card* mCard )
{
	std::vector<Card*>::iterator iter = theDeck.begin();
	while ( *iter != mCard )  {		iter++;		};

	theDeck.erase( iter );
	//shuffled_indices.pop_back();
}

void Deck::reuse_discarded()		// call when action is required. (ie not a state)
{

}

int get_element( int nth )
{
	return nth;
}



void Deck::shuffle()
{
	struct timeb tb;
	srand( tb.millitm );		// Randomize the seed.
	
	// first start with all the numbers:  [0 1 2 3...49 50 51]
	// delete as they are used
	int					number_cards_remaining = 52;
	std::list<int>		start_indices;
	for (int i=0; i<theDeck.size(); i++)
		start_indices.push_back( i );			

	// Now create a random list [ 34 2 27 43 0 ]
	shuffled_indices.clear();
	int rnd2 = 0;
	int rnd  = 0;
	for (int i=0; i < theDeck.size(); i++)
	{
		// Random number points to the start_indices[] array.  [0 1 2 3 4...51]
		rnd2 = rnd = rand() % number_cards_remaining;
		// Look up the indexed into start_indices.
		std::list<int>::iterator iter = start_indices.begin();
		for ( ; rnd>0; rnd-- )
			iter++;
		shuffled_indices.push_back( *iter );
		//printf("shuffled_indices rnd2=%d  %d\n", rnd2, shuffled_indices[i] );
		start_indices.erase( iter );
		number_cards_remaining--;
	}
	if (Debug) print_order(); 
}

void Deck::draw_image_back( Card* card )
{
	//Control::draw();
/*	Fill_l( card->background_color);
	Rect  ( card->left, card->bottom, card->width, card->height );
	VGfloat l = card->left+left_margin;
	VGfloat b = bottom+bottom_margin;
	int min_w = min(ImageInfo.width, width);
	int min_h = min(ImageInfo.height, height);
	if (image!=NULL)
		vgSetPixels(l, b, image, 0, 0, min_w, min_h);  */
}

void Deck::print_order()
{
	printf("Deck order:  \n");
	std::vector<int>::iterator	iter = shuffled_indices.begin();	
	for ( ; iter != shuffled_indices.end(); iter++)
		printf (" %d ", *iter);
	printf("\n");
}

void Deck::order()
{
	for (int i=0; i<theDeck.size(); i++)
	{
		shuffled_indices[i] = i;			
	}
}

Card* Deck::find( char mValue, char mSuit )
{
	for (int i=0; i<52; i++)
	{
		char value = theDeck[i]->get_value();
		char suit  = theDeck[i]->get_suit();
		if ((value==mValue) && (mSuit==suit))
			return theDeck[i];
	}
	return NULL;
}

