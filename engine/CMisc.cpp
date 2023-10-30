//
//  CMisc.cpp
//  IAApp
//
//  Created by Milan Kazarka on 3/29/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#include "CMisc.h"
#include "CCommon.h"

int miscProcessMov( char *strmov, int *movx, int *movy )
{
    if (!strmov || !movx || !movy)
        return 1;
    
    char *strmovx = NULL;
    int n;
    for(n = 0; n < strlen(strmov); n++)
    {
        if (strncmp(strmov+n,":",1)==0 && n!=0)
        {
            strmovx = (char*)malloc(n+1);
            strncpy(strmovx,strmov,n);
            strmovx[n] = 0x00;
            *movx = atoi(strmovx);
            
            if (strmov[n+1])
            {
                *movy = atoi(strmov+n+1);
            }
            
            debug("    mov x(%d) y(%d)\n",*movx,*movy);
        }
    }
    
    if (!strmovx && strlen(strmov)>0)
    {
        
    }
    
    free(strmovx);
    
    return 0;
}
