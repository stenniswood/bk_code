//
//  question_to_user.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 6/13/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef question_to_user_hpp
#define question_to_user_hpp

#include <stdio.h>
#include <vector>
#include "serverthread.hpp"
#include "super_string.hpp"
#include "nlp_sentence.hpp"


enum eElementType {
    QUESTION,
    ACTION
};

enum state {
    INITIAL,
    AWAITING_RESPONSE,
    ANSWERED,
    BAILED,             /* user does not wish to answer the question. */
    FOLLOWUP_REQUIRED   /* incomplete answer                          */
};

/* Basic element in the UserForm array (Question | Action) */
class UserInteractionElement
{
public:
    UserInteractionElement ();
    ~UserInteractionElement();
    
    virtual void PreAction ();
    virtual void PostAction();

    bool    is_pending    ( );
    bool    is_completed  ( );
    
    enum eElementType m_type;
    ServerHandler*    mh;
    int               m_state;
};

class ActionToUserResponse;

/* Basic Question Element */
class QuestionToUser : public UserInteractionElement
{
public:
    QuestionToUser();
    ~QuestionToUser();
    virtual void PreAction      ( );
    virtual void PostAction     ( );
    
    
    int     parse_response      ( SuperString& mUserText, ServerHandler* mh );
    bool    is_unasked          ( );
    bool    is_awaiting_response( );
    void    do_callback         ( );
    
    SuperString  m_question;
    SuperString  m_responses;
    SuperString  m_answer;
    vector<int>  m_choices;         // stores the selected item for each wordlist.
    std::smatch  m_matches;
    ActionToUserResponse*   m_callback_action;
};

/* This class answers "one of" type questions. */
class YesNoQuestionToUser : public QuestionToUser
{
public:
    YesNoQuestionToUser ();
    ~YesNoQuestionToUser();

    int    get_response_catagory( );

    vector<SuperString>  m_index_response;      // a regex for each possible response catagory.
};


//#include "ordering_protocol.hpp"
class UserOrderHistory;

/* Basic Action Element */
class ActionToUserResponse : public UserInteractionElement
{
public:
    ActionToUserResponse ();
    ~ActionToUserResponse();
    
    virtual void PreAction();
    //virtual void PostAction();
    
    void*        m_callback;        //
    void*        m_datum1;          // MenuItem*
    void*        m_datum2;          // Index into m_required_options[m_datum2]
    void*        m_datum3;          // RestaurantCluster* for save_order_cb
    
    QuestionToUser* m_question;     //
    UserOrderHistory*   m_user;     // UserOrderHistory*   where to push the MenuItem to.
};


/********************************************************************/

/********************************************************************/
class FoodOrderOptionAction : public ActionToUserResponse
{
public:
    
};

/********************************************************************/
// A collection of questions & actions.
/********************************************************************/
class UserForm
{
public:
    UserForm ();
    ~UserForm();

    bool    any_unasked_questions(                       );
    bool    any_questions_awaiting_response(             );
    int     find_first_unanswered_question (             );
    
    int     parse_response       ( SuperString mSentence );
    void    ask                  ( ServerHandler* mh     );
    void    on_empty_list        (                       );

    vector<UserInteractionElement*> user_questions;
    void (*all_questions_answered_cb)( UserForm* mQ, ServerHandler* mh );
    ServerHandler* mh;
};

extern UserForm Qs_to_user;


#endif /* question_to_user_hpp */

//void*        m_datum1;          // MenuItem*
//void*        m_datum2;          // Index into m_required_options[m_datum2]
//void*        m_user;            // UserOrderHistory*   where to push the MenuItem to.
//(QuestionToUser* mQ, UserOrderHistory* mUser);
