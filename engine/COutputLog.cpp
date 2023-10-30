//
//  COutputLog.cpp
//  IAApp
//
//  Created by Milan Kazarka on 5/31/15.
//  Copyright (c) 2015 Milan Kazarka. All rights reserved.
//

#include "COutputLog.h"
#include <stdlib.h>
#include <string.h>

char *outputLogEditingBuffer = NULL;

void outputLogEditing( char *raw )
{
    if (!raw)
        return;
    if (!outputLogEditingBuffer)
    {
        outputLogEditingBuffer = (char*)malloc(_OUTPUT_BUFFER_SIZE);
        outputLogEditingBuffer[0] = 0x00;
    }
    strcat(outputLogEditingBuffer,raw);
}
