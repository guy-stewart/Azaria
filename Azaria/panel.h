/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
;
; File:       panel.h
;
; Description:
;	Implements the panel for use by menus, listboxes, edit forms 
;	and some puzzles.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/28/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifndef __PANEL_INCLUDED__
#define __PANEL_INCLUDED__

#include "viewMast.h"

class PANEL : public VIEW {
public:
	PANEL::PANEL(int view_id);
	PANEL::~PANEL();

	int PANEL::Keycmd(int key, int flags);
	WORD PANEL::QueryCursor(SRECT srect);
};

#endif // __PANEL_INCLUDED__
