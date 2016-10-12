//
//  asset_importer.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 2/26/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef asset_importer_hpp
#define asset_importer_hpp

#include <stdio.h>


//void color4_to_float4           ( const aiColor4D *c, float f[4] );
void createGLResourcesInContext ( );
bool loadAsset                  ( );
void drawMeshes                 ( );
//void recursive_render           ( const aiScene *sc, const aiNode* nd );





#endif /* asset_importer_hpp */
