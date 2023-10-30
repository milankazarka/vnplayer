//
//  COutputLog.h
//  IAApp
//
//  Created by Milan Kazarka on 5/31/15.
//  Copyright (c) 2015 Milan Kazarka. All rights reserved.
//

#ifndef __IAApp__COutputLog__
#define __IAApp__COutputLog__

#include <stdio.h>

#define _OUTPUT_BUFFER_SIZE 1024*1024

extern char *outputLogEditingBuffer;

void outputLogEditing( char *raw );

#endif /* defined(__IAApp__COutputLog__) */
