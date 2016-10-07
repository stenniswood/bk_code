#include "vector.hpp"
#include "all_objects.h"
#include "cursor_nav.hpp"

//MathVector eye;             // not implemented yet.
//MathVector center;          // not implemented yet.
GLdouble   eyeX,eyeY,eyeZ;
GLdouble   centerX,centerY,centerZ;
GLdouble   upX, upY, upZ;
GLdouble   centerRadius = 100.;
GLdouble   centerAngle  = 0.0 ;
GLdouble   angle_increment = 5.*3.1415/180.;


void rotate( )
{
	if (centerAngle > 2*M_PI)	centerAngle = 0.	;
	if (centerAngle < 0.    )	centerAngle = 2*M_PI;

	centerX = centerRadius * sin(centerAngle) + eyeX;
	centerZ = centerRadius * cos(centerAngle) + eyeZ;
	//printf("centerAngle=%6.3f cx=%6.2f; cy=%6.2f \n", centerAngle, centerX, centerY );
}

void move_forward( float mAmount )
{
	float F_vec_x = eyeX - centerX;
	float F_vec_y = eyeY - centerY;
	float F_vec_z = eyeZ - centerZ;
	float sum = fabs(F_vec_x) + fabs(F_vec_y) + fabs(F_vec_z);
	
	F_vec_x *= 2*mAmount/sum;
	F_vec_z *= 2*mAmount/sum;
	//printf("Delta x=%6.3f \t", F_vec_x );
	//printf("Delta z=%6.3f \n", F_vec_z );

	eyeX 	+= F_vec_x;
	eyeZ 	+= F_vec_z;
	centerX += F_vec_x;
	centerZ += F_vec_z;
}

void move_sideways( float mAmount )
{
    MathVector forward(3);
    forward[0] = centerX - eyeX;
    forward[1] = centerY - eyeY;
    forward[2] = centerZ - eyeZ;
    MathVector perp = forward.get_perp_xz();
    perp.unitize();
    perp *= mAmount;

    eyeX    += perp[0];
    eyeZ    += perp[2];
    centerX += perp[0];
    centerZ += perp[2];
}

void eye_follow_route(glRoute& mRoute)
{
    printf("eye_follow_route()\n"); 
    static int v=0;
    struct Vertex tmp  = mRoute.m_vertices[v];
    eyeX = tmp.position[0];
    eyeY = tmp.position[1];
    eyeZ = tmp.position[2];

//  glm::vec4 center_world = robot.get_eye_target();
    tmp = mRoute.m_vertices[v+5];
    centerX = tmp.position[0];
    centerY = tmp.position[1];
    centerZ = tmp.position[2];
    v++;
    if (v > mRoute.m_vertices.size())
        v = 0;
}

void assume_robot_view()
{
    MathVector eye  = robot.get_eye_location_world_coords();
    eyeX = eye[0];
    eyeY = eye[1];
    eyeZ = eye[2];
    
    printf("assume_robot_view()\n"); 
    glm::vec4 center_world = robot.get_eye_target_world_coords();
    centerX = center_world[0];
    centerY = center_world[1];
    centerZ = center_world[2];
}
