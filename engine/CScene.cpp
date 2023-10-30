//
//  CScene.cpp
//  IAApp
//
//  Created by Milan Kazarka on 1/21/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#include "CScene.h"
#include "CScope.h"
#include "CCommon.h"
#include "COutputLog.h"

CScene::CScene( TiXmlElement *element )
{
    debug("CScene::CScene\n",NULL);
    
    mvirtual = false;
    mprev = NULL;
    mnext = NULL;
    mchoices = NULL;
    mtextarea = NULL;
    mpages = NULL;
    mcurrentPage = NULL;
    mlayer = NULL;
    moverlay = NULL;
    
    if (element)
    {
        loadElement(element);
    }
}

/**
 load scene speciffic configuration
 */
int CScene::loadElement( TiXmlElement *element )
{
    if (!element)
        return 1;
    
    debug("CScene::loadElement\n",NULL);
    
    melement = element;
    mmusicinfo = NULL;
    mcontext = NULL;
    mcheckpoint = NULL;
    mpages = NULL;
    mcurrentPage = NULL;
    mtextarea = NULL;
    
    if (element->Attribute("id"))
    {
        char raw[256];
        sprintf((char*)raw,"Scene(%s) {\n",element->Attribute("id"));
        outputLogEditing((char*)raw);
    }
    
    if (element->Attribute("virtual")) /* deprecated, remove */
    {
        if (
            strcmp(element->Attribute("virtual"),"yes")==0 ||
            strcmp(element->Attribute("virtual"),"YES")==0
            )
            mvirtual = true;
    }
    
    TiXmlElement *ewidgets = melement->FirstChildElement("widgets");
    if (ewidgets)
    {
        TiXmlElement *etextarea = ewidgets->FirstChildElement("textarea");
        if (etextarea)
        {
            mtextarea = new CTextarea(etextarea);
        }
        
        TiXmlElement *epages = ewidgets->FirstChildElement("pages");
        if (epages)
        {
            TiXmlElement *epage = epages->FirstChildElement("page");
            if (epage)
            {
                CPage *page = new CPage(epage);
                addPage(page);
                while(1)
                {
                    epage = epage->NextSiblingElement();
                    if (!epage)
                        break;
                    page = new CPage(epage);
                    addPage(page);
                }
            }
            
            listPages();
        }
        
        TiXmlElement *elayers = ewidgets->FirstChildElement("layers");
        if (elayers)
        {
            TiXmlElement *elayer = elayers->FirstChildElement("layer");
            if (elayer)
            {
                CLayer *layer = new CLayer(elayer);
                addLayer(layer);
                while(1)
                {
                    elayer = elayer->NextSiblingElement();
                    if (!elayer)
                        break;
                    layer = new CLayer(elayer);
                    addLayer(layer);
                }
            }
        }
        
        // go through the generic overlays - these are used by default when there are none in <page> nodes
        TiXmlElement *eoverlays = ewidgets->FirstChildElement("overlays");
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
    }
    
    TiXmlElement *emusiclist = melement->FirstChildElement("musiclist");
    if (emusiclist)
    {
        TiXmlElement *emusicinfo = emusiclist->FirstChildElement();
        if (emusicinfo)
        {
            CMusicInfo *cmi = new CMusicInfo(emusicinfo);
            addMusicInfo(cmi);
            while(1)
            {
                emusicinfo = emusicinfo->NextSiblingElement();
                if (!emusicinfo)
                    break;
                cmi = new CMusicInfo(emusicinfo);
                addMusicInfo(cmi);
            }
        }
    }
    
    if (melement->FirstChildElement("choices"))
    {
        mechoicedef = melement->FirstChildElement("choices");
    }
    
    // if scene has scopes parse them
    TiXmlElement *escopes = melement->FirstChildElement("scopes");
    if (escopes)
    {
        TiXmlElement *escope = escopes->FirstChildElement("scope");
        if (escope)
        {
            CScope *scope = new CScope(escope);
            scope->mparent = this;
            addChoice(scope);
            while(1)
            {
                escope = escope->NextSiblingElement();
                if (!escope)
                    break;
                scope = new CScope(escope);
                scope->mparent = this;
                addChoice(scope);
            }
        }
    }
    
    TiXmlElement *econtext = melement->FirstChildElement("context");
    if (econtext)
    {
        mcontext = new CContext(econtext);
    } else {
        econtext = melement->FirstChildElement("conditions");
        if (econtext) {
            mcontext = new CContext(econtext);
        }
    }
    
    TiXmlElement *echeckpoint = melement->FirstChildElement("checkpoint");
    if (echeckpoint)
    {
        mcheckpoint = new CCheckpoint(echeckpoint);
    }
    
    outputLogEditing((char*)"}\n");
    
    return 0;
}

// tokenize text pages

CPage *CScene::rewindPages( )
{
    mcurrentPage = NULL;
    //mcurrentPage = mpages;
    return mcurrentPage;
}

CPage *CScene::nextPage( )
{
    debug("CScene::next\n",NULL);
    if (!mcurrentPage)
    {
        debug("    first page\n",NULL);
        mcurrentPage = mpages;
    }
    else
    {
        debug("    next page\n",NULL);
        mcurrentPage = mcurrentPage->mnext;
    }
    return mcurrentPage;
}

int CScene::listPages( )
{
    debug("CScene::listPages\n",NULL);
    
    CPage *current = mpages;
    while(current)
    {
        current->debugPage();
        current = current->mnext;
    }
    
    return 0;
}

int CScene::addPage( CPage *page )
{
    CPage *last = NULL;
    CPage *current = mpages;
    while(current)
    {
        if (current->mnext==NULL)
            last = current;
        current = current->mnext;
    }
    
    if (last)
    {
        last->mnext = page;
        page->mprev = last;
    }
    else
        mpages = page;
    page->mnext = NULL;
    
    return 0;
}

int CScene::addChoice( CChoice *choice ) // CScope
{
    if (!choice)
        return 1;
    
    debug("CScene::addChoice\n",NULL);
    
    if (!mchoices)
    {
        choice->mprev = NULL;
        mchoices = choice;
    }
    else
    {
        CChoice *current = mchoices;
        while(current)
        {
            if (current->mnext==NULL)
            {
                current->mnext=choice;
                choice->mprev = current;
                break;
            }
            current = current->mnext;
        }
    }
    choice->mnext = NULL;
    
    return 0;
}

TiXmlElement *CScene::choiseElementAtIndex( int index ) // <choices><choice>
{
    debug("CScene::choiseElementAtIndex index(%d)\n",index);
    
    if (mechoicedef)
    {
        int n = 0;
        TiXmlElement *echoice = mechoicedef->FirstChildElement("choice");
        if (echoice)
        {
            if (n==index)
                return echoice;
            n++;
            while(1)
            {
                echoice = echoice->NextSiblingElement();
                if (!echoice)
                    break;
                if (n==index)
                    return echoice;
                n++;
            }
        }
    }
    
    return NULL;
}

int CScene::addMusicInfo( CMusicInfo *minfo )
{
    if (!minfo)
        return 1;
    
    debug("CScene::addMusicInfo\n",NULL);
    
    if (!mmusicinfo)
    {
        minfo->mprev = NULL;
        mmusicinfo = minfo;
    }
    else
    {
        CMusicInfo *current = minfo;
        while(current)
        {
            if (current->mnext==NULL)
            {
                current->mnext=minfo;
                minfo->mprev = current;
                break;
            }
            current = current->mnext;
        }
    }
    minfo->mnext = NULL;
    
    return 0;
}

TiXmlElement *CScene::getImageElement( )
{
    debug("CScene::getImageElement\n",NULL);
    TiXmlElement *ewidgets = melement->FirstChildElement("widgets");
    if (ewidgets)
    {
        TiXmlElement *eimage = ewidgets->FirstChildElement("image");
        if (eimage)
        {
            return eimage;
        }
    }
    else
    {
        debug("CScene::getImageElement warning - no widgets element found\n",NULL);
    }
    return NULL;
}

/**  add layer to the scene
 */
int CScene::addLayer( CLayer *layer )
{
    debug("CScene::addLayer\n",NULL);
    if (!layer)
        return 1;
    
    if (!mlayer)
    {
        layer->mprev = NULL;
        mlayer = layer;
    }
    else
    {
        CLayer *current = mlayer;
        while(current)
        {
            if (current->mnext==NULL)
            {
                current->mnext=layer;
                layer->mprev = current;
                break;
            }
            current = current->mnext;
        }
    }
    
    return 0;
}

int CScene::addOverlay( COverlay *overlay )
{
    debug("CScene::addOverlay\n",NULL);
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
    
    return 0;
}

/** rewind the pages
 */
int CScene::rewind( )
{
    debug("CScene::rewind\n",NULL);
    if (mtextarea)
        mtextarea->mcurrentPage = mtextarea->mpages;
    if (mpages)
        mcurrentPage = mpages;
    
    return 0;
}
