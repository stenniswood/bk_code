#ifndef _NLP_FORM_HPP_
#define _NLP_FORM_HPP_

/* 
	Class to handle a form.
	Text will be passed back via ServerHandler object.

	Lets keep this simple for starters, no paraphrasing, etc.	
*/
class nlpForm
{
public:
	nlpForm ();
	~nlpForm();

	void	intro_prompt();
	void	parse_sentence( Sentence& mSentence );
	bool	all_fields_valid();
	void	submit( ServerHandler* mh );
		
	std::string		m_prompt;
	vector<string>	m_field_names;
	vector<string>	m_field_validations;	/* ie. (January|February|...|December)  */	
	vector<string>	m_field_values;			// User spoken results.
private:
	
};


void test_nlpForm();


#endif
