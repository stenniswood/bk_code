//
//  ThreeD_object_protocol.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/20/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#include "ThreeD_object_protocol.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <time.h>
#include <string>

#include "protocol.h"
#include "devices.h"
#include "GENERAL_protocol.hpp"
#include "nlp_extraction.hpp"
#include "client_memory.hpp"
#include "nlp_sentence.hpp"
#include "client_to_socket.hpp"
#include "alias.hpp"
#include "simulator_memory.h"

#include "parameter.hpp"
#include "ThreeD_object_protocol.hpp"
#include "positioner_2d.hpp"
#include "positioner_3d.hpp"
#include "color_protocol.hpp"


extern class IPC_Exception IPC_Error;

/* Suggested statements:
    Make a box a lot longer than it is wide.
    Create a cylinder
    Lay the box on top the cylinder.
    connect the two with a cross bar.
 
 */

#define Debug 0

/***********************************************************************
 
 ***********************************************************************/

void Init_ThreeD_Protocol()
{

}


// Prototype here.  Implementation is in 'robot_legs_protocol.cpp'
int get_room_coord( Sentence& mSentence, MathVector& center );


/**** ACTION INITIATORS:    (4 possible actions) *****/


/*****************************************************************
 Do the work of the Telegram :
 return  -1	=> Not handled
 else number of extra bytes extracted from the mSentence buffer.
 - besides strlen(mSentence)!
 *****************************************************************/
int Parse_ThreeD_Statement( Sentence& mSentence, ServerHandler* mh )
{
    static long last_object_id=-1;
    int retval=-1;
    //enum ServoIndices servo_index;
    //int Robot_id=0;
    Parameter length,width,height;
    length.m_type = FLOAT;
    length.m_name = "(length|long)";
    length.set_std_or_metric_length_units();
    length.construct_regex();
    
    width.m_type    = FLOAT;
    width.m_name    = "(width|wide)";
    width.set_std_or_metric_length_units();
    width.construct_regex();

    height.m_type    = FLOAT;
    height.m_name    = "(height|high)";
    height.set_std_or_metric_length_units();
    height.construct_regex();
    float h=10*12;

    if (Debug)  printf("Parse_Robot_Legs_Statement\n");
    mSentence.restore_reduced();

    try {
    SuperString create_expression = "(create|put|add|make|construct)";
    int create_match = mSentence.m_reduced_sentence.regex_find(create_expression);
        
    if (create_match)
    {
        if (mSentence.is_found_in_sentence("box"))
        {
            length.parse( mSentence.m_sentence );
            if (length.m_matches.size())
            {
                printf("%s\n", length.m_matches[0].str().c_str() );
            }
            width.parse( mSentence.m_sentence );
            height.parse(mSentence.m_sentence );

            enum eObjectTypes object_type = BOX;
            sim_new_object( object_type, 1,length.m_f_value*10, width.m_f_value*10 );       // degrees
            mh->form_response("created new box");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence("door"))
        {
            enum eObjectTypes object_type = DOOR;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new door");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "ibeam" ))
        {
            enum eObjectTypes object_type = IBEAM;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new ibeam");
            retval = 0;
        }

        if (mSentence.is_found_in_sentence("cylinder"))
        {
            enum eObjectTypes object_type = CYLINDER;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new cylinder");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "stairway"))
        {
            enum eObjectTypes object_type = STAIRWAY;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new stairway");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "wall"))
        {
            bool found = length.parse(mSentence.m_reduced_sentence);
            
            if (found)
                printf("%s\n", length.m_matches[0].str().c_str() );
            height.set_default_and_unit( 12*10, "inches" );
            height.parse(mSentence.m_sentence );
            if (height.m_matches.size())
                h = height.m_f_value*10;
            
            enum eObjectTypes object_type = FULL_WALL;
            sim_new_object( object_type, 1, length.m_f_value*10, h );       // degrees
            mh->form_response("created new wall");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "chair"))
        {
            enum eObjectTypes object_type = CHAIR;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new chair");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "table center pole"))
        {
            enum eObjectTypes object_type = TABLE_CENTER_POLE;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new table");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "table"))
        {
            enum eObjectTypes object_type = TABLE_4LEGS;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new table");
            retval = 0;
        }

        if (mSentence.is_found_in_sentence( "drawer"))
        {
            enum eObjectTypes object_type = DRAWER;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new drawer");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "palette"))
        {
            enum eObjectTypes object_type = PALLETTE;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new palette");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "barricade"))
        {
            enum eObjectTypes object_type = BARRICADE;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new barricade");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "bookcase"))
        {
            enum eObjectTypes object_type = BOOKCASE;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new bookcase");
            retval = 0;
        }
        
        if (mSentence.is_found_in_sentence( "picnic table"))
        {
            enum eObjectTypes object_type = PICNIC_TABLE;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new picnic table");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "sphere"))
        {
            enum eObjectTypes object_type = SPHERE;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new sphere");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "cabinet"))
        {
            enum eObjectTypes object_type = CABINET;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new cabinet");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "brick"))
        {
            enum eObjectTypes object_type = BRICK;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new brick");
            retval = 0;
        }
        
        if (mSentence.is_found_in_sentence( "truss"))
        {
            enum eObjectTypes object_type = TRUSS3D;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new truss");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "plane truss"))
        {
            enum eObjectTypes object_type = TRUSS2D;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new 2D truss");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "highway truss"))
        {
            enum eObjectTypes object_type = TRUSSHIGHWAY;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new highway truss");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "rafter"))
        {
            enum eObjectTypes object_type = RAFTER;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new rafter");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "light switch"))
        {
            enum eObjectTypes object_type = LIGHTSWITCH;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new light switch");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "hinge"))
        {
            enum eObjectTypes object_type = HINGE;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new hinge");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "road"))
        {
            enum eObjectTypes object_type = ROAD;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new road");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "terrain"))
        {
            enum eObjectTypes object_type = TERRAIN;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new terrain");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "painting"))
        {
            enum eObjectTypes object_type = PAINTING;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new painting");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "basketball"))
        {
            enum eObjectTypes object_type = BASKETBALL;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new basketball");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "soccer ball"))
        {
            enum eObjectTypes object_type = SOCCERBALL_5;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new soccer ball");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "soccer ball size 4"))
        {
            enum eObjectTypes object_type = SOCCERBALL_4;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new womens soccer ball");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "baseball"))
        {
            enum eObjectTypes object_type = BASEBALL;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new baseball");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "ARENA") | mSentence.is_found_in_sentence( "BASKETBALL ARENA"))
        {
            enum eObjectTypes object_type = BASKETBALL_ARENA;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new painting");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "street sign"))
        {
            enum eObjectTypes object_type = STREET_SIGN;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new street sign");
            retval = 0;
        }

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
                sprintf(NLP_Response, "%s with ID of %ld.", NLP_Response, last_object_id );
            }
        }
    }
    if (mSentence.is_found_in_sentence("show camera"))
    {
        ipc_memory_sim->Command = COMMAND_CHANGE_GLOBAL_VAR;
        ipc_memory_sim->object_id      = 0;
        if (mSentence.is_found_in_sentence("don't") || mSentence.is_found_in_sentence("off"))
        {
            ipc_memory_sim->object_datum1  = 0;
            mh->form_response("okay, camera is off");
        }
        else if (mSentence.is_found_in_sentence("show") || mSentence.is_found_in_sentence("on"))
        {
            ipc_memory_sim->object_datum1  = 1;
            mh->form_response("showing camera on wall inside apartment.");
        }
        ipc_memory_sim->CommandCounter++;
        retval = 0;
    }
    if (mSentence.is_found_in_sentence("robot view"))
    {
        ipc_memory_sim->Command = COMMAND_CHANGE_GLOBAL_VAR;
        ipc_memory_sim->object_id      = 1;
        if (mSentence.is_found_in_sentence("don't") || mSentence.is_found_in_sentence("off"))
        {
            ipc_memory_sim->object_datum1  = 0;
            mh->form_response("showing user's view point");
        }
        else if (mSentence.is_found_in_sentence("show") || mSentence.is_found_in_sentence("on"))
        {
             ipc_memory_sim->object_datum1  = 1;
             mh->form_response("showing robot's viewpoint.");
        }
        ipc_memory_sim->CommandCounter++;
        retval = 0;
    }
    
    if (mSentence.is_found_in_sentence("move") || mSentence.is_found_in_sentence("put"))
    {
        long Object_id = last_object_id;
        MathVector NewLocation(3);
        MathVector NewOrientation(3);
        NewOrientation[0] = 999;
        if (get_room_coord( mSentence, NewLocation )==1)
        {
            if ((mSentence.is_found_in_sentence("along") || mSentence.is_found_in_sentence("against")) &&
                mSentence.is_found_in_sentence("wall"))
            {
                // First we have to know which room we are in.
                ipc_memory_sim->object_type=2;
                NewLocation[2]    = 6;
                NewOrientation[0] = 0;
                NewOrientation[1] = 180;
                NewOrientation[2] = 0;
            }
            sim_move_object( Object_id, NewLocation, NewOrientation );
            mh->form_response("moving");
            retval = 0;
        }
    }
    
    if (mSentence.is_found_in_sentence("delete"))
    {
        long Object_id = last_object_id;
        sim_delete_object( Object_id );
        mh->form_response("dont mind if i do.");
        retval = 0;
    }

    if (mSentence.is_found_in_sentence("rotate"))
    {
        long Object_id = last_object_id;
        MathVector NewLocation(3);
        MathVector NewOrientation(3);
        NewOrientation[0] = mSentence.get_nth_word_as_a_number( 2 );
        NewOrientation[1] = mSentence.get_nth_word_as_a_number( 3 );
        NewOrientation[2] = mSentence.get_nth_word_as_a_number( 4 );
        sim_move_object( Object_id, NewLocation, NewOrientation );
        mh->form_response("moving");
        retval = 0;
    }
    
    if (mSentence.is_found_in_sentence( "stretch"))
    {
        if (mSentence.is_found_in_sentence("it"))
        {
            enum eObjectTypes object_type = BOX;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("created new ibeam");
            retval = 0;
        }
    }
    if (mSentence.is_found_in_sentence( "call it") || mSentence.is_found_in_sentence( "name it"))
    {
    }
    
    // also change color
    if (mSentence.is_found_in_sentence( "make thicker") || mSentence.is_found_in_sentence( "make thinner"))
    {
        if (mSentence.is_found_in_sentence("box"))
        {
            enum eObjectTypes object_type = BOX;
            sim_new_object( object_type, 1 );       // degrees
            mh->form_response("thinning new box");
            retval = 0;
        }
    }
    }
    catch ( IPC_Exception e )
    {
        mh->form_response( e.what() );
    }

    // ALL CAD 
    //const char* end_of_telegram = mSentence + strlen(mSentence) +1 /*nullterminator*/;
    
    int result =  Parse_2D_Positioning_Statement( mSentence, mh );
    if (result>=0)          return ( + result);
    result     =  Parse_3D_Positioning_Statement( mSentence, mh );
    if (result>=0)          return ( + result);
    string Response;
    result     =  Parse_color_statement         ( mSentence, Response );
    if (result>=0)          return ( + result);

    
    if (retval>-1)  printf( "Parse_Robot_Legs_Statement done\n" );
    return retval;
}

