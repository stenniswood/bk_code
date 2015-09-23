#ifndef _PREFERENCES_HPP_
#define _PREFERENCES_HPP_


#include <string>
#include <list>
using namespace std;





class sKeyValuePair
{
public:
	sKeyValuePair(const char* mkey, const char* mvalue):
	key(mkey), value(mvalue) { 
		key   = mkey;
		value = mvalue;
	};
	
	std::string  key;
	std::string  value;
};


/* Better way is to read entire file.  Create a hash table.
	For each line, add the string and the value cast as appropriate
	(void*).  or the value stored as string too and converted
	with proper function.	
*/
class Preferences
{
public:
	Preferences		( char* mFileName	);		// Does not open file.

	char	load_all_keys();		// ==0 means no errors.  >0 is line number of first error.	
	bool	readln_nb	( 	);		// read non blank line.
	void	readln		( 	);
	void	write		( char* mLine	);

	char*  getWord		( 	);
	void   show_file	( char* mFileName 	);
	void 	print_all_keys( );

	void   	open		( BOOL mReadOnly = FALSE);	
	void   	close		( 						);
	BOOL	file_exists	(						);

	char** 	split		( char *src_str, const char deliminator, int *num_sub_str);
	//void   save		( int argc, char* argv[], char* mVectName, byte vector_size );

	// These search thru all keys until match, then returns the value.
	struct sKeyValuePair* 	find_key 	 ( const char* mKey );
	const char* 			find_string	 ( const char* mKey );
	float 					find_float	 ( const char* mKey );
	int 					find_int	 ( const char* mKey );
	unsigned int			find_hex	 ( const char* mKey );
	bool 					find_bool	 ( const char* mKey );	

	// For compatability legacy:
	float 	getFloatValue(  );
	int 	getIntValue	 (  );
	uint16_t getHexValue( );
	char* 	getString	 (  );
	
	char*	getKey		 (  );
	char*   getValue	 ( );

	std::list<sKeyValuePair*>  key_table;
		
protected:
	FILE* fd;
	char* FileName;

	char  Line[128];
	char  key[80];			// temporary var
	char  value[80];		// temporary var

private:

};



#endif
