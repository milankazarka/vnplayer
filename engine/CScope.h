//
//  CScope.h
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

#ifndef __IAApp__CScope__
#define __IAApp__CScope__

#include <iostream>
#include "tinystr.h"
#include "tinyxml.h"
#include "CChoice.h"
#include "CScene.h"
#include "CTextarea.h"

class CScope : public CChoice {
public:
    CObject *mparent; // <scene> or NULL & when NULL, then we're top level
    TiXmlElement *melement; // setup holder
    
    CTextarea *mtextarea; // textarea defined for all scenes, alternatively this can also be in <scene>
    
    CScene *mscenes;
    CScene *mcurrentScene; // current scene
    
    CScope( TiXmlElement *element );
    ~CScope( );
    
    CScene *rewind( );
    int parse( );
    int loadScene( CScene *scene );
    virtual int loadElement( TiXmlElement *element );
    int parseSceneElement( TiXmlElement *escene );
    CScene *getSceneAtId( char *sceneid, bool rewind = false );
};

#endif /* defined(__IAApp__CScope__) */
