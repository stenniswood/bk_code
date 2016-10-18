//
//  family_relationships.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 3/16/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef family_relationships_hpp
#define family_relationships_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "nlp_sentence.hpp"
#include "serverthread.hpp"

using namespace std;

/* the owner "self" will be the center of the world.  Every relations added, must somehow be connected to the "self".
    Relations can be understood outside of self.  For example Joe my neighbor John's son graduated from college.
    And the computer if speaking to Joe could translate, "your dad John".
 
 */
enum eGender
{
    MALE,
    FEMALE
};

enum eRelation {
    WIFE, HUSBAND, CHILD, DAUGHTER, SON,
    BROTHER, SISTER, HALF_BROTHER, STEP_BROTHER, HALF_SISTER, STEP_SISTER, PARENT, MOTHER, FATHER,
    UNCLE, AUNT, NEPHEW, NIECE, COUSIN,
    GRAND_MOTHER, GRAND_FATHER, GRAND_SON, GRAND_DAUGHTER,
    GREAT_GRAND_MOTHER, GREAT_GRAND_FATHER, GREAT_GRAND_SON, GREAT_GRAND_DAUGHTER
};



/* First class describes just 2 people.  1 relationship. */
class Relation
{
public:
    Relation();
    ~Relation();
    
    eRelation   get_relation();                 // my son.  my wife,      my cousin     my aunt
    eRelation   get_inverse_relation(enum eGender mGender);         // my dad.  my husband,   my cousin     my nephew/niece
    
    string      get_relation_string();          // my son.  my wife,      my cousin     my aunt
    string      get_inverse_relation_string(enum eGender mGender);          // my son.  my wife,      my cousin     my aunt
    
    string      m_relation_name;
    enum eRelation  m_relation;
};


/* A person has many relations */
class PersonWithRelations
{
public:
    PersonWithRelations();
    ~PersonWithRelations();
    
    // They may want to add them as:  Mom's sister's daughter. So we traverse to mom, then decide which sister, then add the daughter.
    // Or maybe a friend:  Scott Nordlund's wife is Cindy.  Finds the friend Scott Nordlund, then adds wife PersonWithRelation to him.
    // this is to make the interface as natural as possible without the user having to connect everyone.
    void    add_relation_to_already_established_person( enum eRelation mRelation, PersonWithRelations& mRelative );
    void    add_relation_to_this( enum eRelation mRelation, PersonWithRelations& mRelative );
    

    int             m_id;
    string          m_first_name;
    string          m_last_name;
    enum eGender    m_gender;
    
    vector<Relation>   m_vips;
};


void Init_Family_Protocol  ( );
int  Parse_Family_Statement( Sentence& mSentence, ServerHandler* mh );


#endif /* family_relationships_hpp */
