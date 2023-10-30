//
//  CScene.h
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

#ifndef __IAApp__CScene__
#define __IAApp__CScene__

#include <iostream>
#include "tinystr.h"
#include "tinyxml.h"
#include "CChoice.h"
#include "CTextarea.h"
#include "CMusicInfo.h"
#include "CLayer.h"
#include "CObject.h"
#include "CContext.h"
#include "CCheckpoint.h"
#include "CPage.h"

#define _OVERLAY_COUNT 6
#define _LAYER_COUNT 6

class CScene : public CObject {
public:
    CScene *mprev;
    CScene *mnext;
    
    bool mvirtual; // a virtual scene is skipped when paging, but you can jump into it
    
    CObject *mparent; // <scope>
    
    TiXmlElement *melement;
    
    TiXmlElement *mechoicedef; // <choices>
    
    CChoice *mchoices; // <choice>
    
    // we can either have pages within textarea or right under <scene> which is a simplification
    
    CTextarea *mtextarea;
    
    CPage *mpages;
    CPage *mcurrentPage;
    
    CMusicInfo *mmusicinfo; // list of music info elements
    CLayer *mlayer; // layers used for the background
    COverlay *moverlay; // generic overlays - we use these as the default ones if there's non in the <page> nodes
    CContext *mcontext; // the current context attributes
    CCheckpoint *mcheckpoint;
    
    CScene( TiXmlElement *element );
    int rewind( );
    int loadElement( TiXmlElement *element );
    int addChoice( CChoice *choice ); // <scopes> where <scope>'s C++ parent of CChoice
    int addMusicInfo( CMusicInfo *minfo );
    int addLayer( CLayer *layer );
    int addPage( CPage *page );
    int addOverlay( COverlay *overlay );
    int listPages( );
    TiXmlElement *choiseElementAtIndex( int index );
    TiXmlElement *getImageElement( ); // get the image element, which stores the filename
    
    CPage *rewindPages( );
    CPage *nextPage( );
};

#endif /* defined(__IAApp__CScene__) */
