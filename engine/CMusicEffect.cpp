//
//  CMusicEffect.cpp
//  IAApp
//
//  Created by Milan Kazarka on 3/16/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#include "CMusicEffect.h"

CMusicEffect::CMusicEffect( TiXmlElement *emusiceffect )
{
    memusiceffect = emusiceffect;
    mmusicinfo = NULL;
    
    TiXmlElement *emusic = memusiceffect->FirstChildElement("music");
    if (emusic)
    {
        CMusicInfo *ci = new CMusicInfo(emusic);
        addMusicInfo(ci);
        while(1)
        {
            emusic = emusic->NextSiblingElement();
            if (!emusic)
                break;
            addMusicInfo(ci);
        }
    }
}

int CMusicEffect::addMusicInfo( CMusicInfo *cmi )
{
    debug("    CMusicEffect::addMusicInfo\n",NULL);
    if (!cmi)
        return 1;
    
    if (!mmusicinfo)
    {
        mmusicinfo = cmi;
    }
    else
    {
        CMusicInfo *current = mmusicinfo;
        while(current)
        {
            if (!current->mnext)
            {
                current->mnext = cmi;
                cmi->mprev = NULL;
                break;
            }
            current = current->mnext;
        }
    }
    
    return 0;
}
