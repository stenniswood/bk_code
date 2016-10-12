#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_object.hpp"
#include "gl_atom.hpp"

#include "bookcase.h"
#include "chair.h"
#include "table.hpp"
#include "table_pole.h"
#include "route.hpp"
#include "route_feet.hpp"
#include "multi_path_route.h"

#include "robot_head.h"
#include "robot_gripper.h"
#include "robot_foot.hpp"
#include "robot_leg_piece.hpp"
#include "robot_arm.hpp"
#include "robot.hpp"

#include "brickwall.hpp"
#include "wall.hpp"
#include "glComputableWall.h"
#include "full_wall.h"

#include "palette.hpp"
#include "drawer.hpp"
#include "door.hpp"
#include "cabinet.hpp"
#include "counter.hpp"
#include "abstract_wall.hpp"
#include "Map2D.hpp"
//#include "room2.hpp"
#include "ibeam.hpp"
#include "stairway.hpp"
#include "picnic_table.h"
#include "construction_barricade.h"

#include "physical_world.h"
#include "moving_object.h"
#include "hinge.h"
#include "dwelling_level.h"
#include "dwelling.h"
#include "apartment.h"
#include "walking_robot.h"
#include "terrain.h"
#include "stats.h"
#include "frame.h"
#include "gl_window.h"
#include "verbal_object.h"


#include "truss.h"
#include "rafter.h"
#include "hinge.h"
#include "road.h"
#include "sign.h"
#include "basketball_arena.h"


// FOLLOWING ARE DEFINED IN main.cpp :
extern list<glAtom*>    m_scene;
void                    change_route( MathVector mSource, MathVector mDestination );
extern glWalkingRobot   robot;           // Used in proc_client_commands()

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define dprintf if (Debug) printf("%s ", __FILENAME__); if (Debug) printf



