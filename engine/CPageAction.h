//
//  CPageAction.h
//  IAApp
//
//  Created by Milan Kazarka on 3/29/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#ifndef __IAApp__CPageAction__
#define __IAApp__CPageAction__

#include <iostream>
#include "CCommon.h"

class CPageAction {
public:
    CPageAction *mprev;
    CPageAction *mnext;
    
    TiXmlElement *meaction;
    
    int movx;
    int movy;
    float alpha;
    
    char *layerid;
    char *filename; // in case filename= is present
    
    CPageAction( TiXmlElement *eaction );
};

#endif /* defined(__IAApp__CPageAction__) */
