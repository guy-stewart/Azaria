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
;	Implements the surround style puzzle.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 06/05/96  gas  Created.
; 
----------------------------------------------------------------------------*/

enum {
  SPELL_SPIN=1,
  SPELL_OTHER_ID,
  SPELL_EMOTE,
};

#define TARGET_ME	0x01

enum {
	ENCHANT=0,		//Forces Marriage - Forces someone to follow you
	STALKER,		//player follows someone
	BLINDNESS,		//TURNS PLAYERS SCREEN DARK
	HOLDING,		//LOCKS A PLAYER TO A LOCATION			
	TRANSFER,		//TRANSFERS ENERGY
	ALARM,			//WORKS ON A VIEW AND SIGNALS PLAYER WHO CASTS
	RECORDER,		//WORKS ON A VIEW AND RECORDS NAME OF PLAYER
	TELEKINESIS,	//SMALL EXPLOSION
	EXPLOSION,		//BIG EXPLOSION
	LOCK,			//LOCKS A DOOR
	UNLOCK,			//UNLOCKS A DOOR
	PROTECT,		//PROTECTION FROM SPELLS
	DEATH,			//KILLS INSTANTLY
	INVISIBLE,		//RENDERS A PLAYER INVISIBLE
	STRENGTH,		//INCREASES ABILITY TO MOVE OBJECT
	BANISHMENT,		//SENDS A PLAYER TO A BAD PLACE
	MINDREAD,		//BREAKS IN ON SELECTED COMMUNICATIONS
	HALUCINATE,		//WARPS A PLAYERS VIEW
	NYBREATH,		//ALLOWS PLAYERS TO BREATHE FOUL WIND
	WETBREATH,		//ALLOWS PLAYERS TO BREATHE WATER
	NUMBER_OF_SPELLS,
};

class WIZARD {
	static WORD current_object;
	static DWORD spell_time[NUMBER_OF_SPELLS];

public:
    static void Cast(int spell, int target);    // The game cast a spell
    static void CastMe(int spell, int target);  // a spell was cast on my id
    static void CastYou(int spell, int target); // I cast a spell on another id
    static BOOL Parse(char *buffer);
    static void Excorsize();
    static void Tick(DWORD etime);
};

