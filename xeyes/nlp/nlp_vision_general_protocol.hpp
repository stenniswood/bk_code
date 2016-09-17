#include "nlp_sentence.hpp"
#include "serverthread.hpp"

//char		NLP_Response[255];

void 		Init_General_Protocol( );

int		Get_Statement_symantic_id	( Sentence& mSentence   );
void	process_person_query		( Sentence& theSentence, int mquestion_id );

int		Get_Future_Statement_symantic_id( Sentence& theSentence );
int     process_future_person_query		( int future_action_request );

char*   Parse_Statement	 ( char*  mSentence, ServerHandler* mh );

