/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       compass.cpp
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 05/31/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#include "maya.h"
#include "object.h"
#include "compass.h"
#include "player.h"

#include "Data/Database/Ids.h"

COMPASS::COMPASS(WORD view_id):
VIEW(view_id, "compass", BLT_PORTAL)
{
}

COMPASS::~COMPASS()
{
}

//----------------------------------------------------------------------------
WORD COMPASS::QueryCursor(SRECT p)
{
  return IDS_HANDIDLE;
}

//----------------------------------------------------------------------------
int COMPASS::Action(int etime, int flags)
{
  static int x = 0;

  // Get the offset of the coord by degrees
  x = (local_player->GetTheta()*surface.GetWidth())/36000;

  // center the offset in the viewport:
  x -= viewWidth() / 2;
  if (x<0) x += surface.GetWidth();

  if (x != fbd.m_src_x)
  {
	fbd.m_src_x = x;
	Invalidate();
  }

  return TRUE;
}
