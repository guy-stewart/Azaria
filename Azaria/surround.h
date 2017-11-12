/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
;
; File:       surround.h
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
#ifdef  __SURROUND_INCLUDED__
#pragma message( __FILE__ "included multiple times") 
#else
#define __SURROUND_INCLUDED__

#include "physics.h"
#include "viewMast.h"

// surround behaviors
class SURROUND : public VIEW {
private:
	PARTICLE m_px;
	int m_vx;
	int m_accx;		// acceleration +/- in x
	int m_posx;		// the x of the direction we are currently facing. (from theta)
	int m_opos;		// the direction we were facing last refresh time.
	POINT m_mousepos; // position of mouse at last query cursor time.
	int old_theta;
	SPIRIT *hand[8];
public:
	SURROUND(int view_id);
	~SURROUND();
public:
	virtual int  Action(int etime, int flags);
	virtual void Paint(int etime, int flags);
	virtual int  Keycmd(int key, int flags);
	virtual WORD SURROUND::QueryCursor(SRECT r);

	virtual long GetCanvasWidth() {return surface.GetHeight();}
	virtual long GetCanvasHeight() {return surface.GetWidth();}
};

#endif