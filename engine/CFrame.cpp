//
//  CFrame.cpp
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

#include "CFrame.h"

SFrame initSFrame( )
{
    SFrame s;
    s.x = 0;
    s.y = 0;
    s.width = 0;
    s.height = 0;
    
    return s;
}

/**
 we store positioning in the script in % - as for example '20%'
 - this needs to be extracted and converted to an int x = 20
 */
int extractRelativePositionString( char *str )
{
    if (!str)
        return 0;
    int value = 0;
    
    char *tmp = (char*)malloc(strlen(str)+1);
    strcpy(tmp,str);
    for(int n = 0; n < strlen(tmp); n++)
        if (tmp[n]=='%') tmp[n] = 0x00;
    value = atoi(tmp);
    free(tmp);
    
    return value;
}
