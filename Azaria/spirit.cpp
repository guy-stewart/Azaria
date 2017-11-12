
/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       spirit.c
;
; Description:
;	Implements the spirit class.
;	Spirits only live within the game.  They do not exists within the database.
;	Spirits are the manifestations of objects that exists in the database.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/31/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "maya.h"
#include "viewMast.h"
#include "object.h"
#include "viewman.h"

BOOL SPIRIT::c_monitor = FALSE;

//----------------------------------------------------------------------------
SPIRIT::SPIRIT(	 ONE *canvas,
//				 VIEW *view,
				 int sprite_id,
				 int style)
{
  // save a pointer to the canvas so we can use it for later blitting.
  m_canvas = canvas;

  // animation initialization:
  m_anindex = -1; // default = don't animate
  m_i_own_it = FALSE;
  bMoreFrames = TRUE;
  m_Deanimate = FALSE;
  m_Rewinding = FALSE;

	// get a pointer to the ram sprite
	m_hSprite = sprSelect(sprite_id);
	m_spr = sprGet(m_hSprite);

	m_fbd.SpriteInit(&m_spr->sprite);
	m_fbd.MaskInit(&m_spr->sprite);

	m_pos.x = 0;
	m_pos.y = 0;

	m_on = FALSE;

	m_bone = m_spr->sprite; // copy the image header info

	// depending on the style of the view,
	// the sprite may need to be rotated during blitting ...
	switch (style)
	{
	case BLT_LENSED:
		// The sprite is blitted sideways ...
  		m_fbd.m_style = BLT_FLIPPED;
		m_bone.Init(m_spr->sprite.GetHeight(), m_spr->sprite.GetWidth());
		m_fbdBones.m_style = BLT_RECTFLIP;
		m_fbdBones.m_dy = m_fbd.m_dx;
		m_fbdBones.m_dx = m_fbd.m_dy;
		break;
	default:
  		m_fbd.m_style = style;
		m_bone.Init(m_spr->sprite.GetWidth(), m_spr->sprite.GetHeight());
		m_fbdBones.m_style = BLT_RECT;
		m_fbdBones.m_dx = m_fbd.m_dx;
		m_fbdBones.m_dy = m_fbd.m_dy;
		break;
	}
}

//----------------------------------------------------------------------------
SPIRIT::~SPIRIT()
{
  DeAnimate();

  sprRelease(m_hSprite);
  
  if (m_bone.getBits())
	m_bone.freeBits();
}

//----------------------------------------------------------------------------
void SPIRIT::Move(int x, int y, int flags)
{
	if (!this)
		return;

	m_pos.x = x;
	m_pos.y = y;
}

//----------------------------------------------------------------------------
void SPIRIT::SetFrame(int frame)
{
  m_fbd.m_frame = frame;
}

//----------------------------------------------------------------------------
void SPIRIT::ON()
{
	assert(this);
	if (!this)
		return;

	// save the background ...
	m_fbdBones.BonesOff(m_pos.x, m_pos.y);
	m_fbdBones.Blt(&m_bone, m_canvas);

	// current position is contained in fbd(x,y)
	m_fbd.m_dst_x = m_pos.x;
	m_fbd.m_dst_y = m_pos.y;

	// paint on new image:
	m_fbd.Blt(m_canvas, &m_spr->sprite);

	m_on = TRUE;
}

//----------------------------------------------------------------------------
void SPIRIT::OFF()
{
	assert(this);
	if (m_on)  // don't replace the background if we are not on the canvas:
	{
		// replace the background:
  		m_fbdBones.BonesOn(m_fbd.m_dst_x, m_fbd.m_dst_y);
		m_fbdBones.Blt(m_canvas,&m_bone);

		m_on = FALSE;
	}
}

//----------------------------------------------------------------------------
void SPIRIT::GetRect(RECT *rect)
{
	SetRect(rect, 
			m_pos.x, 		// origin of the rectangle of the sprite on the surface.
			m_pos.y, 
			m_pos.x + m_fbd.m_dx,  // lower right of the rectangle.
			m_pos.y + m_fbd.m_dy);
}

//----------------------------------------------------------------------------
BOOL SPIRIT::PtInSpirit(POINT p)
{
	RECT rect;
	GetRect(&rect);
	return sPtInRect(&rect, p);
}

//----------------------------------------------------------------------------
void SPIRIT::DeAnimate()
{
  if (m_anindex >=0)
  { // if animated, remove from the animation list:
	c_count--;
	if (m_anindex != c_count)
	{ // pull some other spirit into the vacant slot:
	  c_pool[m_anindex] = c_pool[c_count];
	  c_view[m_anindex] = c_view[c_count];
	  c_pool[m_anindex]->m_anindex = m_anindex;
	}
	m_anindex = -1;
  }
}

//----------------------------------------------------------------------------
void SPIRIT::Animate(WORD view_id, BOOL bLoop, BOOL bStartFromEnd, BOOL bRewind, BOOL bDestroy)
{
  if (c_count >= MAX_ANIMATIONS)
  {
	Msg("Animation pool is full");
	return;  // the pool is full.
  }
  if (m_spr->sprite.GetFrames() <= 1)
  {
	if (c_monitor) Msg("Not an animation sequence");
	return; // not an animation sequence
  }
  if (m_anindex != -1)
  {
	if (c_monitor) Msg("Spirit is already animating");
	return; // already animating
  }

  if (c_monitor) Msg("Animating %s", m_spr->name);
  m_Loop = bLoop;
  m_Rewind = bRewind;
  m_StartFromEnd = bStartFromEnd;
  if (m_StartFromEnd)
  {
	  SetFrame(m_spr->sprite.GetFrames()-1);
	  m_Rewinding = TRUE;
  }
  else
  {
	  SetFrame(0);
	  m_Rewinding = FALSE;
  }

  if (c_monitor) Msg("Placing on que at position %d", c_count);
  m_anindex = c_count;
  c_pool[m_anindex] = this;
  c_view[m_anindex] = view_id;
  c_count++;
}

//----------------------------------------------------------------------------
void SPIRIT::Video(WORD view_id, WORD x, WORD y, 
				   WORD ids, 
				   BOOL bStartFromEnd, BOOL bLoop, BOOL bRewind)
{
	VIEW *view = viewman.GetView(view_id);
	SPIRIT* Spirit;

	if (view)
	{
		ONE *canvas = &view->surface;
		if (canvas->m_hdr.user_flags & 1)
			Spirit = new SPIRIT(canvas, ids, BLT_LENSED);
		else
			Spirit = new SPIRIT(canvas, ids, BLT_PORTAL);

		if (Spirit)
		{
			Spirit->m_i_own_it = TRUE;
			Spirit->Move(x, y, 0);

			Spirit->Animate(view_id, bLoop, bStartFromEnd, bRewind, TRUE);
		}
	}
}

//----------------------------------------------------------------------------
void SPIRIT::VideoCut(WORD view_id)
{
  int i;

  for (i=0;i<c_count;i++)
  {
	if ((c_view[i] == view_id)
	  &&(c_pool[i]->m_i_own_it))
	{
	  delete c_pool[i];
	  i--;
	}
  }
}

//----------------------------------------------------------------------------
BOOL SPIRIT::SetNextFrame(int etime)
{
	if (m_Rewinding)
	{
	  bMoreFrames = (CurrFrame() > 0);
	  if (!bMoreFrames)
	  {
		if (m_Loop)
		{
		  bMoreFrames = TRUE; // looping never fails.
		  if (m_Rewind)
		  {
			m_Rewinding = FALSE;
			SetFrame(0); // frame 0 is currently showing
		  }
		  else
		    SetFrame(LastFrame());
		}
		else
		{
			if (m_i_own_it)
				return TRUE;
			else
			  c_bDeanimate = m_Deanimate = TRUE;
		}
	  }
	  else
		return m_fbd.PrevFrame(&m_spr->sprite, etime, FALSE);
	}
	else
	{
	  bMoreFrames = (CurrFrame() < LastFrame());
	  if (!bMoreFrames)
	  {
		if (m_Loop)
		{
		  bMoreFrames = TRUE; // looping never fails.
		  if (m_Rewind)
		  {
			m_Rewinding = TRUE;
		    SetFrame(LastFrame()); // last frame is currently showing
		  }
		  else
			SetFrame(0);
		}
		else if (m_Rewind && !m_StartFromEnd)
		{
			bMoreFrames = TRUE; // looping never fails.
			m_Rewinding = TRUE;
		    SetFrame(LastFrame()); // last frame is currently showing
		}
		else 
		{
			if (m_i_own_it)
				return TRUE;
			else
			  c_bDeanimate = m_Deanimate = TRUE;
		}
	  }
	  else
		return m_fbd.NextFrame(&m_spr->sprite, etime, FALSE);
	}

	return FALSE;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BOOL SPIRIT::c_bDeanimate;
SPIRIT*  SPIRIT::c_pool[MAX_ANIMATIONS];
int SPIRIT::c_view[MAX_ANIMATIONS];
int SPIRIT::c_count;

//----------------------------------------------------------------------------
void SPIRIT::Tick(int etime) 
{
  int i;

  for (i=0;i<c_count;i++)
  {
	if (c_pool[i]->SetNextFrame(etime))
	{
		viewman.Invalidate(c_view[i], TRUE, TRUE);
	}
  }

  if (c_bDeanimate)
  {
	c_bDeanimate = FALSE;
	for (i=c_count-1;i>=0;i--)
	{
	  if (c_pool[i]->m_Deanimate)
	  {
		  c_pool[i]->DeAnimate();
	  }
	}
  }
}

//----------------------------------------------------------------------------
void SPIRIT::Paint(WORD view_id) 
{
  int i;

  for (i=0;i<c_count;i++)
  {
	if ((c_view[i]==view_id) && c_pool[i]->m_i_own_it)
	{
	  c_pool[i]->OFF();
	  if (c_pool[i]->bMoreFrames)
	  {
		  c_pool[i]->ON();
	  }
	  else 
	  {
		delete c_pool[i];
		i--;
	  }
	}
  }
}

//----------------------------------------------------------------------------
void SPIRIT::VideoON(WORD view_id) 
{
  int i;

  for (i=0;i<c_count;i++)
  {
	if ((c_view[i]==view_id) && c_pool[i]->m_i_own_it)
	{
	  if (c_pool[i]->bMoreFrames)
	  {
		  c_pool[i]->ON();
	  }
	  else 
	  {
		delete c_pool[i];
		i--;
	  }
	}
  }
}

//----------------------------------------------------------------------------
void SPIRIT::VideoOFF(WORD view_id) 
{
  int i;

  for (i=0;i<c_count;i++)
  {
	if ((c_view[i]==view_id) && c_pool[i]->m_i_own_it)
	{
	  c_pool[i]->OFF();
	}
  }
}

