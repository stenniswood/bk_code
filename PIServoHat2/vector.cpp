#include "vector.hpp"



Vector::Vector( std::string mFilename )
{
	m_file_name = mFilename;
	selected_line = -1;
}

Vector::~Vector()
{

}

void	Vector::open_file( )
{
	printf("==== VectorFile: Opening %s\n", m_file_name.c_str() );
	m_file.open( m_file_name );
}

void  Vector::read_file( )
{
	std::string oneLine;
	
	while (std::getline( m_file, oneLine ))
	{
		m_file_lines.push_back( oneLine );
	}
	printf("Read %d lines.\n", m_file_lines.size() );
	selected_line = 0;
	parse_line_nums();
	
	process_one_vector(  );
}

void	Vector::select_one_vector( int LineIndex )
{
	selected_line = LineIndex;
}


void Vector::goto_first_sequence()
{
	selected_line = 0;
}

void	Vector::prev_sequence(  )
{
	if (m_goto_pending)	{ m_goto_pending=false;		return ; }
	
	selected_line--;
	if (selected_line<0) 
		selected_line = 0;
}

void	Vector::next_sequence(  )
{	
	if (m_goto_pending)	{ m_goto_pending=false; 	return ; }
	
	selected_line++;
	if (selected_line>m_file_lines.size()) 
		selected_line = m_file_lines.size();
}


void Vector::parse_line_nums()
{
	for (int l=0; l<m_file_lines.size(); l++)
	{
		std::string oneLine = m_file_lines[l];
		// EXTRACT LINE NUMBER:
		int pos = oneLine.find(':');			    
		curr_line_num = -1;
		if (pos != std::string::npos) 
		{			
			// extract Line #
			std::string line_num_str = oneLine.substr( 0, pos );
			curr_line_num = atoi( line_num_str.c_str() );
			m_line_nums.push_back( curr_line_num );				
		}	
	}
}

bool	Vector::process_one_vector(  )
{
	printf("Vector::Processing vector %d...\n", selected_line );
	std::string oneLine = m_file_lines[selected_line];

	// EXTRACT LINE NUMBER:
	int pos = oneLine.find(':');			    
	curr_line_num = -1;
	if (pos != std::string::npos) 
	{			
		// extract Line #
		std::string line_num_str = oneLine.substr( 0, pos );
		curr_line_num = atoi( line_num_str.c_str() );				
	}	
	//printf("LineNum=%d;\t", curr_line_num);	
	
	// EXTRACT LIMB NUMBER:
	std::string limb_delim_str = oneLine.substr( pos+2 );	
	if (limb_delim_str[0] != 'L')
    { 
       parse_command( limb_delim_str );  
       return false;
    } else {
		
		std::string limb_num_str = oneLine.substr(pos+3, 1 );
		curr_limb_num = atoi(limb_num_str.c_str() );
		//printf("Limb Num=%d;\n", curr_limb_num);	
		oneLine.erase(0, pos+5 );
		//printf("remaining string=%s\n", oneLine.c_str() );
		parse_vector_data( oneLine );
	}
	return true;
}

int	Vector::find_line_num( int mLineNum )		// returns index into m_file_lines;
{
	for (int l=0; l<m_file_lines.size(); l++)
	{
		if (m_line_nums[l]==mLineNum)
			return l;
	}
	return -1;
}

void Vector::parse_command(std::string mLine)
{

	int pos = mLine.find(' ');
	std::string mToken = mLine.substr(0, pos);
		
	printf("Vector::parse_command ( %s )\n", mLine.c_str() );
	
	
	if (mToken=="repeat")
	{ 
	}
	else if (mToken=="goto")
	{
		int line_num = atoi( mLine.substr(pos+1).c_str() );
		int f_index = find_line_num( line_num );
		
		if (f_index > m_file_lines.size())
			f_index = m_file_lines.size();
		else if (f_index<0)
			f_index = 0;
		printf("Goto %d\n", f_index);
		selected_line = f_index;	
		m_goto_pending = true;		
	}
	else if (mToken=="delay")
	{
	
	} else {
		printf("vector file - unknown command - ignoring!"); 
	}
// etc.
}

void	Vector::parse_vector_data( std::string mData )
{
	m_angles.clear();
	std::string substr;
	int pos = 0;
	int pos2 = 0;
	//printf("remaining string=%s\n", mData.c_str() );
	
	while ((pos2 = mData.find(' ', pos)) != std::string::npos )
	{
		substr = mData.substr( pos, (pos2-pos)  );
		//printf("substr=%s\n", substr.c_str() );
		float ftmp = atof( substr.c_str() );
		//printf("Angle=%6.2f; pos2=%d\n", ftmp, pos2);
		m_angles.push_back( ftmp );
		pos = pos2+1;
	}
	
	
		substr = mData.substr( pos, (mData.length()-pos)  );
		//printf("substr=%s\n", substr.c_str() );
		float ftmp = atof( substr.c_str() );
		//printf("Angle=%6.2f; pos2=%d\n", ftmp, pos2);
		m_angles.push_back( ftmp );
		pos = pos2+1;
}

struct stOneVector	Vector::get_zeros_vector_package( int mLimbNum, int mNumServos )
{
	struct stOneVector ov;
	ov.limb_num = mLimbNum;
	ov.m_angles.clear();
		
	for (int v=0; v<mNumServos; v++)
		ov.m_angles.push_back( 0.0 );
	return ov;
}

struct stOneVector	Vector::get_vector_package()
{
	struct stOneVector ov;
	ov.limb_num = curr_limb_num;
	ov.m_angles = m_angles;	
	return ov;
}


