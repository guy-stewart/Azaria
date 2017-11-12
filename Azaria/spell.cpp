/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       Spell.h
;
; Description:
;	Liason between the game engine and the game puzzles.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 06/05/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#include "includes.h"

#include "display.h"
#include "gameman.h"

#include "tables.h"
#include "spr_pool.h"
#include "spirit.h"
#include "object.h"

#include "user.h"
#include "cursor.h"
#include "spell.h"

#include "btable.h"
#include "gdb.h"
#include "viewman.h"
#include "player.h"

#include "signal.h"
#include "machterp.h"
#include "machines.h"
#include "teletype.h"

#include "Data/Database/Idents.h"
#include "Data/Database/idv.h"

extern BOOL mon_machines;

#ifdef _DEBUG
static BOOL allow = TRUE;
#else
static BOOL allow = TRUE;
#endif

class GDBT_SPELLS : public GDB_TABLE {
private:
	virtual void ReceiveUpdate(DPID from, DPID to, WORD proc, UDATA *data);

public:
	GDBT_SPELLS():GDB_TABLE(IDT_SPELLS){};

    void CastMe(int spell, int target);
    void CastYou(int spell, int target);
};

GDBT_SPELLS gdbt_spells;

typedef struct {
  int spell;
  int target;
} SPELL_PKT;

#define PROC_CASTYOU 1

WORD WIZARD::current_object;
DWORD WIZARD::spell_time[NUMBER_OF_SPELLS];
static POINT last_pos;
static WORD WhereWasI;
static int spell_caster = 0;
static BOOL protect = FALSE;



//----------------------------------------------------------------------------
static DWORD spell_time_to_live[] =
{
	120000, //	ENCHANT=0,		//Forces Marriage - Forces someone to follow you
	120000, //	STALKER,		//player follows someone
	 1000, //	BLINDNESS,		//TURNS PLAYERS SCREEN DARK
	20000, //	HOLDING,		//LOCKS A PLAYER TO A LOCATION
	20000, //	TRANSFER,		//TRANSFERS ENERGY
	20000, //	ALARM,			//WORKS ON A VIEW AND SIGNALS PLAYER WHO CASTS
	20000, //	RECORDER,		//WORKS ON A VIEW AND RECORDS NAME OF PLAYER
	20000, //	TELEKINESIS,	//LEVITATION
	20000, //	EXPLOSION,		//BIG EXPLOSION
	20000, //	LOCK,			//LOCKS A DOOR
	20000, //	UNLOCK,			//UNLOCKS A DOOR
	20000, //	PROTECT,		//PROTECTION FROM SPELLS
	20000, //	DEATH,			//KILLS INSTANTLY
	20000, //	INVISIBLE,		//RENDERS A PLAYER INVISIBLE
	20000, //	STRENGTH,		//INCREASES ABILITY TO MOVE OBJECT
	240000, //	BANISHMENT,		//SENDS A PLAYER TO A BAD PLACE
	20000, //	MINDREAD,		//BREAKS IN ON SELECTED COMMUNICATIONS
	20000, //	HALUCINATE,		//WARPS A PLAYERS VIEW
	20000, //	NYBREATH,		//ALLOWS PLAYERS TO BREATHE FOUL WIND
	20000, //	WETBREATH,		//ALLOWS PLAYERS TO BREATHE WATER
};

//----------------------------------------------------------------------------
void WIZARD::Tick(DWORD etime)
{
	int i, x;

	for (i=0;i<NUMBER_OF_SPELLS;i++)
	{
	  // if spell is active and counting down ...
	  if (spell_time[i])
	  {
		if (spell_time[i] > etime)
			spell_time[i] -= etime;
		else
		{
			spell_time[i] = 0;

			switch (i)
			{
			case ENCHANT:		//Forces Marriage - Forces someone to follow you
			  local_player->SetFollow(0);
			  break;
			case STALKER:		//player follows someone
			  local_player->SetFollow(0);
			  break;
			case BLINDNESS:		//TURNS PLAYERS SCREEN DARK
			  x = display.SetBlind(-5);
			  if (x>0)
				spell_time[BLINDNESS] = spell_time_to_live[BLINDNESS];
			  break;
			case HOLDING:		//LOCKS A PLAYER TO A LOCATION
			  viewman.SetHolding(FALSE);
			  break;
			case TRANSFER:		//TRANSFERS ENERGY
			  break;
			case ALARM:			//WORKS ON A VIEW AND SIGNALS PLAYER WHO CASTS
			  break;
			case RECORDER:		//WORKS ON A VIEW AND RECORDS NAME OF PLAYER
			  break;
			case TELEKINESIS:	//LEVITATION
			  break;
			case EXPLOSION:		//BIG EXPLOSION
			  break;
			case LOCK:			//LOCKS A DOOR
			  break;
			case UNLOCK:		//UNLOCKS A DOOR
			  break;
			case PROTECT:		//PROTECTION FROM SPELLS
			  protect = FALSE;
			  break;
			case DEATH:			//KILLS INSTANTLY
			  break;
			case INVISIBLE:		//RENDERS A PLAYER INVISIBLE
			  break;
			case STRENGTH:		//INCREASES ABILITY TO MOVE OBJECT
			  break;
			case BANISHMENT:	//SENDS A PLAYER TO A BAD PLACE
			  local_player->SetWealth(0);
			  viewman.Load(WhereWasI);
			  break;
			case MINDREAD:		//BREAKS IN ON SELECTED COMMUNICATIONS
			  break;
			case HALUCINATE:	//WARPS A PLAYERS VIEW
			  break;
			case NYBREATH:		//ALLOWS PLAYERS TO BREATHE FOUL WIND
			  break;
			case WETBREATH:		//ALLOWS PLAYERS TO BREATHE WATER
			  break;
			}
		}
	  }
	}
}

//----------------------------------------------------------------------------
void Signal_Id(int signal)
{
	gdbt_machines.Signal(SID_ID, signal);
	local_player->SendEmote(signal);
}

//----------------------------------------------------------------------------
void WIZARD::Cast(int spell, int signal)
{
  switch (spell)
  {
  case SPELL_SPIN:
	if (!cursorman.HeldObject())
	{
		POINT p;
		p.x=0; p.y=0;
		cursorman.LoadObject(IDD_SPINSPELL, p);
	}
	break;
  case SPELL_OTHER_ID:
	if (signal)
	  gdbt_machines.Signal(SOD_ID, signal);
	else
	  gdbt_machines.Signal(SOD_AURA, signal);
	break;
  case SPELL_EMOTE:
	Signal_Id(signal);
	break;
  }
}

//----------------------------------------------------------------------------
void WIZARD::Excorsize()
{
	switch (current_object)
	{
	case IDD_SPELL03:
		TELETYPE::SetLang(0);
		TELETYPE::Refresh();
		break;
	}
}

//----------------------------------------------------------------------------
void WIZARD::CastMe(int object, int signal)
{
  if (protect)
	return;

  switch (signal)
  {
  case SIG_BOMB:
	Signal_Id(signal);
	break;
  case SIG_OBJECT:
	Excorsize(); // remove old one-shot spell;
	current_object = object;
	switch(object)
	{
	case IDD_SPELL03:
		TELETYPE::SetLang(1);
		Signal_Id(SIG_STRESS);
		TELETYPE::Refresh();
	  break;
	case IDD_ENCHANT:		//Forces Marriage - Forces someone to follow you
		if (spell_caster)
		{
			local_player->SetFollow(spell_caster);
			spell_time[ENCHANT] = spell_time_to_live[ENCHANT];
		}
	  break;
	case IDD_BLINDNESS:		//TURNS PLAYERS SCREEN DARK
	  Signal_Id(SIG_CLEAR);

	  display.SetBlind(+125);
	  spell_time[BLINDNESS] = spell_time_to_live[BLINDNESS];
	  break;
	case IDD_HOLDING:		//LOCKS A PLAYER TO A LOCATION			
	  viewman.SetHolding(TRUE);
	  spell_time[HOLDING] = spell_time_to_live[HOLDING];
	  break;
	case IDD_TRANSFER:		//TRANSFERS ENERGY
	  break;
	case IDD_ALARM:			//WORKS ON A VIEW AND SIGNALS PLAYER WHO CASTS
	  break;
	case IDD_RECORDER:		//WORKS ON A VIEW AND RECORDS NAME OF PLAYER
	  break;
	case IDD_TELEKINESIS:	//SMALL EXPLOSION
	  break;
	case IDD_EXPLOSION:		//BIG EXPLOSION
	  break;
	case IDD_LOCK:			//LOCKS A DOOR
	  break;
	case IDD_UNLOCK:		//UNLOCKS A DOOR
	  break;
	case IDD_PROTECT:		//PROTECTION FROM SPELLS
	  protect = TRUE;
	  spell_time[PROTECT] = spell_time_to_live[PROTECT];
	  break;
	case IDD_DEATH:			//KILLS INSTANTLY
	  break;
	case IDD_INVISIBLE:		//RENDERS A PLAYER INVISIBLE
	  break;
	case IDD_STRENGTH:		//INCREASES ABILITY TO MOVE OBJECT
	  break;
	case IDD_BANISHMENT:	//SENDS A PLAYER TO A BAD PLACE
	  WhereWasI = viewman.GetWhereId();
	  viewman.Load(IDV_BANISH);
	  spell_time[BANISHMENT] = spell_time_to_live[BANISHMENT];
	  break;
	case IDD_MINDREAD:		//BREAKS IN ON SELECTED COMMUNICATIONS
	  break;
	case IDD_HALUCINATE:	//WARPS A PLAYERS VIEW
	  break;
	case IDD_NYBREATH:		//ALLOWS PLAYERS TO BREATHE FOUL WIND
	  break;
	case IDD_WETBREATH:		//ALLOWS PLAYERS TO BREATHE WATER
	  break;
	}
	break;
  case SIG_CLEAR:
	Excorsize(); // remove old one-shot spell;
	break;
  default:
	break;
  }
}

//----------------------------------------------------------------------------
void WIZARD::CastYou(int object, int target)
{
	switch(object)
	{
	case IDD_STALKER:		//player follows someone
	  if (other_player)
	  {
		DPID pid = other_player->GetPid();
		PLAYER *player = gdbt_players.Get(pid);
		if (player)
		{
			viewman.Load(player->GetView());
			local_player->SetFollow(pid);
			spell_time[STALKER] = spell_time_to_live[STALKER];
		}
	  }
	  break;
	default:
	  {
		SPELL_PKT pkt;
		pkt.spell = object;
		pkt.target = target;

		gdbt_spells.SendUpdate(
				other_player->GetPid(),
				0,
				PROC_CASTYOU,
				(UDATA*)&pkt, 
				sizeof(pkt),
				GUARANTEED);
	  }
	}
}

//----------------------------------------------------------------------------
void GDBT_SPELLS::ReceiveUpdate(DPID from, DPID to, WORD proc, UDATA *data)
{
  SPELL_PKT *pkt = (SPELL_PKT*)data;
  	switch (proc)
	{
	case PROC_CASTYOU:
	  WORD wSpell = pkt->spell;
	  spell_caster = from;
	  gdbt_machines.Drop(SID_SPELL, &wSpell);
	  spell_caster = 0;
	  break;
	}
}

//----------------------------------------------------------------------------
BOOL WIZARD::Parse(char *buffer)
{
  static char seps[]   = " ,\t\n";
  char string[MAX_TELETYPE_MESSAGE_LEN];
  char *token;

  strncpy(string, buffer, MAX_TELETYPE_MESSAGE_LEN);

  /* Establish string and get the first token: */
  token = strtok( string, seps );

  if (!token || *token != ';')
	return FALSE;

  strncpy(string, buffer, MAX_TELETYPE_MESSAGE_LEN);
  *token = ' ';

  token = strtok( string, seps );

  // They hit ; (command prefix) and no command:
  if (!token)
	return TRUE;

  //-------------------------------
  // start parsing:
  if (!stricmp(token, "help")
	||!strcmp(token, "?"))
  {
	token = strtok( NULL, seps );
    if (token && !stricmp(token, "x"))
	{
		TELETYPE::Msg(0, ";POS - get cursor position on canvas");
		TELETYPE::Msg(0, ";DPOS - delta from last cursor position");

		token = strtok( NULL, seps );
		if 	(token && !stricmp(token, restore_debug))
		{
			if (GDBT_MESSAGES::c_m == PUBLIC_CODE)
			{
				GDBT_MESSAGES::c_m = SECRET_CODE;
				TELETYPE::Msg(0, "(secret)");
			}
			else
			{
				GDBT_MESSAGES::c_m = PUBLIC_CODE;
				TELETYPE::Msg(0, "(public)");
			}
		}
	}
	else
	{
		TELETYPE::Msg(0, ";FIND <player name>, -or- ;LOOK <player_name>");
		TELETYPE::Msg(0, ";WHERE - Current locataion.");
		TELETYPE::Msg(0, ";<location> - Go to location.");
	}
  }
  else if (!stricmp(token, "dump"))
  {
	TELETYPE::Dump();
  }
  else if (!stricmp(token, "new"))
  {
	  Game::New();
  }
  else if (!stricmp(token, "mon"))
  {
	token = strtok( NULL, seps );
	if (!token)
	{
		TELETYPE::Msg(0, ";MON [all|mach|none] - toggle debug logging");
	}
	else if (!stricmp(token, "all"))
	{
	  mon_machines = TRUE;
	  TELETYPE::Msg(0, "All monitors are ON");
	}
	else if (!stricmp(token, "mach"))
	{
	  mon_machines = !mon_machines;
	  TELETYPE::Msg(0, "Specified monitor turned %s", mon_machines?"ON":"OFF");
	}
	else if (!stricmp(token, "none"))
	{
	  mon_machines = FALSE;
	  TELETYPE::Msg(0, "All monitors are OFF");
	}
	else if (!stricmp(token, restore_debug))
	{
	  allow = TRUE;
	}
  }
  else if (!stricmp(token, "find"))
  {
	PLAYER *player = gdbt_players.GetByName(token+strlen(token)+1);
	if (player)
	{
	  viewman.Load(player->GetView());
	}
  }
  else if (!stricmp(token, "follow"))
  {
	PLAYER *player = gdbt_players.GetByName(token+strlen(token)+1);
	if (player)
	{
	  viewman.Load(player->GetView());
	  local_player->SetFollow(player->GetPid());
	}
	else
	{
	  local_player->SetFollow(0);
	}
  }
  else if (!stricmp(token, "look"))
  {
	// token = strtok( NULL, seps );
	PLAYER *player = gdbt_players.GetByName(token+strlen(token)+1);
	if (player)
	{
		gdbt_players.SendIntimate(player->GetPid(), TRUE);
	}
  }
  else if (!stricmp(token, "get") && (allow))
  {
	token = strtok( NULL, seps );
	if (token)
	{
	  char objName[32];
	  strcpy(objName, "idd_");
	  strcat(objName, token);
		WORD id = objdefGetIDByName(objName);
		POINT pos;
		pos.x = 320;
		pos.y = 240;
		cursorman.LoadObject(id, pos);
	}
  }
  else if (!stricmp(token, "where"))
  {
	TELETYPE::Msg(0, "%s.", viewman.GetWhere());
  }
  else if (!stricmp(token, "wealth"))
  {
	local_player->SetWealth(20);
  }
  else if (!stricmp(token, "pos"))
  {
	POINT p = local_player->GetPos();
	VIEW *pview = viewman.Hit(user.mousePos);
	if (pview)
	{
	  p = pview->MapToCanvas(p);
	  last_pos = p;
	}
	TELETYPE::Msg(0, "x:%d, y:%d.", p.x, p.y);
  }
  else if (!stricmp(token, "dpos"))
  {
	POINT p = local_player->GetPos();
	VIEW *pview = viewman.Hit(user.mousePos);
	if (pview)
	{
	  p = pview->MapToCanvas(p);
	}
	TELETYPE::Msg(0, "dx:%d, dy:%d.", last_pos.x-p.x, last_pos.y-p.y);
  }
  else if (!stricmp(token, "font"))
  {
	token = strtok( NULL, seps );
	if (!token)
	{
		TELETYPE::Msg(0, ";font [tnr | parent | village | city | big | small]");
	}
	else if (!stricmp(token, "tnr"))
	{
	  TELETYPE::c_lang = 0;
	}
	else if (!stricmp(token, "parent"))
	{
	  TELETYPE::c_lang = 1;
	}
	else if (!stricmp(token, "village"))
	{
	  TELETYPE::c_lang = 2;
	}
	else if (!stricmp(token, "city"))
	{
	  TELETYPE::c_lang = 3;
	}
	else if (!stricmp(token, "big"))
	{
	  TELETYPE::c_pitch = 1;
	}
	else if (!stricmp(token, "small"))
	{
	  TELETYPE::c_pitch = 0;
	}
  }
  else if (!stricmp(token, "Happy"))
  {
	Cast(SPELL_EMOTE, SIG_HAPPY);
  }
  else if (!stricmp(token, "Hurt"))
  {
	Cast(SPELL_EMOTE, SIG_HURT);
  }
  else if (!stricmp(token, "Kiss"))
  {
	Cast(SPELL_EMOTE, SIG_KISS);
  }
  else if (!stricmp(token, "Mad"))
  {
	Cast(SPELL_EMOTE, SIG_MAD);
  }
  else if (!stricmp(token, "Sad"))
  {
	Cast(SPELL_EMOTE, SIG_SAD);
  }
  else if (!stricmp(token, "Surprised"))
  {
	Cast(SPELL_EMOTE, SIG_SURPRISED);
  }
  else if (!stricmp(token, "Stress"))
  {
	Cast(SPELL_EMOTE, SIG_STRESS);
  }
  else if (!stricmp(token, "Wave"))
  {
	Cast(SPELL_EMOTE, SIG_WAVE);
  }
  else
  {
	viewman.Load(token, FALSE);
  }

  return TRUE;
}
