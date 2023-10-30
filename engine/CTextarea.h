//
//  CTextarea.h
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

#ifndef __IAApp__CTextarea__
#define __IAApp__CTextarea__

#include <iostream>
#include "CCommon.h"

#include "CPage.h"
#include "CFrame.h"

class CTextarea {
public:
    
    TiXmlElement *metextarea;
    CPage *mpages;
    CPage *mcurrentPage;
    
    // positioning
    SFrame frame;
    
    CTextarea( TiXmlElement *etextarea );
    ~CTextarea( );
    int addPage( CPage *page );
    int listPages( );
    
    CPage *rewind( );
    CPage *next( );
};

#endif /* defined(__IAApp__CTextarea__) */
