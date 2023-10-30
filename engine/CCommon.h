//
//  CCommon.h
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

#ifndef IAApp_CCommon_h
#define IAApp_CCommon_h

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tinystr.h"
#include "tinyxml.h"
#include "CMisc.h"

#ifndef _SAVE_REAL_SCERENS
#define _SAVE_REAL_SCREENS // are we saving screenshots or just backgrounds of scenes
#endif

#define fequal(a,b) (fabs((a) - (b)) < FLT_EPSILON)
#define fequalzero(a) (fabs(a) < FLT_EPSILON)

#ifndef _USE_CONTAINERS
#define _USE_CONTAINERS
#endif

#ifndef _CDEBUG_NO
#define _CDEBUG // for the c++ code
#endif

#ifdef _CDEBUG
    //#define debug(fmt, ...) printf("%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__)
    #define debug(fmt, ...) printf(fmt, __VA_ARGS__)
#else
    #define debug(fmt, ...) printf("");
#endif

#ifndef _DEBUG
#define _DEBUG
#endif

#ifndef _RUNNING_TEXT
#define _RUNNING_TEXT
#endif

#ifndef _ENABLE_MUSIC
#define _ENABLE_MUSIC
#endif

#endif
