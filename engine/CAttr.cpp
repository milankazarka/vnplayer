//
//  CAttr.cpp
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

#include "CAttr.h"
#include "CCommon.h"

CAttr::CAttr( TiXmlElement *element )
{
    debug("CAttr::CAttr element\n",NULL);
    
    melement = element;
    mprev = NULL;
    mnext = NULL;
    mkey = NULL;
    mvalue = NULL;
    
    if (melement->Attribute("key") && melement->Attribute("value"))
    {
        mkey = (char*)malloc(strlen(melement->Attribute("key"))+1);
        strcpy(mkey,melement->Attribute("key"));
    
        mvalue = (char*)malloc(strlen(melement->Attribute("value"))+1);
        strcpy(mvalue,melement->Attribute("value"));
    }
}

CAttr::CAttr( char *key, char *value )
{
    debug("CAttr::CAttr key, value\n",NULL);
    melement = NULL;
    mkey = NULL;
    mvalue = NULL;
    
    mprev = NULL;
    mnext = NULL;
    
    if (key && value)
    {
        mkey = (char*)malloc(strlen(key)+1);
        strcpy(mkey,key);
        
        mvalue = (char*)malloc(strlen(value)+1);
        strcpy(mvalue,value);
    }
}

CAttr::~CAttr()
{
    debug("CAttr::~CAttr\n",NULL);
    if (mkey)
        free(mkey);
    if (mvalue)
        free(mvalue);
}
