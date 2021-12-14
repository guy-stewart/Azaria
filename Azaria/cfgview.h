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
#include <pthread.h>

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

    CFGVIEW(int view_id);
    virtual ~CFGVIEW();

    int  Keycmd(int key, int flags);
    WORD QueryCursor(SRECT srect);
    WORD Grab(POINT p, BOOL bClickOnly, POINT *obj_pos);

    virtual void OnEvent(int control_id, int event_id);

    static int CfgDir(int startat, char *dir, char *ext);
    static const char *CfgStrings(int index);
};

class LISTBOX;
class EDITBOX;
class BUTTON;
class CHECKBOX;

//----------------------------------------------------------------------------
class CFGNET :public CFGVIEW {
  pthread_t thid; /* NULL if thread is not active */
  LISTBOX *listbox;
  BOOL bKeepLooking;
  static int lastProvider;
  static HANDLE Die;
  static HANDLE Dead;
public:
    CFGNET(int view_id);
    ~CFGNET();
    virtual void OnEvent(int control_id, int event_id);

private:
    typedef void * (*Entry)(void *);
    static void EnumThread(CFGNET *thing);
    void CloseEnumThread();
    void OpenEnumThread();
};

//----------------------------------------------------------------------------
class CFGPLYR :public CFGVIEW {
  LISTBOX *egos;
  CHECKBOX * btn_vill;
  CHECKBOX * btn_city;
  CHECKBOX * btn_fem;
  CHECKBOX * btn_male;
public:
    CFGPLYR(int view_id);
    virtual void OnEvent(int control_id, int event_id);
};

//----------------------------------------------------------------------------
class CFGGAME :public CFGVIEW {
  LISTBOX *games;
public:
    CFGGAME(int view_id);
    virtual void OnEvent(int control_id, int event_id);
};

//----------------------------------------------------------------------------
class CFGAV :public CFGVIEW {
public:
    CFGAV(int view_id);
    virtual void OnEvent(int control_id, int event_id);
};

//----------------------------------------------------------------------------
class CTRL_VIEW :public CFGVIEW {
public:
	static CTRL_VIEW *myname;
	static CTRL_VIEW *yourname;
public:
    CTRL_VIEW(int view_id);
    ~CTRL_VIEW();
    virtual void OnEvent(int control_id, int event_id);
   	virtual void Paint(int etime, int flags);

  void SetName(const char *name);
};

#endif // __CFGVIEW_INCLUDED__
