//
//  CScript.cpp
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

#include "CScript.h"
#include <limits.h>
#include "CStats.h"
#include "CCommon.h"
#include "CCurrentContext.h"
#include "COutputLog.h"

CScript *gscript = NULL;

CScript::CScript( char *path )
{
    gscript = this;
    
    tmpid = (char*)malloc(1024);
    
    mpresetNext = NULL;
    mpresetPrev = NULL;
    
    mcurrentScene = NULL;
    mcurrentScope = NULL;
    if (path)
    {
        mdoc = new TiXmlDocument();
        if ( mdoc->LoadFile(path) )
        {
            debug("CScript::CScript file loaded\n",NULL);
            TiXmlElement *escript = mdoc->FirstChildElement("script");
            if (escript)
            {
                TiXmlElement *escope = escript->FirstChildElement("scope");
                if (escope)
                {
                    mscope = new CScope(escope);
                    //mscope->mparent = NULL;
                    mscope->mprev = NULL;
                    
                    if (!mcurrentScope)
                        mcurrentScope = mscope;
                }
            }
        }
        else
        {
            debug("CScript::CScript error, couldn't load script\n",NULL);
        }
    }
    
    CStats *cs = getGStats();
    if (cs)
        cs->printPageList();
    
    if (outputLogEditingBuffer)
    {
        printf("editing_output(%s)\n",outputLogEditingBuffer);
    }
}

CScope *CScript::getScopeAtId( char *scopeid )
{
    debug("CScript::getScopeAtId\n",NULL);
    
    if (!scopeid)
        return NULL;
    
    debug("    scopeid(%s)\n",scopeid);
    
    return getScopeAtIdRecursive(scopeid, NULL);
    /**
    if (mscope)
    {
        if (mscope->melement->Attribute("id")) // check superscope
        {
            if (strcmp(mscope->melement->Attribute("id"),scopeid)==0)
            {
                return mscope;
            }
        }
        
        if (mscope->mscenes)
        {
            CScene *current = mscope->mscenes;
            while(current)
            {
                CScope *cscope = (CScope*)current->mchoices;
                while(cscope)
                {
                    printf("    itterate scope\n");
                    if (cscope->melement->Attribute("id"))
                    {
                        printf("        compare current(%s) wanted(%s)\n",cscope->melement->Attribute("id"),scopeid);
                        if (strcmp(cscope->melement->Attribute("id"),scopeid)==0)
                        {
                            return cscope;
                        }
                    }
                    cscope = (CScope*)cscope->mnext;
                }
                
                current = current->mnext;
            }
        }
    }
     
    return NULL;
    */
}

/**
 recursively get the scope at ID, no matter the depth - pass in NULL if you want to go through the whole tree
 
 */
CScope *CScript::getScopeAtIdRecursive( char *scopeid, CScope *superscope )
{
    debug("CScript::getScopeAtIdRecursive\n",NULL);
    
    CScope *current = superscope;
    if (!superscope)
    {
        current = mscope;
    }
    
    while(current)
    {
        // work on current
        if (current->melement->Attribute("id")) // check superscope
        {
            debug("    check scope id(%s) searched(%s)\n",current->melement->Attribute("id"),scopeid);
            if (strlen(current->melement->Attribute("id"))==strlen(scopeid))
            {
                if (strcmp(current->melement->Attribute("id"),scopeid)==0)
                {
                    return current;
                }
            }
        }
        
        CScene *scene = current->mscenes;
        while(scene)
        {
            if (scene->mchoices)
            {
                CScope *sub = (CScope*)scene->mchoices;
                CScope *res = getScopeAtIdRecursive(scopeid, sub);
                if (res)
                    return res;
            }
            scene = scene->mnext;
        }
        
        current = (CScope*)current->mnext;
    }
    
    return NULL;
}

/**   get the current sceneid within "scopeid:sceneid:page" precission
 */
CScene *CScript::getSceneAtId( char *sceneid, bool rewind ) // todo - only works on one level
{
    debug("CScript::getSceneAtId\n",NULL);
    
    if (!sceneid)
        return NULL;
    
    debug("CScript::getSceneAtId sceneid(%s)\n",sceneid);
    
    // extract the scopeid
    int n;
    char *scopeid = NULL;
    for(n = 0; n < strlen(sceneid); n++)
    {
        if (strncmp(sceneid+n, ":", 1)==0 && strlen(sceneid)>n+1)
        {
            scopeid = (char*)malloc(n+1);
            strncpy(scopeid,sceneid,n);
            scopeid[n] = 0x00;
            debug("    scope(%s)\n",scopeid);
            break;
        }
    }
    if (!scopeid)
        return NULL;
    
    CScope *scope = getScopeAtId(scopeid);
    free(scopeid);
    if (scope)
    {
        debug("CScript::getSceneAtId   get scene(%s)\n",sceneid+n+1);
        CScene *scene = scope->getSceneAtId(sceneid+n+1);
        if (scene)
        {
            return scene;
        }
        else
        {
            // todo - fallback solution
        }
    }
    
    return NULL;
}

CScene *CScript::next( int nchoice )
{
    if (!mcurrentScope)
        return NULL;
    
    if (mpresetPrev)
        mpresetPrev = NULL;
    if (mpresetNext)
    {
        CScene *scene = mpresetNext;
        mpresetNext = NULL;
        mcurrentScene = scene;
        return scene;
    }
    
    if (nchoice>-1)
    {
        if (mcurrentScene->mchoices)
        {
            // we either chose a scope or to go to the first scene of this scope
            
            int n = 0;
            CChoice *currentChoice = mcurrentScene->mchoices;
            while(currentChoice)
            {
                if (n==nchoice)
                {
                    CScope *scope = (CScope*)currentChoice;
                    if (scope->mscenes)
                    {
                        mcurrentScope = scope;
                        mcurrentScene = scope->mscenes;
                        return mcurrentScene;
                    }
                }
                n++;
                currentChoice = currentChoice->mnext;
            }
            // go to the first scene of this scope
            CScene *scene = mcurrentScene;
            while (scene)
            {
                if (!scene->mprev)
                {
                    mcurrentScene = scene;
                    return scene;
                }
                scene = scene->mprev;
            }
        }
    }
    else
    {
        if (!mcurrentScene)
        {
            mcurrentScene = mcurrentScope->mscenes;
        }
        else
        {
            if (!mcurrentScene->mnext)
            {
                // end of scope or end of script - if end of scope, then go upper level & next
                return NULL;
            }
            else
            {
                mcurrentScene = mcurrentScene->mnext;
            }
        }
        
        if (mcurrentScene)
        {
            // we get the next scene if this one is virtual
            if (mcurrentScene->mvirtual==true)
            {
                return next(nchoice);
            }
        }
    }
    
    return mcurrentScene;
}

/**     rewind script to general scope
 */
CScene *CScript::rewind( )
{
    debug("CScript::rewind\n",NULL);
    
    mcurrentScope = mscope;
    mcurrentScene = mcurrentScope->mscenes;
    
    return mcurrentScene;
}

/**     rewind current scope, not the entire script
 */
CScene *CScript::rewindCurrentScope( )
{
    debug("CScript::rewindCurrentScope\n",NULL);
    
    mcurrentScene = mcurrentScope->rewind();
    
    return mcurrentScene;
}

int CScript::onScene( CScene *scene )
{
    debug("CScript::onScene\n",NULL);
    if (!scene)
        return 1;
    
    if (scene->mcontext)
    {
        CCurrentContext *context = getGlobalContext();
        if (context)
        {
            context->onContext(scene->mcontext);
        }
    }
    
    return 0;
}

int CScript::setScene( CScene *scene, bool rewind )
{
    debug("CScript::setScene\n",NULL);
    
    if (!scene)
        return 1;
    
    mcurrentScope = (CScope*)scene->mparent;
    mcurrentScene = scene;
    
    if (rewind)
    {
        scene->rewind();
    }
    
    onScene(scene);
    
    return 0;
}

CScene *CScript::setScope( CScope *scope, bool rewind )
{
    debug("CScript::setScope\n",NULL);
    
    if (!scope)
        return NULL;
    
    mcurrentScope = scope;
    CScene *scene = NULL;
    
    if (rewind)
    {
        scene = rewindCurrentScope();
    }
    else
    {
        // todo - add possibility to save scene position in scope
    }
    
    return scene;
}

// how many places there are in a positive number
int numPlaces (int n)
{
    if (n < 10) return 1;
    return 1 + numPlaces (n / 10);
}

/**  get the ID of the current scene with page position - free() returned value
 */
/**
char *CScript::getCurrentSceneId( )
{
    printf("CScript::getCurrentSceneId\n");
    if (!mcurrentScene)
        return NULL;
    
    if (mcurrentScene->mtextarea)
    {
        if (mcurrentScene->melement->Attribute("id"))
        {
            int n = 0;
            CPage *cpage = mcurrentScene->mtextarea->mpages;
            while(cpage)
            {
                if (cpage==mcurrentScene->mtextarea->mcurrentPage)
                    break;
                n++;
                cpage = cpage->mnext;
            }
            
            char *currentid = (char*)malloc(
                                            strlen(mcurrentScene->melement->Attribute("id"))+
                                            numPlaces(n)+1
                                            );
            sprintf(currentid,"%s:%d",mcurrentScene->melement->Attribute("id"),n);
            return currentid;
        }
    }
    return NULL;
}
*/
 
/**  get the ID of the current scope, scene with page position - free() returned value
 */
char *CScript::getCurrentMultilevelSceneId( )
{
    debug("CScript::getCurrentMultilevelSceneId\n",NULL);
    if (
        !mcurrentScene || !mcurrentScope
        )
        return NULL;
    
    CScene *currentScene = NULL;
    CScope *currentScope = NULL;
    
    if (mpresetPrev)
    {
        currentScope = (CScope*)mpresetPrev->mparent;
        currentScene = mpresetPrev;
    }
    else
    {
        currentScene = mcurrentScene;
        currentScope = mcurrentScope;
    }
    
    if (!currentScene || !currentScope)
        return NULL;
    
    if (currentScope->melement->Attribute("id"))
    {
        if (currentScene->melement->Attribute("id"))
        {
            int n = 0;
            if (currentScene->mtextarea)
            {
                if (currentScene->mtextarea->mpages)
                {
                    CPage *cpage = currentScene->mtextarea->mpages;
                    while(cpage)
                    {
                        if (cpage==currentScene->mtextarea->mcurrentPage)
                            break;
                        n++;
                        cpage = cpage->mnext;
                    }
                }
            }
            
            if (
                (strlen(currentScope->melement->Attribute("id"))+
                strlen(currentScene->melement->Attribute("id"))+
                numPlaces(n)+1) > 1024
                )
            {
                return NULL;
            }
            
            //char *currentid = (char*)malloc(
            //                                strlen(mcurrentScope->melement->Attribute("id"))+
            //                                strlen(mcurrentScene->melement->Attribute("id"))+
            //                                numPlaces(n)+1
            //                                );
            sprintf((char*)tmpid,"%s:%s:%d",currentScope->melement->Attribute("id"),
                                        currentScene->melement->Attribute("id"),n);
            char *ptr = (char*)tmpid;
            return ptr;
        }
    }
    
    return NULL;
}
