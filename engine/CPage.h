//
//  CPage.h
//  IAApp
//
//  Created by Milan Kazarka on 2/9/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

/**
 A page holds a text block. Pages can be for example browsed through CTextarea
 */

#ifndef __IAApp__CPage__
#define __IAApp__CPage__

#include <iostream>
#include "CCommon.h"
#include "COverlay.h"
#include "CMusicEffect.h"
#include "CPageAction.h"
#include "CParagraph.h"
#include "CVideoEffect.h"

class CPage {
public:
    CPage *mprev;
    CPage *mnext;
    
    TiXmlElement *mepage;
    unsigned char *mchardata;
    
    COverlay *moverlay; // overlays associated with this page
    CMusicEffect *mmusiceffect; // per page music effects
    CVideoEffect *mvideoeffect; // per page video effects
    CPageAction *maction;
    
    CPage( TiXmlElement *epage );
    ~CPage( );
    int debugPage( );
    int addOverlay( COverlay *overlay );
    int addAction( CPageAction *action );
};

#endif /* defined(__IAApp__CPage__) */
