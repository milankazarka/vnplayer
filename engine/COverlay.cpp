//
//  COverlay.cpp
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

#include "COverlay.h"

/**
 
 <overlay>
    <effects>
        <effect />
    </effects>
 </overlay>
 
 */
COverlay::COverlay( TiXmlElement *eoverlay )
{
    debug("COverlay::COverlay\n",NULL);
    mprev = NULL;
    mnext = NULL;
    
    meffects = NULL;
    
    meoverlay = eoverlay;
    
    posx = 0.0f;
    posy = 0.0f;
    w = 1.0f;
    h = 1.0f;
    
    loaded = false;
    
    if (meoverlay->Attribute("posx") && meoverlay->Attribute("posy") &&
        meoverlay->Attribute("width") && meoverlay->Attribute("height"))
    {
        posx = atof(meoverlay->Attribute("posx"));
        posy = atof(meoverlay->Attribute("posy"));
        w = atof(meoverlay->Attribute("width"));
        h = atof(meoverlay->Attribute("height"));
    }
    
    TiXmlElement *eeffects = meoverlay->FirstChildElement("effects");
    if (eeffects)
    {
        TiXmlElement *eeffect = eeffects->FirstChildElement("effect");
        if (eeffect)
        {
            CEffect *ce = new CEffect(eeffect);
            addEffect(ce);
            while(1)
            {
                eeffect = eeffect->NextSiblingElement();
                if (!eeffect)
                    break;
                ce = new CEffect(eeffect);
                addEffect(ce);
            }
        }
    }
}

COverlay::~COverlay( )
{
    
}

int COverlay::addEffect( CEffect *effect )
{
    debug("    COverlay::addEffect\n",NULL);
    
    if (!effect)
        return 1;
    
    if (!meffects)
    {
        meffects = effect;
    }
    else
    {
        CEffect *current = meffects;
        while(current)
        {
            if (!current->mnext)
            {
                current->mnext = effect;
                effect->mprev = current;
                break;
            }
            current = current->mnext;
        }
    }
    
    return 0;
}
