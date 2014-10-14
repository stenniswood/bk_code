#ifndef _FILTER_FILE_H_
#define _FILTER_FILE_H_





class FilterFile : public Preferences
{
public:
	FilterFile( char* mFileName );	

	tID	 convert_to_id( char* line );		
	void Read_n_Add(  );
	
private:
	
};


#endif
