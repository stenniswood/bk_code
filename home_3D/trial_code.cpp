


/*

Trial Code:

The following snips either worked, or were good thoughts.  Removed from main.cpp to remove clutter!

*/


void init_construction_objects()
{
/*
txtContainer    mebox;
mebox.width  = CEMENT_BLOCK_LENGTH;
mebox.height = CEMENT_BLOCK_HEIGHT;
mebox.depth  = CEMENT_BLOCK_THICKNESS;
mebox.setup();
mebox.load_image("textures/cinder-block-texture.jpg");
mebox.gl_register( );
mebox.relocate   ( 50., 10., 200. ); */

/*  barricade.create  ( );
barricade.relocate( -10., 0., +60. );

palette.create();
palette.relocate   ( -100., 0., -100.);

brick_wall.create  ( 15.*12, 8        );
brick_wall.Relocate( -150. , 0., -150.); */

/*
truss.m_length = 100*12;
truss.m_separation_distance = 3*12;
truss.m_perpendicular_spacing = 3*12;
truss.create();
truss.relocate(-50,0,50);

truss1.m_length = 15*12;
truss1.create();
truss1.relocate( 50, 0, -200); */

}


// A crippled sideways step:
void walk_down_stairs()
{
/*float heel[3];

//
glm::vec4 vh = robot.m_left_leg.get_heel_in_robot_coords( );
heel[0] = vh[0];
heel[1] = vh[1];
heel[2] = vh[2];
robot.m_left_leg.inverse_kinematic_ankle ( heel );

//
vh = robot.m_right_leg.get_heel_in_robot_coords( );
heel[0] = vh[0];
heel[1] = vh[1];
heel[2] = vh[2];
robot.m_right_leg.inverse_kinematic_ankle ( heel );
*/

}

// A crippled sideways step:
void walk_up_stairs()
{

}

/*void create_nose_line()
{
/*    static bool first_time = true;
if (first_time)
{
//nose.gl_register();
first_time=false;
};

glDoorWay* door = apartment.crosses_doorway( robot.m_nose_line.m_Start, robot.m_nose_line.m_End );
if (door) {
int type = door->m_door_type;
if ((door) && (type==DOOR_TYPE_NORMAL_HINGED))
((glDoor*)door)->open(1.0);
}
}*/

