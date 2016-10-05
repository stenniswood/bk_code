//
//  asset_importer.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 2/26/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#include <iostream>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <list>
#include <vector>
#include <math.h>
#include <string>
#include <string.h>

#include "gl_container.hpp"
#include "gl_object.hpp"
#include "asset_importer.hpp"
#include "Texture.hpp"

using namespace std;


#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/cimport.h>

const aiScene* asset_scene = NULL;
vector<Texture*>  asset_textures;


// This holds info specific to the opengl graphics cards IDs.  And is info not stored in asset_scene
struct MeshHelper
{
    // Display list ID, this one shots *all drawing* of the mesh. Only ever use this to draw. Booya.
    GLuint displayList;
    
    // VAO that encapsulates all VBO drawing state
    GLuint vao;
    
    // VBOs
    GLuint vertexBuffer;
    GLuint indexBuffer;
    GLuint normalBuffer;
    GLuint numIndices;
    
    // Texture
    GLuint textureID;
    Texture*    texture;
    
    // Material
    aiColor4D diffuseColor;
    aiColor4D specularColor;
    aiColor4D ambientColor;
    aiColor4D emissiveColor;
    
    GLfloat opacity;
    GLfloat shininess;
    GLfloat specularStrength;
    
    char twoSided;
};
vector<MeshHelper*> modelMeshes;
void color4_to_float4(const aiColor4D *c, float f[4])
{
    f[0] = c->r;
    f[1] = c->g;
    f[2] = c->b;
    f[3] = c->a;
}

void createGLResourcesInContext()
{
    // create new mesh helpers for each mesh, will populate their data later.
    modelMeshes.clear();
    
    // create OpenGL buffers and populate them based on each meshes pertinant info.
    for (unsigned int i = 0; i < asset_scene->mNumMeshes; ++i)
    {
        
        // current mesh we are introspecting
        const aiMesh* mesh = asset_scene->mMeshes[i];
        
        // the current meshHelper we will be populating data into.
        MeshHelper* meshHelper = new MeshHelper();
        
        // Handle material info
        aiMaterial* mtl = asset_scene->mMaterials[mesh->mMaterialIndex];
        
        // Textures
        int texIndex = 0;
        aiString texPath;
        
        if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath))
        {
            std::string textureKey = texPath.data;
            int  textureNumber = atoi(textureKey.c_str());
            
            //NSLog(@"applyMaterialInContext: have texture %i", [textureNumber unsignedIntValue]);
            meshHelper->textureID = textureNumber;
        }
        else
            meshHelper->textureID = 0;
        
        // Colors
        aiColor4D dcolor = aiColor4D(0.8f, 0.8f, 0.8f, 1.0f);
        if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &dcolor))
            meshHelper->diffuseColor  = dcolor;
        aiColor4D scolor = aiColor4D(0.0f, 0.0f, 0.0f, 1.0f);
        if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &scolor))
            meshHelper->specularColor = scolor;
        aiColor4D acolor = aiColor4D(0.2f, 0.2f, 0.2f, 1.0f);
        if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &acolor))
            meshHelper->ambientColor = acolor;
        aiColor4D ecolor = aiColor4D(0.0f, 0.0f, 0.0f, 1.0f);
        if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &ecolor))
            meshHelper->emissiveColor =ecolor;
        
        // Culling
        unsigned int max = 1;
        int two_sided;
        if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
            meshHelper->twoSided = 'Y';
        else
            meshHelper->twoSided = 'N';
        
        
        // Create a VBO for our vertices
        GLuint vhandle;
        glGenBuffers(1, &vhandle);
        glBindBuffer(GL_ARRAY_BUFFER, vhandle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->mNumVertices, NULL, GL_STATIC_DRAW);
        
        // populate vertices
        Vertex* verts = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        
        for (unsigned int x = 0; x < mesh->mNumVertices; ++x)
        {
            verts->position[0] = mesh->mVertices[x][0];
            verts->position[1] = mesh->mVertices[x][1];
            verts->position[2] = mesh->mVertices[x][2];
            
            if (NULL == mesh->mNormals) {
                verts->normal[0] = 0.0;
                verts->normal[1] = 0.0;
                verts->normal[2] = 0.0;
                
            } else {
                verts->normal[0] = mesh->mNormals[x][0];
                verts->normal[1] = mesh->mNormals[x][1];
                verts->normal[2] = mesh->mNormals[x][2];
            }
            if (NULL == mesh->mTangents)
            {
                // verts->vTangent   = aiVector3D(0.0f,0.0f,0.0f);
                // verts->vBitangent = aiVector3D(0.0f,0.0f,0.0f);
            }
            else
            {
                //  verts->vTangent = mesh->mTangents[x];
                //  verts->vBitangent = mesh->mBitangents[x];
            }
            
            if (mesh->HasVertexColors(0))
            {
                verts->color[0] = mesh->mColors[0][x][0];
                verts->color[1] = mesh->mColors[0][x][1];
                verts->color[2] = mesh->mColors[0][x][2];
            }
            else {
                verts->color[0] = 1.0; //aiColor4D(1.0, 1.0, 1.0, 1.0);
                verts->color[1] = 1.0; //aiColor4D(1.0, 1.0, 1.0, 1.0);
                verts->color[2] = 1.0; //aiColor4D(1.0, 1.0, 1.0, 1.0);
                verts->color[3] = 1.0; //aiColor4D(1.0, 1.0, 1.0, 1.0);
            }
            // This varies slightly form Assimp View, we support the 3rd texture component.
            /*                if (mesh->HasTextureCoords(0))
             verts->vTextureUV = mesh->mTextureCoords[0][x];
             else
             verts->vTextureUV = aiVector3D(0.5f,0.5f, 0.0f);
             
             if (mesh->HasTextureCoords(1))
             verts->vTextureUV2 = mesh->mTextureCoords[1][x];
             else
             verts->vTextureUV2 = aiVector3D(0.5f,0.5f, 0.0f);
             
             // TODO: handle Bone indices and weights
             {
             memset( verts->mBoneIndices, 0, sizeof( verts->mBoneIndices));
             memset( verts->mBoneWeights, 0, sizeof( verts->mBoneWeights));
             }
             */
            ++verts;
        }
        
        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB); //invalidates verts
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // set the mesh vertex buffer handle to our new vertex buffer.
        meshHelper->vertexBuffer = vhandle;
        
        // Create Index Buffer
        
        // populate the index buffer.
        int nidx;
        switch (mesh->mPrimitiveTypes)
        {
            case aiPrimitiveType_POINT:
                nidx = 1;break;
            case aiPrimitiveType_LINE:
                nidx = 2;break;
            case aiPrimitiveType_TRIANGLE:
                nidx = 3;break;
            default: assert(false);
        }
        
        // create the index buffer
        GLuint ihandle;
        glGenBuffers(1, &ihandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ihandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh->mNumFaces * nidx, NULL, GL_STATIC_DRAW);
        
        unsigned int* indices = (unsigned int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY_ARB);
        
        // now fill the index buffer
        for (unsigned int x = 0; x < mesh->mNumFaces; ++x)
        {
            for (unsigned int a = 0; a < nidx; ++a)
            {
                //                 if(mesh->mFaces[x].mNumIndices != 3)
                //                     NSLog(@"whoa dont have 3 indices...");
                *indices++ = mesh->mFaces[x].mIndices[a];
            }
        }
        
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        // set the mesh index buffer handle to our new index buffer.
        meshHelper->indexBuffer = ihandle;
        meshHelper->numIndices = mesh->mNumFaces * nidx;
        
        // create the normal buffer. Assimp View creates a second normal buffer. Unsure why. Using only the interleaved normals for now.
        // This is here for reference.
        
        // Create VAO and populate it
        
        GLuint vaoHandle;
        glGenVertexArrays(1, &vaoHandle);
        
        glBindVertexArray(vaoHandle);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, meshHelper->vertexBuffer);
        
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
        
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(24));
        
        //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        //glTexCoordPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(64));
        //TODO: handle second texture
        
        // VertexPointer ought to come last, apparently this is some optimization, since if its set once, first, it gets fiddled with every time something else is update.
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(Vertex), 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshHelper->indexBuffer);
        
        glBindVertexArray(0);
        
        // save the VAO handle into our mesh helper
        meshHelper->vao = vaoHandle;
        
        // Create the display list
        
        GLuint list = glGenLists(1);
        
        glNewList(list, GL_COMPILE);
        
        float dc[4];
        float sc[4];
        float ac[4];
        float emc[4];
        
        // Material colors and properties
        color4_to_float4( &meshHelper->diffuseColor, dc);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dc);
        
        color4_to_float4( &meshHelper->specularColor, sc);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sc);
        
        color4_to_float4(&meshHelper->ambientColor, ac);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ac);
        
        color4_to_float4(&meshHelper->emissiveColor, emc);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emc);
        
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        
        // Culling
        if(meshHelper->twoSided)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
        
        // Texture Binding
        glBindTexture  (GL_TEXTURE_2D, meshHelper->textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        // This binds the whole VAO, inheriting all the buffer and client state. Weeee
        glBindVertexArray(meshHelper->vao);
        glDrawElements(GL_TRIANGLES, meshHelper->numIndices, GL_UNSIGNED_INT, 0);
        
        glEndList();
        
        meshHelper->displayList = list;
        
        // Whew, done. Save all of this shit.
        modelMeshes.push_back( meshHelper);
        
        //[meshHelper release];
    }
}

/* This works for the couch at least.  Now let's "codify" it.  boil it down to the essence...
 What does property store do for us?
 What opengl objects are created?
 
 
 
 */

bool loadAsset( )
{
    std::string filename = "/Users/stephentenniswood/Documents/couch_sketchup.dae";
    
    // Create an instance of the Importer class
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    unsigned int aiPostProccesFlags;
    aiPostProccesFlags = aiProcessPreset_TargetRealtime_MaxQuality| aiProcess_Triangulate | aiProcess_FlipUVs |aiProcess_PreTransformVertices;
    
    
    // only ever give us triangles.
    aiPropertyStore* props = aiCreatePropertyStore();
    aiSetImportPropertyInteger(props, AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE |   aiPrimitiveType_POINT );
    asset_scene  = (aiScene*)aiImportFileExWithProperties(filename.c_str(), aiPostProccesFlags , NULL, props);
    //asset_scene = importer.ReadFile( filename, aiPostProccesFlags |
    //                                aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_PreTransformVertices );
    aiReleasePropertyStore(props);
    
    // Now Read all textures:
    int count_found = 0;
    for (unsigned int m = 0; m < asset_scene->mNumMaterials; m++)
    {
        int texIndex = 0;
        aiString path;	// filename
        aiReturn texFound = AI_SUCCESS;
        texFound = asset_scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
        if ( (texFound) && (path.length) )
        {
            count_found++;
            Texture* tmpt = new Texture();
            tmpt->load_image(path.data);
            asset_textures.push_back( tmpt );
        }
    }
    //GLuint *textureIds; // = new GLuint[count_found];
    //glGenTextures( count_found, textureIds );
    
    createGLResourcesInContext();
    // If the import failed, report it
    if( !asset_scene )
    {
        //DoTheErrorLogging( importer.GetErrorString());
        return false;
    }
    // Now we can access the file's contents.
    //DoTheSceneProcessing( scene);
    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

void drawMeshes()
{
    glPushMatrix();
    //glTranslatef(-400, 0, 500);
    //glScaled  ( 25.0, 25.0, 25.0);
    //glCallList( modelMeshes[0]->displayList );
    
    for (int i=0; i<modelMeshes.size(); i++)
    {
        //   glCallList(modelMeshes[i]->displayList);
    }
    glPopMatrix();
}

void recursive_render (const aiScene *sc, const aiNode* nd)
{
    // Get node transformation matrix
    aiMatrix4x4 m = nd->mTransformation;
    // OpenGL matrices are column major
    m.Transpose();
    
    // save model matrix and apply node transformation
    glPushMatrix();
    
    float aux[16];
    memcpy(aux,&m,sizeof(float) * 16);
    //multMatrix( modelMatrix, aux );
    //glsetModelMatrix();
    
    // draw all meshes assigned to this node
    for (unsigned int n=0; n < sc->mNumMeshes; ++n) {
        // bind material uniform
        //glBindBufferRange(GL_UNIFORM_BUFFER, materialUniLoc, sc->mMeshes[nd->mMeshes[n]].uniformBlockIndex, 0, sizeof(struct MyMaterial));
        // bind texture
        //glBindTexture( GL_TEXTURE_2D, sc->mMeshes[nd->mMeshes[n]]->texIndex );
        // bind VAO
        //        glBindVertexArray( sc->mMeshes[n]->vao );
        // draw
        //      glDrawElements(GL_TRIANGLES, sc->mMeshes[n]->numFaces*3, GL_UNSIGNED_INT, 0 );
    }
    
    // draw all children
    for (unsigned int n=0; n < nd->mNumChildren; ++n) {
        recursive_render(sc, nd->mChildren[n]);
    }
    glPopMatrix();
}






