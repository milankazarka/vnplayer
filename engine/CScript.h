//
//  CScript.h
//  IAApp
//
//  Created by Milan Kazarka on 1/21/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#ifndef __IAApp__CScript__
#define __IAApp__CScript__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinystr.h"
#include "tinyxml.h"
#include "CScope.h"

class CScript {
public:
    TiXmlDocument *mdoc;
    CScope *mscope; // super scope
    char *tmpid; // this is a temporary holder
    
    CScene *mpresetNext; // preset next scene
    CScene *mpresetPrev; // when quickSave & quickLoad we jump to -one scene
    
    CScene *mcurrentScene; // current scene
    CScope *mcurrentScope; // current scope
        
    CScript( char *path );
    ~CScript( );
    
    int onScene( CScene *scene ); // perform actions based on entering scene
    int setScene( CScene *scene, bool rewind = true ); // set current scene & scope from it's mparent
    CScene *setScope( CScope *scope, bool rewind = true ); // set scope & rewind it, return first scene
    
    CScope *getScopeAtId( char *scopeid );
    CScope *getScopeAtIdRecursive( char *scopeid, CScope *superscope ); // recursive
    CScene *getSceneAtId( char *sceneid, bool rewind = false ); // id="scene01:pagenum"
    
    CScene *next( int nchoice );
    CScene *rewind( );
    CScene *rewindCurrentScope( ); // rewind current scope, don't rewind script
    
    //char *getCurrentSceneId( ); // free the value we receive
    char *getCurrentMultilevelSceneId( ); // get the current scope:scene:page id / free the value we receive
};

extern CScript *gscript;

#endif /* defined(__IAApp__CScript__) */
