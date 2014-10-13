#ifdef  __cplusplus
extern "C" {
#endif


extern BOOL nlp_reply_formulated ;
extern char	NLP_Response[];


struct sObject
{
	sObject(const char* mNewname, int mId) { 
		name = mNewname;
		id   = mId;
	};
	std::string  name;
	int		id;	
};

void diagram_sentence(	std::string* subject,
						std::string* verb, 		
						std::string* adjective,
						std::string* object	);

void 	Init_General_Protocol		( );
int 	find_subject				( char* mSubject  );

struct sObject*	extract_subject		( char* mSentence );
std::string* 	extract_word(char* mSentence, std::list<std::string> *m_word_list );
std::string*    extract_verb		( char*  mCommand );
int 			get_preposition_index(char*  mSentence);
struct sObject* extract_object		( char*  mSentence);

void 	Parse_Statement				( char*  mCommand );
BOOL 	Perform_VIDEO_actions		( char* mAction, 
									  char* mAdjectives, 
									  char* mObjects );


#ifdef  __cplusplus
}
#endif

