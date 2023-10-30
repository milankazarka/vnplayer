//
//  CMusicEffect.h
//  IAApp
//
//  Created by Milan Kazarka on 3/16/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#ifndef __IAApp__CMusicEffect__
#define __IAApp__CMusicEffect__

#include <iostream>
#include "CMusicInfo.h"

class CMusicEffect {
public:
    CMusicInfo *mmusicinfo; // linked list of musis info objects
    
    TiXmlElement *memusiceffect;
    
    CMusicEffect( TiXmlElement *emusiceffect );
    
    int addMusicInfo( CMusicInfo *mi );
};

#endif /* defined(__IAApp__CMusicEffect__) */
