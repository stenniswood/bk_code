#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_

#include <string>
#include <vector>
#include "servo.hpp"

#include <fstream>
#include <iostream>


struct stOneVector 
{
	int limb_num;	
	std::vector<float>	m_angles;
	void 	print() {
		for (int i=0; i<m_angles.size(); i++)
			printf("%6.3f ", m_angles[i]);
	}	
};
struct stOneCounts
{
	int limb_num;	
	std::vector<unsigned short>	m_counts;
	void 	print() {
		for (int i=0; i<m_counts.size(); i++)
			printf("%d ", m_counts[i]);
	}	
};


class VectorSequence
{
public:
	VectorSequence( std::string mFilename );
	~VectorSequence();

	void	open_file	   		( );	
	void	read_file	   		( );
	void    parse_line_nums		( );	
	int		find_line_num  		( int mLineNum );		// returns index into m_file_lines;

	void	select_one_vector 	( int LineIndex );
	bool	process_one_vector	( );

	struct stOneVector	get_vector_package();
	struct stOneVector	get_zeros_vector_package(int mLimbNum, int mNumServos );

	void	goto_first_sequence	( );
	void	prev_sequence		( );
	void	next_sequence		( );

	bool	get_is_command		( )	{ return m_is_command; };
	int 	parse_command		( std::string mToken);
	int 	execute_command		( );

	bool	m_goto_pending;

			
protected:
	void	parse_vector_data	( std::string mData );

private:
	std::string   m_file_name;
	std::ifstream m_file;
	
	std::vector<std::string>  m_file_lines;
	std::vector<int>		  m_line_nums;
	
	bool				m_is_command;
	std::string 		m_token;		//tbd
	int					m_goto_index;	// line index
	long int 			m_delay_time;
	
	std::vector<float>	m_angles;
	int	selected_line;
	int curr_line_num;
	int curr_limb_num;
	
};


#endif
