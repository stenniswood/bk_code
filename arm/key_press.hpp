//
//  key_press.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 10/4/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef key_press_hpp
#define key_press_hpp

#include <stdio.h>


extern GLdouble centerX;
extern GLdouble centerY;
extern GLdouble centerZ;

extern GLdouble eyeX;
extern GLdouble eyeY;
extern GLdouble eyeZ;


void handleKeypress(unsigned char key, int x, int y);


#endif /* key_press_hpp */
