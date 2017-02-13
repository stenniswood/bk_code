
#include "nlp_form.hpp"


nlpForm::nlpForm ()
{

}

nlpForm::~nlpForm()
{

}

void nlpForm::intro_prompt()
{
	m_prompt
}

void nlpForm::intro_prompt()
{

}
void nlpForm::parse_sentence( Sentence& mSentence )
{

}


void test_nlpForm()
{
	nlpForm Birthdate();
	Birthdate.m_field_names.push_back("Month");
	Birthdate.m_field_names.push_back("Date");	
	Birthdate.m_field_names.push_back("Year");
	
	Birthdate.m_field_validations.push_back("(January|February|March|April|May|June|July|August|September|October|November|December)");
	Birthdate.m_field_validations.push_back("0..31");	
	Birthdate.m_field_validations.push_back("1900..2017");	
	
}
