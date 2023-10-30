//
//  CAttr.h
//  IAApp
//
//  Created by Milan Kazarka on 10/19/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#ifndef __IAApp__CAttr__
#define __IAApp__CAttr__

#include <iostream>
#include "tinystr.h"
#include "tinyxml.h"
#include "CObject.h"

/**
 
    <attr key="abc" value="001"/>
    <attr key="xyz" value="002"/>
 
 */

class CAttr : public CObject {
public:
    CAttr *mprev;
    CAttr *mnext;
    
    char *mkey;
    char *mvalue;
    
    TiXmlElement *melement;
    
    CAttr( TiXmlElement *element );
    CAttr( char *key, char *value );
    ~CAttr( );
};

#endif /* defined(__IAApp__CAttr__) */
