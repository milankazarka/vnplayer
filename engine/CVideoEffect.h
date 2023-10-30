//
//  CVideoEffect.h
//  IAApp
//
//  Created by Milan Kazarka on 5/10/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#ifndef __IAApp__CVideoEffect__
#define __IAApp__CVideoEffect__

#include <iostream>
#include "CCommon.h"
#include "CVideo.h"

class CVideoEffect {
public:
    TiXmlElement *mevideoeffect;
    CVideo *mvideo; // linked list of videos
    
    CVideoEffect( TiXmlElement *evideoeffect );
    
    int addVideo( CVideo *video );
};

#endif /* defined(__IAApp__CVideoEffect__) */
