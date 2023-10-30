//
//  CLayers.cpp
//  IAApp
//
//  Created by Milan Kazarka on 3/20/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#include "CLayer.h"
#include "CCommon.h"

CLayer::CLayer( TiXmlElement *elayer )
{
    debug("CLayer::CLayer\n",NULL);

    melayer = elayer;
    mprev = NULL;
    mnext = NULL;
    movx = 0;
    movy = 0;
    
    if (elayer->Attribute("mov"))
    {
        char *strmov = (char*)elayer->Attribute("mov");
        miscProcessMov(strmov, &movx, &movy);
    }
}

CLayer::~CLayer( )
{
    
}

/**  does CLayer match this one
 
 we match the filename and the effects - they need to be in the same order.
 
 <layer filename="IA_PreWork_Cafe_ver2.jpg" id="l01">
    <effects>
        <effect type="endlessScroll"/>
        <effect type="boundaryScroll"/>
        <effect type="brightnessFluctuationNO"/>
    </effects>
 </layer>
 
 todo - make effect matching separate, so that we can add more attributes into <effect> in the future or
 make generic XML node matching
 
 */
bool CLayer::match( CLayer *cl )
{
    debug("CLayer::match\n",NULL);
    if (!cl)
        return false;
    
    if (cl->melayer && melayer)
    {
        if (melayer->Attribute("id") && cl->melayer->Attribute("id"))
        {
            if (strcmp(melayer->Attribute("id"),cl->melayer->Attribute("id"))!=0)
                return false;
            debug("    layer ids match\n",NULL);
        }
        else
        {
            if (
                ( !melayer->Attribute("id") && cl->melayer->Attribute("id") ) ||
                ( melayer->Attribute("id") && !cl->melayer->Attribute("id") )
                )
                return false;
        }
        
        if (melayer->Attribute("alignment") && cl->melayer->Attribute("alignment"))
        {
            if (strcmp(melayer->Attribute("alignment"),cl->melayer->Attribute("alignment"))!=0)
            {
                return false;
            }
        }
        
        if (melayer->Attribute("filename") && cl->melayer->Attribute("filename"))
        {
            if (strcmp(melayer->Attribute("filename"),cl->melayer->Attribute("filename"))==0)
            {
                debug("    filenames match\n",NULL);
                if (melayer->FirstChildElement("effects"))
                {
                    if (!cl->melayer->FirstChildElement("effects"))
                        return false;
                    
                    TiXmlElement *eeffects01 = melayer->FirstChildElement("effects");
                    TiXmlElement *eeffects02 = cl->melayer->FirstChildElement("effects");
                    
                    if (!eeffects02) // no effects to match
                    {
                        debug("    no effects in one of the layers, layers don't match\n",NULL);
                        return false;
                    }
                        
                    TiXmlElement *eeffect01 = eeffects01->FirstChildElement("effect");
                    if (eeffect01)
                    {
                        TiXmlElement *eeffect02 = eeffects02->FirstChildElement("effect");
                        if (!eeffect02)
                            return false;
                        
                        // match the effects
                        if (eeffect01->Attribute("type") && eeffect02->Attribute("type"))
                        {
                            if (strcmp(eeffect01->Attribute("type"),eeffect02->Attribute("type"))!=0)
                            {
                                debug("    effect (%s) doesn't match (%s)\n",eeffect01->Attribute("type"),eeffect02->Attribute("type"));
                                return false;
                            }
                            else
                            {
                                debug("    effect (%s) match (%s)\n",eeffect01->Attribute("type"),eeffect02->Attribute("type"));
                            }
                        }
                        else
                            return false;
                        
                        while(1)
                        {
                            eeffect01 = eeffect01->NextSiblingElement();
                            if (!eeffect01)
                            {
                                if (eeffect02->NextSiblingElement())
                                    return false;
                                break;
                            }
                            
                            if (!eeffect02->NextSiblingElement())
                                return false;
                            
                            eeffect02 = eeffect02->NextSiblingElement();
                            
                            // match the effects
                            if (eeffect01->Attribute("type") && eeffect02->Attribute("type"))
                            {
                                if (strcmp(eeffect01->Attribute("type"),eeffect02->Attribute("type"))!=0)
                                {
                                    debug("    effect (%s) doesn't match (%s)\n",eeffect01->Attribute("type"),eeffect02->Attribute("type"));
                                    return false;
                                }
                                else
                                {
                                    debug("    effect (%s) match (%s)\n",eeffect01->Attribute("type"),eeffect02->Attribute("type"));
                                }
                            }
                            else
                                return false;
                        }
                    }
                    else
                    {
                        if (eeffects02->FirstChildElement("effect"))
                            return false;
                    }
                    
                    debug("    layers effects match\n",NULL);
                    return true;
                }
                else
                {
                    if (cl->melayer->FirstChildElement("effects"))
                    {
                        debug("    one layer includes effects, layers don't match\n",NULL);
                        return false;
                    }
                    
                    debug("    no effects in both layers, layers match\n",NULL);
                    return true;
                }
            }
        }
        else
        {
            if (
                ( melayer->Attribute("filename") && cl->melayer->Attribute("filename") ) ||
                ( !melayer->Attribute("filename") && !cl->melayer->Attribute("filename") )
                )
                return true;
        }
    }
    
    return false;
}
