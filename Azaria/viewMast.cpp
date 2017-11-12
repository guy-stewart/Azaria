/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       viewMast.c
;
; Description:
;	ViewMaster controls access to the backbuffer and notifies (calls) active
;	views when it is time for them to update the display.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/
#include "maya.h"

#include "user.h"
#include "gdb.h"
#include "viewman.h"
#include "machterp.h"
#include "paths.h"
#include "display.h"
#include "player.h"
#include "trigger.h"

#include "Data/Database/Ids.h"

#define DEBUG_VIEW FALSE

#if DEBUG_VIEW
	static ONE oneDebug(640,480);
#else
	static ONE oneDebug;	// Seems the CInit code doesn't relink correctly
					// it tries to init non-existent variable!
#endif

//----------------------------------------------------------------------------
void VIEW::Paint(int etime, int bForceRefresh)
{
	// Here we need to test for dirty rectangles before blitting ...
	// how about keeping a list of dirty rectangles?
	// Or better yet,  we immediatly blt sprite rectangles to the display?
  if (bForceRefresh)
	surface.SetAllDirty();

  // look for sprites that have moved or changed...
  if (spr_changed)
  {
	  if (m_machines_changed)
	  {
		  gdbt_machines.Paint(&surface, view_id);
		  m_machines_changed = FALSE;
	  }
	  spr_changed = FALSE;
  }
}

//----------------------------------------------------------------------------
void VIEW::BlitToDisplay()
{
  viewBlt(&surface, &fbd);
}

//----------------------------------------------------------------------------
void VIEW::SetPainted(int frame) 
{
  updatedFrames |= MSK_FRAME(frame);
  if (updatedFrames == display.GetFrameMask())
	surface.SetClean();
}

//----------------------------------------------------------------------------
int  VIEW::Action(int etime, int flags)
{
	return FALSE;
}

//----------------------------------------------------------------------------
BOOL VIEW::PtInView(POINT p) 
{
  RECT testRect = fbd.m_brect;

  // a point on right or bottom edge is considered outside the region
  // (see PtInRect docs).  This needed to be done so that the surround
  // views would continue to pan even if the mouse was to the most extreme
  // right edge of the display,  esp when running full screen.
  testRect.right++;

  if (sPtInRect(&testRect, p))
  {
	// flip the point if it is a surround view.
	if (fbd.m_style == BLT_LENSED)
	{
	  int x = p.x;
	  p.x = p.y;
	  p.y = x;
	}

	if (maskPtInRect(fbd.m_mask, p))
	  return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
POINT VIEW::MapToScreen(POINT p)
{
	// translate the point from view coordinates to screen coordinates:
	p.x -= fbd.m_src_x - fbd.m_brect.left;
	if (p.x < 0) 
		p.x += GetCanvasWidth();
	p.y -= fbd.m_src_y - fbd.m_brect.top;
	if (p.y < 0) 
		p.y += GetCanvasHeight();

	return p;
}

//----------------------------------------------------------------------------
POINT VIEW::MapToCanvas(POINT p)
{
	// translate the point from screen coordinates to view coordinates:
	p.x += fbd.m_src_x - fbd.m_brect.left;
	p.y += fbd.m_src_y - fbd.m_brect.top;

	// unwrap the original point.
	switch (fbd.m_style)
	{
	case BLT_LENSED:
		if (p.x >= surface.GetHeight())
			p.x -= surface.GetHeight();
		break;
	default:
		if (p.x >= surface.GetWidth())
			p.x -= surface.GetWidth();
		break;
	}

	return p;
}

//----------------------------------------------------------------------------
void VIEW::InvalidateOffset()
{
	if (isMainViewScreen())
	{
		// if main viewscreen then update remote players in our view:
		POINT p;
		p.x = fbd.m_src_x ;
		p.y = fbd.m_src_y ;
		local_player->SetOffset(p);
	}

	Invalidate(); // signal repaint canvas to display
}

//----------------------------------------------------------------------------
WORD VIEW::Grab(POINT p, BOOL bClickOnly, POINT *obj_pos)
{
	WORD obj_def_id;
	POINT wp;

	p = MapToCanvas(p);

	// this point will detect sprites that have wrapped!
	wp = p;
	if (fbd.m_style == BLT_LENSED)
		wp.x += surface.GetHeight();
	else
		wp.x += surface.GetWidth();

	// First,  see if there is a puzzle at this point:
	obj_def_id = gdbt_machines.Grab(view_id, p, bClickOnly, obj_pos);

	*obj_pos = MapToScreen(*obj_pos);

	return obj_def_id;
}

//----------------------------------------------------------------------------
BOOL VIEW::Drop(WORD *obj_def_id, POINT p)
{
	return gdbt_machines.Drop(view_id, obj_def_id, MapToCanvas(p));
}

//----------------------------------------------------------------------------
WORD VIEW::Drag(WORD obj_def_id, POINT p)
{
	return gdbt_machines.Drag(view_id, obj_def_id, MapToCanvas(p));
}

//----------------------------------------------------------------------------
int VIEW::Msg(int key, int flags)
{
  return 0;
}

//----------------------------------------------------------------------------
void VIEW::LostFocus()
{
	Keycmd(KEY_CLEAR, TRUE);
}

//----------------------------------------------------------------------------
int VIEW::Keycmd(int key, int flags)
{
  return key;
}

//----------------------------------------------------------------------------
WORD VIEW::QueryCursor(SRECT r)
{
  POINT p;

  p.x = r.left;
  p.y = r.top;

  p = MapToCanvas(p);

  WORD ids = gdbt_machines.QueryCursor(view_id, 0, p);

  if (!ids)
  {
	ids = trgQueryCursor(view_id, p);

	if (!ids)
  	  ids = IDS_HANDIDLE;
  }

  return ids;
}

//----------------------------------------------------------------------------
void VIEW::Navigate(POINT p, WORD ids)
{
	switch (ids)
	{
	case IDS_HANDTRIG:
	  trgFire(view_id, MapToCanvas(p));
	  break;
	case IDS_HANDALT:
	  gdbt_machines.Grab(view_id, MapToCanvas(p), TRUE, 0);
	  break;
	case IDS_HANDLEFT:
	  Keycmd(KEY_LEFT, KEY_PRESSED);
	  break;
	case IDS_HANDRIGHT:
	  Keycmd(KEY_RIGHT, KEY_PRESSED);
	  break;
	case IDS_HANDFWD:
	case IDS_HANDUP:
	  Keycmd(KEY_UP, KEY_PRESSED);
	  break;
	case IDS_HANDBACK:
	case IDS_HANDDOWN:
	  Keycmd(KEY_DOWN, KEY_PRESSED);
	  break;
	}
}

//----------------------------------------------------------------------------
// removes a view from the active view table
// release all allocated structures associated with a RAM view.
VIEW::~VIEW()
{
  // delete any video clips playing on this view:
  	SPIRIT::VideoCut(view_id);

	switch (fbd.m_style) {
	case BLT_LENSED:
	case BLT_PORTAL:
		maskFree(&fbd.m_mask);
		break;
	}

	gdbt_machines.Release(view_id);

	surface.freeBits();
}

//----------------------------------------------------------------------------
BOOL VIEW::ChangeSurface(char *newSurfaceName)
{
	surface.freeBits();
	return surface.Read(ViewsPath, newSurfaceName);
}

//----------------------------------------------------------------------------
void VIEW::Move(int x_delta, int y_delta)
{
  maskMove(fbd.m_mask, x_delta, y_delta);
  maskGetBoundingRect(fbd.m_mask, &fbd.m_brect);

	switch (fbd.m_style)
	{
	case BLT_LENSED:
		// The rectangle is rotated && flipped back to screen coordinates
		// so that hit detection will still work:
		SetRect(&fbd.m_brect,
		  fbd.m_brect.top,fbd.m_brect.left,fbd.m_brect.bottom,fbd.m_brect.right);
		break;
	}

}

//----------------------------------------------------------------------------
VIEW::VIEW(int _view_id, const char *_name, int _style)
{
	VIEW_DESCRIPTION *p_tbl_view;

	strcpy(name, _name);
	view_id = _view_id;

	p_tbl_view = GetViewDesc(view_id);
	if (!p_tbl_view)
		return; // FALSE

	view_type = p_tbl_view->view_type;

	if (!surface.Read(AltViewsPath, p_tbl_view->surface_filename)
	  &&!surface.Read(ViewsPath, p_tbl_view->surface_filename))
	{
		strncpy(missing_file_name, p_tbl_view->surface_filename, _MAX_FNAME);
		strncpy(missing_file_path1, ViewsPath, _MAX_PATH);
		strncpy(missing_file_path2, AltViewsPath, _MAX_PATH);
		throw(ERROR_FILE_NOT_FOUND);
	}

	// if its compressed then uncompress it:
	if (surface.IsCompressed())
	  surface.UnCompress();

	if (surface.GetPalette() != NULL)
	{
		display.SetPalette(surface.GetPalette(), FALSE);
	}

	memset(&fbd, 0, sizeof(FBLT_DSCR));
	fbd.m_style = _style;
	fbd.setView(); // let blitters recognize this as a view.

	//---------------------------------------------------------------------
	// load the mask for this view:
	if (!maskRead(AltMasksPath, p_tbl_view->portal_filename, &fbd.m_mask)
	  &&!maskRead(MasksPath, p_tbl_view->portal_filename, &fbd.m_mask))
	{
		strncpy(missing_file_name, p_tbl_view->portal_filename, _MAX_FNAME);
		strncpy(missing_file_path1, MasksPath, _MAX_PATH);
		strncpy(missing_file_path2, AltMasksPath, _MAX_PATH);
		throw(ERROR_FILE_NOT_FOUND);
	}

	maskGetBoundingRect(fbd.m_mask, &fbd.m_brect);

	switch (fbd.m_style)
	{
	case BLT_LENSED:
		surface.m_hdr.user_flags = 0x0001;

		// set the bounding rectangle around the canvas:
		// The rectangle is rotated && flipped back to screen coordinates
		// so that hit detection will still work:
		SetRect(&srect, 0,0, surface.GetHeight(), surface.GetWidth());

		// The rectangle is rotated && flipped back to screen coordinates
		// so that hit detection will still work:
		SetRect(&fbd.m_brect,
		  fbd.m_brect.top,fbd.m_brect.left,fbd.m_brect.bottom,fbd.m_brect.right);

		break;
	case BLT_PORTAL:
		surface.m_hdr.user_flags = 0x0002;

		// set the bounding rectangle around the canvas:
		SetRect(&srect, 0,0, surface.GetWidth(), surface.GetHeight());
		break;
	}

	// Load the view_objects relationship rows for this view
	// (select * from tbl_view_obj where view_id = this->view_id)
	InvMachines(); // Repaint view+spirits+machines
	gdbt_machines.Init(view_id);
}

//----------------------------------------------------------------------------
void viewGet(ONE *one, FBLT_DSCR *fbd)
{
	RECT rect;

/*
#if DEBUG_VIEW
  if (oneDebug.Validate())
		  fbd->Blt(&rect, one, &oneDebug);
  oneDebug.Validate();
  return;
#endif
*/
  try
  {
	oneDDBlt(&rect, one, display.GetBackBuffer(), fbd);
  } 
  catch(...)
  {
	errorCriticalStop("exception in viewGet");
  }
}

//----------------------------------------------------------------------------
void viewBlt(ONE *one, FBLT_DSCR *fbd)
{
  RECT rect;  // this is the bounding rectangle that became dirty during the blit.
  HRESULT ddrval;

// try to do the blit safely before we step off into the private world
// of Direct X exclusive mode
#if DEBUG_VIEW
  if (oneDebug.Validate())
		  fbd->Blt(&oneDebug, one);
  oneDebug.Validate();
#endif

  try
  {
  	if (one->Validate())
	{
		ddrval = DDoneBlt(&rect, display.GetBackBuffer(), one, fbd);
		if (ddrval != DD_OK)
		  Msg("viewBlt - %s", display.GetDdError(ddrval));
	}

  }
  catch(...)
  {
	errorCriticalStop("exception in viewBlt");
  }
}
