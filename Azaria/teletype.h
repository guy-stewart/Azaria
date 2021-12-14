/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
;
; File:       teletype.h
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
#ifdef __TELETYPE_INCLUDED__
#pragma message( __FILE__ " - included multiple times.") 
#else
#define __TELETYPE_INCLUDED__

#include "physics.h"
#include "viewMast.h"
#include "scrollview.h"
#include "textutil.h"

//----------------------------------------------------------------------------
#define MAX_TELETYPE_MESSAGE_LEN 80
#define MAX_TELETYPE_MESSAGES 32
#define MAX_NAME 20

//----------------------------------------------------------------------------
typedef struct {
  DWORD pid;
  BYTE	freq;
  BYTE	lang;
  BYTE  length;
  char buffer[MAX_TELETYPE_MESSAGE_LEN];
  char name[MAX_NAME];
}TELE_MESSAGE;

//----------------------------------------------------------------------------
class TELETYPE : public SCROLLVIEW {
private:
  // There should only be one teletype view per game. This will point to it.
  static TELETYPE *c_teletype;
  static BYTE c_freq;
public:
  static BYTE c_lang;
  static BYTE c_pitch;

  WORD m_Slew_Y;

  CFONT *font[8];

  BOOL m_text_dirty;
  // local user text entry buffer:
  char m_buffer[MAX_TELETYPE_MESSAGE_LEN];
  int m_index;


  // message buffers:
  TELE_MESSAGE m_message[MAX_TELETYPE_MESSAGES];
  int m_message_count;
  int m_first_message;
  int m_last_message;

public:		// makers/breakers
    TELETYPE(int view_id);
    ~TELETYPE();
    
    virtual int  Keycmd(int key, int flags);
    virtual int  Msg(int key, int flags);
    virtual void Paint(int etime, int flags);
    virtual WORD QueryCursor(SRECT r);
    virtual WORD Grab(POINT p, BOOL bClickOnly, POINT *obj_pos);

  void Invalidate() {m_text_dirty = TRUE; SCROLLVIEW::Invalidate();}

private:
    void _AppendMessage(DWORD pid, char *message, BYTE freq, BYTE lang); 

public:
    static void Dump();
    static void Msg( DWORD pid, const LPSTR fmt, ... );
    inline static void AppendMessage(DWORD pid, char *message, BYTE freq, BYTE lang) 
		{if (c_teletype) c_teletype->_AppendMessage(pid, message, freq, lang);}
    inline static BYTE GetFreq() {return c_freq;}

  inline static void SetLang(int x) {c_lang = x;}
  inline static void Refresh() {if (c_teletype) c_teletype->Invalidate();}

    static void DelPlayer(DWORD pid)
		{if (c_teletype) c_teletype->_DelPlayer(pid);}
    void _DelPlayer(DWORD pid);

private:
  inline void InitBuffer()
  {
	  memset(m_buffer, 0, sizeof(m_buffer));
	  m_index = 0;
  }
};

//----------------------------------------------------------------------------
#ifdef __GDB_INCLUDED__
class GDBT_MESSAGES : public GDB_TABLE {
public:
  static BYTE c_m;
public:
	GDBT_MESSAGES():GDB_TABLE(IDT_MESSAGES){};
    void SendMessage(char * msg, int length, BYTE freq, BYTE lang);
    void SendFreqData(WORD to_group);
private:
	static void chk(char *s, int l);
  	virtual void ReceiveUpdate(DPID from, DPID to, WORD proc, UDATA *data);
};
#endif

#define PUBLIC_CODE 0xaa
#define SECRET_CODE 0xab

#endif // __TELETYPE_INCLUDED__
