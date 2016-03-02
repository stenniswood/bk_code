
#include <string>
#include <list>


int get_preposition_index(char*  mSentence, std::list<std::string>& mPreposition_list );


void diagram_sentence(	std::string* subject,
						std::string* verb, 		
						std::string* adjective,
						std::string* object,
						std::string* preposition = NULL);

std::string* extract_word(char* mSentence, std::list<std::string> *m_word_list );

/* Scan thru many word lists: */
void extract_nlp_words( std::list<std::string>* subject_list,
						std::list<std::string>* verb_list,
						std::list<std::string>* object_list,
						std::list<std::string>* adjective_list );

// return of 0=> match :
int compare_word( std::string* mSubject, char* mWord );



