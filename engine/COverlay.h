//
//  COverlay.h
//  IAApp
//
//  Created by Milan Kazarka on 3/1/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#ifndef __IAApp__COverlay__
#define __IAApp__COverlay__

#include <iostream>
#include "CCommon.h"
#include "CEffect.h"

class COverlay {
public:
    COverlay *mprev;
    COverlay *mnext;
    
    TiXmlElement *meoverlay;
    CEffect *meffects;
    
    // positioning
    float posx;
    float posy;
    float w;
    float h;
    
    bool loaded;
    
    COverlay( TiXmlElement *eoverlay );
    ~COverlay( );
    
    int addEffect( CEffect *effect );
};

#endif /* defined(__IAApp__COverlay__) */
