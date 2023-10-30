//
//  CCurrentContext.h
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

#ifndef __IAApp__CCurrentContext__
#define __IAApp__CCurrentContext__

#include <iostream>
#include "tinystr.h"
#include "tinyxml.h"
#include "CObject.h"
#include "CAttr.h"
#include "CContext.h"

class CCurrentContext : public CObject {
public:
    CCurrentContext( );
    ~CCurrentContext( );
    
    
    CAttr *mattributes;
    
    int onContext( CContext *context ); // add context information
    int onAttribute( CAttr *attr ); // add attribute
    CAttr *getAttributeAtKey( char *key );
    bool includesAttribute( CAttr *attr ); // includes attributes key/value combination
    int addAttribute( CAttr *attr );
    void flush( ); // flush current context
    void list( ); // list all context information
    
    bool includesContext( CContext *context ); // does the current context include 'context' attributes
};

CCurrentContext *getGlobalContext( );
extern CCurrentContext *gcontext;

#endif /* defined(__IAApp__CCurrentContext__) */
