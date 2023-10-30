//
//  CVideo.h
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

#ifndef __IAApp__CVideo__
#define __IAApp__CVideo__

#include <iostream>
#include "CCommon.h"

class CVideo {
public:
    CVideo *mprev;
    CVideo *mnext;
    
    TiXmlElement *mevideo;
    
    CVideo( TiXmlElement *evideo );
};

#endif /* defined(__IAApp__CVideo__) */
