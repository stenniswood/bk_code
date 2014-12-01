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
	

void Deck::load_resources()
{
	Card* tmp = NULL;
	// Load up all the cards:
	for (int i=0; i<52; i++)
	{
		theDeck[i]->load_resources();
		theDeck[i]->match_image_size();
//		float width  = theDeck[i]->get_image_width();
//		float height = theDeck[i]->get_image_height();
//		theDeck[i]->set_width_height( width, height );
	}
}

Card* Deck::draw_one()
{
	Card* tmp = theDeck[shuffled_indices.back()];
	shuffled_indices.pop_back();
	return tmp;	
}

void Deck::discard	(Card* mCard )
{

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
	// first start with all the numbers:  [0 1 2 3...49 50 51]
	// delete as they are used
	int					number_cards_remaining = 52;
	std::list<int>		start_indices;
	for (int i=0; i<theDeck.size(); i++)
		start_indices.push_back( i );			
	//printf("Ordered Sequence [0 1 2 ... %d ]  \n", theDeck.size());

	// Now create a random list [ 34 2 27 43 0 ]
	shuffled_indices.clear();
	int rnd2 = 0;
	int rnd  = 0;
	for (int i=0; i < theDeck.size(); i++)
	{
		rnd2 = rnd = rand() % number_cards_remaining;
		std::list<int>::iterator iter = start_indices.begin();
		for ( ; rnd>0; rnd-- )
			iter++;
		shuffled_indices.push_back( *iter );
		//printf("shuffled_indices rnd2=%d  %d\n", rnd2, shuffled_indices[i] );
		start_indices.erase( iter );
		number_cards_remaining--;
	}
	print_order(); 
}

void Deck::print_order()
{
	printf("Deck order:  \n");
	std::vector<int>::iterator	iter = shuffled_indices.begin();	
	for ( ; iter != shuffled_indices.end(); iter++)
		printf (" %d ", *iter);
}
void Deck::order()
{
	for (int i=0; i<theDeck.size(); i++)
	{
		shuffled_indices[i] = i;			
	}
}

