/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       viewman.h
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 02/12/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "viewMast.h"

typedef struct {
	WORD map_id;
	BYTE x;
	BYTE y;
} MAPI;

// VIEWS: scene definitions. STATIC.
// this is the view table from the game database ...
typedef struct {
	WORD view_id;
	MAPI map;
	BYTE Z:7;		// used to layer views on top of one another. Fixed.
	BYTE pending_release:1;
	BYTE reserved:1; // for byte alignment
	BYTE backgroundAudio:7;
	WORD view_type;	// ACTOR, ICON, or CURSOR, VC_VIEWSCREEN etc
	WORD behavior_id; // slowly being phased out.
	const char *portal_filename; // if portal, this is its name
	const char *surface_filename;
	VIEW *view;		// RAM instantiation of the view. NULL if not instantiated.
	BYTE player_count; // number of players in this view.
} VIEW_DESCRIPTION;

// view behavior access:
VIEW_DESCRIPTION *GetViewDesc(int view_id);

class ViewMan
{
private:
  VIEW_DESCRIPTION *m_tbl_views;
  static VIEW *c_focus;
  static const char *c_where;
  static WORD c_view_id;

public:
    ViewMan();
private:
    void ReleaseClass(int class_id, WORD except_view_id);

private:
	VIEW_DESCRIPTION *GetFirstbyClass(int type, int *position);
	VIEW_DESCRIPTION *GetNextbyClass(int type, int *position);
	VIEW_DESCRIPTION *GetPrevbyClass(int type, int *position);
	static VIEW_DESCRIPTION *GetFirstActive(int *position);
	static VIEW_DESCRIPTION *GetNextActive(int *position);

public: // interface:
	VIEW *Load(int view_id);
	VIEW *Load(const char *filename, BOOL bSurround);
	void Tick(int etime);
	BOOL Release(int view_id);
	void Release(VIEW_DESCRIPTION *view);
    void PauseMusic();
    void ResumeMusic();

	// view tables access:
	void SetFocus(int view_id);
	VIEW *GetFocus();
	const char *GetWhere();
	WORD GetWhereId();

	static VIEW *GetView(int view_id);
	VIEW *Hit(POINT p);

	static void Invalidate(int view_id, BOOL spirit_moved, BOOL machine_moved = FALSE);
	static void InvRegion(int view_id);

	static void AddPlayer(int view_id);
	static void DelPlayer(int view_id);
	static int  GetPlayer(int view_id);

  // FUNSTUFF
private:
  BOOL holding;
public:
  void SetHolding(BOOL holdme);
};

extern ViewMan viewman;
