//
//  question_to_user.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 6/13/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include "serverthread.hpp"
//#include "super_string.hpp"

#include "question_to_user.hpp"
#include "menu_items.hpp"
#include "specific_restaurants.hpp"

UserForm Qs_to_user;

/******************************************************************************************/
/*************************** UserInteractionElement ***************************************/
/******************************************************************************************/
UserInteractionElement::UserInteractionElement ()
{
    m_state = INITIAL;
    m_type  = QUESTION;
    mh      = NULL;
}
UserInteractionElement::~UserInteractionElement()
{
    
}
void UserInteractionElement::PreAction()
{
    m_state = AWAITING_RESPONSE;
}
void UserInteractionElement::PostAction()
{
    m_state = ANSWERED;
}
bool UserInteractionElement::is_pending( )
{
    return (m_state < AWAITING_RESPONSE);
}
bool UserInteractionElement::is_completed( )
{
    return (m_state >= ANSWERED);
}

/********************************************************
 ******************** QuestionToUser ********************
 ********************************************************/
QuestionToUser::QuestionToUser()
{
    m_type  = QUESTION;
    m_callback_action=NULL;
}
QuestionToUser::~QuestionToUser()
{
    
}
void QuestionToUser::PreAction()
{
    UserInteractionElement::PreAction();
    // Ask the question
}

void QuestionToUser::PostAction()
{
    UserInteractionElement::PostAction();
    // Parse the response.
}

bool QuestionToUser::is_unasked( )
{
    return (m_state < AWAITING_RESPONSE);
}
bool QuestionToUser::is_awaiting_response( )
{
    return (m_state == AWAITING_RESPONSE);
}

int QuestionToUser::parse_response(SuperString& mUserText, ServerHandler* mh)
{
    m_answer = mUserText;
    regex response_e( m_responses );
    regex_search ( mUserText, m_matches, response_e );
    size_t valid_response = m_matches.size();
    
    //string UserText = mUserText.c_str();
    //string tmp = mUserText.c_str();
    //char* t = mUserText.begin();
    
    for(auto it = std::sregex_iterator(mUserText.begin(), mUserText.end(), response_e);
        it != std::sregex_iterator();
        ++it)
    {
        //m_choices.push_back( it->position() );
    }
    
    //int valid_response = mUserText.regex_found_in_sentence( m_responses, 0, &m_choices, &m_choices );
    if (valid_response) {
        m_state = ANSWERED;
        do_callback();
        return 1;
    } else {
        m_state = BAILED;
        return 0;
    }
}

void QuestionToUser::do_callback( )
{
    void (*callback) ( UserInteractionElement* mQ );
    if (m_callback_action) {
        callback         = (void(*)( UserInteractionElement* ))m_callback_action->m_callback;
        if (callback)
            callback( this );
    }
}
/******************************************************************************************/
/**************************** YES NO QUESTION ELEMENT *************************************/
/******************************************************************************************/

YesNoQuestionToUser::YesNoQuestionToUser ()
{
    
}
YesNoQuestionToUser::~YesNoQuestionToUser()
{
    
}
int YesNoQuestionToUser::get_response_catagory( )
{
    for (int i=0; i<m_index_response.size(); i++)
    {
        regex response_e( m_index_response[i] );
        smatch matches;
        regex_search ( m_answer, matches, response_e );
        size_t valid_response = matches.size();
        if (valid_response)
            return i;
    }
    return -1;
}

/******************************************************************************************/
/******************************* ACTION ELEMENT *******************************************/
/******************************************************************************************/
ActionToUserResponse::ActionToUserResponse ()
{
    m_type     = ACTION;
    m_callback = NULL;    //
    m_datum1   = NULL;    // ServerHandler  MenuItem*
    m_datum2   = NULL;    // Index into m_required_options[m_datum2]
    m_question = NULL;    //
    m_user     = NULL;    // UserOrderHistory*   where to push the MenuItem to.
}
ActionToUserResponse::~ActionToUserResponse()
{
    
}
void ActionToUserResponse::PreAction ()
{
    UserInteractionElement::PreAction();
    
    void (*callback)  ( UserInteractionElement* mQ );
    callback        = (void(*)( UserInteractionElement* ))m_callback;
    callback( this );
}

/******************************************************************************************/
/******************************* USER FORM ************************************************/
/******************************************************************************************/
UserForm::UserForm()
{
    
}
UserForm::~UserForm()
{
    
}
bool UserForm::any_unasked_questions( )
{
    for (int q=0; q<user_questions.size(); q++)
        if (user_questions[q]->m_type==QUESTION)
            if ( ((QuestionToUser*)user_questions[q])->is_unasked()==true)
                return true;
    return false;
}

bool UserForm::any_questions_awaiting_response()
{
    for (int q=0; q<user_questions.size(); q++)
        if (user_questions[q]->m_type==QUESTION)
            if ( ((QuestionToUser*)user_questions[q])->is_awaiting_response()==true)
                return true;
    return false;
}

int UserForm::find_first_unanswered_question()
{
    for (int q=0; q<user_questions.size(); q++)
        if (user_questions[q]->m_type==QUESTION)
            if (((QuestionToUser*)user_questions[q])->is_awaiting_response()==true)
                return q;
    return -1;
}

int UserForm::parse_response(SuperString mSentence)
{
    int retval=0;
    int q = find_first_unanswered_question();
    QuestionToUser* Q;
    if (q > -1)
    {
        // Processs response :
        Q = (QuestionToUser*)user_questions[q];
        retval = Q->parse_response( mSentence, mh );
        if ((retval) && (Q->m_state == ANSWERED))
        {
            // remove from queue :
            user_questions.erase( user_questions.begin()+q );
            if (user_questions.size()==0)
                on_empty_list();
        }
        // invalid response
    }
    return retval;
}

void UserForm::ask(ServerHandler* mh)
{
    QuestionToUser* Q=NULL;
    for (int q=0; q<user_questions.size(); q++)  {
        if (user_questions[q]->m_type==QUESTION) {
            Q = (QuestionToUser*)user_questions[q];
            if (Q->is_unasked()) {
                mh->form_response( Q->m_question.c_str() );
                Q->m_state = AWAITING_RESPONSE;
                return;
            }
        }
    }
}

void UserForm::on_empty_list(  )
{
    if (all_questions_answered_cb)
        all_questions_answered_cb( this, mh );
}



