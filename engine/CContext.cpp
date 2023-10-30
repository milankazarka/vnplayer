//
//  CContext.cpp
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

#include "CContext.h"
#include "CCommon.h"

CContext::CContext( TiXmlElement *element )
{
    melement = element;
    mattributes = NULL;
    
    TiXmlElement *eattr = melement->FirstChildElement("attr");
    if (eattr)
    {
        CAttr *attr = new CAttr(eattr);
        addAttribute(attr);
        
        while(1)
        {
            eattr = eattr->NextSiblingElement();
            if (!eattr)
                break;
            
            attr = new CAttr(eattr);
            addAttribute(attr);
        }
    }
    
    list();
}

int CContext::addAttribute( CAttr *attr )
{
    debug("CContext::addAttribute\n",NULL);
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

void CContext::list( )
{
    debug("CContext::list\n",NULL);
    
    CAttr *current = mattributes;
    while(current)
    {
        if (current->mkey && current->mvalue)
        {
            debug("     key(%s)\tvalue(%s)\n",current->mkey,current->mvalue);
        }
        current = current->mnext;
    }
}
