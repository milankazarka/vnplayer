//
//  CEffect.h
//  IAApp
//
//  Created by Milan Kazarka on 3/15/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#ifndef __IAApp__CEffect__
#define __IAApp__CEffect__

#include <iostream>
#include "CCommon.h"

class CEffect {
public:
    CEffect *mprev;
    CEffect *mnext;
    
    TiXmlElement *meeffect;
    
    CEffect( TiXmlElement *eeffect );
};

#endif /* defined(__IAApp__CEffect__) */
