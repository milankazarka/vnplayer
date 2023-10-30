//
//  CPageAction.cpp
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

#include "CPageAction.h"

CPageAction::CPageAction( TiXmlElement *eaction )
{
    mprev = NULL;
    mnext = NULL;
    layerid = NULL;
    filename = NULL;
    meaction = eaction;
    
    movx = 0;
    movy = 0;
    alpha = 1.0;
    
    if (meaction->Attribute("layerid"))
    {
        layerid = (char*)meaction->Attribute("layerid");
    }
    
    if (meaction->Attribute("type"))
    {
        if (strcmp(meaction->Attribute("type"),"layerMove")==0)
        {
            if (meaction->Attribute("mov"))
            {
                char *strmov = (char*)meaction->Attribute("mov");
                miscProcessMov(strmov, &movx, &movy);
            }
        }
        else if (strcmp(meaction->Attribute("type"),"alpha")==0)
        {
            if (meaction->Attribute("alpha"))
            {
                char *stralpha = (char*)meaction->Attribute("alpha");
                alpha = (float)atof(stralpha);
                debug("    alpha(%f)\n",alpha);
            }
        }
        else if (strcmp(meaction->Attribute("type"),"soundEffect")==0)
        {
            if (meaction->Attribute("filename"))
            {
                filename = (char*)meaction->Attribute("filename");
            }
        }
    }
}
