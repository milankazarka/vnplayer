//
//  CFrame.h
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

#ifndef __IAApp__CFrame__
#define __IAApp__CFrame__

#include <iostream>
#include "CCommon.h"

struct SFrame {
    int x;
    int y;
    int width;
    int height;
};

SFrame initSFrame( );
int extractRelativePositionString( char *str ); // 20% char -> 20 int

#endif /* defined(__IAApp__CFrame__) */
