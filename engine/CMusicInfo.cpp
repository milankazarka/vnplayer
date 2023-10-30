//
//  CMusicInfo.cpp
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

#include "CMusicInfo.h"

CMusicInfo::CMusicInfo( TiXmlElement *emusic )
{
    debug("    CMusicInfo::CMusicInfo\n",NULL);
    
    memusic = emusic;
    
    mprev = NULL;
    mnext = NULL;
}

CMusicInfo::~CMusicInfo( )
{
    
}
