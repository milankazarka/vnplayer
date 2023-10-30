//
//  CPage.cpp
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

#include "CPage.h"
#include "CStats.h"
#include "COutputLog.h"

CPage::CPage( TiXmlElement *epage )
{
    debug("CPage::CPage\n",NULL);
    
    mepage = epage;
    
    moverlay = NULL;
    mmusiceffect = NULL;
    mvideoeffect = NULL;
    maction = NULL;
    
    char *chardata = NULL;
    
    CStats *gcs = getGStats();
    if (gcs)
        gcs->parsePage(epage);
    
    // we either read a <text> or text=""
    TiXmlElement *etext = mepage->FirstChildElement("text");
    if (etext)
    {
        if (etext->GetText())
            chardata = (char*)etext->GetText();
    }
    else
    {
        if (mepage->Attribute("text"))
            chardata = (char*)mepage->Attribute("text");
    }
        
    mchardata = NULL;
    if (chardata)
    {
        mchardata = (unsigned char*)malloc(strlen((char*)chardata)+1);
        strcpy((char*)mchardata,(char*)chardata);
        mchardata[strlen((char*)chardata)] = 0x00;
    
        debug("    text(%s)\n",(char*)mchardata);
    }
    
    TiXmlElement *eoverlays = mepage->FirstChildElement("overlays");
    if (eoverlays)
    {
        TiXmlElement *eoverlay = eoverlays->FirstChildElement("overlay");
        if (eoverlay)
        {
            COverlay *overlay = new COverlay(eoverlay);
            addOverlay(overlay);
            while(1)
            {
                eoverlay = eoverlay->NextSiblingElement();
                if (!eoverlay)
                    break;
                overlay = new COverlay(eoverlay);
                addOverlay(overlay);
            }
        }
    }
    
    TiXmlElement *eactions = mepage->FirstChildElement("actions");
    if (eactions)
    {
        TiXmlElement *eaction = eactions->FirstChildElement("action");
        if (eaction)
        {
            CPageAction *action = new CPageAction(eaction);
            addAction(action);
            while(1)
            {
                eaction = eaction->NextSiblingElement();
                if (!eaction)
                    break;
                action = new CPageAction(eaction);
                addAction(action);
            }
        }
    }
    
    TiXmlElement *emusiceffects = mepage->FirstChildElement("musiceffects");
    if (emusiceffects)
    {
        TiXmlElement *emusiceffect = emusiceffects->FirstChildElement("musiceffect");
        if (emusiceffect)
        {
            mmusiceffect = new CMusicEffect(emusiceffect);
        }
    }
    
    TiXmlElement *evideoeffects = mepage->FirstChildElement("videoeffects");
    if (evideoeffects)
    {
        TiXmlElement *evideoeffect = evideoeffects->FirstChildElement("videoeffect");
        if (evideoeffect)
        {
            mvideoeffect = new CVideoEffect(evideoeffect);
        }
    }
    
    char raw[4096];
    if (epage->Attribute("character"))
    {
        sprintf((char*)raw,"    %s:",epage->Attribute("character"));
        outputLogEditing((char*)raw);
    }
    if (chardata)
    {
        outputLogEditing((char*)chardata);
    }
    outputLogEditing((char*)"\n");
    
    mprev = NULL;
    mnext = NULL;
}

CPage::~CPage( )
{
    
}

int CPage::debugPage( )
{
    if (mchardata)
    {
        if (mchardata[0]!=0x00)
        {
            debug("CPage::debugPage chardata(%s)\n",(char*)mchardata);
        }
    }
    return 0;
}

int CPage::addOverlay( COverlay *overlay )
{
    debug("CPage::addOverlay\n",NULL);
    
    if (!overlay)
        return 1;
    
    if (!moverlay)
    {
        overlay->mprev = NULL;
        moverlay = overlay;
    }
    else
    {
        COverlay *current = moverlay;
        while(current)
        {
            if (current->mnext==NULL)
            {
                current->mnext=overlay;
                overlay->mprev = current;
                break;
            }
            current = current->mnext;
        }
    }
    overlay->mnext = NULL;
    
    return 0;
}

int CPage::addAction( CPageAction *action )
{
    debug("CPage::addAction\n",NULL);
    
    if (!action)
        return 1;
    
    if (!maction)
    {
        action->mprev = NULL;
        maction = action;
    }
    else
    {
        CPageAction *current = maction;
        while(current)
        {
            if (current->mnext==NULL)
            {
                current->mnext=action;
                action->mprev = current;
                break;
            }
            current = current->mnext;
        }
    }
    action->mnext = NULL;
    
    return 0;
}
