/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       physics.c
;
; Description:
;	Provides physics behaviors for views and objects.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 03/17/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#include "physics.h"

// stop the particle abruptly.
void PARTICLE::BrickWall()
{
  m_v = 0;
}

// compute the new velocity && position of a particle
// and return the position
int PARTICLE::accelerate(int acceleration, int real_etime)
{
	int etime = real_etime;
	int etime_acc = real_etime;
	int applied_v;
	if (!etime)
		return m_pos;

	// If the caller wants to run in real time:
	if (m_flags & PHY_COMP_ETIME)
	{
		// round etime to the nearest 10
		etime += 5;
		etime /= 10;
		etime *= 10;

		m_etime += etime;
	}
	else
	{
		etime = 20;
	}

	// if the particle is moving but not accelerating,
	if (m_v>0 && acceleration < 0)
		applied_v -=1;

	if (m_v && !acceleration)
	{
		if (m_v > 0)
		{
			m_v -= m_friction*etime;
			if (m_v < 0)
				m_v = 0.0;
		}
		else
		{
			m_v += m_friction*etime;
			if (m_v > 0)
				m_v = 0.0;
		}
	}
	else if (acceleration)
		m_v += m_acc*acceleration*etime;

	// make surevelocity does not exceed terminal velocity:
	if (m_v > m_terminal)
		m_v = m_terminal;
	else if (-m_v > m_terminal)
		m_v = -m_terminal;

	if (!m_v)
	{
		m_etime = 0;
		return m_pos;
	}

	int opos = m_pos;

	if (m_flags & PHY_COMP_ETIME)
	{
		applied_v = (int)((m_v*m_etime)/1000.0);
		m_pos += applied_v;
		m_etime -= (int)((applied_v / m_v)*1000.0);
	}
	else
	{
		m_pos += (int)(m_v)/20; // divide by milliseconds/frame
	}

	// ### How smoothly are we being called?
	// if (m_v ) Msg("t: %d, et:%d, p:%d, av:%d, v:%d", real_etime, m_etime, opos - m_pos, applied_v, (int)m_v);

	if (m_flags & PHY_UNIVERSE_WRAPS)
	{
		if (m_pos>=m_max)	
			m_pos = m_min;
		if (m_pos<0)		
			m_pos += m_max;
	}
	else
	{
		int bounceCheck = m_pos; // for Bouncy panels:
		if (m_pos >= m_max)	m_pos = m_max;
		if (m_pos<m_min)	m_pos = m_min;

		if (bounceCheck != m_pos)
		{
			m_v = -m_v/2;
			m_etime = 0;
		}
	}

	return m_pos;
}

