
#include "fuzzy_signal.hpp"


FuzzySignal::FuzzySignal()
{

}

void FuzzySignal::add_range( string mKey, float low, float high )
{
	struct stRange r;
	r.low  =low; 
	r.high =high;	
	m_ranges[mKey] = r;
}

string FuzzySignal::find_range( float mValue )
{
	return ::find_range( mValue, m_ranges );
}

// Just get a point in the range. 
float FuzzySignal::get_mid_value( string mKey )
{
	float center = (m_ranges[mKey].low + m_ranges[mKey].high) /2.0;
	return center;
}

void FuzzySignal::print_ranges( )
{
	range_map::iterator myiter = m_ranges.begin();
	while (myiter != m_ranges.end())
	{
		print_range( myiter->first, m_ranges );
		myiter++;
	}
}


void test_fuzzy_signal()
{
	FuzzySignal test;
	test.add_range("freezing", -45, 32); 	
	test.add_range("cold",     33, 50); 		
	test.add_range("warm",     50, 69); 		
	test.add_range("room temperature", 70, 77);
	test.add_range("hot", 		78, 100);	
	test.add_range("sweltering", 101, UNBOUND_HIGH);	
	printf("test_fuzzy_signal 2\n");
	
	float val = 0;
	string catagory = test.find_range(val);
	printf("%6.2f is %s\n", val, catagory.c_str() );
	
	val=45; 	catagory = test.find_range(val);
	printf("%6.2f is %s\n", val, catagory.c_str() );

	val=72; 	catagory = test.find_range(val);
	printf("%6.2f is %s\n", val, catagory.c_str() );
	
	val=88;		catagory = test.find_range(val);
	printf("%6.2f is %s\n", val, catagory.c_str() );
	
	val=118; 	catagory = test.find_range(val);	
	printf("%6.2f is %s\n", val, catagory.c_str() );
	
}
