
#include "control.hpp"
#include <list>

// The player also has money!
struct st_chip_stack  {
	int		number_of_1000s;
	int		number_of_100s;
	int		number_of_10s;
	int		number_of_1s;
};


class CardPlayerChips : public CardPlayer
{
public:
	CardPlayerChips ( );

	void	buy_in	( int  mNumber_of_1000s, int  mNumber_of_100s, int  mNumber_of_10s, int  mNumber_of_1s 	);
	void	cash_out( int* mNumber_of_1000s, int* mNumber_of_100s, int* mNumber_of_10s, int* mNumber_of_1s 	);
	void	bet		( int  mNumber_of_1000s, int  mNumber_of_100s, int  mNumber_of_10s, int  mNumber_of_1s 	);	
	virtual int	draw( );
		
private:
	st_chip_stack	in_hand;
	st_chip_stack	wager;

/*	int		number_of_1000s;
	int		number_of_100s;
	int		number_of_10s;
	int		number_of_1s; */
};
