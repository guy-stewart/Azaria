/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
;
; File:       inventory.h
;
; Description:
;	Implements the surround style puzzle.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/28/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "viewMast.h"
#include "btable.h"

#define MAX_INV_OBJECTS 32

// inventory behaviors
class INVENTORY : public VIEW {
	WORD m_objects[MAX_INV_OBJECTS];
	int m_count;
	int m_position;
	BOOL m_NewSpirit;
	SPIRIT *m_Spirit[3];
public:
	INVENTORY(int view_id);
public:
    virtual WORD QueryCursor(SRECT r);
    virtual WORD Grab(POINT p, BOOL bClickOnly, POINT *obj_pos);
    virtual BOOL Drop(WORD *obj_def_id, POINT p);
    virtual int  Keycmd(int key, int flags);
    virtual void Paint(int etime, int flags);

};

INVENTORY *invLoad(int view_id);
