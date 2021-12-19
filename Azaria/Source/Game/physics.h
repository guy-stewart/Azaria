/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       physics.h
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
#ifndef  __PHYSICS_INCLUDED__
#define __PHYSICS_INCLUDED__

// The universe can be 
// - bounded or unbounded, finite or infinite.
//

#define PHY_UNIVERSE_WRAPS	0x01 // bounded and infinite

#define PHY_COMP_ETIME		0x02 // Compensate for elapsed time

class PARTICLE {
// data members:
private:
	int m_pos;
	double m_v;
	double m_acc;
	int m_terminal;
	int m_max;
	int m_min;
	int m_flags;
	double m_friction;
	int m_etime;
public:
	PARTICLE()
	{
		m_acc = 0.0;
		m_friction = 0.0;
		m_terminal = 0;
		m_max = 0;
		m_min = 0;
		m_flags = 0;
		m_pos=0;
		m_v= 0.0;
		m_etime=0;
	};

	void Init(double acc_fact,
			 int terminal_velocity,
			 int max,
			 int flags,
			 double friction = 0.0,
			 int initial_position = 0
			 )
	{
		m_pos = initial_position;
		m_acc = acc_fact;
		m_terminal = terminal_velocity;
		m_max = max;
		m_flags = flags;

		if (!friction)
			m_friction = acc_fact/2.0;
		else
			m_friction = friction;

	};

	int accelerate(int acceleration, int etime);
	void SetTerminal(int terminal) {m_terminal = terminal;}
	void BrickWall();
};

#endif