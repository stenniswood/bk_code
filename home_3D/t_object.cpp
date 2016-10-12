#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "vector.hpp"
#include "verbal_object.h"
#include "t_object.hpp"
#include "proc_client_cmds.h"
#include "object_primitive_creator.hpp"


extern MYSQL *inventory_db;

MathVector   asdf2(3);
void testasdf2()
{
    double xx = 2220.0;
    
    asdf2.dimension(3);
    asdf2[0]=123.0;
    asdf2[1]=(double)123.0;
    asdf2[2]=xx;
}

using namespace std;


size_t	query( string mquery_string, MYSQL_RES*& result, bool mRetrieving )
{
    printf("Query:  %s\n", mquery_string.c_str() );
    if (mysql_query(inventory_db, mquery_string.c_str() ))
    {
        fprintf(stderr, "Object: %s\n", mysql_error(inventory_db));
    }
    
    // Question : what happens when no results?
    //   Answer : if  mysql_store_result returns a non-zero value
    size_t row_count=0;
    result = NULL;
    if (mRetrieving)
    {
        result = mysql_store_result( inventory_db );
        if (result == NULL)  {		// here usually means that the query was an UPDATE or INSERT.
            return 0;
        }
        row_count = mysql_num_rows(result);
    }
    // After using the results, the caller has to mysql_free_result(); !!
    //printf("query: # results=%d\n", row_count );
    
    return row_count;
}

/*******************************************************************
	PRINT FUNCTIONS:
 ********************************************************************/
void print_results(  MYSQL_RES* result )
{
    MYSQL_ROW row;
    int num_fields = mysql_num_fields(result);
    printf(" fields in sql result=%d ", num_fields );
    
    // For each result :
    while ( (row = mysql_fetch_row(result)) )
    {
        printf("\n");
        for (int col=0; col<num_fields; col++)
            printf(" %s ", row[col] );
    }
    printf("\n" );
}


//===========================================================================
//		NAMES DATABASE TABLE CLASS
//===========================================================================
Name::Name()
{
}
Name::~Name()
{
}

////////////////////// START OF NAME QUERIES //////////////////////
/* Does robot have a knowledge of the object (ie. exist in names table)
 May not own the object (indicated by the objects table) For instance paper towels.
 knows about them, but we do not have any now.
 */
void Name::find_matching_names_nq( vector<string> mSentence )
{
    // Once we have the results, need to get the indexes within the sentence too
    long int  size    = mSentence.size();
    query_string = "SELECT * FROM home_inventory.names WHERE ";
    for (int w=0; w<size; w++)
    {
        query_string += "name='";
        query_string += mSentence[w].c_str();
        query_string += "'";
        if (w<(size-1))
        {
            query_string += " OR name='";
            query_string += mSentence[w].c_str();
            query_string += " ";
            query_string += mSentence[w+1].c_str();
            query_string += "'";
            query_string += " OR ";
        }
    }
    query_string += ";";
    //printf("ObjectAwareness: %s\n", query_string.c_str() );
}
void Name::find_name_id_nq	( string mName )
{
    query_string = "SELECT * FROM `home_inventory`.`names` WHERE name='";
    query_string += mName;
    query_string += "';";
}
void Name::find_name_nq( int mName_id )
{
    query_string = "SELECT * FROM `home_inventory`.`names` WHERE _id='";
    char tmp[16];
    sprintf(tmp, "%d", mName_id);
    query_string += tmp;
    query_string += "';";
}
void Name::find_name_by_descriptor_id_nq( int mObjDescriptor_id )
{
    query_string = "SELECT * FROM `home_inventory`.`names` WHERE object_descriptor='";
    char tmp[16];
    sprintf(tmp, "%d", mObjDescriptor_id);
    query_string += tmp;
    query_string += "';";
}

void Name::find_name_by_instance_id_nq( int mObjInstance_id )
{
    query_string  = "SELECT * FROM `home_inventory`.`names` INNER JOIN ";
    query_string += " home_inventory.object_instances ";
    query_string += " ON home_inventory.names.object_descriptor = home_inventory.object_instances.object_description_id ";
    query_string += " WHERE home_inventory.object_instances._id='";
    char tmp[16];
    sprintf(tmp, "%d", mObjInstance_id);
    query_string += tmp;
    query_string += "';";
}

void Name::add_new_name_nq	( string mName, enum eNameType mType, int mDescriptor_id )
{
    query_string = "INSERT INTO `home_inventory`.`names` VALUES (0, '";
    query_string += mName;			query_string += "', ";
    char tmp[16];
    sprintf(tmp, "%d, ", (int)mType);
    query_string += tmp;
    sprintf(tmp, "%d", (int)mDescriptor_id);
    query_string += tmp;
    query_string += "); ";
}

////////////////////// END OF QUERY FUNCTIONS //////////////////////
//* Add one name to the Names table.
size_t Name::sql_add( string mName, enum eNameType  mType, int mDescriptor_id  )
{
    add_new_name_nq( mName, mType, mDescriptor_id );
    query		   ( query_string, result, false );
    size_t row_id = mysql_insert_id(inventory_db);
    printf( "New name_id=%ld\n", row_id );
    return row_id;
}

string Name::get_name( int mName_id 	)
{
    find_name_nq( mName_id );
    size_t num = query( query_string, result, true );
    if (num==0)  return "not found!";
    
    MYSQL_ROW row = mysql_fetch_row( result );
    string name = row[1];
    mysql_free_result( result );
    return name;
}

int Name::get_name_id( string mName )
{
    find_name_id_nq( mName );
    size_t num = query(query_string, result, true);
    if (num==0)  return -1;
    
    MYSQL_ROW row = mysql_fetch_row( result );
    int name_id = atoi(row[0]);
    mysql_free_result( result );
    return name_id;
}

/* This scans the sentence for any object we have knowledge of.
	The names database stores all the physical objects we know about.
	They may or may not belong to the robot's estate.  This can be found
	out by the find_in_instances() query.
 */
void Name::get_all_names_in_sentence ( vector<string> mSentence )
{
    m_names.clear();
    m_name_ids.clear();
    find_matching_names_nq( mSentence );
    size_t found = query( query_string, result, true );
    if (found==0)  return ;
    
    string name;
    MYSQL_ROW row = mysql_fetch_row( result );
    while (row) {
        name = row[1];
        m_names.push_back(name);
        row = mysql_fetch_row( result );
    }
    if (result) 	mysql_free_result( result );
    return;
}

void Name::get_all_names_by_descriptor( int mOjectDescription_id )
{
    m_names.clear();
    m_name_ids.clear();
    
    find_name_by_descriptor_id_nq( mOjectDescription_id );
    size_t num = query( query_string, result, true );
    if (num==0)  return ;
    
    string name;
    MYSQL_ROW row = mysql_fetch_row( result );
    while (row) {
        name = row[1];
        m_names.push_back(name);
        m_name_ids.push_back( atoi(row[0]) );
        row = mysql_fetch_row( result );
    }
    mysql_free_result( result );
    return;
}

void Name::get_all_names_by_instance( int mOjectInstance_id )
{
    m_names.clear();
    m_name_ids.clear();
    
    find_name_by_instance_id_nq( mOjectInstance_id );
    size_t num = query( query_string, result, true );
    if (num==0)  return ;
    
    string name;
    int id;
    MYSQL_ROW row = mysql_fetch_row( result );
    while (row) {
        name 	= row[1];
        id 		= atoi(row[0]);
        m_names.push_back(name);
        m_name_ids.push_back(id);
        
        row = mysql_fetch_row( result );
    }
    mysql_free_result( result );
    return;
}

void Name::print_names()		// the entire vector.
{
    printf("Names : ");
    if (m_names.size() == m_name_ids.size())
    {
        for (int i=0; i<m_names.size(); i++)
            printf( "%d-%s ", m_name_ids[i], m_names[i].c_str() );  // m_name_ids[i]
    } else
    {
        for (int i=0; i<m_names.size(); i++)
            printf( "%s ", m_names[i].c_str() );  // m_name_ids[i]
    }
    printf("\n");
}
////////////////////// END OF NAME FUNCTIONS //////////////////////

//===========================================================================
//===========================================================================
Object  object;

ObjectDescriptor::ObjectDescriptor()
{
    _id=-1;
    moveability=MOVEABLE;
    most_common_location_id=-1;
    box_info_id=-1;
    is_a_container=-1;
    most_common_container_id=-1;
}

ObjectDescriptor::~ObjectDescriptor()
{
}

VerbalObject* ObjectDescriptor::create_opengl_object()
{
    VerbalObject* obj = new_object( object_type, &NewObjectParameters );
    return obj;
}

void ObjectDescriptor::find_object_description_nq( int mObjectDescription_id )
{
    query_string = "SELECT * FROM `home_inventory`.`object_descriptions` WHERE _id='";
    char tmp[16];
    sprintf(tmp, "%d", mObjectDescription_id);
    query_string += tmp;
    query_string += "';";
}

void ObjectDescriptor::add_new_nq(  )
{
    query_string  = "INSERT INTO `home_inventory`.`object_descriptions` ";
    query_string += "(_id, Movability, adjectives, most_common_location_id, ";
    query_string += "box_info_id, is_a_container, most_common_container_id, ";
    query_string += "purpose, description, object_type, asset_filename ) ";
    query_string += " VALUES (0, ";
    char value[16];
    switch( moveability )
    {
        case FIXED 		: sprintf(value, "'FIXED'");		break;
        case ROOM          : sprintf(value, "'ROOM'");			break;
        case MOVEABLE      : sprintf(value, "'MOVEABLE'");		break;
        case FURNITURE 	: sprintf(value, "'FURNITURE'");	break;
        default: break;
    }
    query_string += value;
    query_string += ", '"+adjectives+"', ";
    sprintf(value, "%d, ", most_common_location_id );
    query_string += value;
    sprintf(value, "%d, ", box_info_id );
    query_string += value;
    sprintf(value, "%d, ", is_a_container );
    query_string += value;
    sprintf(value, "%d, ", most_common_container_id );
    query_string += value;
    query_string += "'"+purpose+"',";
    query_string += "'"+description+"',";
    sprintf(value, "%d, ", object_type );			// used for new_object switch()
    query_string += value;
    query_string += "'"+asset_filename+"'";
    query_string += "); ";
    printf("%s\n", query_string.c_str() );
}

void ObjectDescriptor::update_nq( )
{
    query_string  = "UPDATE `home_inventory`.`object_descriptions` SET ";
    query_string += "Movability='";
    char value[16];
    switch( moveability )
    {
        case FIXED 		: sprintf(value, "'FIXED'");		break;
        case ROOM 		: sprintf(value, "'ROOM'");			break;
        case MOVEABLE 	: sprintf(value, "'MOVEABLE'");		break;
        case FURNITURE 	: sprintf(value, "'FURNITURE'");	break;
        default: break;
    }
    query_string += value;
    
    query_string += "adjectives='" +adjectives+ "', ";
    sprintf(value, "%d, ", most_common_location_id );
    query_string += "most_common_location_id=";
    query_string += value;
    sprintf(value, "%d, ", box_info_id );
    query_string += "box_info_id=";
    query_string += value;
    sprintf(value, "%d, ", is_a_container );
    query_string += "is_a_container=";
    query_string += value;
    sprintf(value, "%d, ", most_common_container_id );
    query_string += "most_common_container_id=";
    query_string += value;
    query_string += "); ";
    printf("%s\n", query_string.c_str() );
}

void ObjectDescriptor::extract_result( MYSQL_ROW& row )
{
    _id = atoi(row[0]);
    char m = row[1][0];
    switch(m)
    {
        case 'R':   moveability		= ROOM;
        case 'M':   moveability		= MOVEABLE;
        case 'F':
        case 'U':  moveability		= FURNITURE;
        default:   moveability		= FIXED;
    }
    
    if (row[2])  adjectives              = row[2];
    if (row[3])  most_common_location_id = atoi(row[3]);
    if (row[4])  box_info_id			 = atoi(row[4]);
    if (row[5])  is_a_container			 = atoi(row[5]);
    if (row[6])  most_common_container_id= atoi(row[6]);
    if (row[7])  purpose                 = row[7];
    if (row[8])  description             = row[8];
    if (row[9])  object_type             = atoi(row[9]);
    if (row[10]) asset_filename          = row[10];
}

void ObjectDescriptor::sql_add		( )
{
    add_new_nq();
    query( query_string, result, false );
    
    size_t row_id = mysql_insert_id(inventory_db);
    printf( "New name_id=%ld\n", row_id );
    //return row_id;
    
}
void ObjectDescriptor::sql_update	( )
{
    update_nq();
    query(query_string, result, false);
    
}
void ObjectDescriptor::sql_load	( int mObjectDescriptorId )
{
    _id = mObjectDescriptorId;
    find_object_description_nq( mObjectDescriptorId );
    query(query_string, result, true);
    MYSQL_ROW row = mysql_fetch_row( result );
    if (row)
    {
        // Now Transfer to member variables:
        extract_result( row );
    }
    
    mysql_free_result( result );
}

void ObjectDescriptor::print_contents()
{
    printf("id=%d :  move-able=%d;  most common location & container = %d & %d\n",
           _id, moveability, most_common_location_id, most_common_container_id);
    
    printf("adjectives=%s; box_info_id=%d; is_container=%d; object_type=%d\n",
           adjectives.c_str(), box_info_id, is_a_container, object_type );
    
    printf("purse: %s \n description=%s\n  asset_filename=%s\n",
           purpose.c_str(), description.c_str(), asset_filename.c_str() );
    
}
///////////////////////// END OF ObjectDescriptor FUNCTIONS ////////////////////////////////////

/*===========================================================================
 void			add_new_nq  ( int mObjectDescriptor_id, int mDwellingId )
 
 //===========================================================================*/
ObjectInstance::ObjectInstance()
{
    _id = -1;
    object_description_id = -1;
    adjectives  = "";
    dwelling_id =-1;
    m_owner_id  =-1;
    m_scene_id  =-1;
    m_room_id   =-1;
    m_origin.dimension(3);
    m_orientation.dimension(3);
}
ObjectInstance::~ObjectInstance()
{
}

 ////////////////////// START OF INSTANCE QUERIES //////////////////////
 
 /*void ObjectInstance::find_all_objects_in_scene_nq( int mScene_id )
 {
 query_string = "SELECT * FROM home_inventory.object_instances WHERE Scene_id=";
 char tmp[16];
 sprintf(tmp, "%d", mScene_id);
 query_string += tmp;
 }*/

/* Also dwelling is used to restrict results, so fill in before calling this!      */
void ObjectInstance::find_instances_nq	( string mName )
{
    query_string = "SELECT * FROM `home_inventory`.`names` INNER JOIN ";
    query_string += " home_inventory.object_instances ";
    query_string += "ON `home_inventory`.`names`.object_descriptor = home_inventory.object_instances.object_description_id";
    query_string += "WHERE home_inventory.names.name='"+mName;
    query_string += "' AND home_inventory.object_instances.dwelling_id='";
    char tmp[16];
    sprintf(tmp, "%d", dwelling_id);
    query_string += tmp;
    query_string += "';";
    printf("%s\n", query_string.c_str() );
}
/* Also dwelling is used to restrict results, so fill in before calling this!  */
void ObjectInstance::find_instances_by_descriptor_id_nq( int mObjDescriptor_id 	)
{
    query_string = "SELECT * FROM `home_inventory`.`object_instances` WHERE object_description_id='";
    char tmp[16];
    sprintf(tmp, "%d", mObjDescriptor_id);
    query_string += tmp;
    query_string += "' AND dwelling_id='";
    sprintf(tmp, "%d", dwelling_id);
    query_string += tmp;
    query_string += "' );";
    printf("%s\n", query_string.c_str() );
}
/* Also dwelling is used to restrict results, so fill in before calling this!      */
void ObjectInstance::find_instances_by_id_nq( int mObjectInstance_id	)
{
    query_string = "SELECT * FROM `home_inventory`.`object_instances` WHERE _id='";
    char tmp[16];
    sprintf(tmp, "%d", mObjectInstance_id);
    query_string += tmp;
    query_string += "' AND dwelling_id='";
    sprintf(tmp, "%d", dwelling_id);
    query_string += tmp;
    query_string += "' ;";
    printf("%s\n", query_string.c_str() );
}
/* Also dwelling is used to restrict results, so fill in before calling this!  */
void ObjectInstance::update_nq( int mObjectInstance_id )
{
    query_string = "UPDATE `home_inventory`.`object_instances` SET object_description_id=";
    char tmp[16];
    sprintf(tmp, "%d", mObjectInstance_id);
    query_string += tmp;
    query_string += ", adjective="+adjectives;
    query_string += ", dwelling_id=";
    sprintf(tmp, "%d", dwelling_id);
    query_string += tmp;
    query_string += ", origin_x=";
    sprintf(tmp, "%6.2f", m_origin[0]);
    query_string += tmp;
    query_string += ", origin_y=";
    sprintf(tmp, "%6.2f", m_origin[1]);
    query_string += tmp;
    query_string += ", origin_z=";
    sprintf(tmp, "%6.2f", m_origin[2]);
    query_string += tmp;
    query_string += m_last_moved_timestamp;
    sprintf(tmp, "%d, ", m_room_id);
    query_string += tmp;
    sprintf(tmp, "%d, ", m_scene_id);
    query_string += tmp;
    sprintf(tmp, "%d ", m_owner_id);
    query_string += tmp;
    query_string += " );";
    printf("%s\n", query_string.c_str() );
}

/* Searches each word in the sentence to see if we have an object instance and name
 for it. This means it is one of our belongings.
 */
void ObjectInstance::find_instances_in_sentence ( vector<string> mSentence )
{
    // Once we have the results, need to get the indexes within the sentence too
    size_t  size    = mSentence.size();
    query_string = "SELECT * FROM home_inventory.names INNER JOIN home_inventory.object_instances ON ";
    query_string += "home_inventory.names.object_descriptor = home_inventory.object_instances.object_description_id WHERE ";
    
    for (int w=0; w<size; w++)
    {
        query_string += "name='";
        query_string += mSentence[w].c_str();
        query_string += "'";
        if (w<(size-1))
        {
            query_string += " OR name='";
            query_string += mSentence[w].c_str();
            query_string += " ";
            query_string += mSentence[w+1].c_str();
            query_string += "'";
            query_string += " OR ";
        }
    }
    query_string += ";";
    //printf("ObjectSearch: %s\n", query_string.c_str() );
}

void ObjectInstance::add_new_nq		(  )
{
    query_string  = "INSERT INTO `home_inventory`.`object_instances` ";
    query_string += "(_id, object_description_id, adjective, dwelling_id, origin_x, origin_y, origin_z, last_move, room_id, scene_id, owner_id )";
    query_string += "VALUES (0, ";
    char value[16];
    sprintf(value, "%d", object_description_id );
    query_string += value;
    query_string += ", '' , ";
    sprintf(value, "%d,", dwelling_id );
    query_string += value;
    sprintf(value, "%6.2f,", m_origin[0] );
    query_string += value;
    sprintf(value, "%6.2f,", m_origin[1] );
    query_string += value;
    sprintf(value, "%6.2f, '", m_origin[2] );
    query_string += value;
    query_string += m_last_moved_timestamp;
    sprintf(value, "', %d, ",  m_room_id );
    query_string += value;
    sprintf(value, "%d,", m_scene_id );
    query_string += value;
    sprintf(value, "%d",  m_owner_id );
    query_string += value;
    
    query_string += "); ";
}
////////////////////// END OF QUERY FUNCTIONS //////////////////////

long int ObjectInstance::sql_add( )
{
    add_new_nq();
    query(query_string, result, false);
    size_t row_id = mysql_insert_id( inventory_db );
    printf("row_id=%ld\n", row_id );
    return row_id;
}

void ObjectInstance::sql_update	( )
{
    query(query_string, result, true);
}

void ObjectInstance::extract_result	(MYSQL_ROW& row )
{
    if (row)
    {
        for (int r=0; r<10; r++)
            printf("%s  ", row[r]);
        
        if (row[0])     _id						= atoi(row[0]);
        if (row[1])     object_description_id	= atoi(row[1]);
        if (row[2])     adjectives              = row[2];
        if (row[3])     dwelling_id				= atoi(row[3]);
        
        double xx,yy,zz;
        if (row[4]) xx = strtod(row[4],NULL);
        if (row[5]) yy = strtod(row[5],NULL);
        if (row[6]) zz = strtod(row[6],NULL);
        m_origin[0]	= xx;
        m_origin[1]	= yy;
        m_origin[2]	= zz;
        
        if (row[7])   m_last_moved_timestamp    = row[7];
        if (row[8])   m_room_id                 = atoi(row[8]);
        if (row[9])   m_scene_id				= atoi(row[9]);
        if (row[10])  m_owner_id				= atoi(row[10]);
        
        if (row[11])  m_box_info_id             = atoi( row[11] );
    }
}

/* char* abc = (char*)&(m_origin[0]);
 char* xx_c = (char*)&(xx);
 printf("\nabc:%x %x %x %x\t", *abc, *(abc+1), *(abc+2), *(abc+3) );
 printf("%x %x %x %x\n", *(abc+4), *(abc+5), *(abc+6), *(abc+7) );
 
 printf("xx: %x %x %x %x\t", *xx_c, *(xx_c+1), *(xx_c+2), *(xx_c+3) );
 printf("%x %x %x %x\n", *(xx_c+4), *(xx_c+5), *(xx_c+6), *(xx_c+7) );
 */
void ObjectInstance::sql_load_by_id					( int mObjectInstance_id )
{
    find_instances_by_id_nq( mObjectInstance_id );
    query(query_string, result, true);
    MYSQL_ROW row = mysql_fetch_row( result );
    if (row)
        extract_result(row);
    mysql_free_result(result);
}
void ObjectInstance::sql_load_by_descriptor_id		( int mObjDescriptor_id )
{
    find_instances_by_descriptor_id_nq( mObjDescriptor_id );
    query(query_string, result, true);
    MYSQL_ROW row = mysql_fetch_row( result );
    if (row)
        extract_result(row);
    mysql_free_result(result);
}
void ObjectInstance::sql_load_by_sentence			( vector<string> mSentence )	// could be multiple results, so maybe this is better outside of the class.
{
    find_instances_in_sentence( mSentence );
    query(query_string, result, true);
    //	if (found)
    //		retrieve_sentence_indices( mSentence, m_belongings_found );
    
    MYSQL_ROW row = mysql_fetch_row( result );
    if (row)
        extract_result(row);
    mysql_free_result(result);
}


void ObjectInstance::print_content( )
{
    printf("_id=%d - objDescriptor_id=%d (adjectives:%s);  dwelling=%d; origin=<%6.1f,%6.1f,%6.1f> \n", _id,
           object_description_id, adjectives.c_str(), dwelling_id,
           m_origin[0], m_origin[1], m_origin[2]	);
}

/*===========================================================================
 Form Query Functions:
 ==========================================================================*/

/*===========================================================================
 Query & Repeat Functions:
 ==========================================================================*/
 
 /* for sorting
 static bool compare_sentence_order( struct subject_result p1, struct subject_result p2)
 {
	return p1.word_index < p2.word_index;
 }
 void scan_sentence(vector<string> mSentence )
 {
 
 }
 *
	Results will be in m_sentence_indices
 *
 int	retrieve_sentence_indices( vector<string> mSentence,
 vector<struct subject_result> &m_found )
 {
	MYSQL_RES*		result=NULL;
 // Once we have the results, need to get the indexes within the sentence too
 //int num_fields = mysql_num_fields(result);
 int words      = mSentence.size();
 m_found.clear();
 //printf("Num_fields=%d \n", num_fields );
 
 string s1;
 string s2;
 string cs;
 MYSQL_ROW row;
 int    col = 1;
 struct subject_result pr;
 // For each result,
 //			add it to the m_objects_found
 //			and find where in the sentence and store.
 while ( (row = mysql_fetch_row(result)) )
 {
 pr.text 		= row[col];
 pr.word_index = -1;
 pr.db_id      = -1;
 cs= row[col];
 
 // Now Scan the sentence for that word:
 for (int w=0; w<words; w++)
 {
 s1 = mSentence[w];
 if (w<(words-1))
 s2 = s1 +" "+mSentence[w+1];
 else s2 = "";
 
 if (s1.compare(row[col])==0)
 {
 //printf("%d-%s \n", w, mSentence[w].c_str() );
 pr.word_index = w;
 pr.db_id = atoi( row[3] );
 } else if (s2.compare(cs)==0)
 {
 //printf("%d-%s(%s) \n", w, mSentence[w].c_str(), cs.c_str() );
 pr.word_index = w;
 pr.db_id = atoi( row[3] );
 }
 }
 m_found.push_back( pr );
 }
 
 std::sort(m_found.begin(), m_found.end(), compare_sentence_order );
 return m_found.size();
 }
 */

VerbalObject* Object::create_opengl(  )
{
    VerbalObject* obj = m_descriptor.create_opengl_object();
    if (obj)
    {
        obj->relocate( m_instance.m_origin );
        obj->m_R_position = m_instance.m_orientation;
    }
    return obj;
}

/*  Create a new object (adding name if necessary)
 Add to SQL database.
 */
size_t Object::create_object( string mName )
{
    if (m_name.m_names.size()==0)
        m_name.m_names.push_back( mName );
    else
        m_name.m_names[0] = mName;
    
    // Does the name exist yet ?
    int name_id = m_name.get_name_id( m_name.m_names[0] );
    
    // CREATE NAME :
    if (name_id==-1)
    {
        m_name.sql_add( m_name.m_names[0], NAME_PLACE_HOLDER, m_descriptor._id );
        // get the resulting _id
        //row_id = mysql_insert_id(inventory_db);
    }
    
    // CREATE OBJECT DESCRIPTION : (just create it, no checks for already existing)
    m_descriptor.sql_add( );
    
    // CREATE INSTANCE :
    //* the member data fields must already be filled in prior to this function!!
    size_t object_id = m_instance.sql_add(  );
    return object_id;
}

/* Create x new objects (adding name if necessary)
 
 int	Object::create_objects( string mName, int mQuantity )
 {
	for (int i=0; i<mQuantity; i++)
 create_object( mName );
	return 1;
 }*/
Object::Object()
{
}
Object::~Object()
{
}

int	Object::find_in_sentence		( vector<string> mSentence )
{  return 0; }
int	Object::find_in_belongings		( vector<string> mSentence )
{  return 0; }

void Object::reset                  ( )
{
}
void Object::print_objects			( )
{
}
void Object::print_belongings		( )
{
}
int	Object::get_object_id( int mDwelling, string mName )
{
    return m_name.get_name_id(mName);
}

int	Object::get_name_id	( string mName )
{
    return m_name.get_name_id( mName );
}

void Object::extract_result( MYSQL_ROW& row )
{
    MathVector mv(3);
    mv[0] = 123.0;
    mv[1] = 456.0;
    mv[2] = 789.0;
    
    if (row)
    {
        // EXTRACT OBJECT INSTANCE INFO:
        for (int r=0; r<10; r++)
            printf("%s  ", row[r]);
        
        if (row[0])     m_instance._id						= atoi(row[0]);
        if (row[1])     m_instance.object_description_id	= atoi(row[1]);
        if (row[2])     m_instance.adjectives               = row[2];
        if (row[3])     m_instance.dwelling_id				= atoi(row[3]);
        
        double xx,yy,zz;
        if (row[4]) xx = strtod(row[4],NULL);
        if (row[5]) yy = strtod(row[5],NULL);
        if (row[6]) zz = strtod(row[6],NULL);
        m_instance.m_origin[0]	= xx;
        m_instance.m_origin[1]	= 23;
        m_instance.m_origin[2]	= zz;
        
        if (row[7])   m_instance.m_last_moved_timestamp = row[7];
        if (row[8])   m_instance.m_room_id              = atoi(row[8]);
        if (row[9])   m_instance.m_scene_id				= atoi(row[9]);
        if (row[10])  m_instance.m_owner_id				= atoi(row[10]);
        if (row[11])  m_instance.m_box_info_id          = atoi( row[11] );
        
        
        // EXTRACT OBJECT DESCRIPTION INFO:
        m_descriptor._id = atoi(row[12]);
        char m = row[13][0];
        switch(m)
        {
            case 'R':   m_descriptor.moveability		= ROOM;
            case 'M':   m_descriptor.moveability		= MOVEABLE;
            case 'F':
            case 'U':  m_descriptor.moveability		= FURNITURE;
            default:   m_descriptor.moveability		= FIXED;
        }
        if (row[14])  m_descriptor.adjectives              = row[14];
        if (row[15])  m_descriptor.most_common_location_id = atoi(row[15]);
        if (row[16])  m_descriptor.box_info_id			  = atoi(row[16]);
        if (row[17])  m_descriptor.is_a_container		  = atoi(row[17]);
        if (row[18])  m_descriptor.most_common_container_id= atoi(row[18]);
        if (row[19])  m_descriptor.purpose                 = row[19];
        if (row[20])  m_descriptor.description             = row[20];
        if (row[21])  m_descriptor.object_type             = atoi(row[21]);
        if (row[22])  m_descriptor.asset_filename          = row[22];
        
        // NOW EXTRACT BOX_INFO :
        MathVector size(3);
        int box_id;
        if (row[23])  box_id                    = atoi(row[23]);
        if (row[24])  size.m_elements[0]        = strtod(row[24], NULL);
        if (row[25])  size.m_elements[1]        = strtod(row[25], NULL);
        if (row[26])  size.m_elements[2]        = strtod(row[26], NULL);
        
        double x,y,z;
        if (row[27])  { x = strtod(row[27],NULL);  m_instance.m_origin[0]=x;  }
        if (row[28])  { y = strtod(row[28],NULL);  m_instance.m_origin[1]=y;  }
        if (row[29])  { z = strtod(row[29],NULL);  m_instance.m_origin[2]=z;  }
        
        if (row[30])  m_instance.m_orientation[0]        = atof(row[30]);
        if (row[31])  m_instance.m_orientation[1]        = atof(row[31]);
        if (row[32])  m_instance.m_orientation[2]        = atof(row[32]);
    }
}


/*******************************************************************
 ********************************************************************
 
 int	ObjectInstance::sql_load_object_instances( int mDwellingId, int mOjectDescription_id, vector<int>  &Instance_ids  )
 {
	//find_name_by_descriptor_id_nq( mObjectDescription_id );
	int num = query( query_string, true );
	if (num==0)  return -1;
	
	int instance_id;
	MYSQL_ROW row = mysql_fetch_row( result );
	extract_result( row );			// details of just the first one.
	while (row) {
 instance_id = atoi(row[0]);
 Instance_ids.push_back(instance_id);
 row = mysql_fetch_row( result );
	}
	mysql_free_result( result );
	return 1;
 }*/
/*void ObjectDescriptor::reset(	)
 {
	//m_unidentified_objects.clear();
	m_belongings_found.clear();
	m_objects_found.clear();
	query_string = "";
 }*/
/*void ObjectInstance::sql_load_scene ( int mScene_id )
 {
 find_all_objects_in_scene_nq( mScene_id );
 query(query_string, result, true);
 // Hold the results which will be used in sql_load.cpp!
 
 //mysql_free_result(result);
 }
 void ObjectInstance::sql_load_by_name	( string mName 			 )
 {
	find_instances_nq( mName );
	query(query_string, result, true);
	MYSQL_ROW row = mysql_fetch_row( result );
	if (row)
 extract_result(row);
	mysql_free_result(result);
 }*/
