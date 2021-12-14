/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       textutil.cpp
;
; Description:
;	Implements the panel style puzzle.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 07/31/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "../include/stdapp.h"
#include "../include/one.h"
#include "../include/fblt.h"
#include "tables.h"
#include "spr_pool.h"
#include "spirit.h"

#include "textutil.h"

//----------------------------------------------------------------------------
CFONT::CFONT(int font_id)
{
  	// get a pointer to the font:
	hFont = sprSelect(font_id);
	spr = sprGet(hFont);
	fbd.SpriteInit(&spr->sprite);
};

//----------------------------------------------------------------------------
CFONT::~CFONT()
{
    sprRelease(hFont);
}

//----------------------------------------------------------------------------
// return width of text written
int CFONT::TextOut(ONE *surface, int color, int sx, int sy, const char *buff)
{
  fbd.m_dst_x = sx;
  fbd.m_dst_y = sy;

  return fbd.Print(surface, &spr->sprite, buff, color);
}

