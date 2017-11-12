/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       netctrl.h
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
#ifndef __NETCTRL_INCLUDED__
#define __NETCTRL_INCLUDED__

typedef struct
{
    DWORD           dwType;
} DPMSG_GENERIC;

class  NetManager {
private:
	LPDIRECTPLAY FAR lpDP;		// the global Direct Play object.
	DPSESSIONDESC SDesc;		//
	BOOL session_available; // set to true when we can start txing/rxing
	BOOL m_master;
	DPID m_my_pid;
public:
	static BOOL user_lost_interest;
public:
	NetManager() {session_available=FALSE;}
	~NetManager();

public: // interface
	DWORD GetMessageCount(DPID pid);
	DWORD Receive(DPID *from, DPID *to, char *msg, int max_len);
	HRESULT Send(char *msg, DPID from, DPID to, DWORD msg_len, BOOL bGuaranteed);

	HRESULT CreatePlayer(char *lpFriendlyName, char *lpFormalName);
	HRESULT DestroyPlayer(DPID pid);
	HRESULT EnableNewPlayers(BOOL enable);
	HRESULT	EnumPlayers();
	HRESULT GetPlayerName(DPID pid,
						 char *friendly, 
						 DWORD friendly_len, 
						 char *formal,
						 DWORD formal_len);
    HRESULT SetPlayerName(
						DPID pidID,
						char *lpPlayerFriendlyName,
						char *lpPlayerFormalName);

	static void LostInterest() {user_lost_interest = TRUE;} // lost interest in sessions enum
	int EnumSessions();
	HRESULT Close(BOOL bForceClose);
	HRESULT Open(int ses_ix, char *szFullName, DWORD dwFlags);
	HRESULT GetCaps();
	HRESULT SaveSession(char *szName);
	char *GetSessionDesc(int index);

	void Release();
	int Enumerate(); // return number of service providers in enumeration.
	HRESULT Create(int sp_index);
	char *GetProviderDesc(int index);

	char *error(int code, BOOL bNice);

private:
	void SystemMessage(DPMSG_GENERIC *msg);
};

extern NetManager netman; // the one and only NetManager object.

#endif
