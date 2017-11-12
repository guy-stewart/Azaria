/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       triggers.c
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 02/26/95  gas  Created.
; 06/02/96  gas  Add table of cardinals for view navigation.
; 
----------------------------------------------------------------------------*/

#include "maya.h"
#include "viewman.h"
#include "btable.h"
#include "trigger.h"

#include "data\database\idv.h"
#include "data\database\ids.h"

//----------------------------------------------------------------------------
class PANEL_NAV {
public:
	WORD m_view_id;
	WORD to_view[2]; // 0=fwd, 1=back
public:
	inline int GetGroupKey() {return m_view_id;}
	inline WORD GetPKey() {return m_view_id;}
};

//----------------------------------------------------------------------------
#define BTP_TABLE B_TABLE<PANEL_NAV, int, int, int> 
class TBL_PANEL_NAV : public BTP_TABLE {
public:
	TBL_PANEL_NAV(PANEL_NAV *table, int count):
	  BTP_TABLE(table, count, count) {}

public:
	static BOOL Fire(WORD view_id, int direction /* 0=fwd, 1=back*/, BOOL bFire);
};

//----------------------------------------------------------------------------
class TRIGGER {
public:
	WORD view_id;
	SRECT region;	// left, top, right, bottom
	WORD to_view;

public:
	inline int GetGroupKey() {return view_id;}
	inline SRECT *GetRegion() {return &region;}
};

//----------------------------------------------------------------------------
#define BTT_TABLE B_TABLE<TRIGGER, int, int, int> 
class TBL_TRIGGERS : public BTT_TABLE {
public:
	TBL_TRIGGERS(TRIGGER *table, int count):
	  BTT_TABLE(table, count, count) {}

public:
	BOOL Fire(int view_id, POINT p);
	WORD QueryCursor(int view_id, POINT p);
};

#include "data\database\tbl_triggers.h"

static TBL_CARDINALS tbl_cardinals(cardinals, sizeof(cardinals)/sizeof(CARDINALS));
static TBL_TRIGGERS  tbl_triggers(triggers, sizeof(triggers)/sizeof(TRIGGER));
static TBL_PANEL_NAV tbl_panel_nav(panel_nav, sizeof(panel_nav)/sizeof(PANEL_NAV));

//----------------------------------------------------------------------------
BOOL TBL_TRIGGERS::Fire(int view_id, POINT p)
{
	TRIGGER *trigger = Hit(view_id, p);

	if (trigger)
	{
		viewman.Load(trigger->to_view);
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
WORD TBL_TRIGGERS::QueryCursor(int view_id, POINT p)
{
	TRIGGER *trigger = Hit(view_id, p);

	if (trigger)
		return IDS_HANDTRIG;

	return 0;
}

//----------------------------------------------------------------------------
WORD trgQueryCursor(int view_id, POINT p)
{
  return tbl_triggers.QueryCursor(view_id, p);
}

//----------------------------------------------------------------------------
BOOL trgFire(int view_id, POINT p)
{
  return tbl_triggers.Fire(view_id, p);
}

//----------------------------------------------------------------------------
BOOL TBL_PANEL_NAV::Fire(WORD view_id, int direction/* 0=fwd, 1=bck*/, BOOL bFire)
{
  PANEL_NAV *pnav = tbl_panel_nav.PGet(view_id);
  if (pnav && direction >= 0 && pnav->to_view[direction])
  {
	if (bFire)
	  viewman.Load(pnav->to_view[direction]);
	return TRUE;
  }
  else
	return FALSE;
}

//----------------------------------------------------------------------------
BOOL PanelNav(int view_id, BOOL bBackup, BOOL bFire)
{
  return TBL_PANEL_NAV::Fire(view_id, (bBackup)?1:0, bFire);
}

//----------------------------------------------------------------------------
BOOL TBL_CARDINALS::Fire(WORD view_id, WORD direction, BOOL bFire)
{
  CARDINALS *cardinals = tbl_cardinals.PGet(view_id);

  if (cardinals && direction >= 0 && direction <= 8 && cardinals->m_cardinal[direction])
  {
	if (bFire)
	  viewman.Load(cardinals->m_cardinal[direction]);
	return TRUE;
  }
  else
	return FALSE;
}

//----------------------------------------------------------------------------
BOOL Cardinal(int view_id, int theta, BOOL bBackup, BOOL bFire)
{
  if (bBackup)
	theta += 18000;

  // adjust theta to the center of the compass:
  theta += 2250;
  if (theta > 36000)
	theta -= 36000;

  int cardinal = (theta*8)/36000;

  return TBL_CARDINALS::Fire(view_id, cardinal, bFire);
}

//----------------------------------------------------------------------------
void TBL_CARDINALS::CardHands(int view_id, CARDINALS *hands)
{
  CARDINALS *cardinals = tbl_cardinals.PGet(view_id);

  if (cardinals)
	for (int i=0;i<8;i++)
	{
	  hands->m_cardinal[i] = ViewMan::GetPlayer(cardinals->m_cardinal[i]);
	}
}

//----------------------------------------------------------------------------
BOOL TBL_CARDINALS::InView(int from_view_id, int to_view_id)
{
  CARDINALS *cardinals = tbl_cardinals.PGet(from_view_id);

  if (cardinals)
	for (int i=0;i<8;i++)
	{
	  if (cardinals->m_cardinal[i] == to_view_id)
		return TRUE;
	}

  return FALSE;
}

