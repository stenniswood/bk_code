//
//  family_relationships.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 3/16/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <stdio.h>
#include <string>
#include <vector>

#include "nlp_sentence.hpp"
#include "GENERAL_protocol.hpp"
#include "family_relationships.hpp"

#include <my_global.h>
#include <mysql.h>


#define Debug 0


Relation::Relation()
{
    m_relation = SON;
}

Relation::~Relation()
{
    
}



eRelation  Relation::get_relation()                 // my son.  my wife,      my cousin     my aunt
{
    return m_relation;
}
string  Relation::get_relation_string()                 // my son.  my wife,      my cousin     my aunt
{
    switch (m_relation)
    {
    case  WIFE      :   return "wife";      break;
    case  HUSBAND   :   return "husband";   break;
    case  CHILD     :   return "child";     break;
    case  DAUGHTER  :   return "daughter";  break;
    case  SON       :   return "son";       break;
    case  BROTHER   :   return "brother";   break;
    case  SISTER    :   return "sister";    break;
    case  HALF_BROTHER: return "half_brother";  break;
    case  STEP_BROTHER: return "step_brother";  break;
    case  HALF_SISTER:  return "half_sister";   break;
    case  STEP_SISTER:  return "step_sister";   break;
    case  PARENT    :   return "parent";        break;
    case  MOTHER    :   return "mother";        break;
    case  FATHER    :   return "father";        break;
    case  UNCLE     :   return "uncle";         break;
    case  AUNT      :   return "aunt";          break;
    case  NEPHEW    :   return "nephew";        break;
    case  NIECE     :   return "niece";         break;
    case  COUSIN    :   return "cousin";        break;
    case  GRAND_MOTHER      :   return "grand mother";          break;
    case  GRAND_FATHER      :   return "grand father";          break;
    case  GRAND_SON         :   return "grand son";             break;
    case  GRAND_DAUGHTER    :   return "grand daughter";        break;
    case  GREAT_GRAND_MOTHER:   return "great grand mother";    break;
    case  GREAT_GRAND_FATHER:   return "great grand son";       break;
    case  GREAT_GRAND_SON   :   return "great grand son";       break;
    case  GREAT_GRAND_DAUGHTER: return "great grand daughter";  break;
    default: break;
    }
}

eRelation  Relation::get_inverse_relation( enum eGender mGender)         // my dad.  my husband,   my cousin     my nephew/niece
{
    switch (m_relation)
    {
    case  WIFE      : return HUSBAND;                      break;
    case  HUSBAND   : return WIFE;                         break;
    case  CHILD     : return PARENT;                       break;
    case  DAUGHTER  :  if (mGender==MALE) return FATHER; else return MOTHER;                     break;
    case  SON       :  if (mGender==MALE) return FATHER; else return MOTHER;                     break;
    case  BROTHER   :  if (mGender==MALE) return BROTHER; else return SISTER;                    break;
    case  SISTER    :  if (mGender==MALE) return BROTHER; else return SISTER;                     break;
    case  HALF_BROTHER: if (mGender==MALE) return HALF_BROTHER; else return HALF_SISTER;         break;
    case  STEP_BROTHER: if (mGender==MALE) return STEP_BROTHER; else return STEP_SISTER;         break;
    case  HALF_SISTER: if (mGender==MALE) return HALF_BROTHER; else return HALF_SISTER;          break;
    case  STEP_SISTER: if (mGender==MALE) return STEP_BROTHER; else return STEP_SISTER;          break;
    case  PARENT    :  return CHILD;  break;
    case  MOTHER    :  if (mGender==MALE) return SON; else return DAUGHTER;    break;
    case  FATHER    :  if (mGender==MALE) return SON; else return DAUGHTER;    break;
    case  UNCLE     :  if (mGender==MALE) return NEPHEW; else return NIECE;   break;
    case  AUNT      :  if (mGender==MALE) return NEPHEW; else return NIECE;   break;
    case  COUSIN    :   return COUSIN;  break;
    case  GRAND_MOTHER: if (mGender==MALE) return GRAND_SON; else return GRAND_DAUGHTER;           break;
    case  GRAND_FATHER: if (mGender==MALE) return GRAND_SON; else return GRAND_DAUGHTER;           break;
    case  GREAT_GRAND_MOTHER: if (mGender==MALE) return GREAT_GRAND_SON; else return GREAT_GRAND_DAUGHTER;    break;
    case  GREAT_GRAND_FATHER: if (mGender==MALE) return GREAT_GRAND_SON; else return GREAT_GRAND_DAUGHTER;     break;
    default: break;
    }
    return (eRelation)0;
}

string  Relation::get_inverse_relation_string(enum eGender mGender)          // my son.  my wife,      my cousin     my aunt
{
    eRelation tmp = m_relation;
    m_relation = get_inverse_relation(mGender);
    string retval = get_relation_string();
    m_relation = tmp;
    return retval;
}

/****************************************************************************************************
 
 ****************************************************************************************************/
PersonWithRelations::PersonWithRelations()
{
    
}
PersonWithRelations::~PersonWithRelations()
{
    
}

void    PersonWithRelations::add_relation_to_this( enum eRelation mRelation, PersonWithRelations& mRelative )
{
    
}

// They may want to add them as:  Mom's sister's daughter. So we traverse to mom, then decide which sister, then add the daughter.
// Or maybe a friend:  Scott Nordlund's wife is Cindy.  Finds the friend Scott Nordlund, then adds wife PersonWithRelation to him.
// this is to make the interface as natural as possible without the user having to connect everyone.
void    PersonWithRelations::add_relation_to_already_established_person( enum eRelation mRelation, PersonWithRelations& mRelative )
{
    
}


SuperString word_list;

void Init_Family_Protocol  ( )
{
    word_list = "wife, husband, child,daughter, son, brother, sister, half_brother, step_brother, half_sister, step_sister,\
    parent, mother, father, uncle, aunt, nephew, niece, cousin";
    word_list.split(',');
    word_list.trim_spaces_in_splits();
    
}
int  Parse_Family_Statement( Sentence& mSentence, ServerHandler* mh )
{
    int retval = -1;
    int word_index = -1;
    int index = mSentence.any_one_word_found_in_sentence(word_list);
    word_index = mSentence.get_word_pointer();
    
    Dprintf("Family=%d\n", index);
    // check database if relationship is known.
    if (index>0)
    {
        SuperString str ="Please tell me more about your family.  What is your ";
        str += word_list.m_split_words[word_index] + " name?";
        mh->form_response(str.c_str());
        retval =0;
    }
    
    // if not, ask for name.
    
    // may I ask some more?  who lives with you?
    return retval;
}

