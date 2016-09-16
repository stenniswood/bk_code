#include "nlp_sentence.hpp"
#include "serverthread.hpp"

//char		NLP_Response[255];

void 		Init_General_Protocol( );

int		Get_Statement_symantic_id		( Sentence& mSentence   );
int		Get_Future_Statement_symantic_id( Sentence& theSentence );

char*   Parse_Statement	 ( char*  mSentence, ServerHandler* mh );

