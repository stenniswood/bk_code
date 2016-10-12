#include <vector>
#include <string>
#include <cstring>
#include <my_global.h>
#include <mysql.h>

//#include "Object3d.hpp"
//#include "Container_3D.hpp"
#include "verbal_xyz.hpp"


using namespace std;
extern MYSQL *inventory_db;


Verbal_XYZ::Verbal_XYZ ( ) 
{ 
}

Verbal_XYZ::~Verbal_XYZ( ) 
{ 
}

void Verbal_XYZ::set( float x, float y, float z )
{
	m_x = x;
	m_y = y;
	m_z = z;
}
void Verbal_XYZ::set( MathVector xyz )
{
	m_x = xyz[0];
	m_y = xyz[1];
	m_z = xyz[2];
}


// THREED DATABASE TABLE 
void Verbal_XYZ::add_point_nq( )
{
	char values[255];
	query_string = "INSERT INTO `home_inventory`.`threed` VALUES ";
	// The _id will autoincrement but needs a '0' place holder.
	sprintf(values, "(0, %6.2f, %6.2f, %6.2f );", m_x, m_y, m_z );
	query_string += values;
}

void Verbal_XYZ::retrieve_point_nq( int m_id)
{
	char tmp[16];
	sprintf( tmp, "%d", m_id );
	query_string = "SELECT * FROM `home_inventory`.`threed` WHERE _id = ";
	query_string += tmp;
}

void Verbal_XYZ::update_point_nq( int m_id )
{
	char values[255];
	sprintf(values, "X=%6.2f, Y=%6.2f, Z=%6.2f", m_x, m_y, m_z );	
	query_string  = "UPDATE `home_inventory`.`threed` SET ";
	query_string += values;	
	query_string += " WHERE _id = ";	
	sprintf( values, "%d", m_id );
	query_string += values;
}

size_t Verbal_XYZ::query( bool mRetrieving )
{	
  if (mysql_query(inventory_db, query_string.c_str() ))
  {
      fprintf(stderr, "Object: %s\n", mysql_error(inventory_db));
      //mysql_close(vocab_db);
  }

  long row_count=0;
  // Question : what happens when no results?
  //   Answer : if  mysql_store_result returns a non-zero value
  if (mRetrieving) {
	  result = mysql_store_result(inventory_db);
	  if (result == NULL)  {
		  printf("query Error result==null!\n");
		  return 0; 
	  }
	  row_count = mysql_num_rows(result);
	  printf("row_count=%ld\n", row_count);
  }
  return row_count;
}

void Verbal_XYZ::sql_add()
{
	add_point_nq( );
	query(false);
}

void Verbal_XYZ::sql_load(int m_id)
{
	retrieve_point_nq( m_id );
	query();
	
	MYSQL_ROW row = mysql_fetch_row( result );
	m_x = atof(row[1]);
	m_y = atof(row[2]);
	m_z = atof(row[3]);
	
	print_contents();
}

void Verbal_XYZ::sql_update()
{
	
}

void Verbal_XYZ::print_info		( 			) 
{
	 printf("row_id = %d\n", m_threed_id);
	 print_contents();
}

void Verbal_XYZ::print_contents	( 			) 
{
 	printf("XYZ: <%6.3f, %6.3f, %6.3f> \n", m_x, m_y, m_z );
}	



