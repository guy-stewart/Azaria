/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       viewman.c
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 02/26/96  gas  Created.
; 08/24/96	gas  Hit testing on views no longer attempts to optimize by bailing
;				  if the hot point is still in the view.  This is to allow for
;				  layered views such as those found in the Azaria strategy rooms.
; 
----------------------------------------------------------------------------*/
#include "maya.h"
#include "textutil.h"
#include "paths.h"
#include "music.h"
#include "viewman.h"
#include "surround.h"
#include "compass.h"
#include "panel.h"
#include "scrollview.h"
#include "teletype.h"
#include "cfgview.h"
#include "inventory.h"
#include "gameman.h"
#include "display.h"
#include "player.h"
#include "cursor.h"
#include "user.h"
#include "trigger.h"
#include <string.h>

#include "Data/Database/idv.h"
#include "Data/Database/tbl_views.h"

extern BOOL bPostQuitMessage;

static int tbl_view_count = sizeof(tbl_views)/sizeof(VIEW_DESCRIPTION);

VIEW *ViewMan::c_focus = NULL; // the view that has the current focus.
const char *ViewMan::c_where = "";
WORD ViewMan::c_view_id = 0;

VIEW_DESCRIPTION *GetViewDesc(int view_id);

ViewMan viewman;

//----------------------------------------------------------------------------
ViewMan::ViewMan()
{
  m_tbl_views = tbl_views;
}

//----------------------------------------------------------------------------
void ViewMan::ReleaseClass(int class_id, WORD except_view_id)
{
	VIEW_DESCRIPTION *tbl_view;
	int position;

	if (!class_id)
	  return;

	tbl_view = GetFirstbyClass(class_id, &position);
	while (tbl_view)
	{
		// don't release a newly loaded view,
		if (tbl_view->view_id != except_view_id)
		{
		  if (tbl_view->view)
			  Release(tbl_view->view_id);
		}

		tbl_view = GetNextbyClass(class_id, &position);
	}
}

//----------------------------------------------------------------------------
// create new
VIEW *ViewMan::Load(int view_id)
{
  VIEW *view;
  VIEW_DESCRIPTION *tbl_view;

  if (view_id == 0)
	return NULL;

  tbl_view = GetViewDesc(view_id);

  if (!tbl_view)
	  return NULL;

  if (tbl_view->view)
  {
	Msg("%s was already loaded.", tbl_view->surface_filename);
	return tbl_view->view;
  }

  if ((tbl_view->view_type & VC_MASK) == VC_VIEWSCREEN)
  {
	if (holding)
	  return NULL;

	c_where = tbl_view->surface_filename;
	c_view_id = view_id;
  }

  Msg("Load %s", tbl_view->surface_filename);

  // All backbuffers need repainting:
  gameman.RestoreDisplay();

  switch (tbl_view->behavior_id)
  {
  case VB_DEFAULT:
	  view = new PANEL(view_id);
	  break;
  case VB_PANEL:
	  view = new PANEL(view_id);
	  break;
  case VB_SCROLLVIEW:
	  view = new SCROLLVIEW(view_id);
	  break;
  case VB_INVENTORY:
	  view = invLoad(view_id);
	  break;
  case VB_TELETYPE:
	  view = new TELETYPE(view_id);
	  break;
  case VB_SURROUND:
	  view = new SURROUND(view_id);
	  break;
  case VB_COMPASS:
	  view = new COMPASS(view_id);
	  break;
  case VB_CTRLVIEW:
	  view = new CTRL_VIEW(view_id);
	  break;
  case VB_CFGNET:
	  view = new CFGNET(view_id);
	  break;
  case VB_CFGPLYR:
	  view = new CFGPLYR(view_id);
	  break;
  case VB_CFGGAME:
	  view = new CFGGAME(view_id);
	  break;
  case VB_CFGAV:
	  view = new CFGAV(view_id);
	  break;
  default:
      view = NULL;
  }

  if (view)
  {
	ReleaseClass(tbl_view->view_type & VC_MASK, view_id);

	Music::SetTrack(tbl_view->backgroundAudio);

	tbl_view->view = view;
	SetFocus(view_id);

	// the player's globally known view_id the one in his main view screen:
	if (VC_VIEWSCREEN & (tbl_view->view_type & VC_MASK))
	{
	  local_player->SetView(view_id);
	}
  }

  return view;
}

//----------------------------------------------------------------------------
VIEW* ViewMan::Load(const char *filename, BOOL bSurround)
{
	int i;
	for (i=0;i<tbl_view_count;i++)
		if (!strcasecmp(filename, tbl_views[i].surface_filename))
			return Load(tbl_views[i].view_id);

	return NULL;
}

//----------------------------------------------------------------------------
BOOL ViewMan::Release(int view_id)
{
	VIEW_DESCRIPTION *tbl_view = GetViewDesc(view_id);

	// if no such view or the view is not active:
	if (!tbl_view || !tbl_view->view)
		return FALSE;

	tbl_view->pending_release = TRUE;

	return TRUE;
}

//----------------------------------------------------------------------------
void ViewMan::Release(VIEW_DESCRIPTION *tbl_view)
{
	VIEW *view;

	view = tbl_view->view;

	if (tbl_view->view == c_focus)
	  c_focus = NULL;

	tbl_view->view = NULL;
	tbl_view->pending_release = FALSE;

	if (!view)
	  return;

	delete view;
}

//----------------------------------------------------------------------------
void ViewMan::SetFocus(int view_id)
{
	if (!c_focus || (view_id != c_focus->GetViewID()))	{
	  if (c_focus)
		c_focus->LostFocus();
	  c_focus = GetView(view_id);
	}
}

//----------------------------------------------------------------------------
VIEW *ViewMan::GetFocus()
{
	return c_focus;
}

//----------------------------------------------------------------------------
const char *ViewMan::GetWhere()
{
	return c_where;
}

//----------------------------------------------------------------------------
WORD ViewMan::GetWhereId()
{
	return c_view_id;
}

//----------------------------------------------------------------------------
VIEW_DESCRIPTION *GetViewDesc(int view_id)
{
	int i;
	for (i=0;i<tbl_view_count;i++)
		if (tbl_views[i].view_id == view_id)
			return &(tbl_views[i]);

	return NULL;
}

//----------------------------------------------------------------------------
VIEW *ViewMan::GetView(int view_id)
{
	int i;
	for (i=0;i<tbl_view_count;i++)
		if (tbl_views[i].view_id == view_id)
			return tbl_views[i].view;

	return NULL;
}

//----------------------------------------------------------------------------
VIEW_DESCRIPTION  *ViewMan::GetFirstActive(int *position)
{
	int i;
	for (i=0;i<tbl_view_count;i++) {
		if (tbl_views[i].view) {
			if (position) *position = i;
			return &tbl_views[i];
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
VIEW_DESCRIPTION  *ViewMan::GetNextActive(int *position)
{
	int i;
	for (i=*position+1;i<tbl_view_count;i++)
	{
		if (tbl_views[i].view)
		{
			*position = i;
			return &tbl_views[i];
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
VIEW_DESCRIPTION *ViewMan::GetFirstbyClass(int view_type, int *position)
{
	int i;
	for (i=0;i<tbl_view_count;i++) {
		if (tbl_views[i].view_type & view_type) {
			*position = i;
			return &tbl_views[i];
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
VIEW_DESCRIPTION *ViewMan::GetNextbyClass(int view_type, int *position)
{
	int i;
	for (i=*position+1;i<tbl_view_count;i++)
	{
		if (tbl_views[i].view_type & view_type)
		{
			*position = i;
			return &tbl_views[i];
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
VIEW_DESCRIPTION *ViewMan::GetPrevbyClass(int view_type, int *position)
{
	int i;
	for (i=*position-1;i>0;i--)
	{
		if (tbl_views[i].view_type & view_type)
		{
			*position = i;
			return &tbl_views[i];
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
void ViewMan::InvRegion(int view_id)
{
	VIEW *view = GetView(view_id);
	if (view != NULL)
	{
	  view->InvRegion();
	}
}

//----------------------------------------------------------------------------
void ViewMan::AddPlayer(int view_id)
{
	VIEW_DESCRIPTION *view = GetViewDesc(view_id);
	if (view != NULL)
	{
		view->player_count++;
		if (TBL_CARDINALS::InView(c_view_id, view_id))
		  Invalidate(c_view_id, FALSE);
	}
}

//----------------------------------------------------------------------------
int ViewMan::GetPlayer(int view_id)
{
	VIEW_DESCRIPTION *view = GetViewDesc(view_id);
	if (view != NULL)
	{
		return view->player_count;
	}
	else
	  return 0;
}

//----------------------------------------------------------------------------
void ViewMan::DelPlayer(int view_id)
{
	VIEW_DESCRIPTION *view = GetViewDesc(view_id);
	if (view != NULL)
	{
		if (view->player_count > 0)
			view->player_count--;
		if (TBL_CARDINALS::InView(c_view_id, view_id))
		  Invalidate(c_view_id, FALSE);
	}
}

//----------------------------------------------------------------------------
void ViewMan::Invalidate(int view_id, BOOL spirit_moved, BOOL machine_changed /*=FALSE*/)
{
	VIEW *view = GetView(view_id);
	if (view != NULL)
	{
		if (machine_changed)
			view->InvMachines();
		else if (spirit_moved)
			view->InvSpirits();
		else
			view->Invalidate();
	}
	
	if (view_id == IDV_MAIN_PANEL)
	{
		int position;
		VIEW_DESCRIPTION *tbl_view;
		tbl_view = GetFirstActive(&position);
		while (tbl_view)
		{
			view = tbl_view->view;
			view->Invalidate();
  			tbl_view = GetNextActive(&position);
		}
	}
}

//----------------------------------------------------------------------------
VIEW *ViewMan::Hit(POINT p)
{
	VIEW *pview;
	VIEW_DESCRIPTION *tbl_view;
	int position;
	int Z=0;

	// Odds are, we are still in the focused view:
	pview = GetFocus();
//	if (pview && (pview->GetViewID() >= 100) && pview->PtInView(p))
	if (pview 
	  && (pview->GetViewID() >= 100)
	  && (FALSE)
	  && pview->PtInView(p))
	{
		return pview;
	}
	else
		pview = NULL;

	// search for the new view:
	tbl_view = GetFirstActive(&position);
	while (tbl_view && tbl_view->view)
	{
		if (tbl_view->view->PtInView(p))
		{
			if (!pview || (Z < tbl_view->Z))
			{
				pview = tbl_view->view;
				Z = tbl_view->Z;
			}
		}
		tbl_view = GetNextActive(&position);
	}

	return pview;
}

//----------------------------------------------------------------------------
void ViewMan::Tick(int etime)
{
	VIEW_DESCRIPTION *tbl_view;
	int frame;
	int flags;
	BOOL restore_backbuffer;
	VIEW *view;
	int position;

	frame = display.GetBackBufferId();
	flags = frame;

	// Peel the cursors off the display:
	cursorman.cursorOff(etime, flags);

	if (display.BackBufferNeedsPainting())
	{
		restore_backbuffer = TRUE;
		display.SetBackBufferPainted();
	}
	else
	  restore_backbuffer = FALSE;

	User();  // process pending user actions that affect display.

	tbl_view = GetFirstActive(&position);
	while (tbl_view)
	{
		view = tbl_view->view;
		view->Action(etime, flags);

		if (tbl_view->pending_release)
		{
		  Release(tbl_view);
		}
		else
		{
			view->VIEW::Action(etime, flags);
			if (tbl_view->pending_release)
			{
			  Release(tbl_view);
			}
			else if (view->NeedsPainting(frame) || restore_backbuffer)
			{
				view->Paint(etime, restore_backbuffer);
				view->BlitToDisplay();
				view->SetPainted(frame);
			}
		}

		tbl_view = GetNextActive(&position);
	}

	// Paint the cursors back onto the display:
	cursorman.cursorOn(etime, flags);
}

// FUNSTUFF
void ViewMan::SetHolding(BOOL holdme)
{
  holding = holdme;
}
