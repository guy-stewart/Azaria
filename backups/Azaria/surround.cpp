/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       surround.c
;
; Description:
;	Implements the surround style puzzle.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/28/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include <math.h>
#include "maya.h"
#include "btable.h"
#include "user.h"
#include "object.h"
#include "surround.h"
#include "player.h"
#include "spell.h"
#include "trigger.h"

#include "Data/Database/Ids.h"

#define USE_PARTICLE_PHYSICS TRUE

#if USE_PARTICLE_PHYSICS
	#define TERMINAL_VELOCITY	600		// pixels/frame OR pixels/second
	#define X_ACCEL_FACTOR		1.0		// pixels/millisecond/millisecond
	#define X_FRICTION			1.0
#else
	#define TERMINAL_VELOCITY	30  // pixels/frame
	#define X_ACCEL_FACTOR		1	// pixels/frame/frame
#endif

//----------------------------------------------------------------------------
int SURROUND::Action(int etime, int flags)
{
  // dizzy testing:
  static int dizziness = 0;

  // ### quick hack to get players to follow one another in surround views:
  if (local_player->GetFollow() != 0)
  {
	if (isMainViewScreen())
	{
		// set the orientation:
		m_posx = (local_player->GetTheta()*surface.GetHeight())/36000;
		// center the offset in the viewport:
		m_posx -= viewWidth() / 2;
		if (m_posx<0) m_posx += surface.GetHeight();
		//m_px.Init(X_ACCEL_FACTOR, TERMINAL_VELOCITY, surface.GetHeight(), PHY_COMP_ETIME|PHY_UNIVERSE_WRAPS, X_FRICTION, m_posx);
	}
  }

  else
  {
	// orient the user view direction
#if USE_PARTICLE_PHYSICS
	 m_posx = m_px.accelerate(m_accx, etime);
#else
	if (m_vx && !m_accx)
	{
		m_vx -= (m_vx/abs(m_vx))*X_ACCEL_FACTOR;
	}
	else if ((abs(m_vx) < TERMINAL_VELOCITY) && (m_accx))
		m_vx += m_accx*X_ACCEL_FACTOR;

	m_posx += m_vx;

	if (m_posx>=surface.height)
		m_posx = 0;
	if (m_posx<0)
		m_posx += surface.height;
#endif
  }

	if (m_posx != m_opos)
	{
		fbd.m_src_x = (short)m_posx;
		m_opos = m_posx;

		InvalidateOffset();

		if (isMainViewScreen())
		{
		  // Are we getting dizzy???
		  dizziness += etime;
		  if (dizziness > 200000) // this will take about 10 seconds
		  {
			WIZARD::Cast(SPELL_SPIN, TARGET_ME);
			dizziness = 0;
		  }

		  // Get coord of center of viewport:
		  m_posx += viewWidth() / 2;
		  if (m_posx>surface.GetHeight()) m_posx -= surface.GetHeight();

  		  // if main viewscreen then update remote players in our view:
		  if (local_player->GetFollow() == 0)
		  {
			  if (surface.GetHeight())
  					local_player->SetTheta((WORD)((m_posx * 36000)/surface.GetHeight()));
		  }
		  else
  				local_player->SetTheta(local_player->GetTheta());
		}
	}
	else if (etime > 20)
	  dizziness = 0;

	return FALSE;
}

//----------------------------------------------------------------------------
/*
 * ProcessInput
 */
int SURROUND::Keycmd(int key, int flags)
{
  int theta = (m_mousepos.x * 36000)/surface.GetHeight();

	if (flags & KEY_PRESSED)
	{
	    switch( key )
	    {
		case KEY_UP:
  			Cardinal(view_id, theta, FALSE, TRUE);
			break;
		case KEY_DOWN:
  			Cardinal(view_id, theta, TRUE, TRUE);
			break;
	    case KEY_LEFT:
			m_accx=-1;
			key = 0;
	        break;
	    case KEY_RIGHT:
			m_accx=1;
			key = 0;
	        break;
		case KEY_CLEAR: // num pad 5 w/num-lock off.
			m_accx=0;
			key = 0;
	        break;
	    }
	}
	else
	{
	    switch( key )
	    {
		case KEY_UP:
		case KEY_DOWN:
	    case KEY_LEFT:
	    case KEY_RIGHT:
		case KEY_CLEAR: // num pad 5 w/num-lock off.
			m_accx=0;
			key = 0;
	        break;
	    }
	}

  return key;
} /* ProcessInput */

//----------------------------------------------------------------------------
WORD SURROUND::QueryCursor(SRECT r)
{
  WORD ids = IDS_HANDIDLE;
  WORD vids = VIEW::QueryCursor(r);
  POINT p;
  p.x = r.left;
  p.y = r.top;

  int thickness_x = (fbd.m_brect.right - fbd.m_brect.left) / 4;
  int thickness_y = (fbd.m_brect.bottom - fbd.m_brect.top) / 3;

  m_mousepos = MapToCanvas(p);
  int theta = (m_mousepos.x * 36000)/surface.GetHeight();

  if (p.y < (fbd.m_brect.top + thickness_y*2))
  {
    m_accx=0;
	if (Cardinal(view_id, theta, FALSE, FALSE))
		ids = IDS_HANDFWD;
  }
  else if (p.y > (fbd.m_brect.bottom - thickness_y))
  {
    m_accx=0;
	if (Cardinal(view_id, theta, TRUE, FALSE))
		ids = IDS_HANDBACK;
  }

  {
	// if near the top or bottom of the view ...
	if (p.y < (fbd.m_brect.top + thickness_y/2)
	||  p.y > (fbd.m_brect.bottom - thickness_y))
		thickness_x /= 2; // ... then skinny up the scroll control

	if (r.left < (fbd.m_brect.left + thickness_x))
	{
		m_accx=-1;
		int t = (fbd.m_brect.left+thickness_x);
		m_px.SetTerminal(TERMINAL_VELOCITY*(t-r.left)/(thickness_x));
		return IDS_HANDLEFT;
	}
	else if (r.right > (fbd.m_brect.right - thickness_x))
	{
		m_accx=1;
		int t = (fbd.m_brect.right-thickness_x);
		m_px.SetTerminal(TERMINAL_VELOCITY*(r.right-t)/(thickness_x));
		return IDS_HANDRIGHT;
	}
  }

  if (!vids || vids == IDS_HANDIDLE)
	vids = ids;

  return vids;
}

//----------------------------------------------------------------------------
void SURROUND::Paint(int etime, int flags)
{
  static CARDINALS hands;
  int i;

  // remove card hands:
  for (i=0;i<8;i++)
  {
	hand[i]->OFF();
  }

  VIEW::Paint(etime, flags);

  // restore card hands:
  TBL_CARDINALS::CardHands(view_id, &hands);
  for (i=0;i<8;i++)
  {
	if (hands.m_cardinal[i])
			hand[i]->ON();
  }
}

//----------------------------------------------------------------------------
SURROUND::~SURROUND()
{
  for (int i = 0; i<8; i++)
  {
	delete hand[i];
  }
}

//----------------------------------------------------------------------------
SURROUND::SURROUND(int view_id):
VIEW(view_id, "surround", BLT_LENSED)
{
  // set the orientation:
  m_posx = (local_player->GetTheta()*surface.GetHeight())/36000;
  // center the offset in the viewport:
  m_posx -= viewWidth() / 2;
  if (m_posx<0) m_posx += surface.GetHeight();

  InvalidateOffset();

  for (int i = 0; i<8; i++)
  {
	hand[i] = new SPIRIT(&surface, IDS_LINKHAND, BLT_LENSED);
	hand[i]->Move((i*surface.GetHeight())/8,150,0);
  }

  m_vx = 0;
  m_accx=0;
  m_opos=0;

  fbd.m_dy = viewWidth();
  fbd.m_dx = viewHeight();

  // initialize the lens:
  fbd.lensInit(surface.GetHeight(), fbd.m_dy);
  surface.DoTheDirty();

#if USE_PARTICLE_PHYSICS
	m_px.Init(X_ACCEL_FACTOR, TERMINAL_VELOCITY, surface.GetHeight(), PHY_COMP_ETIME|PHY_UNIVERSE_WRAPS, X_FRICTION, m_posx);
#endif

}
