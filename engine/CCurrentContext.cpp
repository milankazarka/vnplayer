//
//  CCurrentContext.cpp
//  IAApp
//
//  Created by Milan Kazarka on 10/19/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#include "CCurrentContext.h"
#include "CCommon.h"

CCurrentContext *gcontext = NULL;

CCurrentContext::CCurrentContext( )
{
    debug("CCurrentContext::CCurrentContext\n",NULL);
    gcontext = this;
    mattributes = NULL;
}

CCurrentContext::~CCurrentContext( )
{
    debug("CCurrentContext::~CCurrentContext\n",NULL);
}

CCurrentContext *getGlobalContext( )
{
    if (!gcontext)
        new CCurrentContext( );
    return gcontext;
}

/**  we've arrived at a context - add & combine the attributes if nessesary
 */
int CCurrentContext::onContext( CContext *context )
{
    debug("CCurrentContext::onContext\n",NULL);
    if (!context)
        return 1;
    
    list();
    
    CAttr *current = context->mattributes;
    while(current)
    {
        onAttribute(current);
        current = current->mnext;
    }
    
    list();
    
    return 0;
}

int CCurrentContext::onAttribute( CAttr *attr )
{
    if (!attr)
        return 1;
    
    if (!attr->mkey || !attr->mvalue)
        return 1;
    
    debug("CCurrentContext::onAttribute key(%s) value(%s)\n",attr->mkey,attr->mvalue);
    
    CAttr *existing = getAttributeAtKey(attr->mkey);
    if (existing)
    {
        if (existing->mvalue)
        {
            free(existing->mvalue);
        }
        existing->mvalue = (char*)malloc(strlen(attr->mvalue)+1);
        strcpy(existing->mvalue,attr->mvalue);
    }
    else
    {
        CAttr *newAttr = new CAttr(attr->mkey,attr->mvalue);
        addAttribute(newAttr);
    }
    
    return 0;
}

CAttr *CCurrentContext::getAttributeAtKey( char *key )
{
    if (!key)
        return NULL;
    
    debug("CCurrentContext::getAttributeAtKey key(%s)\n",key);
    
    CAttr *current = mattributes;
    if (current) {
        while(current)
        {
            if (current->mkey)
            {
                debug("     attr key(%s) value(%s)\n",current->mkey,current->mvalue);
                if (strcmp(key,current->mkey)==0)
                {
                    return current;
                }
            }
            current = current->mnext;
        }
    } else {
        // this should not be NULL
        printf("CCurrentContext::getAttributeAtKey WARNING: mattributes not available\n");
    }
    
    return NULL;
}

/**
 flush all contextual attributes
 */
void CCurrentContext::flush( )
{
    debug("CCurrentContext::flush\n",NULL);
    
    CAttr *current = mattributes;
    CAttr *next = NULL;
    while(current)
    {
        next = current->mnext;
        delete current;
        current = next;
    }
    mattributes = NULL;
}

int CCurrentContext::addAttribute( CAttr *attr )
{
    debug("CCurrentContext::addAttribute\n",NULL);
    if (!attr)
        return 1;
    
    if (!mattributes)
    {
        attr->mprev = NULL;
        mattributes = attr;
    }
    else
    {
        CAttr *current = mattributes;
        while(current)
        {
            if (current->mnext==NULL)
            {
                current->mnext=attr;
                attr->mprev = current;
                break;
            }
            current = current->mnext;
        }
    }
    attr->mnext = NULL;
    
    return 0;
}

/**  list all current context attributes
 */
void CCurrentContext::list( )
{
    debug("CCurrentContext::list\n",NULL);
    CAttr *current = mattributes;
    while(current)
    {
        if (current->mkey && current->mvalue)
            debug("     context key(%s)\tvalue(%s)\n",current->mkey,current->mvalue);
        current = current->mnext;
    }
}

bool CCurrentContext::includesAttribute( CAttr *attr )
{
    if (!attr)
        return false;
    
    debug("CCurrentContext::includesAttribute\n",NULL);
    
    CAttr *global = getAttributeAtKey(attr->mkey);
    if (global)
    {
        if (global->mvalue && attr->mvalue)
        {
            if (strcmp(global->mvalue,attr->mvalue)==0)
                return true;
        }
    }
    
    return false;
}

bool CCurrentContext::includesContext( CContext *context )
{
    debug("CCurrentContext::includesContext\n",NULL);
    if (!context)
        return false;
    
    CAttr *current = context->mattributes;
    while(current)
    {
        if (!includesAttribute(current))
            return false;
        current = current->mnext;
    }
    
    return true;
}
