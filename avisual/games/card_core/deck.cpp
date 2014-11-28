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
}
	

void Deck::load_resources()
{
	Card* tmp = NULL;
	// Load up all the cards:
	for (int i=0; i<52; i++)
		theDeck[i]->load_resources();
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

void Deck::shuffle()
{
	// first start with all the numbers, as they are taken up, we put a -1 in there.
	// if the next random number falls on a -1, we pick another.
	int					number_cards_remaining = 52;
	std::vector<int>	start_indices;
	for (int i=0; i<theDeck.size(); i++)
	{
		start_indices[i] = i;			
	}

	// Now 
	int tmp = 0;
	for (int i=0; i < theDeck.size(); i++)
	{
		tmp = rand()%52;
		while ( start_indices[tmp] == -1 )
		{
			tmp = rand()%52;							
		};
		shuffled_indices[i] = tmp;
		number_cards_remaining--;
	}
}

void Deck::order()
{
	for (int i=0; i<theDeck.size(); i++)
	{
		shuffled_indices[i] = i;			
	}
}

