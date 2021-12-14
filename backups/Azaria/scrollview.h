/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
;
; File:       scrollview.h
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 03/17/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifndef __SCROLLVIEW_INCLUDED__
#define __SCROLLVIEW_INCLUDED__

#include "physics.h"
#include "viewMast.h"

class SCROLLVIEW : public VIEW {

private:	// Data items:
	PARTICLE m_px;
	PARTICLE m_py;
	int m_accx;
	int m_accy;
	int m_posx;
	int m_posy;
	int m_oposx;
	int m_oposy;
	BOOL m_key_control;
	BOOL m_slewing;
	POINT m_slewtarget;

public:
	SCROLLVIEW(int view_id);
	virtual int  Keycmd(int key, int flags);
	virtual int Action(int etime, int flags);
    virtual WORD QueryCursor(SRECT srect);

	void SlewTo(WORD x, WORD y);
	void SlewRel(long x, long y);
};

#endif // __SCROLLVIEW_INCLUDED__
