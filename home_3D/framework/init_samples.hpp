//
//  init_samples.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 2/26/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef init_samples_hpp
#define init_samples_hpp

#include <stdio.h>

#include "imageloader.h"
#include "all_objects.h"
#include "sign.h"
#include "quarter_629.h"
#include "hampton_complex.h"
#include "basket_ball_court.h"
#include "sports_bench.h"
#include "basketball_arena.h"
#include "atom_objects.h"
#include "truss.h"
#include "rafter.h"
#include "road.h"


extern union uGlobalVars theGlobals;

extern StreetSign          street_signs[10];
extern glRoad              road;
extern glMultiRoute        multiRoute;
extern glMultiRoute        multiRoute2;
extern glMap2D             map2D;
extern glCounter           KitchenSinkCounter;
extern glApartment         apartment;
extern glArena             bball_arena;

extern vector<MathVector>   Destinations;
extern vector<MathVector>   Sources;
extern int DestinationIndex;
extern int SourceIndex;

//extern vector<Room>        Rooms;
//void init_construction_objects();

void select_route();
void init_street_scene();
void init_objects();




#endif /* init_samples_hpp */
