/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       gdb.h
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
#ifndef __GDB_INCLUDED__
#define __GDB_INCLUDED__

#ifndef DPID
typedef DWORD DPID;
#endif

#define DB_LOCAL		0x0001
#define DB_REMOTE		0x0002

#define GUARANTEED			TRUE

#define ALL_PLAYERS			0x0001
#define PLAYERS_IN_VIEW		0x0002
#define TABLE_OWNER			0x0004
#define INTIMATE_PLAYERS	0x0008

#define GLOBAL_LISTENERS	0x0010
#define VIEW_LISTENERS		0x0020
#define FREQ_LISTENERS		0x0040
#define INTIMATE_LISTENERS	0x0080

#define IDT_PLAYERS		0
#define IDT_MACHINES	1
#define IDT_MESSAGES	2
#define IDT_SPELLS		3
#define MAX_GDB_TABLES	4

typedef unsigned char UDATA;

//----------------------------------------------------------------------------
// Global Database Tables:
//
class GDB_TABLE {

	// The Table of all Global Database Tables:
private:
	static int c_count;
	static GDB_TABLE *c_table[MAX_GDB_TABLES];

public:
	static GDB_TABLE *GetTable(int id) 
	{ 
		if (id>=0 && id<c_count) // make sure id is in range
			return c_table[id]; 
		else
			return NULL;
	}

private:
	BYTE m_global_table_id;

public:
	virtual void ReceiveUpdate(DPID from, DPID to, WORD proc, UDATA *data) = 0;

public:

	GDB_TABLE(BYTE global_table_id);

	static Poll();

	//*************************************************
	// The following tranactions are called by the application.
	// The GDB determines how and when the transactions are reflected
	// to clients and or servers.

	// Update specific columns in specific row in global database.
	void SendUpdate(DWORD to, WORD flags, WORD proc, UDATA *data, WORD length, BOOL bGuaranteed=FALSE);
};

#endif // __GDB_INCLUDED__
