#ifdef  __cplusplus
extern "C" {
#endif


extern BOOL nlp_reply_formulated ;
extern char	NLP_Response[];


/*struct sObject
{
	sObject(const char* mNewname, int mId) {
		name = mNewname;
		id   = mId;
	};
	std::string  name;
	int		id;	
};*/

void 	Init_General_Protocol		( );
int 	find_subject				( char* mSubject  );

//struct sObject*	extract_subject		( char* mSentence );
std::string*    extract_verb		( char*  mCommand );
int 			get_preposition_index(char*  mSentence);
//struct sObject* extract_object		( char*  mSentence);

char* 	Parse_Statement				( char*  mCommand );

#ifdef  __cplusplus
}
#endif

