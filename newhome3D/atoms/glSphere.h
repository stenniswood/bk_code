//
//  glSphere.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/2/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__glSphere__
#define __home3D__glSphere__

#include <stdio.h>
#include <vector>
#include "gl_atom.hpp"

const float FIFA_SOCCER_BALL_RADIUS_SIZE5 = 8.75 / 2.;      // dia = 8.75
const float FIFA_SOCCER_BALL_RADIUS_SIZE4 = 4.14;
const float FIFA_SOCCER_BALL_RADIUS_SIZE3 = 3.82;

const float NBA_BASKET_BALL_RADIUS = 8.75 / 2.;
const float BASEBALL_RADIUS        = 1.452;

const float COEFF_RESTITUTION_BASKETBALL = 0.853;
const float COEFF_RESTITUTION_SOCCERBALL = 0.85;
const float COEFF_RESTITUTION_TENNISBALL = 0.75;
const float COEFF_RESTITUTION_BASEBALL   = 0.50;


class glSphere : public glAtom
{
public:
    glSphere    ( float radius, unsigned int rings, unsigned int sectors);
    ~glSphere   ( );

    /* Texture note:  The textures for a sphere have a warping
     see: http://www.flashandmath.com/advanced/sphere/exp1.jpg     */
    void            load_texture         ( string mImageFilename );

    void            set_inertia( float mOneAxis );  // applies same to all 3 axes.
    virtual void    generate_vertices   ( );
    virtual void 	generate_indices    ( );
    void            draw_primitive      ( );
    virtual void            gl_register(  );
    
    virtual bool    evaluate_collision( glMovingObject* mOther );
    
    std::vector<GLfloat> m_normals;
    float 	m_radius;
    float   m_rings;
    float   m_sectors;
};


class glBasketball : public glSphere
{
public:
    glBasketball  ();
    ~glBasketball ();
    
};

class glSoccerball4 : public glSphere
{
public:
    glSoccerball4  ();
    ~glSoccerball4 ();
    
};
class glSoccerball5 : public glSphere
{
public:
    glSoccerball5  ();
    ~glSoccerball5 ();
    
};

class glBaseball : public glSphere
{
public:
    glBaseball  ();
    ~glBaseball ();
    
};


#endif /* defined(__home3D__glSphere__) */
