//
//  CVideoEffect.cpp
//  IAApp
//
//  Created by Milan Kazarka on 5/10/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#include "CVideoEffect.h"

CVideoEffect::CVideoEffect( TiXmlElement *evideoeffect )
{
    debug("CVideoEffect::CVideoEffect\n",NULL);
    
    mvideo = NULL;
    mevideoeffect = evideoeffect;
    
    TiXmlElement *evideo = evideoeffect->FirstChildElement("video");
    if (evideo)
    {
        CVideo *cv = new CVideo(evideo);
        if (cv)
            addVideo(cv);
        while(1)
        {
            evideo = evideo->NextSiblingElement();
            if (!evideo)
                break;
            cv = new CVideo(evideo);
            if (cv)
                addVideo(cv);
        }
    }
}

int CVideoEffect::addVideo( CVideo *video )
{
    debug("CVideoEffect::addVideo\n",NULL);
    if (!video)
        return 1;
    
    if (!mvideo)
    {
        mvideo = video;
    }
    else
    {
        CVideo *current = mvideo;
        while(current)
        {
            if (!current->mnext)
            {
                current->mnext = video;
                video->mprev = NULL;
                break;
            }
            current = current->mnext;
        }
    }
    
    return 0;
}
