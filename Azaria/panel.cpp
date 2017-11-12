/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       panel.c
;
; Description:
;	Implements the panel style puzzle.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/28/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "maya.h"
#include "panel.h"

#include "data\database\ids.h"

#include "user.h"

BOOL PanelNav(int view_id, BOOL bBackup, BOOL query);

PANEL::~PANEL()
{
}

PANEL::PANEL(int view_id):
VIEW(view_id, "panel", BLT_PORTAL)
{
}

//----------------------------------------------------------------------------
int PANEL::Keycmd(int key, int flags)
{
	if (flags & KEY_PRESSED)
	  switch (key)
	  {
		case KEY_UP:
  			PanelNav(view_id, FALSE, TRUE);
			key = 0;
			break;
		case KEY_DOWN:
  			PanelNav(view_id, TRUE, TRUE);
			key = 0;
			break;
	  }

  return key;
}

//----------------------------------------------------------------------------
WORD PANEL::QueryCursor(SRECT p)
{
  WORD vids = VIEW::QueryCursor(p);
  WORD ids = 0;
  int thickness_y = (fbd.m_brect.bottom - fbd.m_brect.top) / 2;

  if (vids != IDS_HANDIDLE)
	return vids;

  if (p.top < (fbd.m_brect.top + thickness_y))
  {
    if (PanelNav(view_id, FALSE, FALSE))
		return IDS_HANDFWD;
  }
  else if (p.bottom > (fbd.m_brect.bottom - thickness_y))
  {
    if (PanelNav(view_id, TRUE, FALSE))
		return IDS_HANDBACK;
  }

  return vids;
}

