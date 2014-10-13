#ifndef _INI_H_
#define _INI_H_


/* Better way is to read entire file.  Create a hash table.
	For each line, add the string and the value cast as appropriate
	(void*).  or the value stored as string too and converted
	with proper function.	
*/
class Preferences
{
public:
	Preferences		( char* mFileName	);

	void   readln_nb( char* mLine 		);		// read non blank line.
	void   readln	( char* mLine 		);
	void   write	( char* mLine 		);

	char*  getWord	( char* mLine 		);
	void   show_file( char* mFileName 	);
	
	void   	open	( BOOL mReadOnly = FALSE);	
	void   	close	( 						);
	BOOL	file_exists();

	char** 	split	( char *src_str, const char deliminator, int *num_sub_str);
	//void   save		( int argc, char* argv[], char* mVectName, byte vector_size );
	
	float 	getFloatValue( char* mLine );
	int 	getIntValue	 ( char* mLine );
	char* 	getString	 ( char* mLine );

	char*	getKey( char* mLine );
	
protected:
	FILE* fd;
	char* FileName;
	
	//char  Line[128];
	char  key[64];
		
private:

};



#endif
