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
};
struct stOneCounts
{
	int limb_num;	
	std::vector<unsigned short>	m_counts;
};


class Vector
{
public:
	Vector( std::string mFilename );
	~Vector();

	void	open_file( );	
	void	read_file( );
	void    parse_line_nums();	
	int		find_line_num( int mLineNum );		// returns index into m_file_lines;
	
	void	select_one_vector( int LineIndex );
	bool	process_one_vector( );
//	void	execute_one_vector(  );
	struct stOneVector	get_vector_package();
	struct stOneVector	get_zeros_vector_package(int mLimbNum, int mNumServos );

	void	goto_first_sequence();		
	void	prev_sequence();
	void	next_sequence();

	void 	parse_command(std::string mToken);

	bool	m_goto_pending;
			
protected:
	void	parse_vector_data( std::string mData );

private:
	std::string   m_file_name;
	std::ifstream m_file;
	
	std::vector<std::string>  m_file_lines;
	std::vector<int>		  m_line_nums;
	

	
	std::vector<float>	m_angles;
	int	selected_line;
	int curr_line_num;
	int curr_limb_num;
	
};


#endif
