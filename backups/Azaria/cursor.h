/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
; 
; File:       cursor.h
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/

class CursorMan
{
private:
	BOOL holding_object; // is the local player holding an object
	BOOL obj_on_top;
public:
	void Init(); // must be called before CursorMan will work.
	CursorMan(){holding_object=FALSE;}
	~CursorMan();
public:// interface:
	void RestoreDisplay(); // call when the display is changing.

	// the local player is holding an object:
	void SetCursor(WORD sprite_id);

	WORD GetCursor();
	void LoadObject(WORD obj_def_id, POINT pos);
	WORD DropObject(POINT *pCenter, POINT *pos);
	WORD HeldObject();

	void AddPlayer(DPID pid, char *name); // called when remote player joins local view
	void DropPlayer(DPID pid); // remote player leaves local view.
	void MovePlayer(DPID pid, POINT point); // remote player moves with local view
	void SetPlayerObject(DPID pid, WORD object_def);
	void DropAllHands();

	void cursorOff(int etime, int frame);
	void cursorOn(int etime, int frame);
	void cursorMove(POINT p);
};

extern CursorMan cursorman;
