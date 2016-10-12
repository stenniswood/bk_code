//
//  dummy.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 3/10/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <vector>
#include <string>

#include <my_global.h>
#include <mysql.h>

//#include "physical_world.h"
//#include "all_objects.h"

#include "dummy.hpp"
#include "vector.hpp"

using namespace std;

//MYSQL *inventory_db ;

void test_function()
{
    
    MathVector fred(3);
    fred.set_xyz( 123., 456., 789. );
    fred.print();
}

/*size_t	query( string mquery_string, MYSQL_RES*& result, bool mRetrieving )
{
    return 1;
}*/

