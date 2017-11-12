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
#ifndef __CFGVIEW_INCLUDED__
#define __CFGVIEW_INCLUDED__

#include "panel.h"

class CONTROL;
//----------------------------------------------------------------------------
class CFGVIEW : public PANEL {
  friend CONTROL;
protected:
#define NAMES_PER_PAGE  20
#define MAX_NAME_LEN	16
  static char names[NAMES_PER_PAGE][MAX_NAME_LEN];

  CONTROL *control[15];
  int control_count;
  int focus; // which control has the focus?
public:

	CFGVIEW::CFGVIEW(int view_id);
	virtual CFGVIEW::~CFGVIEW();

	int  CFGVIEW::Keycmd(int key, int flags);
	WORD CFGVIEW::QueryCursor(SRECT srect);
	WORD CFGVIEW::Grab(POINT p, BOOL bClickOnly, POINT *obj_pos);

	virtual void CFGVIEW::OnEvent(int control_id, int event_id);

	static int CFGVIEW::CfgDir(int startat, char *dir, char *ext);
	static char *CFGVIEW::CfgStrings(int index);
};

class LISTBOX;
class EDITBOX;
class BUTTON;
class CHECKBOX;

//----------------------------------------------------------------------------
class CFGNET :public CFGVIEW {
  unsigned thid; /* NULL if thread is not active */
  LISTBOX *listbox;
  BOOL bKeepLooking;
  static int lastProvider;
  static HANDLE Die;
  static HANDLE Dead;
public:
  CFGNET::CFGNET(int view_id);
  CFGNET::~CFGNET();
  virtual void CFGNET::OnEvent(int control_id, int event_id);

private:
  typedef unsigned int (__stdcall* Entry)(void*);
  static void CFGNET::EnumThread(CFGNET *thing);
  void CFGNET::CloseEnumThread();
  void CFGNET::OpenEnumThread();
};

//----------------------------------------------------------------------------
class CFGPLYR :public CFGVIEW {
  LISTBOX *egos;
  CHECKBOX * btn_vill;
  CHECKBOX * btn_city;
  CHECKBOX * btn_fem;
  CHECKBOX * btn_male;
public:
  CFGPLYR::CFGPLYR(int view_id);
  virtual void CFGPLYR::OnEvent(int control_id, int event_id);
};

//----------------------------------------------------------------------------
class CFGGAME :public CFGVIEW {
  LISTBOX *games;
public:
  CFGGAME::CFGGAME(int view_id);
  virtual void CFGGAME::OnEvent(int control_id, int event_id);
};

//----------------------------------------------------------------------------
class CFGAV :public CFGVIEW {
public:
  CFGAV::CFGAV(int view_id);
  virtual void CFGAV::OnEvent(int control_id, int event_id);
};

//----------------------------------------------------------------------------
class CTRL_VIEW :public CFGVIEW {
public:
	static CTRL_VIEW *myname;
	static CTRL_VIEW *yourname;
public:
  CTRL_VIEW::CTRL_VIEW(int view_id);
  CTRL_VIEW::~CTRL_VIEW();
  virtual void CTRL_VIEW::OnEvent(int control_id, int event_id);

  void SetName(char *name);
};

#endif // __CFGVIEW_INCLUDED__
