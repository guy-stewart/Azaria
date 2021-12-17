/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       textutil.h
;
; Description:
;	Implements the panel style puzzle.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 07/31/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifndef __TEXTUTIL_INCLUDED__
#define __TEXTUTIL_INCLUDED__


class CFONT {
private:
  int hFont;
  SPR_OBJECT *spr; // pointer to the font sprite.
  FBLT_DSCR fbd;	// handy fast blit descriptor
public:
    CFONT(int font_id);
    ~CFONT();

    int TextOut(ONE *surface, int color, int sx, int sy, const char *buff);
    void Print(ONE *surface, const char *buffer, char color)
  {
	fbd.m_dst_x += fbd.Print(surface, &spr->sprite, buffer, color);
  }

  int GetHeight() 
  {
	return spr->sprite.GetHeight()/spr->sprite.GetFrameCount();
  }

  void SetPos(int x, int y)
  {
    fbd.m_dst_x = x;
	fbd.m_dst_y = y;
  }

    void CR()
  {
	fbd.m_dst_x = 0;
	fbd.m_dst_y += GetHeight();
  }

    int GetX() {return fbd.m_dst_x;}
    int GetY() {return fbd.m_dst_y;}

  FBLT_DSCR *GetFBD() {return &fbd;}
};


class XFONT {
private:
  SPIRIT *m_Spirit;
public:
    XFONT(ONE *canvas, int ids) {
			    m_Spirit = new SPIRIT(canvas, ids);
      };

    int TextOut(ONE *canvas, int color, int sx, int sy, const char *buff) {
      return m_Spirit->TextOut(sx, sy, buff);
    }
};

#endif // __TEXTUTIL_INCLUDED__
