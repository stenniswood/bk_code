#ifndef _DECK_H_
#define _DECK_H_

#include <vector>
#include "card.hpp"


class Deck
{
public:
	Deck( );
	
	void			load_resources();	

	Card*			draw_one();
	void			discard	( Card* mCard );

	void 			reuse_discarded();		// call when action is required. (ie not a state)

	void			shuffle	(			 );
	void			print_order(		 );	
	void 			order	(			 );
	Card*			find 	( char mValue, char mSuit );


	// Use to draw card backs when face down:	
	void 			draw_image_back( Card* card );
	virtual int		onCreate  (  );
		
private:
	std::vector<Card*>	theDeck;
	std::vector<int>	shuffled_indices;	// these are indices into theDeck.

	// For all card backs, use this image :
	static VGImage 				image;
	static struct image_info 	ImageInfo;
	static char*				CardBackFilename;
	// not implemeneted yet!		
};


	//void	remove_from	(			 );
#endif
