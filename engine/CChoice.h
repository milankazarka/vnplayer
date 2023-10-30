//
//  CChoice.h
//  IAApp
//
//  Created by Milan Kazarka on 1/21/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#ifndef __IAApp__CChoice__
#define __IAApp__CChoice__

#include <iostream>
#include "tinystr.h"
#include "tinyxml.h"
#include "CObject.h"

class CChoice : public CObject {
public:
    CChoice *mprev;
    CChoice *mnext;
    
    virtual int loadElement( TiXmlElement *element );
};

#endif /* defined(__IAApp__CChoice__) */
