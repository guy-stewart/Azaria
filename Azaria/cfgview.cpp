/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       cfgview.cpp
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

#include <direct.h>

#include "maya.h"
#include "controls.h"
#include "viewMast.h"
#include "cfgview.h"

#include "data\database\ids.h"

char CFGVIEW::names[NAMES_PER_PAGE][MAX_NAME_LEN];

//----------------------------------------------------------------------------
CFGVIEW::CFGVIEW(int view_id):
PANEL(view_id)
//VIEW(view_id, "cfgview", BLT_PORTAL)
{
	control[0] = NULL;
	control_count = 0;
	focus = -1;
}

//----------------------------------------------------------------------------
CFGVIEW::~CFGVIEW()
{
  for (int i=0;i<control_count;i++)
	  delete control[i];
}

//----------------------------------------------------------------------------
void CFGVIEW::OnEvent(int control_id, int event_id)
{
}

//----------------------------------------------------------------------------
WORD CFGVIEW::Grab(POINT p, BOOL bClickOnly, POINT *obj_pos)
{

  POINT vp = MapToCanvas(p);

  for (int i=0;i<control_count;i++)
  {
	if (control[i] && control[i]->PtInRect(vp))
	{
  		control[i]->Grab(vp);
		return 0;
	}
  }

  PANEL::Grab(p, bClickOnly, obj_pos);
  return 0;
}

//----------------------------------------------------------------------------
int CFGVIEW::Keycmd(int key, int flags)
{
  // First, try the control that has the focus ...
  if (focus >= 0)
	  key = control[focus]->Keycmd(key, flags);

  for (int i=0;key && i<control_count;i++)
	if (control[i])
		key = control[i]->Keycmd(key, flags);

  if (key)
	return PANEL::Keycmd(key, flags);
  else
	return 0;
}

//----------------------------------------------------------------------------
WORD CFGVIEW::QueryCursor(SRECT r)
{
  POINT p;
  p.x = r.left;
  p.y = r.top;

  p = MapToCanvas(p);
  for (int i=0;i<control_count;i++)
  {
	if (control[i] && control[i]->PtInRect(p))
	{
	  if (i!= focus)
	  {
		if (focus>=0) control[focus]->SetFocus(p, FALSE);
		focus = i;
		control[focus]->SetFocus(p, TRUE);
	  }
	  return IDS_HANDMACH;
	}
  }

  // no control has focus.
  if (focus>=0)
  {
	control[focus]->SetFocus(p, FALSE);
	focus = -1;
  }

  return PANEL::QueryCursor(r);
}

//----------------------------------------------------------------------------
int CFGVIEW::CfgDir(int startat, char *dir, char *ext)
{
  int current = 0;
  int index = 0;
  struct _finddata_t fileinfo;
  long handle;
  char path[_MAX_PATH];

  _makepath(path, NULL, dir, "*", ext);

  _mkdir(dir);
  handle = _findfirst(path, &fileinfo);

  if (handle != -1L)
  {
	do
	{
	  current++;
	  if (current >= startat)
	  {
		char fname[_MAX_FNAME];
		strncpy(fname,fileinfo.name, _MAX_FNAME);
		fname[_MAX_FNAME-1] = 0;
		_splitpath(fname, NULL, NULL, fname, NULL);

		strncpy(names[index],fname, MAX_NAME_LEN);
		names[index][MAX_NAME_LEN-1] = 0;
		index++;
		if (index >= NAMES_PER_PAGE)
		  break;
	  }
	} while( _findnext( handle, &fileinfo ) == 0 );
	_findclose(handle);
  }

  return index;
}

//----------------------------------------------------------------------------
char *CFGVIEW::CfgStrings(int index)
{
  return names[index];
}

