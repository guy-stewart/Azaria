/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       spirit.h
;
; Description:
;	Implements the spirit class.
;	Spirits only live within the game.  They do not exists within the database.
;	Spirits are the manifestations of objects that exists in the database.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/31/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#ifndef __SPIRIT_INCLUDED__
#define __SPIRIT_INCLUDED__

#define MAX_ANIMATIONS	40

// SPIRIT:
// The spirit table represents the object<->sprite relationship.
class SPIRIT {
public:
  static BOOL c_monitor;
private:
  static SPIRIT* c_pool[MAX_ANIMATIONS];
  static int c_view[MAX_ANIMATIONS];
  static int c_count;
  static BOOL c_bDeanimate;
private:
	int m_hSprite;		// handle of sprSelect selected sprite
	SPR_OBJECT *m_spr;	// This is the sprite image, (stored in spr_pool)
	FBLT_DSCR m_fbd;		// 
	POINT m_pos;		// current x,y position of upper left corner of sprite
	int m_anindex;		// index into the animation pool, or -1
	ONE *m_canvas;		// canvas this sprite lives on.
	ONE m_bone;		// storage for the background underneath the sprite.
	FBLT_DSCR m_fbdBones;	// blit descriptor for the bones
	BYTE m_on:1;			// flag set to TRUE when sprite is on the scene.
	BYTE m_Loop:1;
	BYTE m_Rewind:1;
	BYTE m_StartFromEnd:1;
	BYTE m_Rewinding:1;
	BYTE m_Deanimate:1;
	BYTE m_i_own_it:1;
	BYTE bMoreFrames:1;
public:
    SPIRIT(ONE *canvas, int sprite_id, int style);
    ~SPIRIT();

public: // interface:
	void ON();
	void OFF();
	void Move(int x, int y, int flags);
	void SetFrame(int frame);
    BOOL PtInSpirit(POINT p);
	void Animate(WORD view_id, BOOL bLoop, BOOL bStartFromEnd=FALSE, BOOL bRewind=FALSE, BOOL bDestroy=FALSE);
	void DeAnimate();

	static void Video(WORD view_id, WORD x, WORD y, WORD ids, BOOL bStartFromEnd, BOOL bLoop, BOOL bRewind);
    static void VideoCut(WORD view_id);
	static void Tick(int etime);
	static void Paint(WORD view_id);
    static void VideoON(WORD view_id);
    static void VideoOFF(WORD view_id);

	WORD LastFrame() {return m_spr->sprite.GetFrames()-1;}
	WORD CurrFrame() {return m_fbd.GetFrame();}
	void SetFramesPerSecond(int FramesPerSecond) {m_fbd.SetFramesPerSecond(FramesPerSecond);}

	void GetRect(RECT *rect);
	int GetWidth() {return m_spr->sprite.GetWidth();}
private:
    BOOL SetNextFrame(int etime);
};

int objdefGetClass(WORD object_id);
int objdefGetLookIconId(WORD object_id);
int objdefGetLookActorId(WORD object_id);
int objdefGetLookCursorId(WORD object_id);

#endif
