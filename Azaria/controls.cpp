/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       cfgview.cpp
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

#include "maya.h"
#include "textutil.h"
#include "controls.h"
#include "viewMast.h"
#include "cfgview.h"

#include "Data/Database/Ids.h"
#include "Data/Database/tbl_wave.h"

#include "user.h"

void SetRect(RECT * rect, int xLeft, int yTop, int xRight, int yBottom)
{
    rect->left = xLeft;
    rect->top = yTop;
    rect->right = xRight;
    rect->bottom = yBottom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CONTROL::CONTROL(CFGVIEW *xView, WORD xCid, WORD ids_background, int x, int y)
{
  view = xView;
  control_id = xCid;

  // this is the background of the control
  m_hSprite = sprSelect(ids_background);
  m_spr = sprGet(m_hSprite);
  m_fbd.SpriteInit(&m_spr->sprite);

  m_fbd.m_dst_x = x;
  m_fbd.m_dst_y = y;

  SetRect(&rect, 
		  x, 		// origin of the rectangle of the sprite on the surface.
		  y, 
		  x + m_fbd.m_dx,  // lower right of the rectangle.
		  y + m_fbd.m_dy);

  // paint on new image:
  m_fbd.Blt(&view->surface, &m_spr->sprite);
}

//----------------------------------------------------------------------------
CONTROL::~CONTROL()
{
  sprRelease(m_hSprite);
}

//----------------------------------------------------------------------------
BOOL CONTROL::PtInRect(POINT p)
{
    return sPtInRect(&rect, p);
}

//----------------------------------------------------------------------------
void CONTROL::Refresh()
{
  m_fbd.Blt(&view->surface, &m_spr->sprite);
  view->Invalidate();
}

//----------------------------------------------------------------------------
void CONTROL::SetStrValue(char *newStr)
{
  strcpy(StrVal, newStr);
  Refresh();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BUTTON::BUTTON(CFGVIEW *view,			// Surface on which to paint the button
			   int control_id,
			   int x, int y,		// position on the canvas
			   WORD ids_image,		// image of the button
			   WORD ids_image_hi)	// image of the highlighted button
:CONTROL(view, control_id, ids_image, x, y)
{
  sprBtn = LoadBtnImage(&view->surface, view->view_id, ids_image, x, y);
  sprHilight = LoadBtnImage(&view->surface, view->view_id, ids_image_hi, x, y);

  sprBtn->ON();
}

BUTTON::~BUTTON()
{
  delete sprBtn;
  delete sprHilight;
}

//----------------------------------------------------------------------------
SPIRIT *BUTTON::LoadBtnImage(ONE *canvas, 
							 WORD view_id,
							 WORD sprite_id, 
							 int x, 
							 int y)
{
  SPIRIT *spirit;

  if (canvas->m_hdr.user_flags & 1)
	  spirit = new SPIRIT(canvas, sprite_id, BLT_LENSED);
  else
	  spirit = new SPIRIT(canvas, sprite_id, BLT_PORTAL);

  if (spirit)
  {
	  spirit->Move(x, y, 0);
	  spirit->Animate(view_id, TRUE /* LOOP */);
  }

  return spirit;
}

//----------------------------------------------------------------------------
WORD BUTTON::Grab(POINT p)
{
    Wave::Play(SOUND_BTNPRESS);
	view->OnEvent(control_id, 0);
	return 0;
}

//----------------------------------------------------------------------------
int BUTTON::Keycmd(int key, int flags)
{
  return key;
}

//----------------------------------------------------------------------------
void BUTTON::Refresh()
{
  CONTROL::Refresh();
}

//----------------------------------------------------------------------------
void BUTTON::SetFocus(POINT p, BOOL bHasFocus)
{
  if (bHasFocus)
  {
    Wave::Play(SOUND_BTNDRAG);
	sprBtn->OFF();
	sprHilight->ON();
  }
  else
  {
	sprHilight->OFF();
	sprBtn->ON();
  }

  view->Invalidate();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CHECKBOX::CHECKBOX(CFGVIEW *view,	// Surface on which to paint the button
			   int control_id,
			   int x, int y,		// position on the canvas
			   WORD ids_image,		// image of the button
			   WORD ids_image_hi):	// image of the highlighted button
BUTTON(view, control_id, x, y, ids_image, ids_image_hi)
{
  bIsChecked = FALSE;
}

//----------------------------------------------------------------------------
void CHECKBOX::SetFocus(POINT p, BOOL bHasFocus)
{
  if (bHasFocus)
  {
    Wave::Play(SOUND_BTNDRAG);
  }
}

//----------------------------------------------------------------------------
void CHECKBOX::SetChecked(BOOL bChecked)
{
  bIsChecked = bChecked;

  if (bIsChecked)
  {
	sprBtn->OFF();
	sprHilight->ON();
  }
  else
  {
	sprHilight->OFF();
	sprBtn->ON();
  }

  view->Invalidate();
}

//----------------------------------------------------------------------------
WORD CHECKBOX::Grab(POINT p)
{
  Wave::Play(SOUND_BTNPRESS);

  SetChecked(!bIsChecked);

  view->OnEvent(control_id, bIsChecked);
  return 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
LISTBOX::LISTBOX(CFGVIEW *xView, 
				 WORD ids, 
				 int x, int y, 
				 int xo, int yo,	// offset of text into the control bitmap
				 int xCount, 
				 const STRVAL xStrVal):
CONTROL(xView, 0, ids, x, y),
font(IDS_FONTTNB14)
{
  IntVal = &Selected; // control returns value of selected item
  left = x+xo;
  top = y+yo;

  m_count = xCount;
  StrFunc = xStrVal;

  m_row_height = font.GetHeight();

  Init();
}

//----------------------------------------------------------------------------
LISTBOX::~LISTBOX()
{
}

//----------------------------------------------------------------------------
WORD LISTBOX::Grab(POINT p)
{
  Wave::Play(SOUND_CLICK);
  int row = (p.y - top)/m_row_height;
  ChangeSel(row);
  return FALSE;
}

//----------------------------------------------------------------------------
void LISTBOX::Write(int position, COLORREF color)
{
	char *szDesc;
	int y;

	szDesc = StrFunc(position);
	if (!szDesc)
		return;

	y = position * (m_row_height) + top;
	font.TextOut(&view->surface, BLACK, left+1, y+1, szDesc);
	font.TextOut(&view->surface, color, left, y, szDesc);

	view->Invalidate();
}

//----------------------------------------------------------------------------
void LISTBOX::Init()
{
	Selected = 0;
	Refresh();
}

//----------------------------------------------------------------------------
void LISTBOX::ChangeSel(int new_selection)
{
	Write(Selected, CLR_UNSELECTED);
	Selected = new_selection;
	Write(Selected, CLR_SELECTED);
}

//----------------------------------------------------------------------------
int LISTBOX::Keycmd(int input, int flags)
{
	if (flags & KEY_RELEASED)
		return 0;

   switch( input )
    {
    case KEY_UP: 
    case KEY_LEFT:
	  Wave::Play(SOUND_CLICK);
		if (Selected > 0)
			ChangeSel(Selected-1);
		input = 0; // handled
        break;

    case KEY_DOWN:
    case KEY_RIGHT:
	  Wave::Play(SOUND_CLICK);
		if (Selected < (m_count-1))
			ChangeSel(Selected+1);
		input = 0; // handled
        break;
    }
   return input;
}

//----------------------------------------------------------------------------
void LISTBOX::Refresh()
{
  CONTROL::Refresh();
  for (int i=0;i<m_count;i++)
  {
	  Write(i, CLR_UNSELECTED);
  }
  if (m_count && Selected < m_count)
	Write(Selected, CLR_SELECTED);
}

//----------------------------------------------------------------------------
void LISTBOX::SetFocus(POINT p, BOOL bHasFocus)
{
}


//----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
// EDIT BOX STUFF
////////////////////////////////////////////////////////////////////////////
EDITBOX::EDITBOX(CFGVIEW *view, 
				 WORD ids, 
				 int x, int y, 
				 int xo, int yo, 
				 char *inital_value, 
				 int max):
CONTROL(view, 0, ids, x, y),
font(IDS_FONTTNB18)
{
  StrVal = BUFF;

  left = x+xo;
  top = y+yo;

  strcpy(BUFF, inital_value);
  LEN = strlen(BUFF);
  MAXLEN = max;
  Write(BUFF, CLR_SELECTED);
}

//----------------------------------------------------------------------------
void EDITBOX::Write(char *buff, int color)
{
	font.TextOut(&view->surface, BLACK, left+1, top+1, buff);
	font.TextOut(&view->surface, color, left, top, buff);
	view->Invalidate();
}

//----------------------------------------------------------------------------
int EDITBOX::Keycmd(int input, int flags)
{
	if (flags & KEY_RELEASED)
		return 0;

	if (((input >= '0') && (input <= '9'))	// 0..9
	 || (input == ' ')
	 || ((input >= 'a') && (input <='z'))	// a..z
	 || ((input >= 'A') && (input <='Z')))	// A..Z
	{
		if (LEN < MAXLEN)
		{
			BUFF[LEN] = input;
			LEN++;
			BUFF[LEN] = 0;
			Write(BUFF, CLR_SELECTED);
		}
	   input = 0; // handled.
	}
	else
	{
		switch (input)
		{
		case KEY_UP: 
		case KEY_LEFT:
  		case KEY_BACK:
			if (LEN > 0)
			{
				Write(BUFF, CLR_ERASE);
				LEN--;
				BUFF[LEN] = 0;
				Refresh();
			}
			input = 0; // handled.
			break;

		case KEY_DOWN:
		case KEY_RIGHT:
			input = 0; // handled.
			break;
		}
	}

   return input;
}

//----------------------------------------------------------------------------
WORD EDITBOX::Grab(POINT p)
{
  return FALSE;
}

//----------------------------------------------------------------------------
void EDITBOX::Refresh()
{
  CONTROL::Refresh();
  Write(BUFF, CLR_SELECTED);
}

//----------------------------------------------------------------------------
void EDITBOX::SetFocus(POINT p, BOOL bHasFocus)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SLIDER::SLIDER(CFGVIEW *view,			// Surface on which to paint the button
				int control_id,
				int _x, int _y,		// position on the canvas
				int xvalue,
				int _width,
				int xgranularity,
				int xmin, int xmax,
				WORD ids_image,		// image of the button
				WORD ids_image_hi)	// image of the highlighted button
:CONTROL(view, control_id, ids_image, x, _y),
left(view, 0, _x, _y, IDS_BTN_LEFT,  IDS_BTN_LEFT_HI),
right(view, 1, _x+_width, _y, IDS_BTN_RIGHT, IDS_BTN_RIGHT_HI),
thumb(&view->surface, IDS_BTN_THUMB, BLT_PORTAL),
bLeftFocus(FALSE)
{
  IntVal = &value;

  value = xvalue;
  min = xmin;
  max = xmax;
  granularity = xgranularity;
  x = _x + left.GetWidth();
  y = _y;
  width = _width - left.GetWidth() - thumb.GetWidth();

  thumb.Move(x+(value*width/(max-min)), y, 0);
  thumb.Animate(view->view_id, TRUE /* LOOP */);
  thumb.ON();
  view->Invalidate();
}

//----------------------------------------------------------------------------
WORD SLIDER::Grab(POINT p)
{
  if (left.PtInRect(p))
  {
	if (value > (min+granularity))
	  value -= granularity;
	else
	  value = min;
  }
  else
  {
	if (value < (max-granularity))
	  value += granularity;
	else
	  value = max;
  }

  Wave::Play(SOUND_CLICK);

  thumb.OFF();
  thumb.Move(x+(value*width/(max-min)), y, 0);
  thumb.ON();

  view->OnEvent(control_id, value);
  view->Invalidate();

  return FALSE;
}

//----------------------------------------------------------------------------
void SLIDER::SetFocus(POINT p, BOOL bHasFocus)
{
  if (left.PtInRect(p) && bHasFocus)
  {
	left.SetFocus(p, bHasFocus);
	bLeftFocus = TRUE;
  }
  else if (bLeftFocus)
  {
	bLeftFocus = FALSE;
	left.SetFocus(p, bHasFocus);
  }
  else
  {
	right.SetFocus(p, bHasFocus);
  }
}

//----------------------------------------------------------------------------
int SLIDER::Keycmd(int key, int flags)
{
  return key;
}

//----------------------------------------------------------------------------
BOOL SLIDER::PtInRect(POINT p)
{
  return left.PtInRect(p) || right.PtInRect(p);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
LABEL::LABEL(CFGVIEW *view,			// Surface on which to paint the button
			   int control_id,
			   int x, int y,		// position on the canvas
			   WORD ids_image)		// image of the label
:CONTROL(view, control_id, ids_image, x, y)
{
}

WORD LABEL::Grab(POINT p)
{
  return FALSE;
}

//----------------------------------------------------------------------------
void LABEL::SetFocus(POINT p, BOOL bHasFocus)
{
}

//----------------------------------------------------------------------------
int LABEL::Keycmd(int key, int flags)
{
  return key;
}
