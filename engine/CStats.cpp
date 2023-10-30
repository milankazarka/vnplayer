//
//  CStats.cpp
//  IAApp
//
//  Created by Milan Kazarka on 5/19/14.
//  Copyright (c) 2014 Milan Kazarka. All rights reserved.
//

/* Copyright (C) Milan Kazarka - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Milan Kazarka <milan.kazarka.office@gmail.com>, November 2014
 */

#include "CStats.h"
#include "CCommon.h"
#include <cctype>

CStats *gstats = NULL;

#include <cctype>

int CountWords(const char* str)
{
    if (str == NULL)
        return 0;  // let the requirements define this...
    
    bool inSpaces = true;
    int numWords = 0;
    
    while (*str != 0x00)
    {
        if (std::isspace(*str))
        {
            inSpaces = true;
        }
        else if (inSpaces)
        {
            numWords++;
            inSpaces = false;
        }
        
        ++str;
    }
    
    return numWords;
}

CStats::CStats( )
{
    gstats = this;
    nwords = 0;
    pagelist = NULL;
}

CStats::~CStats( )
{
    
}

CStats *getGStats( )
{
    if (!gstats)
        new CStats();
    return gstats;
}

int CStats::parsePage( TiXmlElement *epage )
{
    if (!epage)
        return 0;
    
    if (epage->Attribute("text"))
    {
        int num = CountWords(epage->Attribute("text"));
        nwords += num;

        debug("CStats::parsePage words(%d)\n",nwords);

        addPageText(epage->Attribute("character"),epage->Attribute("text"));
    }
    
    return 0;
}

static const char *none = (const char*)"";

int CStats::addPageText( const char *character, const char *txtptr )
{
    if (!txtptr)
        return 1;
    
    struct PAGETEXT *pt = (struct PAGETEXT*)malloc(sizeof(struct PAGETEXT));
    pt->next = NULL;
    if (character)
        pt->characterptr = character;
    else
        pt->characterptr = none;
    
    pt->txtptr = txtptr;
    
    if (!pagelist)
    {
        pagelist = pt;
    }
    else
    {
        PAGETEXT *current = pagelist;
        while(current)
        {
            if (!current->next)
            {
                current->next = pt;
                break;
            }
            current = current->next;
        }
    }
    
    return 0;
}

int CStats::printPageList( )
{
    debug("CStats::printPageList\n",NULL);
    PAGETEXT *current = pagelist;
    while(current)
    {
        if (strlen(current->characterptr)>0)
        {
            debug("%s: %s\n",current->characterptr,current->txtptr);
        }
        else
        {
            debug("%s\n",current->txtptr);
        }
        current = current->next;
    }
    
    return 0;
}
