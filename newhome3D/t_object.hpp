//
//  test.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/12/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef test_hpp
#define test_hpp

#include <stdio.h>

//void test_mv();
void testasdf2();

#include <my_global.h>
#include <mysql.h>

/*********************************************************************
	Class Object  connects words in the sentence with actual physical
	objects in the database.  Note uses the 'home_inventory' database
	
	Table OverView:
	
	Names				- list of all names we know about.  This is separate so that synonyms can also be entered.
	Name Associations	- Connects names to objects (so that synonyms can be assigned)
	Object Descriptions - describe a known object (we already know about bookcase, barricade etc).
	Object Instances    - this table is a list of something we physically have.
 
 These are SQL wrapper functions for
 ********************************************************************/
extern MYSQL *inventory_db ;
void    connect_inventory_db();
size_t	query( std::string mquery_string, MYSQL_RES*& result, bool mRetrieving );


struct db_object_results
{
    std::string	name;
    int		name_id;
    int		object_id;
};

enum eMoveability {
    ROOM,
    FIXED,
    MOVEABLE,
    FURNITURE
};

enum eNameType {
    NAME_PLACE_HOLDER,
    NAME_PHYSICAL_OBJECT,
    NAME_ROOM,
    NAME_PERSON
};

class Name
{
public:
    Name();
    ~Name();
    
    // Does robot have a knowledge of the object (ie. exist in names table)
    // May not own the object (indicated by the objects table)
    void			find_matching_names_nq	( std::vector<std::string> mSentence );
    void			find_name_id_nq			( std::string mName 	);
    void			find_name_nq			( int mName_id 	);
    void 			find_name_by_descriptor_id_nq( int mObjDescriptor_id );
    void 			find_name_by_instance_id_nq( int mObjInstance_id );
    void			add_new_name_nq			( std::string mName, enum eNameType mType, int mDescriptor_id );
    
    size_t          sql_add						( std::string mName, enum eNameType mType, int mDescriptor_id );
    //int			get_rooms                   ( );
    
    std::string		get_name					( int mName_id 	);
    int				get_name_id					( std::string mName  );
    void			get_all_names_in_sentence	( std::vector<std::string> mSentence );
    void			get_all_names_by_descriptor	( int mOjectDescription_id );
    void			get_all_names_by_instance	( int mOjectInstance_id    );
    
    void			sql_add						();
    void 			print_names					();		// the entire vector.
    
    std::string		query_string;
    MYSQL_RES*		result;
    
    std::vector<std::string>  m_names;			// all names which match that object.
    std::vector<int>		m_name_ids;
};

class ObjectDescriptor
{
public:
    ObjectDescriptor ();
    ~ObjectDescriptor();
    
    void			find_object_description_nq( int mObjectDescription_id );
    void 			add_new_nq( );
    void			update_nq();
    
    VerbalObject*   create_opengl_object();
    
    void			extract_result	( MYSQL_ROW& row );
    
    void			sql_add		( );
    void			sql_update	( );
    void			sql_load	( int mObjectDescriptorId );
    void			print_contents();
    
    // DATA:
    std::string		query_string;
    MYSQL_RES*		result;
    
    int					_id;
    enum eMoveability 	moveability;
    std::string			adjectives;
    int					most_common_location_id;
    int					box_info_id;
    int					is_a_container;
    int					most_common_container_id;
    std::string 		purpose;
    std::string 		description;
    int					object_type;
    std::string 		asset_filename;
};

class ObjectInstance
{
public:
    ObjectInstance ();
    ~ObjectInstance();
    
    // SQL MANIPULATION TABLE : 	(all instances which match - maybe 3 remote controls for example)
    void			find_instances_nq				  ( std::string mName 			);
    void			find_instances_by_id_nq			  ( int mObjectInstance_id	);
    void			find_instances_by_descriptor_id_nq( int mObjDescriptor_id 	);
    void			find_instances_in_sentence 		  ( std::vector<std::string> mSentence );
    void			add_new_nq  					  ( );
    void			update_nq						  ( int mObjectInstance_id );
    
    void			extract_result				( MYSQL_ROW& row );
    long int		sql_add						( );		// returns row_id.
    void			sql_update					( );
    void			sql_load_by_id				( int mObjectInstance_id );
    void			sql_load_by_descriptor_id	( int mObjDescriptor_id  );
    void			sql_load_by_sentence		( std::vector<std::string> mSentence );	// could be multiple results, so maybe this is better outside of the class.
    
    void			print_content				();
    // VARS:
    std::string		query_string;
    MYSQL_RES*		result;
    
    int				_id;
    int				object_description_id;
    std::string		adjectives;
    int 			dwelling_id;
    MathVector 		m_origin;
    MathVector 		m_orientation;
    int				m_box_info_id;
    
    double          m_origin_x;
    double          m_origin_y;
    double          m_origin_z;
    
    std::string     m_last_moved_timestamp;
    int				m_room_id;
    int             m_scene_id;
    int				m_owner_id;
};

/* An object in a 3D floor plan.  Handles the Name, Object_descriptor, and Object_instances SQL tables.
	Can Load the object by a name.  We have to deal with possibility of multiple results!
 For example "office chair" this can even be "which one?  the grey one downstairs, or the beige one?"
	
	Strategy for multiple results is this:
 all row_ids will be stored (or the result will not be freed), and the first occurence will
 be loaded into the member variables!
 */
class Object
{
public:
    Object ( );
    Object ( float x, float y, float z=0 );
    ~Object( );
    
    int				identify_objects_in_room( int mDwelling_id, int mRoomId );		// output is a vector of instance_ids.
    int				identify_objects		( std::vector<std::string> mSentence );	// output is a vector of instance_ids.
    int				move_to					( MathVector mNewLocation );			// will add name if necessary, descriptor if nec., and instance for sure.
    
    void            extract_result          ( MYSQL_ROW& row );
    
    int				get_object_type			( );	// for drawing purposes. (use one in object_description.object_type)
    int				sql_add					( );	// will add name if necessary, descriptor if nec., and instance for sure.
    int				sql_update				( );	// will add name if necessary, descriptor if nec., and instance for sure.
    VerbalObject*   create_opengl			( );
    
    // These are here for compatibility reasons (to not break the rest of the rooms app!)
    size_t          create_object			( std::string mName );		// adds to sql name, descriptor, & instance.
    int				get_object_id			( int mDwelling, std::string mName 		);
    void			print_objects			( );
    void			print_belongings		( );
    int				get_name_id				( std::string mName );
    void			reset					( );
    int				find_in_sentence		( std::vector<std::string> mSentence );
    int				find_in_belongings		( std::vector<std::string> mSentence );
    
    // The currently loaded object's data classes:
    Name			 m_name;
    ObjectDescriptor m_descriptor;
    ObjectInstance   m_instance;
    std::vector<int> m_found_instances;
    
#ifdef COMPATIBILITY
    // for compatibility with previous class :
    std::vector<struct subject_result>  	m_objects_found;			// IE baseball, plate, keys, candle, etc
    std::vector<struct db_object_results>	m_db_objects;
#endif
};


extern Object object;



#endif /* test_hpp */
