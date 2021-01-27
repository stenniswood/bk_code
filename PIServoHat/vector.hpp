#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_

#include <string>
#include <vector>
#include "servo.hpp"

#include <fstream>
#include <iostream>


struct stFloatVector 
{
	int limb_num;	
	std::vector<float>	m_angles;
	void 	print() {
		printf("L%d: ", limb_num);
		for (int i=0; i<m_angles.size(); i++)
			printf("%6.3f ", m_angles[i]);
		printf("\n");
	}	
};
struct stCountVector
{
	int limb_num;	
	std::vector<unsigned short>	m_counts;
	void 	print() {
		printf("L%d: ", limb_num);
		for (int i=0; i<m_counts.size(); i++)
			printf("%6d ", m_counts[i]);
		printf("\n");			
	}	
};


/* Given 2 vectors, create n vector sequence so that the end points are reached
   simultaneously */
void time_resample(struct stFloatVector mStart, struct stFloatVector mEnd, int n,
				std::vector<struct stFloatVector>& mSequence );

struct stGosubInfo {
	std::string 		m_token;		//ie. gosub or goto
	std::string			m_goto_label;	// line index
	int					m_return_index;	// -1 for a goto routine.
	bool				m_pending;
};

class VectorSequence
{
public:
	VectorSequence( std::string mFilename );
	~VectorSequence();

	void	open_file	   		( );	
	void	read_file	   		( );
	void    parse_line_nums		( );	
	int		find_label			( std::string mLabel );		// returns index into m_file_lines;

	void	select_one_vector 	( int LineIndex );
	bool	process_one_vector	( );

	struct stFloatVector	get_vector_package();
	struct stFloatVector	get_zeros_vector_package(int mLimbNum, int mNumServos );

	void	goto_first_sequence	( );
	void	prev_sequence		( );
	void	next_sequence		( );

	bool	get_is_command		( )	{ return m_is_command; };
	int 	parse_command		( std::string mToken);
	int 	execute_command		( );

	int		get_group_size()	{ return m_group_size; };
	bool	m_goto_pending;

			
protected:
	void	parse_vector_data	( std::string mData );

private:
	std::string   m_file_name;
	std::ifstream m_file;
	
	std::vector<std::string>  m_file_lines;
	std::vector<int>		  m_line_nums;
	std::vector<std::string>  m_labels;				// 1 to 1 correspond to m_file_lines.
	
	bool				m_is_command;

	std::string 		m_token;		//tbd
	std::string			m_goto_label;	// line index
	int					m_return_index;	// for a gosub routine.
	long int 			m_delay_time;
	int					m_group_size;
	
	std::vector<float>	m_angles;
	int	selected_line;
	int curr_line_num;
	int curr_limb_num;
	
};


#endif
