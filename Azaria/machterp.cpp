/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       machterp.cpp
;
; Description:
;	Mach Interpreter.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 04/03/96  gas  Created.
----------------------------------------------------------------------------*/

#define __MACHINES_CPP__

#include "maya.h"
#include <dsWave.h>
#include <xfile.h>

#include "data\database\tbl_wave.h"
#include "data\database\ids.h"

#include "gdb.h"
#include "btable.h"
#include "gameman.h"
#include "object.h"
#include "signal.h"
#include "machterp.h"
#include "player.h"
#include "viewman.h"
#include "cursor.h"
#include "spell.h"

// used strictly to offset video playback within a machine
static int yOffset = 0;
static int xOffset = 0;

WORD MACHINE::c_Rand;
WORD MACHINE::c_Temp1;
WORD MACHINE::c_Temp2;
WORD MACHINE::c_Temp3;
WORD MACHINE::c_ViewId;
DPID MACHINE::c_Pid;
WORD MACHINE::c_RefMachine;

void __cdecl _Msg( LPSTR fmt, ... );

#define MACH_LOWER_DEBUG_LIMIT 0x0100
BOOL mon_machines = FALSE;
BOOL mon_user_mach = TRUE;

//----------------------------------------------------------------------------
// local datagram ids
// #define PROC_REQUESTTABLE		0x0001
// #define PROC_SENDTABLE			0x0002
#define PROC_SYNCPOINT				0x0003
#define PROC_SET_MACHINE_STATE		0x0004
#define PROC_SEND_OBJECT			0x0005
#define PROC_TRANS_DROP				0x0006
#define PROC_TRANS_DRAG				0x0007
#define PROC_TRANS_GRAB				0x0008
#define PROC_SET_ALL_MACHINE_STATE	0x0010

typedef struct {
	WORD  id;
	REGISTERS reg;
} SETSTATE_PKT;

typedef struct {
	WORD  id;			// id of machine instance
	WORD obj_def_id;	// id of object involved in the transition
	DWORD pid;			// player originating the transition request.
} TRANS_PKT;			// header 'proc' contains 'drop', 'grab', or 'drag'

typedef struct {
	WORD obj_def_id;
} OBJECT_PKT;

typedef struct {
	WORD id;
	WORD syncpoint;
} SYNCPOINT_PKT;

typedef struct {
	WORD min_id;
	WORD max_id;
	WORD count;
} ALLSTATE_PKT_HDR;

#define MAX_STATE_DATALEN 500
typedef struct {
	ALLSTATE_PKT_HDR hdr;
	BYTE data[MAX_STATE_DATALEN];
} SETALLSTATE_PKT;

//----------------------------------------------------------------------------
// Returns true if the transition can be made with the given input:
BOOL TRANSITION::MatchTransition(BYTE input, BYTE bParm, WORD wParm, WORD obj_def_id)
{
	BOOL rc = FALSE;

	if (input != m_input)
		return FALSE;

	switch (input) 
	{
	case CLICK:
	case GRAB:
		return TRUE;
		break;
	case DROP:
	case DRAG:
	  if (!obj_def_id // if transition accepts any object
		|| obj_def_id == wParm) // or the dropped object def matches the trans object def
			rc = TRUE;
		break;
	case WAIT:
	  if (obj_def_id == wParm) // the signals must match
			rc = TRUE;
	  /*
	  if (!obj_def_id
		|| obj_def_id == wParm) // or the dropped object def matches the trans object def
			rc = TRUE;
	  */
	  break;
	case DRAGFOCUS:
	  if (obj_def_id == wParm) // match TRUE/FALSE focus message to transition
		rc = TRUE;
	  break;
	default:
		rc = TRUE;
	}

	return rc;
}

//----------------------------------------------------------------------------
// for the given machine
// Loops through all transitions from a given state
// looking for one that can be traversed with the given input:
TRANSITION *TBL_MACHINE_DEF::FindTransition(MACHINE *machine, BYTE state, BYTE input, BYTE bParm, WORD wParm)
{
	int position;
	int key = MAKELONGKEY(machine->m_machine_id, state); // the table is sorted by [machine:state]

	TRANSITION *trans = GetFirst(key, &position);
	while (trans)
	{
		WORD obj_def_id = (trans->m_bParm)
					?machine->GetRegVal(trans->m_bParm)
					:trans->m_wParm;

		if (trans->MatchTransition(input, bParm, wParm, obj_def_id))
			break;
		else
			trans = GetNext(key, &position);
	}
	return trans;
}

//----------------------------------------------------------------------------
// for the given machine
// Loops through all transitions from a given state
// looking for one that can be traversed with the given input:
TRANSITION *TBL_MACHINE_DEF::FindFirstAutoTrans(WORD machine_id, BYTE state, int *position)
{
	int key = MAKELONGKEY(machine_id, state); // the table is sorted by [machine:state]
	TRANSITION *t = GetFirst(key, position);

	while (t && (t->m_input>LAST_AOP))
	{
		t = GetNext(key, position);
	}

	return t;
}

//----------------------------------------------------------------------------
TRANSITION *TBL_MACHINE_DEF::FindNextAutoTrans(WORD machine_id, BYTE state, int *position)
{
	int key = MAKELONGKEY(machine_id, state); // the table is sorted by [machine:state]
	TRANSITION *t = GetNext(key, position);

	while (t && (t->m_input>LAST_AOP))
	{
		t = GetNext(key, position);
	}

	return t;
}

//----------------------------------------------------------------------------
// Execute the given transition.  There may be subsequent transitions that
// can be completed without further input.
void MACHINE::Transition(TRANSITION *trans, BYTE bParm)
{
	m_reg.state = trans->m_new_state;

	// If there are AutoTransitions from the current state -
	AutoTransition(0);
}

//----------------------------------------------------------------------------
WORD MACHINE::GetRegVal(BYTE reg_id)
{
	WORD x = 0;

	// Do they want the registers of another machine?
	if (reg_id >= R_BPARM && reg_id <= R_WIP3)
	{
		MACHINE *machine = gdbt_machines.Get(c_RefMachine);
		if (machine)
		  return machine->GetRegVal(reg_id-R_OFFSET);
		else
		  return 0;
	}

	// Is this an indirection register (points to another register)?
	if (reg_id & 0x80)
	{
		reg_id = (BYTE)GetRegVal(reg_id&~0x80);
	}

	switch (reg_id)
	{
	case BPARM:
		x = m_reg.bParm;
		break;
	case WPARM:
		x = m_reg.wParm;
		break;
	case WOBJECT:
		x = m_reg.wObject;
		break;
	case WSPRITE:
		x = m_reg.wSprite;
		break;
	case BFRAME:
		x = m_reg.bFrame;
		break;
	case WACCEPT:
		x = m_reg.wAccept;
		break;
	case DETIME:
		x = (WORD)m_reg.dEtime;
		break;
	case WTHIS:
		x = (WORD)m_id;
		break;
	case WIP1:
		x = m_wip.wIP1;
		break;
	case WIP2:
		x = m_wip.wIP2;
		break;
	case WIP3:
		x = m_wip.wIP3;
		break;
	case WIP4:
		x = m_wip.wIP4;
		break;

// Shared registers:
	case WRAND:
		x = c_Rand;
		break;
	case WTEMP1:
		x = c_Temp1;
		break;
	case WTEMP2:
		x = c_Temp2;
		break;
	case WTEMP3:
		x = c_Temp3;
		break;
	case WVIEWID:
		x = c_ViewId;
		break;


// database psuedo-registers
// These can be acted on by arithmetic, compare, and move operations
	case LKARMA:
		x = local_player->GetKarma();
		break;
	case LSTRENGTH:
		x = local_player->GetStrength();
		break;
	case LWISDOM:
		x = local_player->GetWisdom();
		break;
	case LENERGY:
		x = local_player->GetEnergy();
		break;
	case LSEX:
		x = local_player->GetSex();
		break;
	case LWEALTH:
		x = local_player->GetWealth();
		break;
	case LVIEW:
		x = local_player->GetView();
		break;

	case OKARMA:
		if (other_player)
			x = other_player->GetKarma();
		break;
	case OSTRENGTH:
		if (other_player)
			x = other_player->GetStrength();
		break;
	case OWISDOM:
		if (other_player)
			x = other_player->GetWisdom();
		break;
	case OENERGY:
		if (other_player)
			x = other_player->GetEnergy();
		break;
	case OSEX:
		if (other_player)
			x = other_player->GetSex();
		break;
	case OWEALTH:
		if (other_player)
			x = other_player->GetWealth();
		break;
	case OVIEW:
		if (other_player)
			x = other_player->GetView();
		break;
	}

	return x;
}

//----------------------------------------------------------------------------
void MACHINE::SetRegVal(BYTE reg_id, WORD value)
{
	// Do they want the registers of another machine?
	if (reg_id >= R_BPARM && reg_id <= R_WIP3)
	{
		MACHINE *machine = gdbt_machines.Get(c_RefMachine);
		if (machine)
		  machine->SetRegVal(reg_id-R_OFFSET, value);

		return;
	}

	// Is this an indirection register (points to another register)?
	if (reg_id & 0x80)
	{
		reg_id = (BYTE)GetRegVal(reg_id&~0x80);
	}

	switch (reg_id)
	{
	case BPARM:
		m_reg.bParm = (BYTE)value;
		break;
	case WPARM:
		m_reg.wParm = value;
		break;
	case WOBJECT:
		m_reg.wObject = value;
		break;
	case WSPRITE:
		m_reg.wSprite = value;
		break;
	case BFRAME:
		m_NewFrame = TRUE;
		m_reg.bFrame = (BYTE)value;
		break;
	case WACCEPT:
		m_reg.wAccept = value;
		break;
	case DETIME:
		m_reg.dEtime = value;
		break;

// Shared registers:
	case WRAND:
		c_Rand = value;
		break;
	case WTEMP1:
		c_Temp1 = value;
		break;
	case WTEMP2:
		c_Temp2 = value;
		break;
	case WTEMP3:
		c_Temp3 = value;
		break;
	case WVIEWID:
		c_ViewId = value;
		break;

// database psuedo-registers
// These can be acted on by arithmetic, compare, and move operations
	case LKARMA:
		local_player->SetKarma(value);
		break;
	case LSTRENGTH:
		local_player->SetStrength(value);
		break;
	case LWISDOM:
		local_player->SetWisdom(value);
		break;
	case LENERGY:
		local_player->SetEnergy(value);
		break;
	case LSEX:
		local_player->SetSex(value);
		break;
	case LWEALTH:
		local_player->SetWealth(value);
		break;

	case OKARMA:
		if (other_player)
			other_player->SetKarma(value);
		break;
	case OSTRENGTH:
		if (other_player)
			other_player->SetStrength(value);
		break;
	case OWISDOM:
		if (other_player)
			other_player->SetWisdom(value);
		break;
	case OENERGY:
		if (other_player)
			other_player->SetEnergy(value);
		break;
	case OSEX:
		if (other_player)
			other_player->SetSex(value);
		break;
	case OWEALTH:
		if (other_player)
			other_player->SetWealth(value);
		break;
	}
}

//----------------------------------------------------------------------------
BOOL MACHINE::Tick(int etime)
{
	if (m_reg.dEtime <= etime)
	{
		m_reg.dEtime = 0;
		m_Timing = FALSE;
		return TRUE;
	}
	else
	{
		m_reg.dEtime -= etime;
		return FALSE;
	}
}

//----------------------------------------------------------------------------
// Completes any AutoTransitions that can be made immediately
// Places timed or 'machine inter-dependent' AutoTransitions on the Agenda.
BOOL MACHINE::AutoTransition(int etime)
{
#define TIMESLICE	500 // Actually,  number of transitions before preemption

	int SliceCount = 0; // Protect ourselves from infinite loops.
	TRANSITION *trans;
	int position;
	WORD x, x2;
	BOOL AddToAgenda = FALSE;
	BOOL MakeTransition = FALSE;

	position = 0;
	trans = btbl_machine_def.FindFirstAutoTrans(m_machine_id, m_reg.state, &position);

	while (trans)
	{
		switch(trans->m_input) {
			case EPSILON:
				MakeTransition = TRUE;
				break;
			case PLAYWAVE:
				MakeTransition = TRUE;
				if (local_player->GetView() == m_view_id
				 || m_AlwaysVisible)
				{
				 Wave::Play((WAVE_RAMID)trans->m_wParm);
				}
				break;
			case STOPWAVE:
				MakeTransition = TRUE;
				break;
			case ESTIME:
				AddToAgenda = TRUE;
				if (!m_Timing)
				{
					x = (trans->m_bParm)
						?GetRegVal(trans->m_bParm)
						:trans->m_wParm;
					m_reg.dEtime = 1000*x;
					m_Timing = TRUE;
				}
				else
					MakeTransition = Tick(etime);
				break;
			case EMTIME:
				AddToAgenda = TRUE;
				if (!m_Timing)
				{
					x = (trans->m_bParm)
						?GetRegVal(trans->m_bParm)
						:trans->m_wParm;
					m_reg.dEtime = x;
					m_Timing = TRUE;
				}
				else
					MakeTransition = Tick(etime);

				break;
			case IFSTATE:
				{
				  x = GetRegVal((BYTE)trans->m_wParm);

				  MACHINE *machine = gdbt_machines.Get(x);
				  if (machine && machine->m_reg.state == trans->m_bParm)
				  {
				  	MakeTransition = TRUE;
				  }
				  else
					AddToAgenda = TRUE;
				}
				break;
			case IFSTATEi:
				{
				  MACHINE *machine = gdbt_machines.Get(trans->m_wParm);
				  if (machine && machine->m_reg.state == trans->m_bParm)
				  {
				  	MakeTransition = TRUE;
				  }
				  else
					AddToAgenda = TRUE;
				}
				break;
			case NOTSTATE:
				{
				  x = GetRegVal((BYTE)trans->m_wParm);
				  MACHINE *machine = gdbt_machines.Get(x);
				  if (machine && machine->m_reg.state != trans->m_bParm)
				  {
				  	MakeTransition = TRUE;
				  }
				}
				break;
			case NOTSTATEi:
				{
				  MACHINE *machine = gdbt_machines.Get(trans->m_wParm);
				  if (machine && machine->m_reg.state != trans->m_bParm)
				  {
				  	MakeTransition = TRUE;
				  }
				}
				break;
			case MAP_OBJ:
				{
					// trans holds OP,  machine holds OBJECT:
					int key = MAKELONGKEY(trans->m_wParm, m_reg.wObject);
					OBJECT_OP *oop = tbl_object_ops.Find(key);
					if (oop)
					{
						m_reg.wObject = oop->dst_obj_def;
					}
					MakeTransition = TRUE;
				}
				break;
			case MIX:
				{
					// trans holds OP,  machine holds OBJECT:
					WORD op = GetRegVal(trans->m_bParm);
					x = GetRegVal((BYTE)trans->m_wParm);
					int key = MAKELONGKEY(op, x);
					OBJECT_OP *oop = tbl_object_ops.Find(key);
					if (oop)
					{
						m_reg.wObject = oop->dst_obj_def;
					}
					MakeTransition = TRUE;
				}
				break;
			case XIM:
				{
					MakeTransition = TRUE;
					OBJECT_OP *oop = tbl_object_ops.UFind(m_reg.wObject);
					if (oop)
					{
						SetRegVal(trans->m_bParm, oop->op);
						SetRegVal((BYTE)trans->m_wParm, oop->src_obj_def);
					}
				}
				break;

			case SET_YOFFSET:
				if (trans->m_bParm == ADD)
				  yOffset = +trans->m_wParm;
				else
				  yOffset = -trans->m_wParm;

				MakeTransition = TRUE;
				break;

			case SET_XOFFSET:
				if (trans->m_bParm == ADD)
				  xOffset = +trans->m_wParm;
				else
				  xOffset = -trans->m_wParm;

				MakeTransition = TRUE;
				break;

			case ASHOW:
			case SHOW: // set wSprite, show it, animate.
				MakeTransition = TRUE;
				switch (trans->m_bParm)
				{
				case IMMEDIATE:
					m_reg.wSprite = trans->m_wParm;
					break;
				case WSPRITE:
					break;
				default:
					m_reg.wSprite = objdefGetLookActorId(GetRegVal(trans->m_bParm));
				}

				viewman.Invalidate(m_view_id, FALSE, TRUE);
				m_Dirty = TRUE;
				m_NewSprite = TRUE;

				m_reg.Animate = m_reg.Loop = (trans->m_input == ASHOW);
				break;

			case ANIMATE: // animates the wSprite.
				MakeTransition = TRUE;
				m_reg.Animate = TRUE;
				x = (trans->m_bParm)
					?GetRegVal(trans->m_bParm)
					:trans->m_wParm;
				m_reg.Loop = (x & V_LOOP) != 0;
				m_reg.PlayReverse = (x & V_REVERSE) != 0;
				m_reg.bStartFromEnd = (x & V_LASTFRAME) != 0;
				m_reg.bRewind = (x & V_REWIND) != 0;
				break;

			case VIDEO:
				MakeTransition = TRUE;
				x = trans->m_bParm;
				SPIRIT::Video(m_view_id, m_rect.left+xOffset, m_rect.top+yOffset, 
							  trans->m_wParm, (x & V_LASTFRAME)!=0,
							  (x&V_LOOP)!=0, (x&V_REVERSE)!=0);
				xOffset = 0;
				yOffset = 0;
				break;

			case ADD:
				MakeTransition = TRUE;
				x = GetRegVal(trans->m_bParm);
				x += GetRegVal((BYTE)trans->m_wParm);
				SetRegVal(trans->m_bParm, x);
				break;

			case SUB:
				MakeTransition = TRUE;
				x = GetRegVal(trans->m_bParm);
				x -= GetRegVal((BYTE)trans->m_wParm);
				SetRegVal(trans->m_bParm, x);
				break;
			case MUL:
				MakeTransition = TRUE;
				x = GetRegVal(trans->m_bParm);
				x *= GetRegVal((BYTE)trans->m_wParm);
				SetRegVal(trans->m_bParm, x);
				break;
			case DIV:
				MakeTransition = TRUE;
				x = GetRegVal(trans->m_bParm);
				x2 = GetRegVal((BYTE)trans->m_wParm);
				if (x2 == 0)
				  Msg("Divide by zero");
				else
				  x /= x2;
				SetRegVal(trans->m_bParm, x);
				break;
			case MOD:
				MakeTransition = TRUE;
				x = GetRegVal(trans->m_bParm);
				x %= GetRegVal((BYTE)trans->m_wParm);
				SetRegVal(trans->m_bParm, x);
				break;
			case ADDI:
				MakeTransition = TRUE;
				x = GetRegVal(trans->m_bParm);
				x += trans->m_wParm;
				SetRegVal(trans->m_bParm, x);
				break;
			case SUBI:
				MakeTransition = TRUE;
				x = GetRegVal(trans->m_bParm);
				x -= trans->m_wParm;
				SetRegVal(trans->m_bParm, x);
				break;
			case MULI:
				MakeTransition = TRUE;
				x = GetRegVal(trans->m_bParm);
				x *= trans->m_wParm;
				SetRegVal(trans->m_bParm, x);
				break;
			case DIVI:
				MakeTransition = TRUE;
				x = GetRegVal(trans->m_bParm);
				x2 = trans->m_wParm;
				if (x2 == 0)
				  Msg("Divide by zero");
				else
				  x /= x2;
				SetRegVal(trans->m_bParm, x);
				break;
			case MODI:
				MakeTransition = TRUE;
				x = GetRegVal(trans->m_bParm);
				x %= trans->m_wParm;
				SetRegVal(trans->m_bParm, x);
				break;
			case ASSIGN:
				MakeTransition = TRUE;
				SetRegVal(trans->m_bParm, trans->m_wParm);
				break;
			case MOV:
				MakeTransition = TRUE;
				SetRegVal(trans->m_bParm, GetRegVal((BYTE)trans->m_wParm));
				break;
			case GTi:
				x = GetRegVal(trans->m_bParm);
				if (x > trans->m_wParm)
					MakeTransition = TRUE;
				break;
			case GTEi:
				x = GetRegVal(trans->m_bParm);
				if (x >= trans->m_wParm)
					MakeTransition = TRUE;
				break;
			case LTi:
				x = GetRegVal(trans->m_bParm);
				if (x < trans->m_wParm)
					MakeTransition = TRUE;
				break;
			case LTEi:
				x = GetRegVal(trans->m_bParm);
				if (x <= trans->m_wParm)
					MakeTransition = TRUE;
				break;
			case EQUALi:
				x = GetRegVal(trans->m_bParm);
				if (x == trans->m_wParm)
					MakeTransition = TRUE;
				break;
			case NEQUALi:
				x = GetRegVal(trans->m_bParm);
				if (x != trans->m_wParm)
					MakeTransition = TRUE;
				break;
			case GT:
			  if (GetRegVal(trans->m_bParm) > GetRegVal((BYTE)trans->m_wParm))
				MakeTransition = TRUE;
			  break;
			case GTE:
			  if (GetRegVal(trans->m_bParm) >= GetRegVal((BYTE)trans->m_wParm))
				MakeTransition = TRUE;
			  break;
			case LT:
			  if (GetRegVal(trans->m_bParm) < GetRegVal((BYTE)trans->m_wParm))
				MakeTransition = TRUE;
			  break;
			case LTE:
			  if (GetRegVal(trans->m_bParm) <= GetRegVal((BYTE)trans->m_wParm))
				MakeTransition = TRUE;
			  break;
			case EQUAL:
			  if (GetRegVal(trans->m_bParm) == GetRegVal((BYTE)trans->m_wParm))
				MakeTransition = TRUE;
			  break;
			case NEQUAL:
			  if (GetRegVal(trans->m_bParm) != GetRegVal((BYTE)trans->m_wParm))
				MakeTransition = TRUE;
			  break;
			case IS_A:
				  x = GetRegVal(trans->m_bParm);
				 if  (tbl_isa.Find(MAKELONGKEY(trans->m_wParm, x))
				  || (objdefGetClass(x) == trans->m_wParm))
						MakeTransition = TRUE;
				break;
			case LOADVIEW:
				x = (trans->m_bParm)
					?GetRegVal(trans->m_bParm)
					:trans->m_wParm;
				if (m_Local || c_Pid==local_player->GetPid())
					viewman.Load(x);
				MakeTransition = TRUE;
				break;
			case SPELL_ME:
				MakeTransition = TRUE;
				WIZARD::CastMe(GetRegVal(trans->m_bParm), trans->m_wParm);
				break;
			case SPELL_YOU:
				MakeTransition = TRUE;
				WIZARD::CastYou(GetRegVal(trans->m_bParm), trans->m_wParm);
				break;
			case SIGNALri:
				{
					MakeTransition = TRUE;
					MACHINE *machine = gdbt_machines.Get(trans->m_wParm);
					if (machine)
						machine->Signal(GetRegVal(trans->m_bParm));
				}
				break;
			case SIGNALi:
				{
					MakeTransition = TRUE;
					MACHINE *machine = gdbt_machines.Get(trans->m_wParm);
					if (machine)
						machine->Signal(trans->m_bParm);
				}
				break;
			case SIGNAL:
				{
					MakeTransition = TRUE;
					x = GetRegVal(trans->m_bParm);
					MACHINE *machine = gdbt_machines.Get(x);
					if (machine)
						machine->Signal(trans->m_wParm);
				}
				break;
			case HANDOFF:
				x = (trans->m_bParm)
					?GetRegVal(trans->m_bParm)
					:trans->m_wParm;

				if (m_Local || c_Pid==local_player->GetPid())
				{
				  POINT pCenter, pPos;
					if (cursorman.HeldObject())
						cursorman.DropObject(&pCenter, &pPos);

					cursorman.LoadObject(x, pPos);
				}
				else if (Game::IsOwner(local_player->GetPid()))
				{
					gdbt_machines.SendObject(c_Pid, x);
				}

				MakeTransition = TRUE;
				break;
			case C_ACCEPT: // set up to accept objects of a specific class
				m_reg.wAccept  = (trans->m_bParm)
					?GetRegVal(trans->m_bParm)
					:trans->m_wParm;
				m_reg.AcceptClass = TRUE;

				MakeTransition = TRUE;
				break;

			case O_ACCEPT: // set up to accept specific objects.
				m_reg.wAccept  = (trans->m_bParm)
					?GetRegVal(trans->m_bParm)
					:trans->m_wParm;
				m_reg.AcceptClass = FALSE;

				MakeTransition = TRUE;
				break;

			case SYNCPOINT:
				// if the syncpoint has already been signaled:
				if (tbl_syncpoints[trans->m_wParm].signaled)
				{
					tbl_syncpoints[trans->m_wParm].signaled = FALSE;
					MakeTransition = TRUE;
					break;
				}

				// syncpoint times are only timed by the bard or her henchmen
				if (!Game::IsOwner(local_player->GetPid()))
				{
					// we neither add this to our agenda
					// nor do we make the transition.
					// We will make the transition when our Master 
					// notifies us by sending a SynPoint message
					break;
				}

				AddToAgenda = TRUE;

				// if this is a wait syncpoint:
				if (GetRegVal(trans->m_bParm))
				{
					if (!m_Timing)
					{
						m_reg.dEtime =  1000 * GetRegVal(trans->m_bParm);
						m_Timing = TRUE;
					}
					else
					{
						MakeTransition = Tick(etime);
					}
				}
				else
					MakeTransition = Tick(etime);

				if (MakeTransition)
				{
					gdbt_machines.SendSync(m_id, trans->m_wParm);
				}

				break;
			case RAND:
				MakeTransition = TRUE;
				{
					int r;
					// generate a value between 0 and RAND_MAX
					r = rand();

					// set WRAND to a value between WPARM and WPARM+BPARM
					SetRegVal(WRAND, r % (int)trans->m_bParm + trans->m_wParm);
				}
				break;

			case REF_MACHINE:
				MakeTransition = TRUE;
				c_RefMachine = (trans->m_bParm)
								?GetRegVal(trans->m_bParm)
								:trans->m_wParm;
				break;

			case SENDKEYI:
				MakeTransition = TRUE;
				{
				  VIEW *view = viewman.GetView(trans->m_wParm);
				  if (view)
				  {
					view->Keycmd(trans->m_bParm, KEY_PRESSED);
					view->Keycmd(trans->m_bParm, KEY_RELEASED);
				  }
				}
				break;

			case SENDKEY:
				MakeTransition = TRUE;
				{
				  VIEW *view = viewman.GetView(trans->m_wParm);
				  if (view)
				  {
					x = GetRegVal(trans->m_bParm);
					view->Keycmd(x, KEY_PRESSED);
					view->Keycmd(x, KEY_RELEASED);
				  }
				}
				break;
			case SENDMSGI:
				MakeTransition = TRUE;
				{
				  VIEW *view = viewman.GetView(trans->m_wParm);
				  if (view)
				  {
					view->Msg(trans->m_bParm, 0);
				  }
				}
				break;
			case SENDMSG:
				MakeTransition = TRUE;
				{
				  VIEW *view = viewman.GetView(trans->m_wParm);
				  if (view)
				  {
					view->Msg(GetRegVal(trans->m_bParm), 0);
				  }
				}
				break;

			case MAPi:
				MakeTransition = TRUE;
				{
					// get index to user map table:
					x = trans->m_wParm;
					// ensure x is valid index to a user map table
					if (x<tbl_map_tables_count)
					{
						WORD index = GetRegVal(trans->m_bParm);
						// ensure index is in range of table:
						if (index < tbl_map_tables[x].sizeoftable)
						{
						  SetRegVal(trans->m_bParm, tbl_map_tables[x].table[index]);
						}
					}
				}
				break;

			case MAP:
				MakeTransition = TRUE;
				{
					// get index to user map table:
					x = GetRegVal((BYTE)trans->m_wParm);
					// ensure x is valid index to a user map table
					if (x<tbl_map_tables_count)
					{
						WORD index = GetRegVal(trans->m_bParm);
						// ensure index is in range of table:
						if (index < tbl_map_tables[x].sizeoftable)
						{
						  SetRegVal(trans->m_bParm, tbl_map_tables[x].table[index]);
						}
					}
				}
				break;

			default:
				Msg("Unknown transition");
				break;
		}

		if (MakeTransition)
		{
#if 1
				if (mon_machines && m_id >= MACH_LOWER_DEBUG_LIMIT) // scene 1 and above
				{
				  if ((SliceCount % 8)==0)
				  {
					  if (SliceCount == 0)
						_Msg("%s: (B:%hx  W:%hx  O:%hx  A:%hx  S:%hx  F:%hx  E:%hx) state:%hd",
							m_name, 
							m_reg.bParm, m_reg.wParm, m_reg.wObject, m_reg.wAccept, 
							m_reg.wSprite, m_reg.bFrame, m_reg.dEtime,
							m_reg.state);
					  else
						_Msg("\n\t");
				  }


				  if ((trans->m_input >= FIRST_AOP) && (trans->m_input <= LAST_AOP))
					_Msg(" -%s->%hd", strAutoOps[trans->m_input], trans->m_new_state);
				  else
					_Msg(" -x%hx->%hd", trans->m_input, trans->m_new_state);
				}
#endif

				m_reg.state = trans->m_new_state;
				position = 0;

				if (SliceCount++ > TIMESLICE)
				{
					AddToAgenda = FALSE;
					Msg("\n! ! ! ! ! ! !  WARNING: TIMESLICE EXCEEDED - POSSIBLE INFINITE LOOP.");
					Msg("-------------  REMOVING %s PROCESS FROM AGENDA.", m_name);
					goto ExceededTimeSlice;
				}

				 // in a new state,  don't know if needs to be on agenda.
				AddToAgenda = FALSE;
				MakeTransition = FALSE;
				trans = btbl_machine_def.FindFirstAutoTrans(m_machine_id, m_reg.state, &position);
		}
		else
			trans = btbl_machine_def.FindNextAutoTrans(m_machine_id, m_reg.state, &position);
	}

ExceededTimeSlice:

	if (mon_machines && m_id >= MACH_LOWER_DEBUG_LIMIT && SliceCount) _Msg("\n");

	// If there are AutoTransitions from the current state -
	if (AddToAgenda && !m_onAgenda)
	{
		gdbt_machines.AddAgenda(this);
		m_onAgenda = 1;
	}

	return AddToAgenda;
}

//----------------------------------------------------------------------------
void MACHINE::SlamState(REGISTERS *reg)
{
	if (m_reg.wSprite != reg->wSprite)
	{
		viewman.Invalidate(m_view_id, FALSE, TRUE);
		m_Dirty = TRUE;
		m_NewSprite = TRUE;
	}
	m_reg = *reg;
}

//----------------------------------------------------------------------------
void MACHINE::SendState(DPID pid, WORD group)
{
	SETSTATE_PKT pkt;

	pkt.id = m_id;
	pkt.reg = m_reg;
	gdbt_machines.SendUpdate(
			pid,
			group,
			PROC_SET_MACHINE_STATE,
			(UDATA*)&pkt, 
			sizeof(pkt),
			GUARANTEED);
}

//----------------------------------------------------------------------------
void MACHINE::SendTrans(WORD group, WORD proc, WORD obj_def_id, DPID originator)
{
	TRANS_PKT pkt;

	pkt.id = m_id;
	pkt.obj_def_id = obj_def_id;

	pkt.pid = originator;

	gdbt_machines.SendUpdate(
			0,
			group,
			proc,
			(UDATA*)&pkt, 
			sizeof(pkt),
			GUARANTEED);
}

//----------------------------------------------------------------------------
void GDBT_MACHINES::SendObject(DWORD pid_to, WORD obj_def_id)
{
	OBJECT_PKT pkt;

	pkt.obj_def_id = obj_def_id;
	SendUpdate(
			pid_to,
			0,
			PROC_SEND_OBJECT,
			(UDATA*)&pkt, 
			sizeof(pkt),
			GUARANTEED);
}

//----------------------------------------------------------------------------
// Initiate Transition: Initiate the transition across the global database.
BOOL MACHINE::InitiateTransition(WORD proc, WORD obj_def, BOOL local, BOOL autoTrans)
{
	BOOL bTransition = FALSE;

	// if the transition was initiated locally:
	if (local) // This is especially usefull for drags that result in a 'HANDOFF'
		c_Pid = local_player->GetPid();

		// All nodes execute autoTrans in sync, no commo required:
		// Local machines do not require commo w/master.
	if (autoTrans || m_Local)
	{
		bTransition = TRUE;
	}
		// If I own the machine, then execute the transition:
	else if (Game::IsOwner(local_player->GetPid()))
	{
		bTransition = TRUE;

		// and tell the other players to execute the transition:
		SendTrans(ALL_PLAYERS, proc, obj_def, c_Pid);
	}
		// else send the transition request to the machine owner:
	else if (local) // 'local' means initiated locally.
	{
		SendTrans(TABLE_OWNER, proc, obj_def, local_player->GetPid());
	}
		// message from table or row owner to make the transition:
	else
	{
		bTransition = TRUE;
	}
	return bTransition;
}

//----------------------------------------------------------------------------
// ON DROP:
// Finds an ondrop transition that matches the dropped object,  or a transition
// that accepts any object.  The accepted object definition is stored in wParm.
BOOL MACHINE::Drop(WORD *object_def, BOOL local, BOOL autoTrans)
{
	TRANSITION * trans = NULL;
	BOOL bTransition = FALSE;

//|| (m_bParm && wParm == GetRegVal(m_bParm)) // or the dropped obj def matches the given register

 	if  (!m_reg.wAccept // not being filtered by an acceptor
	  || (!m_reg.AcceptClass && (*object_def == m_reg.wAccept))
	  || (m_reg.AcceptClass
		&& (tbl_isa.Find(MAKEDWORDKEY(m_reg.wAccept, *object_def))
			|| (objdefGetClass(*object_def) == m_reg.wAccept))))
	{
		trans = btbl_machine_def.FindTransition(this, m_reg.state, DROP, 0, *object_def);
	}

	if (trans) // a match transition was found.
	{
		bTransition = InitiateTransition(PROC_TRANS_DROP, *object_def, local, autoTrans);

		if (bTransition)
		{
			WORD x;

			x = m_reg.wObject;
			m_reg.wObject = *object_def;

			*object_def = NULL;

			if (mon_user_mach) Msg("%s: %hd -DROP(0x%x)-> %hd", m_name, m_reg.state, (int)m_reg.wObject, trans->m_new_state);
			// execute the transition:
			Transition(trans,0);
		}
		else
		{
			// The object has been handed to the machine owner
			// for resolution within the machine.  If the transition cannot be
			// made for whatever reason,  the object will be handed back to
			// this player.
			*object_def = 0;
		}
	}

	return trans != NULL;
}

//----------------------------------------------------------------------------
// ON GRAB:
// Finds an onGrab transition
WORD MACHINE::Grab(BOOL local, BOOL autoTrans, BOOL bClickOnly)
{
	TRANSITION * trans;
	WORD wObject;
	BOOL bTransition = FALSE;

	trans = btbl_machine_def.FindTransition(this, m_reg.state, CLICK, 0, 0);

	if (!trans && !bClickOnly)
		trans = btbl_machine_def.FindTransition(this, m_reg.state, GRAB, 0, 0);

	if (trans) // a match transition was found.
	{
		bTransition = InitiateTransition(PROC_TRANS_GRAB, 0, local, autoTrans);
		if (bTransition)
		{
		  if (trans->m_input == GRAB)
		  {
			if (trans->m_wParm)
			{
				wObject = trans->m_wParm;
			}
			else if (trans->m_bParm)
			{
				wObject = GetRegVal(trans->m_bParm);
				SetRegVal(trans->m_bParm, 0);
			}
			else
			{
				wObject = m_reg.wObject;
				m_reg.wObject = 0;
			}
		  }
		  else
			wObject = 0;

		  if (mon_user_mach) Msg("%s: %hd -GRAB(0x%x)->%hd", m_name, m_reg.state, (int)wObject, trans->m_new_state);

		  Transition(trans,0);

		  return wObject;
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
// ON DRAG:
// Finds an onDrag transition
WORD MACHINE::Drag(WORD object_def, BOOL local, BOOL autoTrans)
{
	TRANSITION * trans;
	BOOL bTransition = FALSE;

	trans = btbl_machine_def.FindTransition(this, m_reg.state, DRAG, 0, object_def);

	if (trans) // a match transition was found.
	{
		bTransition = InitiateTransition(PROC_TRANS_DRAG, object_def, local, autoTrans);

		if (bTransition)
		{
			if (mon_user_mach) Msg("%s: %hd -DRAG(0x%x)->%hd", m_name, m_reg.state, (int)m_reg.wObject, trans->m_new_state);

			// execute the transition:

			Transition(trans,0);
			return object_def;
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
void MACHINE::DragFocus(BOOL bHasFocus)
{
	TRANSITION * trans;

	trans = btbl_machine_def.FindTransition(this, m_reg.state, DRAGFOCUS, 0, bHasFocus);

	if (trans) // a match transition was found.
	{
		if (mon_user_mach) Msg("%s: %hd -DRAGFOCUS(0x%x)->%hd", m_name, m_reg.state, (int)m_reg.wObject, trans->m_new_state);

		// execute the transition:
		Transition(trans,0);
	}
}

//----------------------------------------------------------------------------
void MACHINE::Signal(WORD wParm)
{
	TRANSITION * trans;
	BOOL bTransition = FALSE;

	trans = btbl_machine_def.FindTransition(this, m_reg.state, WAIT, 0, wParm);

	if (trans) // a match transition was found.
	{
		if (mon_machines) Msg("\n    %s: %hd -Signal->%hd", m_name, m_reg.state, trans->m_new_state);
		Transition(trans,0);
	}
}

//----------------------------------------------------------------------------
void MACHINE::Paint(ONE *canvas)
{
	if (m_Spirit)
		m_Spirit->OFF();

	if (m_NewSprite)
	{
		m_NewSprite = FALSE;

		if (m_Spirit)
		{
			delete m_Spirit;
			m_Spirit = NULL;
		}

		if (m_reg.wSprite)
		{
			if (canvas->m_hdr.user_flags & 1)
				m_Spirit = new SPIRIT(canvas, m_reg.wSprite, BLT_LENSED);
			else
				m_Spirit = new SPIRIT(canvas, m_reg.wSprite, BLT_PORTAL);

			if (m_Spirit)
			{
				m_Spirit->Move(m_rect.left, m_rect.top, 0);

				if (m_reg.Animate)
				{
				  m_Spirit->Animate(m_view_id, m_reg.Loop, m_reg.bStartFromEnd, m_reg.bRewind);
				  if (!m_reg.Loop)
				  {
					m_reg.Animate = FALSE;
					// when re-entering the view,  we want to see the last frame.
					m_reg.bFrame = (BYTE)m_Spirit->LastFrame();
				  }
				}
				else
			  	  m_Spirit->SetFrame(m_reg.bFrame);
			}
		}
	}

  	m_Dirty = FALSE;

	if (m_Spirit)
	{
	  if (m_NewFrame)
	  {
		  m_NewFrame = FALSE;
		  m_Spirit->SetFrame(m_reg.bFrame);
	  }

	  m_Spirit->ON();
	}
}

//----------------------------------------------------------------------------
void MACHINE::Erase()
{
	if (m_Spirit)
		m_Spirit->OFF();

}
//----------------------------------------------------------------------------
void MACHINE::Release()
{
	if (m_Spirit)
	{
		// m_Spirit->OFF();
		delete m_Spirit;
		m_Spirit = NULL;
	}
}

//----------------------------------------------------------------------------
void GDBT_MACHINES::SendSync(WORD id,	// id of the machine instance
							 WORD syncpoint)	// id of the syncpoint
{
	SYNCPOINT_PKT pkt;

	pkt.id = id;
	pkt.syncpoint = syncpoint;

	SendUpdate(
			0,
			ALL_PLAYERS,
			PROC_SYNCPOINT,
			(UDATA*)&pkt, 
			sizeof(pkt),
			GUARANTEED);
}

//----------------------------------------------------------------------------
int GDBT_MACHINES::CompressMachine(BYTE *output, REGISTERS *r)
{
  WORD *f;
  BYTE *input;
  int i, bytecount;

  f = (WORD*)output;
  *f = 0;
  output+=2;
  bytecount = 2;

  assert(sizeof(*r) == 16);

  for (i=0, input=(BYTE*)r; i<16; i++, input++)
  {
	if (*input)
	{
		*f |= 1<<i;
		*output = *input;
		output++;
		bytecount++;
	}
  }

  return bytecount;
}

//----------------------------------------------------------------------------
int GDBT_MACHINES::UncompressMachine(BYTE *input, REGISTERS *r)
{
  WORD f;
  BYTE *output;
  int i, bytecount;

  f = (WORD)*((WORD*)input);
  input+=2;
  bytecount = 2;

  for (i=0, output=(BYTE*)r; i<16; i++, output++)
  {
	if (f & 1<<i)
	{
		*output = *input;
		input++;
		bytecount++;
	}
	else
	  *output = 0;
  }

  return bytecount;
}

//----------------------------------------------------------------------------
void GDBT_MACHINES::SendAllStates(DWORD pid)
{
  int i, datalen, count;
  BOOL bFirst = TRUE;
  SETALLSTATE_PKT pkt;
  BYTE *data;
  WORD *pWord;
  WORD id;

	pkt.hdr.count = 0;
	datalen = 0;
	data = (BYTE*)&pkt.data;

	for (i=0;i<m_count;i++)
	{
		// don't send local machine states.
		if (m_table[i].m_Local)
		  continue;

		id = m_table[i].m_id;

		if (bFirst)
		{
			bFirst = FALSE;
			pkt.hdr.min_id = id;
			pkt.hdr.max_id = id;
		}
		else
		{
			// the table is not ordered by machine id,
			// so the range of machine id's is stored:
			pkt.hdr.min_id = min(pkt.hdr.min_id, id);
			pkt.hdr.max_id = max(pkt.hdr.max_id, id);
		}

		pkt.hdr.count++;

		// store the machine table index in the packet
		datalen+=2;
		pWord = (WORD*)data;
		*pWord = m_table[i].m_id;
		data+=2;

		count = CompressMachine(data, &m_table[i].m_reg);
		datalen += count;
		data += count;

		if ((datalen+(18+sizeof(pkt.hdr))) >= MAX_STATE_DATALEN)
		{
  			SendUpdate(
				pid,
				NULL,
				PROC_SET_ALL_MACHINE_STATE,
				(UDATA*)&pkt, 
				datalen + sizeof(ALLSTATE_PKT_HDR),
				GUARANTEED);

			bFirst = FALSE;
			pkt.hdr.count = 0;
			datalen = 0;
			data = (BYTE*)&pkt.data;

			GDB_TABLE::Poll();
		}
	}

	if (datalen)
		SendUpdate(
			pid,
			NULL,
			PROC_SET_ALL_MACHINE_STATE,
			(UDATA*)&pkt, 
			datalen + sizeof(ALLSTATE_PKT_HDR),
			GUARANTEED);
}

//----------------------------------------------------------------------------
BOOL GDBT_MACHINES::UncompressAllStates(BYTE *__pkt)
{
  SETALLSTATE_PKT *pkt = (SETALLSTATE_PKT*)__pkt;
  int i;
  BYTE* data = (BYTE*)pkt->data;

	for (i=0; i<pkt->hdr.count; i++)
	{
		WORD id = (*(WORD*)data);
		data+=2;

		MACHINE *m = UGet(id);

		if (!m
		|| (id < pkt->hdr.min_id)
		|| (id > pkt->hdr.max_id))
		{
		  Msg("Corrupt Game State Packet RX'd");
		  return FALSE;
		}

		data += UncompressMachine(data, &m->m_reg);
	}

	return TRUE;
}

//----------------------------------------------------------------------------
void GDBT_MACHINES::ReceiveUpdate(DPID from, DPID to, WORD proc, UDATA *data)
{
	MACHINE *machine;

	switch (proc)
	{
	case PROC_SYNCPOINT:
	 	{
			SYNCPOINT_PKT *pkt = (SYNCPOINT_PKT *)data;
			tbl_syncpoints[pkt->syncpoint].signaled = TRUE;
			if (pkt->id)
			{
				machine = UGet(pkt->id);
				if (machine)
					machine->AutoTransition(0); // see if it is waiting on the semaphore
			}
		}
		break;
	case PROC_SET_MACHINE_STATE:
	 	{
			SETSTATE_PKT *pkt = (SETSTATE_PKT *)data;
			machine = UGet(pkt->id);
			if (machine)
				machine->SlamState(&pkt->reg);
			else
				Msg("invalid id %d in PROC_SET_MACHINE_STATE", pkt->id);
		}
		break;

	case PROC_SEND_OBJECT: // The dest node of this packet is the dest of the object.
		{
			OBJECT_PKT *pkt = (OBJECT_PKT*)data;
			POINT pCenter, pPos;

			// ### if I am forced to drop the object,  we should roll back the trans
			// that allowed us to GET the object.
			if (cursorman.HeldObject())
			{
					cursorman.DropObject(&pCenter, &pPos);
			}

			cursorman.LoadObject(pkt->obj_def_id, pPos);
		}
		break;

	case PROC_TRANS_DROP:
	case PROC_TRANS_DRAG:
	case PROC_TRANS_GRAB:
	 	{
			TRANS_PKT *pkt = (TRANS_PKT *)data;
			machine = UGet(pkt->id);
			WORD obj_def_id;
			if (machine)
			{
				machine->c_Pid = pkt->pid;
				switch (proc)
				{
				case PROC_TRANS_DROP:
					obj_def_id = pkt->obj_def_id;
					machine->Drop(&obj_def_id, FALSE, FALSE);
					// The player dropped, or attempted to drop something,
					// and I am the owner of this machine. If there is a reward
					// or the object was rejected,  then send the object back 
					// to the player
					if (obj_def_id && Game::IsOwner(local_player->GetPid()))
					{
						SendObject(pkt->pid, obj_def_id);
					}
					break;
				case PROC_TRANS_GRAB:
					obj_def_id = machine->Grab(FALSE, FALSE, FALSE);
					// Send the object to the remote player:
					if (obj_def_id&& Game::IsOwner(local_player->GetPid()))
					{
						SendObject(pkt->pid, obj_def_id);
					}
					break;
				case PROC_TRANS_DRAG:
					machine->Drag(pkt->obj_def_id, FALSE, FALSE);
					machine->c_Pid = local_player->GetPid();
					break;
				}
			}
		}
		break;
	case PROC_SET_ALL_MACHINE_STATE:
		UncompressAllStates(data);
		break;
	}
}

//----------------------------------------------------------------------------
void GDBT_MACHINES::Signal(WORD machine_sid, WORD wParm)
{
	MACHINE *m = UGet(machine_sid);
	if (m)
	{
		m->Signal(wParm);
	}
}

//----------------------------------------------------------------------------
BOOL GDBT_MACHINES::Drop(WORD view_id, WORD *obj_def_id, POINT p)
{
	// Search the table of machines for a machine to drop this object into:
	MACHINE *m = Hit(view_id, p);

	if (!m)
		return FALSE;

	// The object was dropped onto a machine.  See if there is transition.
	return m->Drop(obj_def_id, TRUE, FALSE);
}

//----------------------------------------------------------------------------
BOOL GDBT_MACHINES::Drop(WORD sid, WORD *obj_def_id)
{
	// Search the table of machines for a machine to drop this object into:
	MACHINE *m = UGet(sid);

	if (!m)
		return FALSE;

	// The object was dropped onto a machine.  See if there is transition.
	return m->Drop(obj_def_id, TRUE, FALSE);
}

//----------------------------------------------------------------------------
WORD GDBT_MACHINES::Grab(WORD view_id, POINT p, BOOL bClickOnly, POINT *px)
{
	// Search the table of machines for a machine to grab an object from:
	MACHINE *m = Hit(view_id, p);

	if (!m)
	{
	  tbl_altviews.Link(view_id, p, TRUE);
	  return NULL;
	}

	// px is the position of the object as it is ejected from the machine:
	px->x = m->m_rect.left;
	px->y = m->m_rect.top;

	// The object was dropped onto a machine.  See if there is transition
	return m->Grab(TRUE, FALSE, bClickOnly);
}

//----------------------------------------------------------------------------
WORD GDBT_MACHINES::Drag(WORD view_id, WORD obj_def_id, POINT p)
{
	// Search the table of machines for a machine to drag the object across:
	MACHINE *machine = Hit(view_id, p);

	// only send drag message once while in the region of the machine.
	if (machine==m_machine_last_drag)
		return obj_def_id;

	if (m_machine_last_drag)
	  m_machine_last_drag->DragFocus(FALSE);

	if (machine)
	{
		obj_def_id = machine->Drag(obj_def_id, TRUE, FALSE);
		machine->DragFocus(TRUE);
	}

	m_machine_last_drag = machine;

	return obj_def_id;
}

//----------------------------------------------------------------------------
WORD GDBT_MACHINES::QueryCursor(WORD view_id, WORD obj_def_id, POINT p)
{
	// Search the table of machines for a machine to drag the object across:
	MACHINE *machine = Hit(view_id, p);

	if (machine)
	  return IDS_HANDMACH;
	else
	{
	  if (tbl_altviews.Link(view_id, p, FALSE))
		return IDS_HANDALT;
	}
	return 0;
}

//----------------------------------------------------------------------------
void GDBT_MACHINES::Paint(ONE *canvas, WORD view_id)
{
	int position;
	MACHINE *machine;

	SPIRIT::VideoOFF(view_id);

	machine = GetFirst(view_id, &position);
	while (machine)
	{
		machine->Erase();
		machine = GetNext(view_id, &position);
	}

	tbl_altviews.Paint(canvas, view_id);

	machine = GetLast(view_id, &position);
	while (machine)
	{
		machine->Paint(canvas);
		machine = GetPrev(view_id, &position);
	}

	SPIRIT::VideoON(view_id);
}

//----------------------------------------------------------------------------
// append a new machine to the agenda
void GDBT_MACHINES::AddAgenda(MACHINE *machine)
{
	assert(m_AgendaCount < m_MaxAgenda);
	m_Agenda[m_AgendaCount++]=machine;
}

//----------------------------------------------------------------------------
// Warm up all the machines when a game is loaded.
void GDBT_MACHINES::Init()
{
	int i;

	for (i=0;i<m_count;i++)
	{
		m_table[i].AutoTransition(0);
		m_table[i].Signal(SIG_INIT);
	}

	MACHINE::c_Pid = local_player->GetPid();

	/* Seed the random-number generator with current time so that
	* the numbers will be different every time we run.
	*/
	srand( (unsigned)time( NULL ) );

	// Finally,  let the game do any init it wants:
	MachInit();
}

//----------------------------------------------------------------------------
// Release all the machines when a view is dumped.
void GDBT_MACHINES::Release(WORD view_id)
{
	int position;

	MACHINE *machine = GetFirst(view_id, &position);
	while (machine)
	{
		machine->Release();
		machine = GetNext(view_id, &position);
	}

	tbl_altviews.Release(view_id);
}

//----------------------------------------------------------------------------
// Refresh all the machines when a view is loaded.
void GDBT_MACHINES::Init(WORD view_id)
{
	int position;

	MACHINE *machine = GetFirst(view_id, &position);
	while (machine)
	{
		machine->m_Dirty = TRUE;
		machine->m_NewSprite = TRUE;
		machine->Signal(SIG_VIEWINIT);
		machine = GetNext(view_id, &position);
	}
}

//----------------------------------------------------------------------------
// process any machines on the agenda
void GDBT_MACHINES::Tick(int etime)
{
	int i;

	for (i=0;i<m_AgendaCount;i++)
	{
		if (!m_Agenda[i]->AutoTransition(etime))
		{
			// remove the entry:
			m_Agenda[i]->m_onAgenda = FALSE;
			memcpy(m_Agenda+i, m_Agenda+i+1, sizeof(MACHINE*)*(m_AgendaCount-(i+1)));
			m_AgendaCount--;
			i--; // we have not done this guy yet ..
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BOOL GDBT_MACHINES::Save(char *filename)
{
	XFile file;
	int i;

	if (!file.Open(filename, XFile::modeWrite))
		return FALSE;

	for (i=0;i<m_count;i++)
	{
		file.Write(&m_table[i].m_reg, sizeof(m_table[i].m_reg));
	}

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL GDBT_MACHINES::Load(char *filename)
{
	XFile file;
	int i;

	if (!file.Open(filename, XFile::modeRead))
		return FALSE;

	for (i=0;i<m_count;i++)
	{
		file.Read(&m_table[i].m_reg, sizeof(m_table[i].m_reg));
	}

	Init();
	return TRUE;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void ALTVIEW::Release()
{
  delete m_Spirit;
  m_Spirit = NULL;
  m_sprite = 0; // force a repaint next time around
}

void ALTVIEW::Paint(ONE *canvas)
{
  MACHINE *machine = gdbt_machines.Get(m_machine_id);
  if (machine)
  {
	if (m_sprite != machine->m_reg.wSprite)
	{
	  m_sprite = machine->m_reg.wSprite;
	  if (m_Spirit)
	  {
		delete m_Spirit;
		m_Spirit = NULL;
	  }
	  if (machine->m_reg.wSprite)
	  {
	  	if (canvas->m_hdr.user_flags & 1)
			m_Spirit = new SPIRIT(canvas, m_sprite, BLT_LENSED);
		else
			m_Spirit = new SPIRIT(canvas, m_sprite, BLT_PORTAL);

		if (m_Spirit)
		{
			m_Spirit->Move(m_region.left, m_region.top, 0);
			if (m_Animate)
			  m_Spirit->Animate(m_view_id, m_Loop);
		}
	  }
	}
  }

  if (m_Spirit)
	  m_Spirit->ON();

}

//----------------------------------------------------------------------------
void ALTVIEW::Erase()
{
  if (m_Spirit)
	m_Spirit->OFF();
}

//----------------------------------------------------------------------------
BOOL TBL_ALTVIEWS::Link(WORD view_id, POINT p, BOOL bFire)
{
	// Search the table of machines for a machine to grab an object from:
	ALTVIEW *a = Hit(view_id, p);

	if (!a)
	  return FALSE;

	if (a->m_Link)
	{
	  MACHINE *machine = gdbt_machines.Get(a->m_machine_id);
	  if (machine)
	  {
		if (bFire)
		  viewman.Load(machine->m_view_id);
		return TRUE;
	  }
	}

	return FALSE;
}

//----------------------------------------------------------------------------
void TBL_ALTVIEWS::Paint(ONE *canvas, WORD view_id)
{
	int position;
	ALTVIEW *altview;

	altview = GetFirst(view_id, &position);
	while (altview)
	{
		altview->Erase();
		altview = GetNext(view_id, &position);
	}

	altview = GetLast(view_id, &position);
	while (altview)
	{
		altview->Paint(canvas);
		altview = GetPrev(view_id, &position);
	}

}

//----------------------------------------------------------------------------
void TBL_ALTVIEWS::Release(WORD view_id)
{
	int position;
	ALTVIEW *altview;

	altview = GetFirst(view_id, &position);
	while (altview)
	{
		altview->Release();
		altview = GetNext(view_id, &position);
	}
}
