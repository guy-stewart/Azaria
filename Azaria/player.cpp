/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       player.cpp
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 03/25/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "maya.h"
#include "signal.h"

#include "gdb.h"
#include "gameman.h"
#include "player.h"
#include "object.h"
#include "cursor.h"
#include "viewman.h"
#include "netctrl.h"
#include "teletype.h"
#include "spell.h"

#include "cfgview.h"

//----------------------------------------------------------------------------
#define PROC_STARTUPINFO		0x01
#define PROC_ADD_PLAYER			0x02
#define PROC_SET_VIEWID			0x03
#define PROC_SET_POS			0x04
#define PROC_SET_DETAILS		0x05
#define PROC_SET_OBJECT			0x06
#define PROC_SET_INTIMATE		0x07
#define PROC_INTIMATE_DETAILS	0x08
#define PROC_EMOTE				0x09

typedef struct {
	BYTE bRequestReply:1;
	BYTE bOwnsGame:1;
	WORD  view_id;
	POINT pos;
	WORD obj_def_id;
	FREQOUT FreqOut;
} STARTUPINFO_PKT;

typedef struct {
	DPID pid;
	WORD index;
} ADDPLAYER_PKT;

typedef struct {
	WORD  view_id;
	POINT pos;
} SETVIEW_PKT;

typedef struct {
	DWORD x:12;
	DWORD y:12;
	DWORD r:8;
} POS_PKT;

typedef struct {
	POINT pos;
	WORD obj_def_id;
} DETAILS_PKT;

typedef struct {
	WORD obj_def_id;
} SET_OBJECT_PKT;

typedef struct {
	BOOL intimate; // true: set intimate,  false: clear intimate.
} SET_INTIMATE_PKT;

typedef struct {
  BYTE emotion;
} EMOTE_PKT;

//----------------------------------------------------------------------------
static LOCAL_PLAYER __local_player;
LOCAL_PLAYER *local_player = &__local_player;

static OTHER_PLAYER __other_player;
OTHER_PLAYER *other_player = &__other_player;
VIEW *g_view = NULL;

GDBT_PLAYERS gdbt_players(sizeof(PLAYER));	// bytes per record

static void SendHeldObject(DWORD to, WORD flags, WORD obj_def_id);

//----------------------------------------------------------------------------
BOOL GDBT_PLAYERS::GetPlayerName(DWORD pid, char *name, int len)
{

  if (pid == local_player->GetPid())
  {
	  strncpy(name, local_player->GetName(), len);
  }
  else
  {
	PLAYER *player = PGet(pid);

	if (player)
	  strncpy(name, player->GetName(), len);
	else
	  strncpy(name, "Unknown", len);
  }

  name[len] = 0;
  return TRUE;
}

//----------------------------------------------------------------------------
PLAYER *GDBT_PLAYERS::GetNextByView(WORD view_id, int *position)
{
	int i;
	for (i=*position;i<m_count;i++)
	{
		if (m_players[i].GetView() == view_id)
		{
			*position = i+1;
			return &m_players[i];
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
PLAYER *GDBT_PLAYERS::GetByName(char *name)
{
	int i;
	for (i=0;i<m_count;i++)
	{
		if (!strcasecmp(m_players[i].GetName(), name))
		{
			return &m_players[i];
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
PLAYER *GDBT_PLAYERS::GetByTouching(WORD view_id, POINT p)
{
	PLAYER *player;
	int position = 0;

	player = GetNextByView(view_id, &position);
	while (player)
	{
		POINT pp = player->GetPos();
		RECT r;

		SetRect(&r,pp.x, pp.y, pp.x+20, pp.y+20);

		if (sPtInRect(&r, p))
		{
			// tell the player we want to know her innermost attributes.
			SendIntimate(player->GetPid(), TRUE);
			return player;
		}

		player = GetNextByView(view_id, &position);
	}

	return NULL;
}

//----------------------------------------------------------------------------
// This gets called when a local player joins a game and is not the master
// (gets called from within NetManager::EnumPlayers)
BOOL GDBT_PLAYERS::AddPlayer(DWORD pid, BOOL bOwner)
{
	if (m_count >= MAX_PLAYERS)
	{
		Msg("Player table is full - attempt to add player %d", pid);
		return FALSE;
	}

	if (pid == 0)
	{
		Msg("Ignoring attempt to add player 0");
		return FALSE;
	}

	if (PGet(pid))
	{
		Msg("Ignoring attempt to add duplicate player %d", pid);
		return FALSE;
	}

	PLAYER newGuy(pid);

	netman.GetPlayerName(pid, newGuy.GetName(), MAX_FRIENDLY_NAME-1, NULL, 0);

	Insert(&newGuy);

	Msg("Added player %d to local player table", pid);

    TELETYPE::Msg(pid, "%s has joined the game", newGuy.GetName());

	return TRUE;
}

void GDBT_PLAYERS::DelPlayer(DWORD pid)
{
	PLAYER *player = PGet(pid);

	if (player)
	{
		ViewMan::DelPlayer(player->m_view_id);
		Delete(pid);
		if (pid == other_player->GetPid())
		{
			other_player->SetPid(0);
			// show player in other ID
			WIZARD::Cast(SPELL_OTHER_ID, SIG_CLEAR);
			CTRL_VIEW::yourname->SetName("");
		}
	}

}

//----------------------------------------------------------------------------
// This sends the new player id to all players
void GDBT_PLAYERS::SendStartupInfo(DPID to, BOOL bRequestReply, BOOL bOwnsGame)
{
	STARTUPINFO_PKT pkt;

	pkt.bRequestReply = bRequestReply;
	pkt.bOwnsGame = bOwnsGame;
	pkt.view_id = local_player->GetView();
	pkt.pos = local_player->GetPos();
	// pkt.held_object = 0;
	pkt.FreqOut = local_player->GetFreqOut();

	gdbt_players.SendUpdate(
			to,
			(to)?0:ALL_PLAYERS,
			PROC_STARTUPINFO,
			(UDATA*)&pkt, 
			sizeof(pkt),
			GUARANTEED);
}

//----------------------------------------------------------------------------
void GDBT_PLAYERS::ReceiveUpdate(DPID from, DPID to, WORD proc, UDATA *data)
{
	switch (proc)
	{
	case PROC_STARTUPINFO:
		{
			STARTUPINFO_PKT *pkt = (STARTUPINFO_PKT *)data;
			PLAYER *player = PGet(from);
			if (player)
			{
				if (pkt->bOwnsGame)
					Game::SetOwner(from);
				player->SetView(pkt->view_id);
				player->SetFreqOut(pkt->FreqOut);
				if (pkt->bRequestReply)
				{
				  BOOL bOwnsGame =  local_player->GetPid() == Game::GetOwner();
				  SendStartupInfo(from, FALSE, bOwnsGame);
				  if (bOwnsGame)
					  Game::SendState(from);
				}
			}
		}
		break;

	case PROC_ADD_PLAYER:
		{
			ADDPLAYER_PKT *pkt = (ADDPLAYER_PKT*)data;
			AddPlayer(pkt->pid, FALSE);
		}
		break;
	case PROC_SET_VIEWID:
		{
			SETVIEW_PKT *pkt = (SETVIEW_PKT *)data;
			PLAYER *player = PGet(from);
			if (player)
			{
				player->SetView(pkt->view_id);
				player->SetPos(pkt->pos);

				// if we are following this guy around ...
				if (from == local_player->GetFollow())
				  viewman.Load(pkt->view_id);
			}
			else
				Msg("PROC_SET_VIEWID:Could not find player %d", from);
		}
		break;
	case PROC_SET_POS:
		{
			POS_PKT *pkt = (POS_PKT *)data;
			PLAYER *player = PGet(from);
			if (player)
			{
				POINT p;
				p.x = pkt->x;
				p.y = pkt->y;
				player->SetPos(p);

				// if we are following this guy around ...
				if (from == local_player->GetFollow())
				{
				  local_player->SetTheta((WORD)(pkt->r*200));
				}
			}
			else
				Msg("PROC_SET_POS:Could not find player %d", from);
		}
		break;
	case PROC_SET_DETAILS:
		{
			DETAILS_PKT *pkt  = (DETAILS_PKT*)data;
			PLAYER *player = PGet(from);
			if (player)
			{
				cursorman.SetPlayerObject(from, pkt->obj_def_id);
				player->SetPos(pkt->pos);
			}
			else
				Msg("PROC_SET_DETAILS:Could not find player %d", from);
		}
		break;
	case PROC_SET_OBJECT: // some other player has grabbed an object.
		{
			SET_OBJECT_PKT *pkt = (SET_OBJECT_PKT *)data;
			PLAYER *player = PGet(from);
			if (player)
			{
				cursorman.SetPlayerObject(from, pkt->obj_def_id);
				player->MoveHand();
			}
			else
				Msg("PROC_SET_OBJECT:Could not find player %d", from);
		}
		break;
	case PROC_SET_INTIMATE: // Sender has us in other id window.  Set intimate flag.
		{
			SET_INTIMATE_PKT *pkt = (SET_INTIMATE_PKT *)data;
			PLAYER *player = PGet(from);
			if (player)
			{
				// is this player looking at me?
				player->SetIntimate(pkt->intimate);

				// send intimate details to this player:
				if (pkt->intimate)
					local_player->SendIntimateDetails(from);
			}
		}
		break;
	case PROC_INTIMATE_DETAILS: // get the intimate detail of the player in other id window.
		{
			INTIMATE_DETAILS_PKT *pkt = (INTIMATE_DETAILS_PKT *)data;
			PLAYER *player = PGet(from);
			if (player)
			{
				other_player->SetWealth(pkt->Wealth);
				other_player->SetKarma(pkt->Karma);
				other_player->SetEnergy(pkt->Energy);
				other_player->SetStrength(pkt->Strength);
				other_player->SetWisdom(pkt->Wisdom);
				other_player->SetSex(pkt->Sex);
				other_player->SetPid(from);
				// show player in other ID
				WIZARD::Cast(SPELL_OTHER_ID, 0);
				CTRL_VIEW::yourname->SetName(player->GetName());
			}
		}
		break;
	case PROC_EMOTE: // get the intimate detail of the player in other id window.
		{
			EMOTE_PKT *pkt = (EMOTE_PKT *)data;
			PLAYER *player = PGet(from);
			if (player)
			{
			  WIZARD::Cast(SPELL_OTHER_ID, pkt->emotion);
			}
		}
	}
}

//----------------------------------------------------------------------------
void PLAYER::SetView(WORD view_id)
{
	if (m_view_id == local_player->GetView())
	{
		cursorman.DropPlayer(m_pid);
	}

	// decrement count of players in this view
	ViewMan::DelPlayer(m_view_id);

	m_view_id = view_id;

	// increment count of players in this view
	ViewMan::AddPlayer(m_view_id);

	if (m_view_id == local_player->GetView())
	{
		cursorman.AddPlayer(m_pid, friendly_name);
		local_player->SendDetails(m_pid, 0);
	}
}

//----------------------------------------------------------------------------
// called when the players hand has moved due to panning or cursor movement.
void PLAYER::MoveHand()
{
	POINT p = m_pos;
	POINT offset = local_player->GetOffset();

	p = g_view->MapToScreen(p);

	if (!g_view->PtInView(p))
		return;

	cursorman.MovePlayer(m_pid, p);
}

//----------------------------------------------------------------------------
void PLAYER::SetPos(POINT p)
{
	m_pos = p;
	MoveHand();
}

//----------------------------------------------------------------------------
void PLAYER::SetName(const char *name)
{
	strncpy(friendly_name, name, MAX_FRIENDLY_NAME);
	friendly_name[MAX_FRIENDLY_NAME-1] = 0;
	TELETYPE::Refresh();
}

//----------------------------------------------------------------------------
void LOCAL_PLAYER::SendView(DWORD to, WORD group)
{
	SETVIEW_PKT pkt;

	pkt.view_id = m_view_id;
	pkt.pos = m_pos;

	gdbt_players.SendUpdate(
		to,
		group,
		PROC_SET_VIEWID,
		(UDATA*)&pkt, 
		sizeof(pkt));
}

//----------------------------------------------------------------------------
void LOCAL_PLAYER::SetView(WORD view_id)
{
	g_view = viewman.GetView(view_id);

	// First, drop all the hands that we are holding in the current view:
	cursorman.DropAllHands();

	// Change views and tell everyone:
	m_view_id = view_id;

	// Hold hands with everyone in the new view:
	PLAYER *player;
	int position = 0;
	player = gdbt_players.GetNextByView(m_view_id, &position);
	while (player)
	{
		cursorman.AddPlayer(player->GetPid(), player->GetName());
		player = gdbt_players.GetNextByView(m_view_id, &position);
	}

	SendView(0, ALL_PLAYERS);
	SendDetails(0, PLAYERS_IN_VIEW);
}

//----------------------------------------------------------------------------
void LOCAL_PLAYER::SetOffset(POINT offset)	// offset of player into current view canvas
{
	PLAYER *player;
	int position = 0;

	SetPos(m_pos);

	m_offset = offset;

	// adjust all other players hands for the new offset
	player = gdbt_players.GetNextByView(m_view_id, &position);
	while (player)
	{
		player->MoveHand();
		player = gdbt_players.GetNextByView(m_view_id, &position);
	}
}

//----------------------------------------------------------------------------
void LOCAL_PLAYER::SetPos(POINT pos)
{
	m_bDirty = TRUE;
	m_pos = pos;
	cursorman.cursorMove(pos);
}

//----------------------------------------------------------------------------
void LOCAL_PLAYER::SetName(const char *name)
{
  PLAYER::SetName(name);
  netman.SetPlayerName(m_pid, name, name);
  TELETYPE::Refresh();
  CTRL_VIEW::myname->SetName(name);
}

//----------------------------------------------------------------------------
void LOCAL_PLAYER::Reflect() // update remote nodes:
{
  POS_PKT pkt;
  POINT pos;

  if (!m_bDirty)
	return;

  m_bDirty = FALSE;

	if (!g_view->PtInView(m_pos))
	{
		// ### hide remote instances of my hand
		return;
	}

	// Remote players view us relative to our current scroll position:
	pos = g_view->MapToCanvas(m_pos);
	pkt.x = (WORD)pos.x;
	pkt.y = (WORD)pos.y;
	pkt.r = local_player->GetTheta()/200;

	gdbt_players.SendUpdate(
		0,
		PLAYERS_IN_VIEW,
		PROC_SET_POS,
		(UDATA*)&pkt, 
		sizeof(pkt));
}

//----------------------------------------------------------------------------
static void SendHeldObject(DWORD to, WORD flags, WORD obj_def_id)
{
	SET_OBJECT_PKT pkt;

	pkt.obj_def_id = obj_def_id;

	gdbt_players.SendUpdate(
		to,
		flags,
		PROC_SET_OBJECT,
		(UDATA*)&pkt, 
		sizeof(pkt));
}

//----------------------------------------------------------------------------
void LOCAL_PLAYER::SetObject(WORD obj_def_id)
{
	SendHeldObject(0, PLAYERS_IN_VIEW, obj_def_id);
}

//----------------------------------------------------------------------------
void LOCAL_PLAYER::SendDetails(DWORD to, WORD flags)
{
	DETAILS_PKT pkt;

	pkt.pos = m_pos;
	pkt.obj_def_id = cursorman.HeldObject();
	gdbt_players.SendUpdate(
		to,
		flags,
		PROC_SET_DETAILS,
		(UDATA*)&pkt, 
		sizeof(pkt));
}

//----------------------------------------------------------------------------
void GDBT_PLAYERS::SendIntimate(DWORD to, BOOL intimate)
{
	SET_INTIMATE_PKT pkt;

	// if was looking at another player ...
	if (other_player->GetPid())
	{
	  // ... let her know we are no longer interested ...
	  DPID pid = other_player->GetPid();
	  other_player->SetPid(0); // do not recurse infinitly
	  SendIntimate(pid, FALSE);
	}

	pkt.intimate = intimate;
	SendUpdate(
		to,
		NULL,
		PROC_SET_INTIMATE,
		(UDATA*)&pkt, 
		sizeof(pkt));
}

//----------------------------------------------------------------------------
void INTIMATE_PLAYER::GetDetailsPkt(INTIMATE_DETAILS_PKT *pkt)
{
	m_Wealth = pkt->Wealth;
	m_Karma = pkt->Karma;
	m_Energy = pkt->Energy;
	m_Strength = pkt->Strength;
	m_Wisdom = pkt->Wisdom;
	m_Sex = pkt->Sex;
	m_Culture = pkt->Culture;
	m_knows[LANG_PARENT] = pkt->knowsParent;
	m_knows[LANG_VILLAGE] = pkt->knowsVillage;
	m_knows[LANG_CITY] = pkt->knowsCity;
	//SetView(pkt->view_id);
}

//----------------------------------------------------------------------------
void INTIMATE_PLAYER::SetDetailsPkt(INTIMATE_DETAILS_PKT *pkt)
{
	pkt->Wealth = m_Wealth;
	pkt->Karma = m_Karma;
	pkt->Energy = m_Energy;
	pkt->Strength = m_Strength;
	pkt->Wisdom = m_Wisdom;
	pkt->Sex = m_Sex;
	pkt->Culture = m_Culture;
	pkt->knowsParent = m_knows[LANG_PARENT];
	pkt->knowsVillage = m_knows[LANG_VILLAGE];
	pkt->knowsCity = m_knows[LANG_CITY];
	pkt->view_id = m_view_id;
}

//----------------------------------------------------------------------------
void LOCAL_PLAYER::SendEmote(int emotion)
{
  EMOTE_PKT pkt;

  pkt.emotion = emotion;

  gdbt_players.SendUpdate(
	  0,INTIMATE_PLAYERS,
	  PROC_EMOTE,
	  (UDATA*)&pkt, 
	  sizeof(pkt));
}

//----------------------------------------------------------------------------
void LOCAL_PLAYER::SendIntimateDetails(DWORD to)
{
	WORD flags;
	INTIMATE_DETAILS_PKT pkt;

	SetDetailsPkt(&pkt);

	// if no PID was specified then send to all intimate players
	if (!to)
		flags = INTIMATE_PLAYERS;
	else
		flags = NULL;

	gdbt_players.SendUpdate(
		to,
		flags,
		PROC_INTIMATE_DETAILS,
		(UDATA*)&pkt, 
		sizeof(pkt));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BOOL LOCAL_PLAYER::Save(char *filename)
{
	XFile file;
	INTIMATE_DETAILS_PKT pkt;

	SetDetailsPkt(&pkt);

	if (!file.Open(filename, XFile::modeWrite))
		return FALSE;

	file.Write(&pkt, sizeof(pkt));
	file.Write(friendly_name, MAX_FRIENDLY_NAME);
	return TRUE;
}

//----------------------------------------------------------------------------
WORD LOCAL_PLAYER::Load(char *filename)
{
	XFile file;
	INTIMATE_DETAILS_PKT pkt;

	if (!file.Open(filename, XFile::modeRead))
		return FALSE;

	file.Read(&pkt, sizeof(pkt));
	file.Read(friendly_name, MAX_FRIENDLY_NAME);

	GetDetailsPkt(&pkt);

	return pkt.view_id;
}
