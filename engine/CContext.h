//
//  CContext.h
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

#ifndef __IAApp__CContext__
#define __IAApp__CContext__

#include <iostream>
#include "tinystr.h"
#include "tinyxml.h"
#include "CObject.h"
#include "CAttr.h"

class CContext : public CObject {
public:
    TiXmlElement *melement;
    CAttr *mattributes;
    
    CContext( TiXmlElement *element );
    int addAttribute( CAttr *attr );
    void list( );
};

#endif /* defined(__IAApp__CContext__) */
