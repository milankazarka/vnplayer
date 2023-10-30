//
//  CStats.h
//  IAApp
//
//  Created by Milan Kazarka on 5/19/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#ifndef __IAApp__CStats__
#define __IAApp__CStats__

#include <iostream>
#include "tinyxml.h"
#include "tinystr.h"

struct PAGETEXT {
    const char *characterptr;
    const char *txtptr;
    struct PAGETEXT *next;
};

class CStats {
public:
    int nwords; // number of words in script
    
    PAGETEXT *pagelist; // linked list of all page texts
    
    CStats( );
    ~CStats( );
    
    int parsePage( TiXmlElement *epage );
    int addPageText( const char *character, const char *txtptr );
    int printPageList( );
};

extern CStats *gstats;

CStats *getGStats( );

#endif /* defined(__IAApp__CStats__) */
