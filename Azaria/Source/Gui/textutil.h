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


// Cursor with a font:
class CFONT {
private:
  SPIRIT *sprite;
  int x, y;
public:
    CFONT(ONE *canvas, int font_id) {
      	sprite = new SPIRIT(canvas, font_id);
    };
    ~CFONT() {
      delete sprite;
    };

    int TextOut(ONE *surface, int color, int sx, int sy, const char *buff) {
      //----------------------------------------------------------------------------
      x = sx;
      y = sy;
      x += sprite->TextOut(sx, sy, buff);
    }

  void Print(ONE *surface, const char *buff, char color)
  {
      sprite->GetImage()->FillForeground(color);
      x += sprite->TextOut(x, y, buff);
  }

  int GetHeight() 
  {
	  return sprite->GetHeight()/sprite->FrameCount();
  }

  void SetPos(int sx, int sy)
  {
    x = sx;
    y = sy;
  }

  void CR()
  {
     x = 0;
	   y += GetHeight();
  }

  int GetX() {return x;}
  int GetY() {return y;}
};

#endif // __TEXTUTIL_INCLUDED__
