//
//  CLayers.h
//  IAApp
//
//  Created by Milan Kazarka on 3/20/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#ifndef __IAApp__CLayers__
#define __IAApp__CLayers__

#include <iostream>
#include "CCommon.h"

class CLayer {
public:
    TiXmlElement *melayer;
    CLayer *mprev;
    CLayer *mnext;
    
    int movx;
    int movy;
    
    CLayer( TiXmlElement *elayer );
    ~CLayer( );
    
    bool match( CLayer *cl );
};

#endif /* defined(__IAApp__CLayers__) */
