//
//  CScope.cpp
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

#include "CScope.h"
#include "CCommon.h"

CScope::CScope( TiXmlElement *element )
{
    debug("CScope::CScope\n",NULL);

    melement = NULL;
    mtextarea = NULL;
    mprev = NULL;
    mnext = NULL;
    mcurrentScene = NULL;
    mscenes = NULL;
    if (element)
    {
        loadElement(element);
    }
}

int CScope::loadElement( TiXmlElement *element )
{
    melement = element;
    parse();
    return 0;
}

CScope::~CScope( )
{
    melement = NULL;
    mtextarea = NULL;
    mprev = NULL;
    mnext = NULL;
    mcurrentScene = NULL;
    mscenes = NULL;
}

int CScope::parse( )
{
    if (melement)
    {
        TiXmlElement *etextarea = melement->FirstChildElement("textarea");
        if (etextarea)
        {
            mtextarea = new CTextarea(etextarea);
        }
        
        TiXmlElement *escenes = melement->FirstChildElement("scenes");
        if (escenes)
        {
            TiXmlElement *escene = escenes->FirstChildElement("scene");
            if (escene)
            {
                parseSceneElement(escene);
                while(1)
                {
                    escene = escene->NextSiblingElement();
                    if (!escene)
                        break;
                    parseSceneElement(escene);
                }
            }
        }
    }
    return 0;
}

/** parse <scene> element - add it to the hierarchy of child objects
 */
int CScope::parseSceneElement( TiXmlElement *escene )
{
    if (!escene)
        return 1;
    
    CScene *scene = new CScene(escene);
    scene->mparent = this;
    
    if (!mscenes)
    {
        scene->mprev = NULL;
        mscenes = scene;
    }
    else
    {
        CScene *current = mscenes;
        while(current)
        {
            if (current->mnext==NULL)
            {
                current->mnext=scene;
                scene->mprev = current;
                break;
            }
            current = current->mnext;
        }
    }
    scene->mnext = NULL;
     
    return 0;
}

/**
 load this scene
 */
int CScope::loadScene( CScene *scene )
{
    debug("CScope::loadScene\n",NULL);

    if (!scene)
        return 1;
    return 0;
}

CScene *CScope::getSceneAtId( char *sceneid, bool rewind )
{
    debug("CScope::getSceneAtId\n",NULL);

    if (!sceneid)
        return NULL;
    
    if (mscenes)
    {
        debug("    get sceneid(%s)\n",sceneid);

        char *sceneidfirst = sceneid;
        int pagenum = 0; // we jump to page n if needed
        int n;
        for(n = 0; n < strlen(sceneid); n++)
        {
            if (strncmp(sceneid+n, ":", 1)==0 && strlen(sceneid)>n+1)
            {
                sceneidfirst = (char*)malloc(n+1);
                strncpy(sceneidfirst,sceneid,n);
                sceneidfirst[n] = 0x00;
                
                char *pageid = sceneid+n+1;
                debug("    pageid(%s)\n",pageid);

                pagenum = atoi(pageid);
            }
        }
        
        CScene *current = mscenes;
        while(current)
        {
            if (current->melement)
            {
                if (current->melement->Attribute("id"))
                {
                    if (strcmp(current->melement->Attribute("id"),sceneidfirst)==0)
                    {
                        if (sceneidfirst && sceneidfirst!=sceneid)
                        {
                            if (current->mtextarea)
                            {
                                if (rewind==false && pagenum!=0) // too many ifs
                                {
                                    n = 0;
                                    CPage *cpage = current->mtextarea->mpages;
                                    while(cpage)
                                    {
                                        if (n==pagenum)
                                        {
                                            current->mtextarea->mcurrentPage = cpage;
                                            break;
                                        }
                                        n++;
                                        cpage = cpage->mnext;
                                    }
                                }
                                else
                                    current->mtextarea->mcurrentPage = current->mtextarea->mpages;
                            }
                            free(sceneidfirst);
                        }
                        else
                            if (current->mtextarea)
                                current->mtextarea->mcurrentPage = current->mtextarea->mpages;
                        
                        return current;
                    }
                }
            }
            current = current->mnext;
        }
    }
    
    return NULL;
}

/**  rewind the scope and it's first scene and return it
 */
CScene *CScope::rewind( )
{
    debug("CScope::rewind\n",NULL);
    mcurrentScene = mscenes;
    if (mcurrentScene)
        mcurrentScene->rewind();
    else
    {
        debug("CScope::rewind warning, no scene to rewind\n",NULL);
    }
    
    return mcurrentScene;
}
