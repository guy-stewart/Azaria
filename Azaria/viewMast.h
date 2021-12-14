/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in V  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  V document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       viewMast.h
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
#ifndef __VIEWMAST_INCLUDED__
#define __VIEWMAST_INCLUDED__

	// view classiification:
#define VC_MASK				0x7F00	// View Classes - mutual exclusion flag.
#define VC_UNIQUE			0x0000	// There is only one instance of this class
#define VC_PANEL			0x0100  // -  only 1 view of each class is shown at a time.
#define VC_VIEWSCREEN		0x0200

#define VO_MASK				0x001F  // View Objects
#define OBJECTLESS_VIEW		0x0001
#define ICON_VIEW			0x0002
#define ACTOR_VIEW			0x0004
#define CURSOR_VIEW			0x0008

// flag values for keyCmd flags:
#define KEY_PRESSED		0x01
#define KEY_RELEASED	0x02

// flags values for frame buffer cleanup:
#define MSK_BUFFER		0x0007
#define MSK_RESTORE		0x1000

#define BUFFNUM(FLAGS)	 (FLAGS&MSK_BUFFER)
#define MSK_FRAME(FLAGS) (1<<BUFFNUM(FLAGS))

// RAM VIEWS.  Instantiated views are stored in this structure:
class VIEW {

public:
	VIEW(int view_id, const char *name, int style);
	virtual ~VIEW();

private:
	char name[16];	// for debugging purposes.
	int updatedFrames;		// keep track of frames that need to be refreshed.
	BOOL spr_changed;	// is there 1 or more sprites that need to be drawn
	BOOL m_machines_changed;

public:
	WORD view_id;
	ONE surface;			// surface that gets painted if view paint is used.
	unsigned char *restore_buffer;	// the original untouched image used to refresh the surface
	long restore_size;
	RECT srect;				// rectangle around the surface/canvas
	FBLT_DSCR fbd;			// descriptor used to paint the view.
	int view_type;		// how are the sprites drawn (icon, cursor, actor)

public:
	virtual void Paint(int etime, int flags);
	virtual int  Action(int etime, int flags);
	virtual int  Keycmd(int key, int flags);
	virtual int  Msg(int key, int flags);
	virtual void Navigate(POINT p, WORD ids);
	virtual BOOL Drop(WORD *obj_def_id,  POINT p);
	virtual WORD Grab(POINT p, BOOL bClickOnly, POINT *obj_pos);
	virtual WORD Drag(WORD obj_def_id, POINT p);
	virtual WORD QueryCursor(SRECT p);
	virtual void LostFocus();

	virtual long GetCanvasWidth()	{return surface.GetWidth();}
	virtual long GetCanvasHeight()	{return surface.GetHeight();}

	inline BOOL AcceptsObjects() {return (!((view_type) & OBJECTLESS_VIEW));}
	inline WORD GetViewID() {return view_id;}

	void BlitToDisplay();

	POINT MapToCanvas(POINT p); // map screen coord to canvas coord.
	POINT MapToScreen(POINT p);	// map canvas coord to screen coord.

	BOOL PtInView(POINT p);

	BOOL ChangeSurface(char *newSurfaceName);
    void Move(int x_delta, int y_delta);

public:
	inline BOOL NeedsPainting(int frame) 
	{
	  return (!(updatedFrames & (MSK_FRAME(frame))));
	}

	void SetPainted(int frame);

	inline void Invalidate() {updatedFrames = 0;surface.SetAllDirty();}; // paint canvas to display
	inline void InvSpirits() {spr_changed=TRUE, Invalidate();}; // repaint canvas also
	inline void InvMachines() {m_machines_changed = TRUE, InvSpirits();}
	inline void InvRegion() {updatedFrames = 0;}

protected:
	inline BOOL isMainViewScreen() {return (VC_VIEWSCREEN & (view_type & VC_MASK));}
	void InvalidateOffset();

public:
	inline int Top()		{return fbd.m_brect.top;}
	inline int Bottom()		{return fbd.m_brect.bottom;}
	inline int Left()		{return fbd.m_brect.left;}
	inline int Right()		{return fbd.m_brect.right;}
public:
	inline int viewHeight()		{return Bottom() - Top() + 1;}
	inline int viewWidth()		{return Right() - Left() + 1;}
};

BOOL viewSwapSurface(VIEW *view, ONE *newSurface);

void viewBlt(ONE *one, FBLT_DSCR *fbd);
void viewGet(ONE *one, FBLT_DSCR *fbd);

#endif  // __VIEWMAST_INCLUDED__
