//
//  CTextarea.cpp
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

#include "CTextarea.h"

CTextarea::CTextarea( TiXmlElement *etextarea )
{
    debug("CTextarea::CTextarea\n",NULL);
    
    metextarea = etextarea;
    mpages = NULL;
    mcurrentPage = NULL;
    
    frame = initSFrame();
    if (
        metextarea->Attribute("posx") &&
        metextarea->Attribute("posy") &&
        metextarea->Attribute("width") &&
        metextarea->Attribute("height")
        )
    {
        frame.x = extractRelativePositionString((char*)metextarea->Attribute("posx"));
        frame.y = extractRelativePositionString((char*)metextarea->Attribute("posy"));
        frame.width = extractRelativePositionString((char*)metextarea->Attribute("width"));
        frame.height = extractRelativePositionString((char*)metextarea->Attribute("height"));
    }
    
    TiXmlElement *epages = metextarea->FirstChildElement("pages");
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
    }
    listPages();
}

int CTextarea::addPage( CPage *page )
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

CTextarea::~CTextarea( )
{
    
}

int CTextarea::listPages( )
{
    debug("CTextarea::listPages\n",NULL);
    
    CPage *current = mpages;
    while(current)
    {
        current->debugPage();
        current = current->mnext;
    }
    
    return 0;
}

// tokenize text pages

CPage *CTextarea::rewind( )
{
    mcurrentPage = NULL;
    //mcurrentPage = mpages;
    return mcurrentPage;
}

CPage *CTextarea::next( )
{
    debug("CTextarea::next\n",NULL);
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
