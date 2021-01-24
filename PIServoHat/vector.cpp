#include <unistd.h>

#include "vector.hpp"



VectorSequence::VectorSequence( std::string mFilename )
{
	m_file_name = mFilename;
	selected_line = -1;
}

VectorSequence::~VectorSequence()
{

}

void	VectorSequence::open_file( )
{
	printf("==== VectorFile: Opening %s\n", m_file_name.c_str() );
	m_file.open( m_file_name );
}

void  VectorSequence::read_file( )
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

void VectorSequence::goto_first_sequence()
{
	selected_line = 0;
}

void	VectorSequence::select_one_vector( int LineIndex )
{
	if ((LineIndex<0) | (LineIndex>m_file_lines.size()))	
	{
		printf("Selected vector is past bounds!\n");
		return;
	}
	
	selected_line = LineIndex;	
	process_one_vector( );	
}

void	VectorSequence::prev_sequence(  )
{
	if (m_goto_pending)	{ m_goto_pending=false;		return ; }	
	selected_line--;
	select_one_vector( selected_line );
}

void  VectorSequence::next_sequence(  )
{
	int tmpSL = selected_line+1;
	
	if (m_goto_pending)
	{
		//select_one_vector( m_goto_index );
		tmpSL = m_goto_index;
		m_token = "unknown";		
		m_goto_pending = false;
	}

	select_one_vector( tmpSL );
}

void VectorSequence::parse_line_nums()
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

int	VectorSequence::find_line_num( int mLineNum )		// returns index into m_file_lines;
{
	for (int l=0; l<m_file_lines.size(); l++)
	{
		if (m_line_nums[l]==mLineNum)
			return l;
	}
	return -1;
}


/* Parses the Selected_line into its components. */
bool	VectorSequence::process_one_vector(  )
{
	printf("VectorSequence::Processing vector %d...\n", selected_line );
	std::string oneLine = m_file_lines[selected_line];

	// EXTRACT LINE NUMBER:
	int pos = oneLine.find(':');
	curr_line_num = -1;
	if (pos != std::string::npos)
	{
		std::string line_num_str = oneLine.substr( 0, pos );
		curr_line_num = atoi( line_num_str.c_str() );				
	}
	printf("LineNum=%d;\t", curr_line_num);	
	
	// EXTRACT LIMB NUMBER : 
	std::string limb_delim_str = oneLine.substr( pos+2 );	
	if (limb_delim_str[0] != 'L')
    {
		// EXTRACT TOKEN : 
		pos     = limb_delim_str.find  ( ' '   );
		m_token = limb_delim_str.substr( 0, pos);
    
		// Check for a special command (ie. delay, goto, etc)
		std::string str = limb_delim_str.substr( pos+1 );
		int result      = parse_command        ( str );
		
		m_is_command = (result>0);
		return !m_is_command;
    } 
    else 
    {
		// normal vector...
		m_is_command = false;
		std::string limb_num_str = oneLine.substr( pos+3, 1 );
		curr_limb_num = atoi(limb_num_str.c_str() );
		//printf("Limb Num=%d;\n", curr_limb_num);
		oneLine.erase(0, pos+5 );
		//printf("remaining string=%s\n", oneLine.c_str() );
		parse_vector_data( oneLine );		
		return true;
	}
	return true;
}

int VectorSequence::parse_command(std::string mLine)
{
	printf("VectorSequence::parse_command ( %s )\n", mLine.c_str() );
	int pos = 0; 

	if (m_token=="repeat")
	{ 
		return 1;
	}
	else if (m_token=="goto")	// This will change the selected_line!
	{
		// Extract Line Num : 
		int line_num = atoi( mLine.c_str() );

		// Retrieve the Line text : 
		m_goto_index = find_line_num( line_num );
		if (m_goto_index<0) {
			printf("Error in GOTO statement -- line not found.\n");
		}
		// Select it as the line:
		printf("Goto %d : %d\n", line_num, m_goto_index );

		// Flag as pending : 
		m_goto_pending = true;
		return 1;
	}
	else if (m_token=="delay_sec")
	{
		m_delay_time = atoi( mLine.substr(pos+1).c_str() );	
		//sleep(m_delay_time);
		return 1;	
	}
	else if (m_token=="delay_us")
	{
		m_delay_time = atoi( mLine.substr(pos+1).c_str() );	
		//usleep(m_delay_time);
		return 1;	
	} 
	else {
		printf("vector file - unknown command - ignoring!"); 
		return 0;		// Not processed.		
	}
	return 0;		// Not processed.
}

int VectorSequence::execute_command()
{
	int retval = 0;
	if (m_token=="repeat")
	{ 
		retval = 1;
	}
	else if (m_token=="goto")	// This will change the selected_line!
	{
		// Select it as the line : 
		// printf("Goto %d : %d\n", m_goto_index, line_num );		
		//select_one_vector( m_goto_index );
		//m_goto_pending = false;
		retval = 1;		
	}
	else if (m_token=="delay_sec")
	{
		sleep(m_delay_time);
		retval = 1;
	}
	else if (m_token=="delay_us")
	{
		usleep(m_delay_time);
		retval = 1;
	} else {
		printf("vector file - unknown command - ignoring!");
		retval = 0;		// Not processed.
	}
	
	m_token = "processed";	// so it doesn't do it twice.
	return retval;		// Not processed.
}


void	VectorSequence::parse_vector_data( std::string mData )
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

struct stOneVector	VectorSequence::get_zeros_vector_package( int mLimbNum, int mNumServos )
{
	struct stOneVector ov;
	ov.limb_num = mLimbNum;
	ov.m_angles.clear();
		
	for (int v=0; v<mNumServos; v++)
		ov.m_angles.push_back( 0.0 );
	return ov;
}

struct stOneVector	VectorSequence::get_vector_package()
{
	struct stOneVector ov;
	ov.limb_num = curr_limb_num;
	ov.m_angles = m_angles;	
	return ov;
}


