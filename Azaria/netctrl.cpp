/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       netctrl.c
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 02/21/95  gas  Created.
; 
----------------------------------------------------------------------------*/

#define INITGUID
#include "includes.h"
#include "netctrl.h"
#include "gameman.h"

// {AD0582E0-DD77-11cf-BB1B-444553540000}
DEFINE_GUID(ONE_GUID, 0xad0582e1, 0xdd77, 0x11cf, 0xbb, 0x1b, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);

NetManager netman;  // the one and only NetManager object.

#define DESCSTRLEN 50
BOOL NetManager::user_lost_interest;

extern HRESULT WINAPI DirectPlayCreate( LPGUID lpGUID, IDirectPlay **lplpDP, IUnknown *pUnk);
extern HRESULT WINAPI DirectPlayEnumerate( LPDPENUMDPCALLBACKA, LPVOID );

HRESULT WINAPI DirectPlayCreate( LPGUID lpGUID, IDirectPlay **lplpDP, IUnknown *pUnk)
{
    assert(false);
    return 0;
}
HRESULT WINAPI DirectPlayEnumerate( LPDPENUMDPCALLBACKA, LPVOID )
{
    assert(false);
    return 0;
}

//___________________________________________________________________________
//
//  Direct Play Session Management (DATA)
//
typedef struct {
	DWORD ses_id;
	char SesDesc[DESCSTRLEN];
} TBL_SESSIONS;

#define MAX_SES	30
static TBL_SESSIONS tbl_ses[MAX_SES];
static int tbl_ses_count = 0;

//___________________________________________________________________________
//
//  Direct Play Management (DATA)
//
typedef struct {
	LPGUID pGUID;
	char DrvDesc[DESCSTRLEN];
} TBL_SERVICE_PROVIDERS;

#define MAX_SP	8
static TBL_SERVICE_PROVIDERS tbl_sp[MAX_SP];
static int tbl_sp_count = 0;
static int tbl_sp_selected = -1;

//----------------------------------------------------------------------------
NetManager::~NetManager()
{
	Release();
}

#define CS(X,MSG) case X: return (bNice)?MSG:#X;
const char * NetManager::error(int hr, BOOL bNice)
{
			return "Unknown";
}

//___________________________________________________________________________
//
//  Direct Play MESSAGE Management
//
//  GetMessageCount	Retrieves the number of messages waiting for a player.
//  Receive			Retrieves messages that have been sent to a player.  
//  Send			Sends messages to other players or all of the players in a session. 

//----------------------------------------------------------------------------
DWORD NetManager::GetMessageCount(DPID pid)
{
	DWORD count;
	lpDP->GetMessageCount(pid, &count);
	return count;
}

//----------------------------------------------------------------------------
void NetManager::SystemMessage(DPMSG_GENERIC *msg)
{
	DPMSG_ADDPLAYER	*add_msg = (DPMSG_ADDPLAYER*)msg;

	// process system message
	switch (msg->dwType)
	{
	case DPSYS_ADDPLAYER:	// A player or group has been added to the session.
		// dwPlayerType indicates whether it is a player or a group.
		if (add_msg->dwPlayerType == TRUE)	
		{
			if (add_msg->dpId == m_my_pid)
			{
				// Let the game manager know we are joining into a network game
				// gameman.JoinNetGame(m_my_pid, m_master);
			}

			// Add the player to our local player table
			gameman.AddPlayer(add_msg->dpId, m_master);
		}
		break;
	case DPSYS_ADDPLAYERTOGROUP:	// An existing player has been added to an existing group.
		break;
	case DPSYS_DELETEGROUP:	// A group has been deleted from the session.
		break;
	case DPSYS_DELETEPLAYER:// A player has been deleted from the session.
		{
			DPMSG_DELETEPLAYER *pkt = (DPMSG_DELETEPLAYER*)msg;
			gameman.DelPlayer(pkt->dpId);
		}
		break;
	case DPSYS_DELETEPLAYERFROMGRP:	// A player has been removed from a group.
		break;
	case DPSYS_SESSIONLOST:	// The connection has been lost.
		gameman.SessionLost();
		Close(TRUE);
		break;
	}
}

//----------------------------------------------------------------------------
DWORD NetManager::Receive(DPID *from, DPID *to, const char *msg, int max_len)
{
	DWORD msg_len;
	HRESULT hr;

	if (lpDP == NULL || !session_available)
		return 0;

dpx_receive_retry:
	msg_len = max_len;
	hr = lpDP->Receive(from, to, DPRECEIVE_ALL, msg, &msg_len);
	if (hr == DPERR_NOMESSAGES)
	{
		return 0;
	}
	else if (hr != DP_OK)
	{
		Msg("Receive from %x to %x failed:%s ", *from, *to, error(hr, TRUE));
		return 0;
	}

	// check for system message
	if (*to == 0 || *from == 0)
	{
		SystemMessage((DPMSG_GENERIC*)msg);
		goto dpx_receive_retry;
	}

	// skip messages from myself
	if (*from == m_my_pid)
		goto dpx_receive_retry;

	// if the message is not to local player
	if (*to != m_my_pid)
		goto dpx_receive_retry;

	return msg_len;
}

//----------------------------------------------------------------------------
HRESULT NetManager::Send(const char *msg, DPID from, DPID to, DWORD msg_len, BOOL bGuaranteed)
{
	HRESULT hr;

	if (lpDP == NULL || !session_available)
		return E_FAIL;

	hr = lpDP->Send(from, to, (bGuaranteed)?DPSEND_GUARANTEE:0, msg, msg_len);
	if (hr != DP_OK)
	{
		switch (hr)
		{
		case DPERR_INVALIDPLAYER:
			Msg("Send from %d to %d failed:DPERR_INVALIDPLAYER ", from, to);
			gameman.DelPlayer(to);
			break;
		case DPERR_INVALIDOBJECT:
			Msg("Send from %d to %d failed:DPERR_INVALIDOBJECT ", from, to);
			break;
		case DPERR_INVALIDPARAMS:
			Msg("Send from %d to %d failed:DPERR_INVALIDPARAMS ", from, to);
			break;
		case DPERR_BUSY:
			Msg("Send from %d to %d failed:DPERR_BUSY ", from, to);
			break;
		default:
			Msg("Send from %d to %d failed: %x ", from, to, hr);
		}
	}

	return hr;
}

//___________________________________________________________________________
//
//  Direct Play GROUP Management
//
//  AddPlayerToGroup		Adds a player to an existing group. 
//  CreateGroup				Creates an empty group of players for a session.
//  DeletePlayerFromGroup	Deletes a player from a group.
//  DestroyGroup			Destroys a group of players for a session.
//  EnumGroupPlayers		Enumerates the players in a group.
//  EnumGroups				Enumerates all of the groups associated 


//___________________________________________________________________________
//
//  Direct Play PLAYER Management
//
//  CreatePlayer		Creates a player for a session.
//  DestroyPlayer		Deletes a player from a sessions. 
//  EnableNewPlayers	Enables or disables the addition of new players or groups.
//  EnumPlayers			Enumerates all of the players in a specified session.
//  GetPlayerCaps		Retrieves the capabilities of a player's connection.
//  GetPlayerName		Retrieves a player's friendly and formal names.
//  SetPlayerName		Changes the player's friendly and formal names.

//----------------------------------------------------------------------------
HRESULT NetManager::CreatePlayer(char *lpFriendlyName, char *lpFormalName)
{
	DPID pid;
	HRESULT hr;

	if (lpDP == NULL)
		return E_FAIL;

	hr = lpDP->CreatePlayer(&pid, lpFriendlyName, lpFormalName, 0);
	if (hr == DPERR_NOCONNECTION)
	{
		session_available = FALSE;
	}

	if (hr != DP_OK)
	{
		Msg("Unable to open session");
		session_available = FALSE;
	}
	else
	{
		session_available = TRUE;
		m_my_pid = pid;
		gameman.JoinNetGame(pid, lpFriendlyName, m_master);
	}

	return hr;
}

//----------------------------------------------------------------------------
HRESULT NetManager::DestroyPlayer(DPID pid)
{
	if (lpDP == NULL)
		return E_FAIL;

	return lpDP->DestroyPlayer(pid);
}

//----------------------------------------------------------------------------
HRESULT NetManager::EnableNewPlayers(BOOL enable)
{
	if (lpDP == NULL)
		return E_FAIL;

	return lpDP->EnableNewPlayers(enable);
}

//----------------------------------------------------------------------------
static BOOL EnumPlayersCallback(
						DPID pidID,
						LPSTR lpFriendlyName,
						LPSTR lpFormalName,
						DWORD dwFlags,
						LPVOID lpContext)
{
	gameman.AddPlayer(pidID, FALSE);
	return TRUE;
}

//----------------------------------------------------------------------------
HRESULT NetManager::EnumPlayers()
{
	HRESULT hr;

	if (lpDP == NULL)
		return 0;

	hr = lpDP->EnumPlayers(0, EnumPlayersCallback, 0, 0);
	return hr;
}


//----------------------------------------------------------------------------
HRESULT NetManager::GetPlayerName(DPID pid,
						 char *friendly, 
						 DWORD friendly_len, 
						 char *formal,
						 DWORD formal_len)
{
  DWORD a_len = friendly_len;
  DWORD b_len = formal_len;

  if (lpDP)
	return lpDP->GetPlayerName(pid, friendly, &a_len, formal,  &b_len);
  else
	return DPERR_UNAVAILABLE;
}

//----------------------------------------------------------------------------
HRESULT NetManager::SetPlayerName(
	DPID pidID,
	const char *lpPlayerFriendlyName,
	const char *lpPlayerFormalName)
{
  if (lpDP)
    return lpDP->SetPlayerName(pidID, lpPlayerFriendlyName, lpPlayerFormalName);
  else
	return DPERR_UNAVAILABLE;
}


//___________________________________________________________________________
//
//  Direct Play SESSION Management
//
//  Close				Closes a communication channel. 
//  EnumSessions		Enumerates all of the sessions connected to a specified DirectPlay object.
//  GetCaps				Retrieves the capabilities of the specified DirectPlay object.
//  Open				Establishes a new gaming session or connects to an existing one. 
//  SaveSession			Saves the current session in the registry.

//
// Enumerate Game Sessions within a Service Providers.
//
static BOOL FAR PASCAL EnumSessionsCallback(
			LPDPSESSIONDESC lpDPSGameDesc,
    		LPVOID lpContext,
    		LPDWORD lpdwTimeOut,
    		DWORD dwFlags)
{
	if (dwFlags & DPESC_TIMEDOUT)
		return FALSE;

	if (!lpDPSGameDesc)
		return FALSE;

	tbl_ses[tbl_ses_count].ses_id = lpDPSGameDesc->dwSession;
	strncpy(tbl_ses[tbl_ses_count].SesDesc, lpDPSGameDesc->szSessionName, DESCSTRLEN);
	tbl_ses[tbl_ses_count].SesDesc[DESCSTRLEN-1] = 0;

	tbl_ses_count++;
	if (tbl_ses_count >=MAX_SES
		|| NetManager::user_lost_interest)
		return FALSE;
	else
		return TRUE;

	return TRUE;
}

//----------------------------------------------------------------------------
int NetManager::EnumSessions()
{
	DWORD dwTimeout;
	HRESULT hr;

	dwTimeout = 2000; // 10 seconds
	tbl_ses_count = 0;

	if (!lpDP)
		return 0;

	memset(&SDesc, 0, sizeof(DPSESSIONDESC));
	SDesc.dwSize = sizeof(DPSESSIONDESC);
	SDesc.guidSession = ONE_GUID;

	// could return DPERR_NOSESSIONS
	user_lost_interest = FALSE;
	hr = lpDP->EnumSessions(&SDesc, dwTimeout, EnumSessionsCallback, NULL, 0);

	return tbl_ses_count;
}

//----------------------------------------------------------------------------
char *NetManager::GetSessionDesc(int index)
{
	if (index < 0
	  || index >= tbl_ses_count)
	{
		return NULL;
	}
	else
	{
		return tbl_ses[index].SesDesc;
	}
}


//----------------------------------------------------------------------------
HRESULT NetManager::Close(BOOL bForceClose)
{
	HRESULT hr;

	if (session_available || bForceClose)
	{
		if (lpDP)
			hr = lpDP->Close();
		usleep(100); // give the dll a time slice so it can shut down
	}
	else
		hr  = DPERR_UNAVAILABLE; 

	session_available = FALSE;

	return hr;
}

//----------------------------------------------------------------------------
HRESULT NetManager::Open(int ses_ix, char *szFullName, DWORD dwFlags)
{
	HRESULT hr;

	if (tbl_sp_selected < 0)
		return DPERR_UNAVAILABLE;

	Close(FALSE);

	memset(&SDesc, 0, sizeof(DPSESSIONDESC));
	SDesc.dwSize = sizeof(DPSESSIONDESC);

	SDesc.guidSession = ONE_GUID; // This is the GUID of the GAME.

	if (dwFlags == DPOPEN_CREATESESSION) // Create new session
	{
		m_master= TRUE; // I am the master of this game
		SDesc.dwFlags = DPOPEN_CREATESESSION;
        strcpy(SDesc.szSessionName, szFullName);
		SDesc.dwMaxPlayers = 100;
	}
	else
	{
		m_master= FALSE; // I am NOT the master of this game
		SDesc.dwFlags = DPOPEN_OPENSESSION;
		SDesc.dwSession = tbl_ses[ses_ix].ses_id;
	}

	hr = lpDP->Open(&SDesc);
	if (hr != DP_OK)
	{
		Msg("Unable to open session: %s", error(hr, TRUE));
		return hr;
	}

	return DP_OK;
}

//----------------------------------------------------------------------------
HRESULT NetManager::GetCaps()
{
	DPCAPS DPCaps;
	return lpDP->GetCaps(&DPCaps);
}

//----------------------------------------------------------------------------
HRESULT NetManager::SaveSession(char *szName)
{
	return lpDP->SaveSession(szName);
}

//___________________________________________________________________________
//
//  Direct Play Management
//

//----------------------------------------------------------------------------
void NetManager::Release()
{
  if (lpDP)
	lpDP->Release();

  lpDP = NULL;

  // If another instance of the app is running on the desktop,
  // then let him have some time to shut down his session.
  usleep(100);
}

//
// Load a service provider DLL and initialize it.
//
HRESULT NetManager::Create(int sp_index)
{
	// If a service provider was already loaded, then close it:
	Release();

	tbl_sp_selected = sp_index;
	return DirectPlayCreate(tbl_sp[sp_index].pGUID, &lpDP, NULL);
}

//----------------------------------------------------------------------------
// Enumerate Service Providers.
static BOOL FAR PASCAL EnumProvidersCallback(
		LPGUID lpGUID,
		LPSTR lpDrvDesc,
		DWORD dwMajorVersion,
		DWORD dwMinorVersion,
		LPVOID lpContext)
{
	if (!lpGUID || !lpDrvDesc)
	  return FALSE;

	tbl_sp[tbl_sp_count].pGUID = lpGUID;  // This is the GUID of the SP
	strncpy(tbl_sp[tbl_sp_count].DrvDesc, lpDrvDesc, DESCSTRLEN);
	tbl_sp[tbl_sp_count].DrvDesc[DESCSTRLEN-1] = 0;

	tbl_sp_count++;
	if (tbl_sp_count >=MAX_SP)
		return FALSE;
	else
		return TRUE;
}

//----------------------------------------------------------------------------
int NetManager::Enumerate()
{
	// We cannot ennumerate sessions IF a session is already open:
	Close(FALSE);
	Release();

	tbl_sp_count = 0;
	DirectPlayEnumerate(EnumProvidersCallback, NULL);
	return tbl_sp_count;
}

//----------------------------------------------------------------------------
const char *NetManager::GetProviderDesc(int index)
{
	if (index < 0
	  || index >= tbl_sp_count)
	{
		return NULL;
	}
	else
	{
		return tbl_sp[index].DrvDesc;
	}
}


