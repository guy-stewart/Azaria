/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
;
; File:       cfgview.h
;
; Description:
;	Implements the panel for use by menus, listboxes, edit forms 
;	and some puzzles.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 07/31/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifndef __CONTROLS_INCLUDED__
#define __CONTROLS_INCLUDED__

#include "textutil.h"

#define LB_BORDER 10

#define MAX_PANEL_BUFF_LEN 32

#define BLACK	  0
#define RED		  249
#define BLUE	  252
#define TEAL_1	  57
#define TEAL_2	  58
#define TEAL_3	  59
#define TEAL_4	  60
#define YELLOW	  114
#define GOLD	  64

#define WHITE	  255
#define GREEN	  85

#define CLR_SELECTED	GOLD
#define CLR_UNSELECTED	TEAL_1
#define CLR_ERASE		BLACK
#define CLR_ERROR		RED

class CFGVIEW;
//----------------------------------------------------------------------------
class CONTROL {
protected:
  CFGVIEW *view;	// view on which the control resides.
  WORD control_id;	// id of the control in the view
  char *StrVal;
  int  *IntVal;

private:
  RECT rect;		// position of control within view.
  int m_hSprite;		// handle of sprSelect selected sprite
  SPR_OBJECT *m_spr;	// This is the sprite image, (stored in spr_pool)
  FBLT_DSCR m_fbd;		// 

public:
    CONTROL(CFGVIEW *view, WORD cid, WORD ids, int x, int y);
    ~CONTROL();

  virtual int Keycmd(int key, int flags)=0;
    virtual WORD Grab(POINT p)=0;
    virtual void SetFocus(POINT p, BOOL bHasFocus)=0;
    virtual void Refresh();
    virtual BOOL PtInRect(POINT p);

  char *GetStrValue() {return StrVal;}
  void  SetStrValue(char *newStr);
  int   GetIntValue() {return *IntVal;}

  int GetWidth() {return rect.right - rect.left;}
};

//----------------------------------------------------------------------------
class LISTBOX : public CONTROL {
  typedef char *(*STRVAL)(int position);
public:
    LISTBOX(CFGVIEW *view, 
					WORD ids, 
					int x, int y,				// location of control within the view
					int xo, int yo,				// offset of text into the contol image
					int xCount, 
                    STRVAL const xStrVal);
  ~LISTBOX();
private:
  int left, top;
  int TopItem; // scroll position in list - first visible item
  int Selected; // currently selected item
  int m_count;    // number of items in list
  CFONT font;
  int m_row_height;

  STRVAL StrFunc;

public:
  void SetCount(int count) {m_count = count;}
  void Init();
  void Write(int position, COLORREF color);

    virtual int  Keycmd(int key, int flags);
    virtual WORD Grab(POINT p);
    virtual void SetFocus(POINT p, BOOL bHasFocus);
    virtual void Refresh();

  void ChangeSel(int new_selection);
};

//----------------------------------------------------------------------------
class BUTTON : public CONTROL {
protected:
  SPIRIT *sprBtn;
  SPIRIT *sprHilight;
public:
    BUTTON(CFGVIEW *view,		// Surface on which to paint the button
				int control_id,
				int x, int y,		// position on the canvas
				WORD ids_image,		// image of the button
				WORD ids_image_hi);	// image of the highlighted button

    ~BUTTON();

    virtual int  Keycmd(int key, int flags);
    virtual WORD Grab(POINT p);
    virtual void SetFocus(POINT p, BOOL bHasFocus);
    virtual void Refresh();

    SPIRIT *LoadBtnImage(ONE *canvas,
								WORD view_id,
								WORD sprite_id,
								int x, int y);
};

//----------------------------------------------------------------------------
// acts just like a button,  except for the Grab && Setfocus functions
class CHECKBOX : public BUTTON {
  BOOL bIsChecked;

public:
    CHECKBOX (CFGVIEW *view,	// Surface on which to paint the button
				int control_id,
				int x, int y,		// position on the canvas
				WORD ids_image,		// image of the button
				WORD ids_image_hi);	// image of the highlighted button

    virtual WORD Grab(POINT p);
    virtual void SetFocus(POINT p, BOOL bHasFocus);

  void SetChecked(BOOL bChecked);
};

//----------------------------------------------------------------------------
class EDITBOX : public CONTROL {
  int LEN;
  int MAXLEN;
  char BUFF[MAX_PANEL_BUFF_LEN];
  CFONT font;
  int left, top;

public:
    EDITBOX(CFGVIEW *view, 
					WORD ids, 
					int x, int y,				// location of control within the view
					int xo, int yo,				// offset of text into the contol image
					char *intial_value, 
					int max_len);

    ~EDITBOX();

    BOOL Init(char *defStr, int maxlen);
    void Write(char *buff, int color);

    virtual int  Keycmd(int key, int flags);
    virtual WORD Grab(POINT p);
    virtual void SetFocus(POINT p, BOOL bHasFocus);
    virtual void Refresh();
};

//----------------------------------------------------------------------------
// acts just like a button,  except for the Grab && Setfocus functions
class SLIDER : public CONTROL {
  BUTTON left;
  BUTTON right;
  SPIRIT thumb;
  BOOL bLeftFocus;
  int min, max, value,width, granularity;
  int x, y;
public:
    SLIDER(CFGVIEW *view,	// Surface on which to paint the button
				int control_id,
				int x, int y,		// position on the canvas
				int value,
				int width,
				int granularity,
				int min, int max,
				WORD ids_image,		// image of the button
				WORD ids_image_hi);	// image of the highlighted button

    virtual WORD Grab(POINT p);
    virtual void SetFocus(POINT p, BOOL bHasFocus);
    virtual int  Keycmd(int key, int flags);
    virtual BOOL PtInRect(POINT p);
};

//----------------------------------------------------------------------------
// acts just like a button,  except for the Grab && Setfocus functions
class LABEL : public CONTROL {
public:
    LABEL(CFGVIEW *view,	// Surface on which to paint the button
				int control_id,
				int x, int y,		// position on the canvas
				WORD ids_image);	// image of the label

    virtual WORD Grab(POINT p);
    virtual void SetFocus(POINT p, BOOL bHasFocus);
    virtual int  Keycmd(int key, int flags);
};


#endif
