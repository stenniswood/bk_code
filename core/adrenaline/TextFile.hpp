#ifndef _TEXTFILE_H_
#define _TEXTFILE_H_

/* This class functions as a generic text file class.  It is especially intended
   for .ini, .csv, or other text file with parameter parsing.  Could also be extended
   to read g_code.
*/
class TextFile
{
public:
	TextFile		( char* mFileName );

	void   readln_nb( char* mLine );		// read non blank line.
	void   readln	( char* mLine );
	void   write	( char* mLine );

	// .INI Type Retrievals:
	char*  get_word				( char* mLine 			);
	int	   get_int_variable		( char* mVariableName 	);
	float  get_float_variable	( char* mVariableName 	);
	char*  get_string_variable	( char* mVariableName 	);

	// BASE File Operations:
	BOOL	file_exists	(						 );
	void   	open		( BOOL mReadOnly = FALSE );
	void   	close		( 						 );
	void 	show_file	( char* mFileName 		 );
		
	char** 	split	( char *src_str, const char deliminator, int *num_sub_str);
	//void   save		( int argc, char* argv[], char* mVectName, byte vector_size );

protected:
	// GETS the right side of '=' sign.
	float 	getFloatValue( char* mLine );
	int 	getIntValue	 ( char* mLine );
	char* 	getString	 ( char* mLine );

private:
	char*	FileName;
	FILE*	fd;	
};

#endif
