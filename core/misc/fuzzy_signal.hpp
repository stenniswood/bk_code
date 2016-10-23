
#include "fuzzy.hpp"

/* this will break a float value into several ranges / catagories
	which can be used to relate to verbal descriptions
	
	For example:  knee potentiometer (-10,180)
		-10,10 Straight
		10,30	slight bend
		31,55   45 degree
		80,100  right angle
		101,180 [almost] all the way		
*/

class FuzzySignal
{
public:
	FuzzySignal();

	void 		add_range ( string mKey, float low, float high);
	string 		find_range( float mValue   );
	float 		get_mid_value( string mKey);	
	void 		print_ranges( );

private:	
	range_map	m_ranges;	
};


void test_fuzzy_signal();

