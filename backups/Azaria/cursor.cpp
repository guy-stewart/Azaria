/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
;
; File:       cursor.c
;
; Description:
;	Implements the panel overlay cursor.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/30/96  gas  Created.
; 04/18/95  gas  Added CURSOR_HAND code to handle representation of other 
;                player hands in current view.
; 
----------------------------------------------------------------------------*/

#include "maya.h"
#include "textutil.h"
#include "viewman.h"

#include "cursor.h"
#include "player.h"

#include "Data/Database/Ids.h"

CursorMan cursorman;

#define MAX_BUFFERS			3
#define MAX_CURSOR_HANDS	20
#define MAX_NAME_LEN		16

class HELD_OBJECT;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class CURSOR {
public:
    CURSOR();
    CURSOR(int hSprite, POINT p) {Init(hSprite, p);};
private:
	static CFONT *font;
	POINT m_pos;
	POINT mp[MAX_BUFFERS]; // position of old cursors.
	int hSpr;
	SPR_OBJECT *spr;
	ONE poneBack[MAX_BUFFERS];
	FBLT_DSCR fbd;
	int m_valid; // bitwise flags for backgrounds that need to be restored
	FBLT_DSCR fbdS;
	int cursorState;
	WORD m_ReincarnateAs; // IDS of sprite to re-incarnate as.
	WORD m_hSprite; // IDS of current sprite (mostly to ask what cursor is currently in use).
	char name[MAX_NAME_LEN];
	BOOL m_Damned:1;
	BOOL m_Looped:1;
	BOOL m_HasName:1;
public:
	BOOL m_message;

protected:
	virtual void kill();
	// how to tell if a cursor has buffers that need restoring:
	inline BOOL isClean(int buffnum) {return (m_valid & (1<<buffnum)) == 0;};
	inline BOOL isAllClean() {return m_valid == 0;};
	inline void setAllClean() {m_valid = 0;}
	inline BOOL isDirty(int buffnum) {return !isClean(buffnum);};
	inline void setDirty(int buffnum) {m_valid |= 1<<buffnum;};
	inline void setClean(int buffnum) {m_valid &= ~(1<<buffnum);};
public: // interface:
	void Show(int etime, int flags);
	void Hide(int etime, int flags);
	void Init(int hSprite, POINT p);
	void SetCursor(WORD sprite_id);
	WORD GetCursor();
	inline void MoveTo(POINT p) {m_pos = p;}
	inline POINT GetPos() {return m_pos;}
	inline void SetName(char *_name) 
	{strncpy(name, _name, MAX_NAME_LEN); name[MAX_NAME_LEN-1] = 0; m_HasName = TRUE;}

	// how to tell the state of a cursor:
public:
	inline BOOL isActive() {return cursorState == 1;}
	inline BOOL isDying() {return cursorState == 2;}
	inline BOOL isDead() {return cursorState == 3;}
	inline BOOL isDamned() {return m_Damned;}

	// Anyone can set the state of a cursor:
public:
	inline void setActive() {cursorState = 1;}
	inline void setDying() {cursorState = 2;}
	inline void setDead() {cursorState = 3;}
	inline void setDamned(BOOL damned) {m_Damned = damned;}

	friend CursorMan;
	friend HELD_OBJECT;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class HELD_OBJECT : public CURSOR {
public:
	WORD m_obj_def_id;
	WORD hSprite;
	BOOL m_AnotherObject; // pick up another object before end of flip sequence

public:
    HELD_OBJECT();

private:
	virtual void kill();
public:
	BOOL LoadObject(WORD obj_def_id, POINT p);
	WORD DropObject(POINT *pCenter, POINT *pPos);
	WORD GetDef()
	{
		return m_obj_def_id;
	}
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class CURSOR_HAND : public CURSOR {
private:
	DPID m_pid;
	PLAYER *m_player;
	BOOL m_holding_object;
	HELD_OBJECT m_held_object;
private:
	virtual void kill();
public:
	static BOOL dead_hands;
public:
	inline DPID GetPid() {return m_pid;}
	inline void SetPid(DPID pid) {m_pid = pid;}
	void drop();
	void LoadObject(WORD obj_def_id);
	void DropObject();

public:
	void Show(int etime, int flags);
	void Hide(int etime, int flags);
	void MoveTo(POINT p);
	static void ClearDeck();
};

BOOL CURSOR_HAND::dead_hands = FALSE;
static int other_hands_count = 0;
static CURSOR_HAND tbl_other_hands[MAX_CURSOR_HANDS];

//----------------------------------------------------------------------------
static CURSOR mouse_cursor;
static HELD_OBJECT held_object;
static POINT held_offset;

//----------------------------------------------------------------------------
CURSOR::CURSOR()
{
	setDead();
	m_pos.x = 0;
	m_pos.y = 0;
	spr = NULL;
	m_valid = 0;
	m_message = FALSE;
	m_Damned = FALSE;
    m_Looped = FALSE;
	m_HasName = FALSE;

	// don't reincarnate
	m_ReincarnateAs = 0;
}

//----------------------------------------------------------------------------
void CURSOR::Init(int hSprite, POINT p)
{
	// make sure we are cleaning up after ourself
	assert(!spr);

	m_hSprite = hSprite;

	// Select the sprite:
	hSpr = sprSelect(hSprite);
	spr = sprGet(hSpr);

	m_pos = p;

	fbd.ZeroInit();
	fbd.SpriteInit(&spr->sprite);
	fbd.MaskInit(&spr->sprite);
	fbd.m_style = BLT_PORTAL;

	fbdS.ZeroInit();
	fbdS.m_dx = (short)spr->sprite.GetWidth();
	fbdS.m_dy = fbd.m_dy;
	fbdS.m_style = BLT_RECT;

	for (int i=0;i<MAX_BUFFERS;i++)
	{
		poneBack[i].Init(spr->sprite.GetWidth(), spr->sprite.GetHeight());
	}

	m_valid = 0; // there are not yet any backgrounds to restore.
	setActive(); // the cursor is in the active state
}

//----------------------------------------------------------------------------
void CURSOR::SetCursor(WORD sprite_id)
{
  m_ReincarnateAs = sprite_id;
  if (isActive())
  {
	setDamned(TRUE);
	//setDying();
  }
}

//----------------------------------------------------------------------------
WORD CURSOR::GetCursor()
{
  if (m_ReincarnateAs)
	return m_ReincarnateAs;
  else
	return m_hSprite;
}

//----------------------------------------------------------------------------
void CURSOR_HAND::kill()
{
	// CURSOR::kill();
	setDead();

	assert(other_hands_count >0);

	if (other_hands_count <= 0)
		return;

	// wait until after all hands are off deck 
	// before shuffling the array:
	dead_hands = TRUE;
}

//----------------------------------------------------------------------------
// after one or more hands dies
// (because we changed views or other player changed views,  
// and the hand has been removed from all backbuffers,
// the hand (or hands) can be removed from the array.
void CURSOR_HAND::ClearDeck()
{
	int i;

	// scan the array removing all dead hands:
	for (i=other_hands_count-1;i>=0;i--)
	{
		if (tbl_other_hands[i].isDead())
		{
			other_hands_count--;
		}
		else
			break; // can't remove any hands off tail
	}

	dead_hands = FALSE;
}

//----------------------------------------------------------------------------
void CURSOR_HAND::Show(int etime, int flags)
{
	if (m_holding_object)
		m_held_object.Show(etime, flags);
	CURSOR::Show(etime, flags);
}

//----------------------------------------------------------------------------
void CURSOR_HAND::Hide(int etime, int flags)
{
	CURSOR::Hide(etime, flags);
	if (!m_held_object.isDead())
		m_held_object.Hide(etime, flags);
}

//----------------------------------------------------------------------------
void CURSOR_HAND::MoveTo(POINT p)
{
	CURSOR::MoveTo(p);
	m_held_object.MoveTo(p);
}

//----------------------------------------------------------------------------
void CURSOR_HAND::DropObject()
{
	POINT pCenter, pPos;
	m_held_object.DropObject(&pCenter, &pPos);
	m_holding_object = FALSE;
}

//----------------------------------------------------------------------------
void CursorMan::AddPlayer(DPID pid, char *name)
{
	CURSOR_HAND *hand;

	// find a dying or unused slot:
	for (int i=0;i<MAX_CURSOR_HANDS;i++)
	{
		hand = &tbl_other_hands[i];
		if (hand->isDying() || hand->isDead())
		{
			hand->SetPid(pid);
			hand->SetName(name);
			hand->setActive();
			if (i>=other_hands_count)
				other_hands_count=i+1;
			break;
		}
	}
}

//----------------------------------------------------------------------------
void CURSOR_HAND::drop()
{
	if (isActive())
	{
		setDying();
	}

	if (m_holding_object)
		DropObject();
}

//----------------------------------------------------------------------------
void CURSOR_HAND::LoadObject(WORD obj_def_id)
{
  POINT p;

	if (m_holding_object)
		return;

	m_held_object.LoadObject(obj_def_id, p);
	m_holding_object = TRUE;
}

//----------------------------------------------------------------------------
void CursorMan::DropAllHands()
{
	for (int i=0;i<other_hands_count;i++)
	{
		tbl_other_hands[i].drop();
	}
}

//----------------------------------------------------------------------------
void CursorMan::DropPlayer(DPID pid)
{
	for (int i=0;i<other_hands_count;i++)
	{
		if (tbl_other_hands[i].GetPid() == pid)
		{
			tbl_other_hands[i].drop();
			break;
		}
	}
}

//----------------------------------------------------------------------------
void CursorMan::MovePlayer(DPID pid, POINT p)
{
	for (int i=0;i<other_hands_count;i++)
	{
		if (tbl_other_hands[i].GetPid() == pid)
		{
			tbl_other_hands[i].MoveTo(p);
			break;
		}
	}
}

//----------------------------------------------------------------------------
void CursorMan::SetPlayerObject(DPID pid, WORD object_def_id)
{
	for (int i=0;i<other_hands_count;i++)
	{
		if (tbl_other_hands[i].GetPid() == pid)
		{
			if (object_def_id)
				tbl_other_hands[i].LoadObject(object_def_id);
			else
			{
				tbl_other_hands[i].DropObject();
			}
			break;
		}
	}
}

//----------------------------------------------------------------------------
void CURSOR::kill()
{
	int i;

	setDead();

	sprRelease(hSpr);
	spr = NULL;

	maskFree(&fbd.m_mask);
	for (i=0;i<MAX_BUFFERS;i++)
	{
		setClean(i); // don't bother writing again!
		poneBack[i].freeBits();
	}

	// if our hand is to be reincarnated:
	if (m_ReincarnateAs)
	{
	  Init(m_ReincarnateAs, GetPos());
	  m_ReincarnateAs = 0;
	}
}

//----------------------------------------------------------------------------
void CURSOR::Hide(int etime, int flags)
{
	int nBuff = BUFFNUM(flags);

	if (nBuff < 0 || nBuff >= MAX_BUFFERS)
		return;

	// Check for the unexpected:
	if (isDead()) 
		return;

	// make sure this buffer needs to be restored:
	if (isDirty(nBuff))
	{
		fbdS.BonesOn(mp[nBuff].x, mp[nBuff].y);
		viewBlt(&poneBack[nBuff], &fbdS);
		if (m_message) Msg("CursorOff(buff=%d)", nBuff);
		setClean(nBuff); // don't bother writing again!
	}

	if (isDying() && isAllClean())
	{
		if (m_message) Msg("Kill Cursor");
		kill();
	}
}

//----------------------------------------------------------------------------
void CURSOR::Show(int etime, int flags)
{
	int nBuff = BUFFNUM(flags);

	if (nBuff < 0 || nBuff >= MAX_BUFFERS)
		return;

	if (!isActive()) return;

	mp[nBuff] = m_pos;
	
	if ((mp[nBuff].x <= 640) && (mp[nBuff].y <= 480)
	 && (mp[nBuff].x >= 0) && (mp[nBuff].y >= 0))
	{
		fbdS.BonesOff(mp[nBuff].x, mp[nBuff].y);
		viewGet(&poneBack[nBuff], &fbdS);
		if (m_message) Msg("CursorOn(buff=%d)", nBuff);
		setDirty(nBuff);

		// animate the cursor thing:
		if (isDamned())
		{
			if (!fbd.PrevFrame(&spr->sprite, etime, FBLT_DSCR::DONTLOOP))
			{
				setDying();
				setDamned(FALSE);
			}
		}
		else
		{
		  fbd.NextFrame(&spr->sprite, etime, (m_Looped)?FBLT_DSCR::LOOP:FBLT_DSCR::DONTLOOP);
		}

		fbd.m_dst_x = (short)m_pos.x;
		fbd.m_dst_y = (short)m_pos.y;
		viewBlt(&spr->sprite, &fbd);
		if (m_HasName)
		{
			//font->dPrint(buffer, color);
		}
	}
}

//----------------------------------------------------------------------------
void CursorMan::cursorOff(int etime, int flags)
{
	int i;

	if (obj_on_top)
	{
		if (!held_object.isDead())
			held_object.Hide(etime, flags);
		mouse_cursor.Hide(etime, flags);
	}
	else
	{
		mouse_cursor.Hide(etime, flags);
		if (!held_object.isDead())
			held_object.Hide(etime, flags);
	}

	for (i=other_hands_count-1; i>=0; i--)
	{
		tbl_other_hands[i].Hide(etime, flags);
	}

	if (CURSOR_HAND::dead_hands)
	{
		CURSOR_HAND::ClearDeck();
	}
}

//----------------------------------------------------------------------------
void CursorMan::cursorOn(int etime, int flags)
{
	for (int i=0; i<other_hands_count; i++)
	{
		tbl_other_hands[i].Show(etime, flags);
	}

	if (obj_on_top)
	{
		mouse_cursor.Show(etime, flags);
		if (!held_object.isDead())
			held_object.Show(etime, flags);
	}
	else
	{
		if (!held_object.isDead())
			held_object.Show(etime, flags);
		mouse_cursor.Show(etime, flags);
	}
}

//----------------------------------------------------------------------------
void CursorMan::cursorMove(POINT p)
{
	VIEW *view = viewman.GetFocus(); // get the view the cursor is on

	mouse_cursor.MoveTo(p);

	if ((p.x > 0) && held_offset.x > p.x)
	  held_offset.x = p.x;
	p.x -= held_offset.x;

	if ((p.y > 0) && held_offset.y > p.y)
	  held_offset.y = p.y;
	p.y -= held_offset.y;

	held_object.MoveTo(p);
}

//----------------------------------------------------------------------------
void CursorMan::Init()
{
  POINT p;
  p.x = 320;p.y=240;

  obj_on_top = FALSE;

 // CURSOR::font = new CFONT(IDS_FONTTNR12);

	if (mouse_cursor.isDead())
	{
		mouse_cursor.Init(IDS_HANDIDLE, p);
		mouse_cursor.fbd.SetFramesPerSecond(40);
		mouse_cursor.m_message = FALSE;
	}

	for (int i=0; i<MAX_CURSOR_HANDS; i++)
	{
		tbl_other_hands[i].Init(IDS_OTHERHAND, p);
		tbl_other_hands[i].setDead();
	}
}

//----------------------------------------------------------------------------
CursorMan::~CursorMan()
{
}

//----------------------------------------------------------------------------
HELD_OBJECT::HELD_OBJECT():
CURSOR()
{
  m_Looped = TRUE;
}

//----------------------------------------------------------------------------
void HELD_OBJECT::kill()
{
	CURSOR::kill();

	if (m_AnotherObject)
	{
		Init(hSprite, GetPos());
	}
}

//----------------------------------------------------------------------------
BOOL HELD_OBJECT::LoadObject(WORD obj_def_id, POINT pos)
{
	m_obj_def_id = obj_def_id;

	hSprite = objdefGetLookCursorId(m_obj_def_id);

	if (!hSprite)
	{
  		m_obj_def_id = 0;
		return FALSE;
	}

	if (isDead())
	{
		Init(hSprite, pos);
	}
	else
		m_AnotherObject = TRUE;

	held_offset.x = mouse_cursor.m_pos.x - pos.x;
	held_offset.y = mouse_cursor.m_pos.y - pos.y;

  	int w = spr->sprite.GetWidth();
	int h = (spr->sprite.GetHeight()/spr->sprite.GetFrames());
	if (held_offset.x < 0 || held_offset.x > w)
	{
		pos.x = mouse_cursor.m_pos.x - w/2;
		held_offset.x = w/2;
	}

	if (held_offset.y < 0 || held_offset.y > h)
	{
		pos.y = mouse_cursor.m_pos.y - h/2;
		held_offset.y = h/2;
	}

	MoveTo(pos);

	return TRUE;
}

//----------------------------------------------------------------------------
WORD HELD_OBJECT::DropObject(POINT *obj_center, POINT *pos) // return the coord of object center.
{
	m_AnotherObject = FALSE;

	if (isActive())
	{
		setDying();
  		obj_center->x = spr->sprite.GetWidth()/2;
		obj_center->y = (spr->sprite.GetHeight()/spr->sprite.GetFrames())/2;
	}

	pos->x = mouse_cursor.m_pos.x - held_offset.x ;
	pos->y = mouse_cursor.m_pos.y - held_offset.y;

	return m_obj_def_id;
}

//----------------------------------------------------------------------------

void CursorMan::LoadObject(WORD obj_def_id, POINT pos)
{
	if (holding_object)
		return;

	holding_object = held_object.LoadObject(obj_def_id, pos);
	if (holding_object)
		local_player->SetObject(obj_def_id);
}

//----------------------------------------------------------------------------
WORD CursorMan::DropObject(POINT *pCenter, POINT *pos)
{
	if (!holding_object)
		return NULL;
	else
	{
		local_player->SetObject(0);
		holding_object = FALSE;
		return held_object.DropObject(pCenter, pos);
	}
}

//----------------------------------------------------------------------------
WORD CursorMan::HeldObject()
{
	if (!holding_object)
		return 0;
	else
		return held_object.GetDef();
}

//----------------------------------------------------------------------------
void CursorMan::SetCursor(WORD sprite_id)
{
  mouse_cursor.SetCursor(sprite_id);
}

//----------------------------------------------------------------------------
WORD CursorMan::GetCursor()
{
  return mouse_cursor.GetCursor();
}

void CursorMan::RestoreDisplay()
{
	for (int i=0; i<other_hands_count; i++)
	{
		tbl_other_hands[i].setAllClean();
	}

	if (!held_object.isDead())
	{
		held_object.setAllClean();
	}

	mouse_cursor.setAllClean();
}
