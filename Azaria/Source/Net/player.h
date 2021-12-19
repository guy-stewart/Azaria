/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       player.h
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

#define MAX_PLAYERS	256

#ifndef DWORD
typedef uint32_t DWORD;
#endif

#define MAX_FRIENDLY_NAME	16

typedef struct {
	BYTE Global:1;
	BYTE View:1;
	BYTE Freq:1;
	BYTE Intimate:1;
	BYTE FreqFreq;
}FREQOUT;

//----------------------------------------------------------------------------
class GDBT_PLAYERS ;

class PLAYER {
	friend GDBT_PLAYERS;

protected:
	DPID m_pid;	// player id as known via direct play.
	POINT m_pos;	// x,y position of cursor on display
	WORD m_view_id;		// current view in main viewer (VC_VIEWSCREEN class view).
	WORD m_intimate:1;	// This player is viewing my ID !
#define FEMALE	0
#define MALE	1
	WORD m_Sex:1;
#define CULT_CITY	0
#define CULT_VILLAGE	1
	WORD m_Culture:1;

	FREQOUT m_Freq;
	WORD held_object;
	char friendly_name[MAX_FRIENDLY_NAME+1];

public:
    PLAYER(DPID pid=0) 
	{
	  m_pid=pid; m_view_id=0; m_intimate=FALSE;
	  strncpy(friendly_name, "Nameless", MAX_FRIENDLY_NAME);
	  m_Freq.Global = 1;
	  m_Freq.View = 1;
	  m_Freq.Freq = 1;
	  m_Freq.Intimate = 1;
	}

	inline DPID GetGroupKey() {return m_pid;}
	inline DPID GetPKey() {return m_pid;}
	inline DPID GetUKey() {return m_view_id;}

	inline void SetPid(DWORD pid) {m_pid = pid;}
	inline DWORD GetPid() {return m_pid;}
	inline int GetView() {return m_view_id;}
	inline POINT GetPos() {return m_pos;}
	inline void SetIntimate(BOOL intimate) {m_intimate = intimate;}
	inline BOOL Intimate() {return m_intimate;}

	void SetView(WORD view_id);
	void SetPos(POINT pos);
	void SetName(const char *name);
	char *GetName() {return friendly_name;}

	int GetSex() {return m_Sex;}
	void SetSex(int sex) {m_Sex = sex;}
	int GetCult() {return m_Culture;}
	void SetCult(int cult) {m_Culture = cult;}

	inline void SetHeldObject(WORD obj_def_id) {held_object = obj_def_id;}
	inline WORD GetHeldObject() {return held_object;}

	inline BOOL GetListenGlobal() {return m_Freq.Global;}
	inline BOOL GetListenView() {return m_Freq.View;}
	inline BOOL GetListenFreq() {return m_Freq.Freq;}
	inline BOOL GetListenIntimate() {return m_Freq.Intimate;}
	inline BYTE GetFreq() {return m_Freq.FreqFreq;}
	inline FREQOUT GetFreqOut() {return m_Freq;}

	inline void SetListenGlobal(BOOL listen) {m_Freq.Global = listen;}
	inline void SetListenView(BOOL listen) {m_Freq.View = listen;}
	inline void SetListenFreq(BOOL listen) {m_Freq.Freq = listen;}
	inline void SetListenIntimate(BOOL listen) {m_Freq.Intimate = listen;}
	inline void SetFreq(BYTE Freq) {m_Freq.FreqFreq = Freq;}
	inline void SetFreqOut(FREQOUT FreqOut) {m_Freq = FreqOut;}

	// called when the players hand has moved due to panning or cursor movement.
	// Is also used to jiggle newly obtained object.
	void MoveHand();
};


//----------------------------------------------------------------------------
// Only include the global database table for files that use it:
#ifdef __GDB_INCLUDED__

#include "btable.h"

#define BPD_TABLE B_TABLE<PLAYER, DPID, DPID, DPID>

class GDBT_PLAYERS : public BPD_TABLE, public GDB_TABLE {
private:
	PLAYER m_players[MAX_PLAYERS];
private:
	virtual void ReceiveUpdate(DPID from, DPID to, WORD proc, UDATA *data);
	inline void SetCount(int count) {m_count = count;}
public:
	GDBT_PLAYERS(int bytes_per_record):
		BPD_TABLE(m_players, 0, MAX_PLAYERS),
		GDB_TABLE(IDT_PLAYERS)
		{};

	void SendStartupInfo(DPID to_specific_player, BOOL bRequestReply, BOOL bOwnsGame);
	void SendIntimate(DWORD to, BOOL intimate);
	BOOL SetOtherPlayer();
	BOOL AddPlayer(DWORD pid, BOOL bOwner);
    void DelPlayer(DWORD pid);
	PLAYER *Get(DWORD pid) {return BPD_TABLE::PGet(pid);}
	PLAYER *GetPlayer(int index) {return &(m_players[index]);};
	DWORD GetPlayerPid(int index) {return m_players[index].GetPid();};
	PLAYER *GetNextByView(WORD view_id, int *position);
    PLAYER *GetByTouching(WORD view_id, POINT p);
    PLAYER *GetByName(char *name);
    BOOL GetPlayerName(DWORD pid, char *name, int len);
};

extern GDBT_PLAYERS gdbt_players;

#endif // ifdef GDB_TABLE


//----------------------------------------------------------------------------

enum {
	LANG_PARENT=0,
	LANG_VILLAGE,
	LANG_CITY,
};

typedef struct {
	WORD Wealth;
	WORD Karma;
	WORD Energy;
	WORD Strength;
	WORD Wisdom;
	WORD Sex:1;
	WORD Culture:1;
	WORD knowsParent:1;
	WORD knowsVillage:1;
	WORD knowsCity:1;
	WORD view_id;
} INTIMATE_DETAILS_PKT;

class INTIMATE_PLAYER : public PLAYER {
protected:
	WORD m_theta;
	POINT m_offset;

	WORD m_Wealth;
	WORD m_Karma;
	WORD m_Energy;
	WORD m_Strength;
	WORD m_Wisdom;
	WORD m_knows[3]; // parent, village, city
public:
	INTIMATE_PLAYER():PLAYER() 
	{
		m_theta=0;
		m_Wealth = 10;
		m_Wisdom = 60;
		m_Strength = 3;
		m_Energy = 13;
		m_Sex = FEMALE;
		m_knows[LANG_PARENT] = FALSE; // parent
		m_knows[LANG_VILLAGE] = FALSE; // village
		m_knows[LANG_CITY] = FALSE; // city
	}

    void GetDetailsPkt(INTIMATE_DETAILS_PKT *pkt);
    void SetDetailsPkt(INTIMATE_DETAILS_PKT *pkt);

	inline WORD GetKarma() {return m_Karma;}
	inline WORD GetEnergy() {return m_Energy;}
	inline WORD GetStrength() {return m_Strength;}
	inline WORD GetWisdom() {return m_Wisdom;}
	inline WORD GetSex() {return m_Sex;}
	inline WORD GetWealth() {return m_Wealth;}
	inline void SetKarma(WORD karma) {m_Karma = karma;}
	inline void SetEnergy(WORD energy) {m_Energy = energy;}
	inline void SetStrength(WORD strength) {m_Strength = strength;}
	inline void SetWisdom(WORD wisdom) {m_Wisdom = wisdom;}
	inline void SetSex(WORD sex) {m_Sex = sex;}
	inline void SetWealth(WORD wealth) {m_Wealth = wealth;}

	inline BOOL knows(int x) {return m_knows[x];}

	POINT GetOffset() {return m_offset;}
	void SetTheta(WORD theta) {m_theta = theta;}
	WORD GetTheta() {return m_theta;}
};

class LOCAL_PLAYER : public INTIMATE_PLAYER {
private:
  POINT m_continent;
  BOOL m_bDirty;	// player needs to update remote nodes.
  DPID m_follow;
public:
	void Reflect(); // update remote nodes.

	inline POINT GetPosContinent() {return m_continent;}
	inline void  SetPosContinent(POINT p) {m_continent = p;}

	inline void SetKarma(WORD karma) 
	{
		m_Karma = karma; 
		SendIntimateDetails(0);
	}
	inline void SetEnergy(WORD energy) 
	{
		m_Energy = energy;
		SendIntimateDetails(0);
	}
	inline void SetStrength(WORD strength) 
	{
		m_Strength = strength;
		SendIntimateDetails(0);
	}
	inline void SetWisdom(WORD wisdom) 
	{
		m_Wisdom = wisdom;
		SendIntimateDetails(0);
	}
	inline void SetSex(WORD sex) 
	{
		m_Sex = sex;
		SendIntimateDetails(0);
	}
	inline void SetWealth(WORD wealth) 
	{
		m_Wealth = wealth;
		SendIntimateDetails(0);
	}

	// follow someone else:
	void SetFollow(DPID pid) {m_follow = pid;}
	DPID GetFollow() {return m_follow;}

	// specialized setters/getters
	void SetView(WORD view_id);
	void SetOffset(POINT offset);
	void SetPos(POINT p);
	void SetObject(WORD obj_def_id);
	void SetName(const char *name);

	// Send stuff across the 'net
	void SendView(DWORD to, WORD group);
	void SendDetails(DWORD to, WORD group);
	void SendIntimateDetails(DWORD to);
	void SendEmote(int emotion);

    BOOL Save(char *filename);
    WORD Load(char *filename); // returns view player was in when exiting.
};

class OTHER_PLAYER : public INTIMATE_PLAYER {
};

extern LOCAL_PLAYER *local_player;
extern OTHER_PLAYER *other_player;
