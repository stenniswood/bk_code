//
//  create_object_nlp.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/14/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <list>
#include <regex>
#include <sys/types.h>
#include <time.h>

#include "sign.h"
#include "client_memory.hpp"
#include "nlp_sentence.hpp"
#include "simulator_memory.h"
#include "parameter.hpp"
#include "object_primitive_creator.hpp"
#include "create_object_nlp.hpp"
#include "it.hpp"


extern class IPC_Exception IPC_Error;

#define Debug 0

Parameter       length,width,height,radius;
Parameter       rise,run,num_steps;             // for stairway
vector<string>  m_objects_reg_names;
vector<string>  m_objects_reg_expressions;

static bool     nlp_reply_formulated=false;

/***********************************************************************
 
 ***********************************************************************/
void Init_createable_object_parameters()
{
    // Initialize all parameters to be searched
    length.m_type = FLOAT;
    length.m_requested_unit = "inch";
    length.m_name = "(length|long)";
    length.set_std_or_metric_length_units();
    length.construct_regex();
    
    width.m_type    = FLOAT;
    width.m_requested_unit = "inch";
    width.m_name    = "(width|wide)";
    width.set_std_or_metric_length_units();
    width.construct_regex();
    
    radius.m_type    = FLOAT;
    radius.m_requested_unit = "inch";
    radius.m_name    = "(radius|diameter)";
    radius.set_std_or_metric_length_units();
    radius.construct_regex();
    
    rise.m_type    = FLOAT;
    rise.m_requested_unit = "inch";
    rise.m_name    = "(rise|high|up)";
    rise.set_std_or_metric_length_units();
    rise.construct_regex();
    
    run.m_type    = FLOAT;
    run.m_requested_unit = "inch";
    run.m_name    = "(run|over)";
    run.set_std_or_metric_length_units();
    run.construct_regex();
    
    num_steps.m_type    = FLOAT;
    num_steps.m_name    = "(steps)";
    num_steps.set_std_or_metric_length_units();
    num_steps.construct_regex();
    
    height.m_type    = FLOAT;
    height.m_requested_unit = "inch";
    height.m_name    = "(height|high)";
    height.set_std_or_metric_length_units();
    height.construct_regex();
}

void Init_createable_object_names()
{
    string name;
    name = "(box|cuboid|cube)";     m_objects_reg_names.push_back(name);
    name = "(door)";                m_objects_reg_names.push_back(name);
    name = "(ibeam)";               m_objects_reg_names.push_back(name);
    name = "(cylinder)";            m_objects_reg_names.push_back(name);
    name = "(stairway)";            m_objects_reg_names.push_back(name);
    name = "(wall)";                m_objects_reg_names.push_back(name);
    name = "(chair)";               m_objects_reg_names.push_back(name);
    name = "(table center pole)";   m_objects_reg_names.push_back(name);
    name = "(table)";               m_objects_reg_names.push_back(name);
    name = "(drawer)";              m_objects_reg_names.push_back(name);
    name = "(palette)";             m_objects_reg_names.push_back(name);
    name = "(barricade)";           m_objects_reg_names.push_back(name);
    name = "(bookcase)";            m_objects_reg_names.push_back(name);
    name = "(picnic table)";        m_objects_reg_names.push_back(name);
    name = "(sphere)";              m_objects_reg_names.push_back(name);
    name = "(cabinet)";             m_objects_reg_names.push_back(name);
    name = "(brick)";               m_objects_reg_names.push_back(name);
    name = "(truss)";               m_objects_reg_names.push_back(name);
    name = "(plane truss)";         m_objects_reg_names.push_back(name);
    name = "(highway truss)";       m_objects_reg_names.push_back(name);
    name = "(rafter)";              m_objects_reg_names.push_back(name);
    name = "(light switch)";        m_objects_reg_names.push_back(name);
    name = "(hinge)";               m_objects_reg_names.push_back(name);
    name = "(road)";                m_objects_reg_names.push_back(name);
    name = "(terrain)";             m_objects_reg_names.push_back(name);
    name = "(painting)";            m_objects_reg_names.push_back(name);
    name = "(basketball)";          m_objects_reg_names.push_back(name);
    name = "(soccerball)";          m_objects_reg_names.push_back(name);
    name = "(soccerball size 4)";   m_objects_reg_names.push_back(name);
    name = "(baseball)";            m_objects_reg_names.push_back(name);
    name = "(basketball arena)";    m_objects_reg_names.push_back(name);
    m_objects_reg_names.push_back(sign_regexp);
}
void Init_createable_object_expressions()
{
    string exp;
    exp = integer_e + "?(box|cuboid|cube) ?" + length.m_regexpression + " " + width.m_regexpression + " " + height.m_regexpression;
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(door) ?" + length.m_regexpression + " " + width.m_regexpression;
    m_objects_reg_expressions.push_back( exp );

    exp = integer_e + "?(ibeam) ?" + length.m_regexpression;
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(cylinder) ?" + radius.m_regexpression + height.m_regexpression;
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(stairway) ?" + num_steps.m_regexpression + rise.m_regexpression + run.m_regexpression;
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(wall) ?" + length.m_regexpression;
    m_objects_reg_expressions.push_back( exp );

    exp = integer_e + "?(chair)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(table center pole)";
    m_objects_reg_expressions.push_back( exp );

    exp = integer_e + "?(table)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(drawer) ?" + length.m_regexpression + " " + width.m_regexpression;
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(palette)";
    m_objects_reg_expressions.push_back( exp );

    exp = integer_e + "?(barricade)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(bookcase) ?" + width.m_regexpression + " " + height.m_regexpression;
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(picnic table)";
    m_objects_reg_expressions.push_back( exp );

    exp = integer_e + "?(sphere) ?" + radius.m_regexpression;
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(cabinet)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(brick)";
    m_objects_reg_expressions.push_back( exp );

    exp = integer_e + "?(truss)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(plane truss)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(highway truss)";
    m_objects_reg_expressions.push_back( exp );
    
    exp = integer_e + "?(rafter) ?" + length.m_regexpression;
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(light switch)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(hinge)";
    m_objects_reg_expressions.push_back( exp );

    exp = integer_e + "?(road)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(terrain) ?" + length.m_regexpression + "" + width.m_regexpression;
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(painting) ?" + length.m_regexpression + "" + width.m_regexpression;
    m_objects_reg_expressions.push_back( exp );
    
    
    exp = integer_e + "?(basketball)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(soccerball)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(soccerball size 4)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?(baseball)";
    m_objects_reg_expressions.push_back( exp );
    exp = integer_e + "?((basketball )?arena)";
    m_objects_reg_expressions.push_back( exp );

    exp = integer_e + sign_regexp;
    m_objects_reg_expressions.push_back( exp );
}

void Init_Create_Protocol()
{
    Init_createable_object_parameters   ();
    Init_createable_object_names        ();
    Init_createable_object_expressions  ();
}

int Quantity=0;

int get_quantity( const char* mString )
{
    Quantity=1;                     /* implied 1 */
    std::regex regex  ( integer_e );
    std::smatch matches;
    std::string str(mString);
    std::regex_search( str, matches, regex );
    if (matches.size())
        Quantity = stoi(matches[0]);
    return Quantity;
}

/* Scan all known objects */
string& find_match_object( Sentence& mSentence )
{
    static string retval;
    retval = "";
    for (int i=0; i<m_objects_reg_expressions.size(); i++)
    {
        int match = mSentence.m_sentence.regex_find( m_objects_reg_expressions[i] );
        if (match)
        {
            retval = mSentence.m_sentence.regex_matches[0];
            
            // IF first MATCH (mSentence.m_sentence.regex_matches[1]) is an INTEGER NUMBER
            get_quantity( retval.c_str() );

            int match2 = mSentence.m_sentence.regex_find( m_objects_reg_names[i] );
            if (match2)
                retval = mSentence.m_sentence.regex_matches[0];

            //get_object_name();
            return retval;
        }
    }
    return retval;
}

void create_objects(enum eObjectTypes object_type)
{
    VerbalObject* obj=NULL;
    for (int q=0; q<Quantity; q++) {
        obj = new_object( object_type, &NewObjectParameters );
        referers.push_back( obj );
    }    
}

/*****************************************************************
 Do the work of the Telegram :
 return  -1	=> Not handled
 else number of extra bytes extracted from the mSentence buffer.
 - besides strlen(mSentence)!
 ServerHandler
 *****************************************************************/
int Parse_Create_Statement( Sentence& mSentence, string* mh )
{
    static long last_object_id=-1;
    int retval=-1;
    
    float h=10*12;
    
    if (Debug)  printf("Parse_Robot_Legs_Statement\n");
    mSentence.restore_reduced();

    // Is it a create statement?
    SuperString create_expression = "(create|put|add|make|construct)";
    int create_match = mSentence.m_reduced_sentence.regex_find(create_expression);
    if (create_match==0)
        return -1;          // No, go home

    // Yes, which object does user want us to create?
    string  object_name = find_match_object( mSentence );
    
    if (object_name.compare( "box" )==0)
    {
        length.parse( mSentence.m_sentence );
        if (length.m_matches.size())
        {
            printf("%s\n", length.m_matches[0].str().c_str() );
        }
        length.set_default_and_unit(36.0,  "inches");
        width.set_default_and_unit ( 1.5,  "inches");
        height.set_default_and_unit(7*12,  "inches");
        
        width.parse( mSentence.m_sentence );
        height.parse(mSentence.m_sentence );
        
        enum eObjectTypes object_type = BOX;
        set_default_parameters        ( object_type, &NewObjectParameters );
        NewObjectParameters.floats[0] = length.m_f_value*10;
        NewObjectParameters.floats[1] = width.m_f_value *10;
        
        create_objects( object_type);
        
        //sim_new_object( object_type, 1,length.m_f_value*10, width.m_f_value*10 );       // degrees
        *mh = "created new box";
        nlp_reply_formulated = true;
        retval = 0;
    }
    if (object_name.compare( "door" )==0)
    {
        enum eObjectTypes object_type = DOOR;
        width.set_default_and_unit (36.,  "inches");
        height.set_default_and_unit(7*12, "inches");
        
        width.parse( mSentence.m_sentence );
        height.parse(mSentence.m_sentence );
        set_default_parameters        ( object_type, &NewObjectParameters );
        NewObjectParameters.floats[0] = width.m_f_value;
        NewObjectParameters.floats[1] = height.m_f_value;
        create_objects( object_type);
        
        //sim_new_object( object_type, 1 );       // degrees
        *mh = "created new door";
        nlp_reply_formulated = true;
        retval = 0;
    }
        if (object_name.compare( "ibeam" )==0)
        {
            enum eObjectTypes object_type = IBEAM;
            set_default_parameters        ( object_type, &NewObjectParameters );
            NewObjectParameters.integers[0] = 0xFFFFFFFF;           // color
            NewObjectParameters.integers[1] = length.m_f_value*10;  //
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new ibeam";
            nlp_reply_formulated = true;
            retval = 0;
        }
        
        if (object_name.compare("cylinder")==0)
        {
            radius.parse(mSentence.m_sentence);
            enum eObjectTypes object_type = CYLINDER;
            NewObjectParameters.floats[0]   = radius.m_f_value*10;           // color
            NewObjectParameters.floats[1]   = height.m_f_value*10;
            NewObjectParameters.integers[0] = 1;
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new cylinder";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "stairway")==0)
        {
            enum eObjectTypes object_type = STAIRWAY;
            rise.parse(mSentence.m_sentence);
            run.parse(mSentence.m_sentence);
            width.parse(mSentence.m_sentence);
            num_steps.parse(mSentence.m_sentence);
            NewObjectParameters.integers[0] = num_steps.m_i_value*10;           // color
            NewObjectParameters.floats[0]   = rise.m_f_value*10;           // color
            NewObjectParameters.floats[1]   = run.m_f_value*10;
            //NewObjectParameters.floats[2] = width.m_f_value*10;
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new stairway";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "wall")==0)
        {
            bool found = length.parse(mSentence.m_reduced_sentence);
            
            if (found)
                printf("%s\n", length.m_matches[0].str().c_str() );
            length.parse(mSentence.m_sentence );
            height.parse(mSentence.m_sentence );
            if (height.m_matches.size())
                h = height.m_f_value*10;
            
            enum eObjectTypes object_type = FULL_WALL;
            NewObjectParameters.floats[0] = length.m_f_value*10;
            NewObjectParameters.floats[1] = height.m_f_value*10;
            create_objects( object_type);
            //sim_new_object( object_type, 1, length.m_f_value*10, h );       // degrees
            *mh = "created new wall";
            retval = 0;
        }
        if (object_name.compare( "chair")==0)
        {
            enum eObjectTypes object_type = CHAIR;
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new chair";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "table center pole")==0)
        {
            enum eObjectTypes object_type = TABLE_CENTER_POLE;
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new table";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "table")==0)
        {
            enum eObjectTypes object_type = TABLE_4LEGS;
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new table";
            nlp_reply_formulated = true;
            retval = 0;
        }
        
        if (object_name.compare( "drawer")==0)
        {
            enum eObjectTypes object_type = DRAWER;
            NewObjectParameters.floats[0] = length.m_i_value*10;
            NewObjectParameters.floats[1] = width.m_f_value*10;
            NewObjectParameters.floats[1] = height.m_f_value*10;
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new drawer";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "palette")==0)
        {
            enum eObjectTypes object_type = PALLETTE;
            
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new palette";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "barricade")==0)
        {
            enum eObjectTypes object_type = BARRICADE;
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new barricade";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "bookcase")==0)
        {
            enum eObjectTypes object_type = BOOKCASE;
            
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new bookcase";
            nlp_reply_formulated = true;
            retval = 0;
        }
        
        if (object_name.compare( "picnic table")==0)
        {
            enum eObjectTypes object_type = PICNIC_TABLE;
            
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new picnic table";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "sphere")==0)
        {
            enum eObjectTypes object_type = SPHERE;
            NewObjectParameters.floats[1] = radius.m_f_value*10;
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new sphere";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "cabinet")==0)
        {
            enum eObjectTypes object_type = CABINET;
            
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new cabinet";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "brick")==0)
        {
            enum eObjectTypes object_type = BRICK;
            set_default_parameters( object_type, &NewObjectParameters );
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new brick";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "truss")==0)
        {
            enum eObjectTypes object_type = TRUSS3D;
            set_default_parameters( object_type, &NewObjectParameters );
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new truss";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "plane truss")==0)
        {
            enum eObjectTypes object_type = TRUSS2D;
            set_default_parameters( object_type, &NewObjectParameters );
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new 2D truss";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "highway truss")==0)
        {
            enum eObjectTypes object_type = TRUSSHIGHWAY;
            set_default_parameters( object_type, &NewObjectParameters );
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new highway truss";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "rafter")==0)
        {
            enum eObjectTypes object_type = RAFTER;
            set_default_parameters( object_type, &NewObjectParameters );
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new rafter";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "light switch")==0)
        {
            enum eObjectTypes object_type = LIGHTSWITCH;
            set_default_parameters( object_type, &NewObjectParameters );
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new light switch";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "hinge")==0)
        {
            enum eObjectTypes object_type = HINGE;
            set_default_parameters( object_type, &NewObjectParameters );
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new hinge";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "road")==0)
        {
            enum eObjectTypes object_type = ROAD;
            set_default_parameters( object_type, &NewObjectParameters );
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new road";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "terrain")==0)
        {
            enum eObjectTypes object_type = TERRAIN;
            set_default_parameters( object_type, &NewObjectParameters );
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new terrain";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "painting")==0)
        {
            enum eObjectTypes object_type = PAINTING;
            NewObjectParameters.floats[0] = length.m_i_value*10;
            NewObjectParameters.floats[1] = width.m_f_value*10;
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh= "created new painting";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "basketball")==0)
        {
            enum eObjectTypes object_type = BASKETBALL;
            
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new basketball";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "soccerball")==0)
        {
            enum eObjectTypes object_type = SOCCERBALL_5;
            
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new soccer ball";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "soccerball size 4")==0)
        {
            enum eObjectTypes object_type = SOCCERBALL_4;
            
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new womens soccer ball";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "baseball")==0)
        {
            enum eObjectTypes object_type = BASEBALL;
            
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new baseball";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if ((object_name.compare( "arena")==0) | (object_name.compare( "BASKETBALL ARENA")==0))
        {
            enum eObjectTypes object_type = BASKETBALL_ARENA;
            
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new painting";
            nlp_reply_formulated = true;
            retval = 0;
        }
        if (object_name.compare( "street sign")==0)
        {
            enum eObjectTypes object_type = STREET_SIGN;
            set_default_parameters( object_type, &NewObjectParameters );
            
            create_objects( object_type);
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new street sign";
            nlp_reply_formulated = true;
            retval = 0;
        }

        char tmp_response[80];
        // Add on the ID to the response.
        if (retval==0) {
            /* Client Memory Acknowledgement:  
             Viki increases count.
             Instant acknowledges.
             
             Sim Memory Acknowledgement.
             Instant increases the count.
             home3D acknowledges.
             */
            int result = sim_wait_for_response();
            if (result) {
                last_object_id = ipc_memory_sim->new_object_ids[ipc_memory_sim->num_valid_ids-1];
                sim_acknowledge_response();
                sprintf(tmp_response, "%s with ID of %ld.", mh->c_str(), last_object_id );
            }
        }
    
    if (retval>-1)  printf( "Parse_Create_Statement done\n" );
    return retval;
}



