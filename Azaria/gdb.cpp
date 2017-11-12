/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       gdb.c
;
; Description:
;	Routing of global database commands.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 03/05/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include <dplay.h>
#include "maya.h"
#include "gdb.h"

#include "netctrl.h"
#include "player.h"
#include "gameman.h"

// class storage variables for the global database tables-
int GDB_TABLE::c_count;
GDB_TABLE* GDB_TABLE::c_table[MAX_GDB_TABLES];

void gdbTX_AllPlayersInView(char *pkt, int size);
void gdbTX_AllPlayers(char *pkt, int size);

typedef struct {
	BYTE tbl;
	BYTE proc;
} GDB_CMD_HEADER;

#define MAX_PKT_LEN	600
typedef struct {
	GDB_CMD_HEADER cmd;
	char data[MAX_PKT_LEN];
} GDB_GENERIC_PKT;

static GDB_GENERIC_PKT gpkt;
static GDB_GENERIC_PKT rx_pkt;

static void gdbRX_UpdateObject(DPID from, DPID to, GDB_GENERIC_PKT *pkt);

//----------------------------------------------------------------------------
static void gdbTX_AllPlayers(char *pkt, int size, BOOL bGuaranteed)
{
	DWORD pid = local_player->GetPid();
	int i;
	PLAYER *player;

	for (i=0;i<gdbt_players.Count();i++)
	{
		player = gdbt_players.GetPlayer(i);

		if (player->GetPid() == pid)
			continue; // do not broadcast to ourselves.

		netman.Send(pkt, pid, player->GetPid(), size, bGuaranteed);
	}
}

//----------------------------------------------------------------------------
static void gdbTX_AllPlayersInView(char *pkt, int size, BOOL bGuaranteed)
{
	DWORD pid = local_player->GetPid();
	int view_id = local_player->GetView();
	int i;
	PLAYER *player;

	for (i=0;i<gdbt_players.Count();i++)
	{
		player = gdbt_players.GetPlayer(i);
		if (player->GetPid() == pid)
			continue; // do not broadcast to ourselves.

		if (player->GetView() ==  view_id)
			netman.Send(pkt, pid, player->GetPid(), size, bGuaranteed);
	}
}

//----------------------------------------------------------------------------
static void gdbTX_IntimatePlayers(char *pkt, int size, BOOL bGuaranteed)
{
	DWORD pid = local_player->GetPid();
	int i;
	PLAYER *player;

	for (i=0;i<gdbt_players.Count();i++)
	{
		player = gdbt_players.GetPlayer(i);
		if (player->GetPid() == pid)
			continue; // do not broadcast to ourselves.

		if (player->Intimate())
			netman.Send(pkt, pid, player->GetPid(), size, bGuaranteed);
	}
}

//----------------------------------------------------------------------------
static void gdbTX_GlobalListeners(char *pkt, int size, BOOL bGuaranteed)
{
	DWORD pid = local_player->GetPid();
	int i;
	PLAYER *player;

	for (i=0;i<gdbt_players.Count();i++)
	{
		player = gdbt_players.GetPlayer(i);
		if (player->GetPid() == pid)
			continue; // do not broadcast to ourselves.

		if (player->GetListenGlobal())
			netman.Send(pkt, pid, player->GetPid(), size, bGuaranteed);
	}
}

//----------------------------------------------------------------------------
static void gdbTX_ViewListeners(char *pkt, int size, BOOL bGuaranteed)
{
	int i;
	PLAYER *player;
	DWORD pid = local_player->GetPid();
	int view_id = local_player->GetView();

	for (i=0;i<gdbt_players.Count();i++)
	{
		player = gdbt_players.GetPlayer(i);
		if (player->GetPid() == pid)
			continue; // do not broadcast to ourselves.

		if (player->GetListenView() && (player->GetView() ==  view_id))
			netman.Send(pkt, pid, player->GetPid(), size, bGuaranteed);
	}
}

//----------------------------------------------------------------------------
static void gdbTX_FreqListeners(char *pkt, int size, BOOL bGuaranteed)
{
	DWORD pid = local_player->GetPid();
	int i;
	PLAYER *player;

	for (i=0;i<gdbt_players.Count();i++)
	{
		player = gdbt_players.GetPlayer(i);
		if (player->GetPid() == pid)
			continue; // do not broadcast to ourselves.

		if (player->GetListenFreq())
			netman.Send(pkt, pid, player->GetPid(), size, bGuaranteed);
	}
}

//----------------------------------------------------------------------------
static void gdbTX_IntimateListeners(char *pkt, int size, BOOL bGuaranteed)
{
	DWORD pid = local_player->GetPid();
	int i;
	PLAYER *player;

	for (i=0;i<gdbt_players.Count();i++)
	{
		player = gdbt_players.GetPlayer(i);
		if (player->GetPid() == pid)
			continue; // do not broadcast to ourselves.

		if (player->Intimate() && player->GetListenIntimate())
			netman.Send(pkt, pid, player->GetPid(), size, bGuaranteed);
	}
}

//----------------------------------------------------------------------------
GDB_TABLE::GDB_TABLE(BYTE global_table_id)
{
	assert(global_table_id >= 0);
	assert(global_table_id < MAX_GDB_TABLES);

	// set class variables:
	c_count++;
	c_table[global_table_id] = this;

	// set instance:
	m_global_table_id = global_table_id;
}

//----------------------------------------------------------------------------
GDB_TABLE::~GDB_TABLE()
{
	c_count--;
	c_table[m_global_table_id] = NULL;
}

//----------------------------------------------------------------------------
void GDB_TABLE::SendUpdate(DWORD to, WORD flags, WORD proc, UDATA *data, WORD length, BOOL bGuaranteed)
{
	GDB_GENERIC_PKT pkt;
	int pkt_size = sizeof(GDB_CMD_HEADER) + length;

	pkt.cmd.tbl = m_global_table_id;
	pkt.cmd.proc = (BYTE)proc;

	memcpy(pkt.data, data, length);

	if (flags & ALL_PLAYERS)
		gdbTX_AllPlayers((char*)&pkt, pkt_size, bGuaranteed);
	else if (flags & PLAYERS_IN_VIEW)
		gdbTX_AllPlayersInView((char*)&pkt, pkt_size, bGuaranteed);
	else if (flags & TABLE_OWNER)
	  netman.Send((char*)&pkt, local_player->GetPid(), Game::GetOwner(), pkt_size, bGuaranteed);
	else if (flags & INTIMATE_PLAYERS)
		gdbTX_IntimatePlayers((char*)&pkt, pkt_size, bGuaranteed);
	else if (flags & GLOBAL_LISTENERS)
		gdbTX_GlobalListeners((char*)&pkt, pkt_size, bGuaranteed);
	else if (flags & VIEW_LISTENERS)
		gdbTX_ViewListeners((char*)&pkt, pkt_size, bGuaranteed);
	else if (flags & FREQ_LISTENERS)
		gdbTX_FreqListeners((char*)&pkt, pkt_size, bGuaranteed);
	else if (flags & INTIMATE_LISTENERS)
		gdbTX_IntimateListeners((char*)&pkt, pkt_size, bGuaranteed);
	else
		netman.Send((char*)&pkt, local_player->GetPid(), to, pkt_size, bGuaranteed);
}

//----------------------------------------------------------------------------
static void gdbRX_UpdateObject(DPID from, DPID to, GDB_GENERIC_PKT *pkt)
{
	GDB_TABLE *gdbt = GDB_TABLE::GetTable(pkt->cmd.tbl);

	if (gdbt) // goes straight to table ....
		gdbt->ReceiveUpdate(from, to, pkt->cmd.proc, (UDATA*)pkt->data);
	else
		Msg("gdbRX UpdateObject: Unknown table - %d", pkt->cmd.tbl);
}

//----------------------------------------------------------------------------
int GDB_TABLE::Poll()
{
	DPID from, to;
	int msg_len;
	int count = 0;

	msg_len = netman.Receive(&from, &to, (char*)&rx_pkt, MAX_PKT_LEN);
	while (msg_len)
	{
		gdbRX_UpdateObject(from, to, (GDB_GENERIC_PKT*)&rx_pkt);
		count++;

		msg_len = netman.Receive(&from, &to, (char*)&rx_pkt, MAX_PKT_LEN);
	}

	return count;
}
