/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       scrollview.c
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/28/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "maya.h"
#include "btable.h"
#include "Data/Database/idv.h"
#include "Data/Database/Ids.h"
#include "user.h"
#include "player.h"
#include "scrollview.h"
#include "trigger.h"

#define ACCEL_FACTOR		0.4		// units/millisecond
#define FRICTION_FACTOR		0.4		// units/millisecond. accel_factor must be multiple of friction_factor.
#define TERMINAL_VELOCITY	400		// units/second

//----------------------------------------------------------------------------
SCROLLVIEW::SCROLLVIEW(int view_id):
VIEW(view_id, "scrollview", BLT_PORTAL)
{
	m_accx=0;
	m_accy=0;
	m_posx=0;
	m_posy=0;
	m_oposx=0;
	m_oposy=0;
	m_key_control = FALSE;

	if (view_id == IDV_CONTINENT)
	{
  	  POINT p = local_player->GetPosContinent();
	  m_posy=p.y;
	  m_posx=p.x;
	}

	m_px.Init(ACCEL_FACTOR, TERMINAL_VELOCITY, surface.GetWidth() - viewWidth(), PHY_COMP_ETIME, FRICTION_FACTOR, m_posx);
	m_py.Init(ACCEL_FACTOR, TERMINAL_VELOCITY, surface.GetHeight() - viewHeight(),PHY_COMP_ETIME, FRICTION_FACTOR, m_posy);
}

//----------------------------------------------------------------------------
void SCROLLVIEW::SlewRel(long x, long y)
{
  if (x>=0)
  {
	if (fbd.m_src_x + x < surface.GetWidth())
	  x = fbd.m_src_x + x;
	else
	  x = surface.GetWidth();
  }
  else
  {
	 if (-x < fbd.m_src_x)
	   x = fbd.m_src_x + x;
	 else
	   x = 0;
  }

  if (y>=0)
  {
	if (fbd.m_src_y + y < surface.GetHeight())
	  y = fbd.m_src_y + y;
	else
	  y = surface.GetHeight();
  }
  else
  {
	 if (-y < fbd.m_src_y)
	   y = fbd.m_src_y + y;
	 else
	   y = 0;
  }

  SlewTo(x,y);
}
//----------------------------------------------------------------------------
void SCROLLVIEW::SlewTo(WORD x, WORD y)
{
  m_slewing = TRUE;

  m_slewtarget.x = x;
  m_slewtarget.y = y;

  if (x > m_posx)
	m_accx=+1;
  else if (x < m_posx)
	m_accx=-1;

  if (y > m_posy)
	m_accy=+1;
  else if (y < m_posy)
	m_accy=-1;

}

//----------------------------------------------------------------------------
int SCROLLVIEW::Action(int etime, int flags)
{
	m_posx = m_px.accelerate(m_accx, etime);
	m_posy = m_py.accelerate(m_accy, etime);

	if (m_posx != m_oposx || m_posy != m_oposy)
	{
		m_oposx = m_posx;
		m_oposy = m_posy;

		if (m_slewing)
		{
		  if ((m_accx > 0 && m_posx > m_slewtarget.x)
		  ||  (m_accx < 0 && m_posx < m_slewtarget.x)) // we have achieved our target
		  {
			m_posx > m_slewtarget.x;
			m_accx = 0;
			m_px.BrickWall();
		  }

		  if ((m_accy > 0 && m_posy > m_slewtarget.y)
		  ||  (m_accy < 0 && m_posy < m_slewtarget.y)) // we have achieved our target
		  {
			m_posy > m_slewtarget.y;
			m_accy = 0;
			m_py.BrickWall();
		  }

		  if (!m_accx && !m_accy)
			m_slewing = FALSE;
		}

		fbd.m_src_x = (short)m_posx;
		fbd.m_src_y = (short)m_posy;

		InvalidateOffset();

		// 
		if (view_id == IDV_CONTINENT)
		{
		  POINT p;
		  p.x = m_posx;
		  p.y = m_posy;
  		  local_player->SetPosContinent(p);
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------
WORD SCROLLVIEW::QueryCursor(SRECT r)
{
  WORD ids = IDS_HANDIDLE;
  WORD vids = VIEW::QueryCursor(r);
  POINT p;
  p.x = r.left;
  p.y = r.top;

  int thickness_x = (fbd.m_brect.right - fbd.m_brect.left) / 4;
  int thickness_y = (fbd.m_brect.bottom - fbd.m_brect.top) / 4;

  if (!m_key_control)
  {
	m_accx=0;
	m_accy=0;

	if (r.left < (fbd.m_brect.left + thickness_x))
	{
		m_accx=-1;
		ids = IDS_HANDLEFT;
		int t = (fbd.m_brect.left+thickness_x);
		m_px.SetTerminal(TERMINAL_VELOCITY*(t-r.left)/(thickness_x));
	}
	else if (r.right > (fbd.m_brect.right - thickness_x))
	{
		ids = IDS_HANDRIGHT;
		m_accx=1;
		int t = (fbd.m_brect.right-thickness_x);
		m_px.SetTerminal(TERMINAL_VELOCITY*(r.right-t)/(thickness_x));
	}

	if (r.top < (fbd.m_brect.top + thickness_y))
	{
		m_accy=-1;
		ids = IDS_HANDUP;
		int t = (fbd.m_brect.top+thickness_y);
		m_py.SetTerminal(TERMINAL_VELOCITY*(t-r.top)/(thickness_y));
	}
	else if (r.bottom > (fbd.m_brect.bottom - thickness_y))
	{
		m_accy=1;
		ids = IDS_HANDDOWN;
		int t = (fbd.m_brect.bottom-thickness_y);
		m_py.SetTerminal(TERMINAL_VELOCITY*(r.bottom-t)/(thickness_y));
	}
  }

  if (!vids || vids == IDS_HANDIDLE)
	vids = ids;

  return vids;
}

//----------------------------------------------------------------------------
int  SCROLLVIEW::Keycmd(int key, int flags)
{
  if (!key)
	return key;

	if (flags & KEY_PRESSED)
	{
	    switch( key )
	    {
	    case KEY_LEFT:
			m_accx=-1;
			key = 0;
			m_key_control = TRUE;
	        break;
	    case KEY_RIGHT:
			m_accx=1;
			key = 0;
			m_key_control = TRUE;
	        break;
	    case KEY_UP:
			m_accy=-1;
			key = 0;
			m_key_control = TRUE;
	        break;
	    case KEY_DOWN:
			m_accy=1;
			key = 0;
			m_key_control = TRUE;
	        break;
		case KEY_CLEAR: // num pad 5 w/num-lock off.
			m_accx=0;
			m_accy=0;
			key = 0;
	        break;
	    }
	}
	else
	{
	    switch( key )
	    {
	    case KEY_LEFT:
			m_accx=0;
			key = 0;
	        break;
	    case KEY_RIGHT:
			m_accx=0;
			key = 0;
	        break;
	    case KEY_UP:
			m_accy=0;
			key = 0;
	        break;
	    case KEY_DOWN:
			m_accy=0;
			key = 0;
	        break;
		case KEY_CLEAR: // num pad 5 w/num-lock off.
			key = 0;
	        break;
	    }
	}

	return key;
}

