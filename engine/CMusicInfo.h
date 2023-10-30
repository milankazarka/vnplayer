//
//  CMusicInfo.h
//  IAApp
//
//  Created by Milan Kazarka on 3/1/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#ifndef __IAApp__CMusicInfo__
#define __IAApp__CMusicInfo__

#include <iostream>
#include "CCommon.h"

class CMusicInfo {
public:
    CMusicInfo *mprev;
    CMusicInfo *mnext;
    
    TiXmlElement *memusic;
    
    CMusicInfo( TiXmlElement *emusic );
    ~CMusicInfo( );
};

#endif /* defined(__IAApp__CMusicInfo__) */
