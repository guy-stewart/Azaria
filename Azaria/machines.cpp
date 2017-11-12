/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       machines.cpp
;
; Description:
;	Local machine behavior.
;
; HISTORY
;  
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 04/03/96  gas  Created.
----------------------------------------------------------------------------*/

#include "maya.h"
#include "signal.h"

#include "gdb.h"
#include "btable.h"
#include "object.h"
#include "user.h"
#include "spell.h"

#include "machterp.h"
#include "machines.h"

#include "data\database\tbl_wave.h"
#include "data\database\idents.h"
#include "data\database\idv.h"
#include "data\database\ids.h"

//----------------------------------------------------------------------------
#define SYNC_FOULWIND		0x0000
#define SYNC_FISH1			0x0001
#define SYNC_BIRDSCHIRP		0x0002
#define NUMBER_OF_SYNCS		0x0003
SYNC tbl_syncpoints[NUMBER_OF_SYNCS];

//----------------------------------------------------------------------------
#define ISA_BAIT		0x7001
#define ISA_BAITEDPOLE	0x7002
#define ISA_LOCKDROP	0x7003 // keys, bombs, etc. Anything that can be dropped on a mine cultural lock

// ISA tables are sorted by both keys [ISA_XXX+IDD_XXX]
static ISA isa_table[] = {
	ISA_BAIT,		IDD_BAIT0,
	ISA_BAIT,		IDD_BAIT1,
	ISA_BAIT,		IDD_BAIT2,
	ISA_BAIT,		IDD_BAIT3,
	ISA_BAIT,		IDD_BAIT4,
	ISA_BAIT,		IDD_BAIT5,
	ISA_BAIT,		IDD_BAIT6,
	ISA_BAIT,		IDD_BAIT7,
	ISA_BAIT,		IDD_BAIT8,
	ISA_BAIT,		IDD_BAIT9,
	ISA_BAITEDPOLE,	IDD_POLE1B0,
	ISA_BAITEDPOLE,	IDD_POLE1B1,
	ISA_BAITEDPOLE,	IDD_POLE1B2,
	ISA_BAITEDPOLE,	IDD_POLE1B3,
	ISA_BAITEDPOLE,	IDD_POLE1B4,
	ISA_BAITEDPOLE,	IDD_POLE1B5,
	ISA_BAITEDPOLE,	IDD_POLE1B6,
	ISA_BAITEDPOLE,	IDD_POLE1B7,
	ISA_BAITEDPOLE,	IDD_POLE1B8,
	ISA_BAITEDPOLE,	IDD_POLE1B9,
	ISA_BAITEDPOLE,	IDD_POLE2B0,
	ISA_BAITEDPOLE,	IDD_POLE2B1,
	ISA_BAITEDPOLE,	IDD_POLE2B2,
	ISA_BAITEDPOLE,	IDD_POLE2B3,
	ISA_BAITEDPOLE,	IDD_POLE2B4,
	ISA_BAITEDPOLE,	IDD_POLE2B5,
	ISA_BAITEDPOLE,	IDD_POLE2B6,
	ISA_BAITEDPOLE,	IDD_POLE2B7,
	ISA_BAITEDPOLE,	IDD_POLE2B8,
	ISA_BAITEDPOLE,	IDD_POLE2B9,
	ISA_LOCKDROP,	IDD_BOMB1,
	ISA_LOCKDROP,	IDD_BOMB2,
	ISA_LOCKDROP,	IDD_CITLOCK,
	ISA_LOCKDROP,	IDD_VILLOCK,
};

#define count_isa (sizeof(isa_table)/sizeof(isa_table[0]))
TBL_ISA tbl_isa(isa_table, count_isa);

//----------------------------------------------------------------------------
// Object Mapping Operators:
// This table can be used to either combine objects,
// seperate objects 
// or map objects.
#define OP_COOK		0x8001
#define OP_BURN		0x8002
#define OP_POUCH	0x8003
#define OP_KING_ME	0x8101

static OBJECT_OP object_ops[] = {
	{IDD_POLE1,		IDD_BAIT0, IDD_POLE1B0},
	{IDD_POLE1,		IDD_BAIT1, IDD_POLE1B1},
	{IDD_POLE1,		IDD_BAIT2, IDD_POLE1B2},
	{IDD_POLE1,		IDD_BAIT3, IDD_POLE1B3},
	{IDD_POLE1,		IDD_BAIT4, IDD_POLE1B4},
	{IDD_POLE1,		IDD_BAIT5, IDD_POLE1B5},
	{IDD_POLE1,		IDD_BAIT6, IDD_POLE1B6},
	{IDD_POLE1,		IDD_BAIT7, IDD_POLE1B7},
	{IDD_POLE1,		IDD_BAIT8, IDD_POLE1B8},
	{IDD_POLE1,		IDD_BAIT9, IDD_POLE1B9},

	{IDD_POLE2,		IDD_BAIT0, IDD_POLE2B0},
	{IDD_POLE2,		IDD_BAIT1, IDD_POLE2B1},
	{IDD_POLE2,		IDD_BAIT2, IDD_POLE2B2},
	{IDD_POLE2,		IDD_BAIT3, IDD_POLE2B3},
	{IDD_POLE2,		IDD_BAIT4, IDD_POLE2B4},
	{IDD_POLE2,		IDD_BAIT5, IDD_POLE2B5},
	{IDD_POLE2,		IDD_BAIT6, IDD_POLE2B6},
	{IDD_POLE2,		IDD_BAIT7, IDD_POLE2B7},
	{IDD_POLE2,		IDD_BAIT8, IDD_POLE2B8},
	{IDD_POLE2,		IDD_BAIT9, IDD_POLE2B9},

	{OP_BURN,		IDD_FISH1,  IDD_FISHASH1},
	{OP_BURN,		IDD_FISH2,  IDD_FISHASH2},
	{OP_BURN,		IDD_FISH3,  IDD_FISHASH3},
	{OP_BURN,		IDD_FISH4,  IDD_FISHASH4},
	{OP_BURN,		IDD_FISH5,  IDD_FISHASH5},
	{OP_BURN,		IDD_FISH6,  IDD_FISHASH6},
	{OP_BURN,		IDD_FISH7,  IDD_FISHASH7},
	{OP_BURN,		IDD_FISH8,  IDD_FISHASH8},
	{OP_BURN,		IDD_FISH9,  IDD_FISHASH9},
	{OP_BURN,		IDD_FISH10, IDD_FISHASH10},
	{OP_KING_ME,	IDD_WKING,  IDD_WKINGME},
	{OP_KING_ME,	IDD_BKING,  IDD_BKINGME},
};

//static WORD chemcost[] = {0,3,3,5,5,10,10,10,15,15,15,20,20,25,25,25,25,50,60,99,};
static WORD chemcost[] = {1,2,3,3,4,4,4,5,5,5,5,7,7,7,8,8,8,14,21,28,};
#define CHEM_COUNT 20

static WORD template_objects[] = {
IDD_SPELL01,
IDD_SPELL02,
IDD_SPELL03,
IDD_SPELL04,
IDD_SPELL05,
IDD_SPELL06,
IDD_SPELL07,
IDD_SPELL08,
IDD_SPELL09,
IDD_SPELL10,
IDD_NEWSHOVL,
IDD_BOMB1,
IDD_BOMB2,
IDD_SPELL01,
IDD_SPELL02,
IDD_SPELL03,
IDD_H2OMASK,
IDD_NYSMASK,
IDD_SPELL04,
IDD_SPELL05,
IDD_SPELL06,
IDD_SPELL07,
IDD_MATCH,
IDD_SPELL08,
IDD_SPELL09,
IDD_SPELL10,
IDD_POLE2,
IDD_SPELL01,
IDD_SPELL01,
IDD_BUCKE,
IDD_SPELL01,
IDD_SPELL01,
IDD_SPELL01,
IDD_SPELL01,
IDD_SPELL01,
};

#include "data\database\tbl_mach.h"

// used by machines to lookup the table in the tbl_map_table
enum {
	CHEMCOST=0,
	TEMPLATE_OBJECTS,
	TEMPLATE_COST,
	TEMPLATE_CHEM1,
	TEMPLATE_CHEM2,
	SCROLL_SHIELD,
	SCROLL_SPELL,
	SCROLL_PLANT1,
	SCROLL_PLANT2,
	SCROLL_PLANT3,
	SCROLL_FISH1,
	SCROLL_FISH2,
	SCROLL_FISH3,
	SCROLL_NYSTROM,
	PLANT_FILMSTRIP_MAPPING,
	FISHASH_FILMSTRIP_MAPPING,
};

#define TMAP_ENTRY(X) X, sizeof(X)/sizeof(WORD)

TBL_MAP_ENTRY tbl_map_tables[] = {
  TMAP_ENTRY(chemcost),
  TMAP_ENTRY(template_objects),
  TMAP_ENTRY(template_cost),
  TMAP_ENTRY(template_chem1),
  TMAP_ENTRY(template_chem2),
  TMAP_ENTRY(scroll_shield),
  TMAP_ENTRY(scroll_spell),
  TMAP_ENTRY(scroll_plant1),
  TMAP_ENTRY(scroll_plant2),
  TMAP_ENTRY(scroll_plant3),
  TMAP_ENTRY(scroll_fish1), 
  TMAP_ENTRY(scroll_fish2), 
  TMAP_ENTRY(scroll_fish3), 
  TMAP_ENTRY(scroll_nystrom),
  TMAP_ENTRY(plant_filmstrip_mapping),
  TMAP_ENTRY(fishash_filmstrip_mapping),
};

int tbl_map_tables_count = sizeof(tbl_map_tables)/sizeof(tbl_map_tables[0]);

#define count_object_ops (sizeof(object_ops)/sizeof(object_ops[0]))
TBL_OBJECT_OPS tbl_object_ops(object_ops, count_object_ops);

//----------------------------------------------------------------------------
	// Compute sprite: ### validate this
#define COMPUTE_SPRITE(M_XX, S, END, VAL, MAX_VAL, SPRITE_COUNT, BASE_SPRITE)	\
	{M_XX,	S,   S+1,	ASSIGN,		WSPRITE,	SPRITE_COUNT},		\
	{M_XX,	S+1, S+2,	MUL,		WSPRITE,	VAL},				\
	{M_XX,	S+2, S+3,	ASSIGN,		WPARM,		MAX_VAL},			\
	{M_XX,	S+3, S+4,	DIV,		WSPRITE,	WPARM},				\
	{M_XX,	S+4, S+5,	ASSIGN,		WPARM,		BASE_SPRITE},		\
	{M_XX,	S+5, S+6,	ADD,		WSPRITE,	WPARM},				\
	{M_XX,	S+6, END,	SHOW,		WSPRITE,	0}

#define ANIMATE_SPRITES(M_XX, S, END, BASE_SPRITE, COUNT, FPS)	\
	{M_XX,	S,   S+1,	ASSIGN,		WSPRITE,	BASE_SPRITE},	\
	{M_XX,	S+1, S+2,	ASSIGN,		WPARM,		COUNT},			\
	{M_XX,	S+2, END,	LTEi,		WPARM,		0},				\
	{M_XX,	S+2, S+3,	SHOW,		WSPRITE, 0},				\
	{M_XX,	S+3, S+4,	EMTIME,		0,		1000/FPS},			\
	{M_XX,	S+4, S+5,	ADDI,		WSPRITE,1},					\
	{M_XX,	S+5, S+2,	SUBI,		WPARM,1}

// play sprite sequence backwards
#define rANIMATE_SPRITES(M_XX, S, END, BASE_SPRITE, COUNT, FPS)	\
	{M_XX,	S,   S+1,	ASSIGN,		WSPRITE,	BASE_SPRITE},	\
	{M_XX,	S+1, S+2,	ASSIGN,		WPARM,		COUNT},			\
	{M_XX,	S+2, END,	LTEi,		WPARM,		0},				\
	{M_XX,	S+2, S+3,	SHOW,		WSPRITE, 0},				\
	{M_XX,	S+3, S+4,	EMTIME,		0,		1000/FPS},			\
	{M_XX,	S+4, S+5,	SUBI,		WSPRITE,1},					\
	{M_XX,	S+5, S+2,	SUBI,		WPARM,1}

//----------------------------------------------------------------------------
TRANSITION tbl_machine_def[] = {
	{M_BARD,	0, 1, ASSIGN,		DETIME, 1000},
	{M_BARD,	1, 3, SYNCPOINT,	DETIME, SYNC_FOULWIND},
	//{M_BARD,	2, 3, PLAYWAVE,		0, SOUND_WZPISD3},
	{M_BARD,	3, 4, ESTIME,		0, 500},
	{M_BARD,	4, 3, SIGNALi,		0, S17_aFOULWIND},

	// the vial on the main panel:
#define MAX_VIAL_STORAGE 20
	{M_VIAL,	0, 1, SHOW,			0,	IDS_NYSVIAL},
	{M_VIAL,	1, 2, DRAG,			0,	IDD_SCOOPE},
	{M_VIAL,	1, 5, DRAG,			0,	IDD_SCOOPF},
	{M_VIAL,	1, 1, GRAB,			0,	IDD_SCOOPE},
	{M_VIAL,	1, 1, DROP,			0,	IDD_SCOOPE},
	{M_VIAL,	1, 8, DROP,			0,	IDD_SCOOPF},
	{M_VIAL,	2, 1, EQUALi,		LWEALTH, 0},
	{M_VIAL,	2, 3, SUBI,			LWEALTH, 1},
	{M_VIAL,	3, 4, PLAYWAVE,		0, SOUND_SLURP},
	{M_VIAL,	4, 1, HANDOFF,		0, IDD_SCOOPF},
	{M_VIAL,	5, 1, GTEi,			LWEALTH, MAX_VIAL_STORAGE},
	{M_VIAL,	5, 6, ADDI,			LWEALTH, 1},
	{M_VIAL,	6, 7, PLAYWAVE,		0, SOUND_SPIT},
	{M_VIAL,	7, 1, HANDOFF,		0, IDD_SCOOPE},
	{M_VIAL,	8, 1, GTEi,			LWEALTH, MAX_VIAL_STORAGE},
	{M_VIAL,	8, 1, ADDI,			LWEALTH, 1},

	{M_BACKBUTTON,	0, 1, CLICK,		0,	0},
	{M_BACKBUTTON,	1, 2, PLAYWAVE,		0,	SOUND_POPUP},
	{M_BACKBUTTON,	2, 0, LOADVIEW,		0,	IDV_TOPMENU},

	{M_MAPBUTTON,	0, 1, CLICK,		0,	0},
	{M_MAPBUTTON,	1, 2, PLAYWAVE,		0,	SOUND_POPUP},
	{M_MAPBUTTON,	2, 0, LOADVIEW,		0,	IDV_CONTINENT},

	{M_COMPASS,		0, 1, CLICK,		0,	0},
	{M_COMPASS,		1, 0, PLAYWAVE,		0,	SOUND_CLICK},

	{M_INV_LEFT,	0, 1, CLICK,		0,	0},
	{M_INV_LEFT,	1, 2, SENDKEYI,		KEY_LEFT, IDV_INVENTORY},
	{M_INV_LEFT,	2, 0, PLAYWAVE,		0,	SOUND_CLICK},

	{M_INV_RIGHT,	0, 1, CLICK,		0,	0},
	{M_INV_RIGHT,	1, 2, SENDKEYI,		KEY_RIGHT, IDV_INVENTORY},
	{M_INV_RIGHT,	2, 0, PLAYWAVE,		0,	SOUND_CLICK},

	{M_TEXT_UP,		0, 1, CLICK},
	{M_TEXT_UP,		1, 2, PLAYWAVE,		0,	SOUND_CLICK},
	{M_TEXT_UP,		2, 3, SENDMSGI,		8, IDV_S_TELETYPE},
	{M_TEXT_UP,		3, 4, EMTIME,		0, 250},
	{M_TEXT_UP,		4, 0, SENDMSGI,		9, IDV_S_TELETYPE},

	{M_TEXT_DOWN,	0, 1, CLICK},
	{M_TEXT_DOWN,	1, 2, PLAYWAVE,		0,	SOUND_CLICK},
	{M_TEXT_DOWN,	2, 3, SENDMSGI,		10, IDV_S_TELETYPE},
	{M_TEXT_DOWN,	3, 4, EMTIME,		0, 250},
	{M_TEXT_DOWN,	4, 0, SENDMSGI,		11, IDV_S_TELETYPE},

	// frequencies that local player is listening to
	{M_LISTEN,		0, 1, CLICK},
	{M_LISTEN,		1, 2, SENDMSG,		WIP1, IDV_S_TELETYPE},
	{M_LISTEN,		2, 3, PLAYWAVE,		0,	SOUND_CLICK},
	{M_LISTEN,		3, 4, SHOW,			0,	IDS_REDEAR},
	{M_LISTEN,		4, 5, CLICK},
	{M_LISTEN,		5, 6, SENDMSG,		WIP2, IDV_S_TELETYPE},
	{M_LISTEN,		6, 7, PLAYWAVE,		0,	SOUND_CLICK},
	{M_LISTEN,		7, 0, SHOW},

	// frequency that local player transmits on
	{M_TALK,		0, 20,WAIT},
	{M_TALK,		0, 1, CLICK},
	{M_TALK,		1, 2, MOV,			WPARM,	WTHIS},
	{M_TALK,		2, 3, SUBI,			WPARM,	SMP_TALK1}, // WPARM = 0..3 for smp_talk1 .. smp_talk4
	{M_TALK,		3, 4, SENDKEY,		WPARM, IDV_S_TELETYPE},
	{M_TALK,		4, 5, ASSIGN,		WPARM,	0}, // so that WPARM does not get transmitted during table xfer.
	{M_TALK,		5, 6, PLAYWAVE,		0,	SOUND_CLICK},
	{M_TALK,		6, 7, SHOW,			0,	IDS_REDDOT},
	{M_TALK,		7, 8, SIGNAL,		WIP1},
	{M_TALK,		8, 9, SIGNAL,		WIP2},
	{M_TALK,		9, 0, SIGNAL,		WIP3},
	{M_TALK,		20, 0, SHOW},


#define M_DEC_ID_REG(M_XX, REGISTER)				\
	{M_XX,	0, 1, WAIT},					\
	{M_XX,	1, 0, LTEi,	REGISTER, 0},				\
	{M_XX,	1, 2, SUBI,	REGISTER, 1},				\
	{M_XX,	2, 3, PLAYWAVE,	0, SOUND_HURT},			\
	{M_XX,	3, 4, GTEi,	REGISTER, 1},				\
	{M_XX,	3, 4, PLAYWAVE,	0, SOUND_THUNDER},		\
	{M_XX,	4, 0, SIGNALi, SIG_SUB, SID_AURA}

#define M_INC_ID_REG(M_XX, REGISTER, MAX)			\
	{M_XX,	0, 1, WAIT},					\
	{M_XX,	1, 0, GTEi,	REGISTER, MAX},				\
	{M_XX,	1, 2, ADDI,	REGISTER, 1}

#define MAX_SEX			1
#define MAX_WISDOM		200
#define MAX_STRENGTH	5
#define MAX_ENERGY		23
#define MAX_KARMA		10
#define MAX_WEALTH		100

	M_DEC_ID_REG(M_DEC_WISDOM, LWISDOM),
	M_INC_ID_REG(M_INC_WISDOM, LWISDOM, MAX_WISDOM),

	M_DEC_ID_REG(M_DEC_STRENGTH, LSTRENGTH),
	M_INC_ID_REG(M_INC_STRENGTH, LSTRENGTH, MAX_STRENGTH),

	M_DEC_ID_REG(M_DEC_ENERGY, LENERGY),
	M_INC_ID_REG(M_INC_ENERGY, LENERGY, MAX_ENERGY),

	M_DEC_ID_REG(M_DEC_KARMA, LKARMA),
	M_INC_ID_REG(M_INC_KARMA, LKARMA, MAX_KARMA),

	M_DEC_ID_REG(M_DEC_WEALTH, LWEALTH),
	M_INC_ID_REG(M_INC_WEALTH, LWEALTH, MAX_WEALTH),

	//------------------------------------------------------
	// ID machine definitions go here:
	//
	{M_IDSPELL,	0, 100, DROP},
	{M_IDSPELL,	1, 2, SPELL_ME, WOBJECT, SIG_OBJECT},
	{M_IDSPELL,	2, 3, PLAYWAVE,	0, SOUND_POPUP},
	{M_IDSPELL,	3, 4, ASHOW, WOBJECT},
	{M_IDSPELL,	4, 10, GRAB},
	{M_IDSPELL,	4, 100, DROP},
	{M_IDSPELL,	4, 20, WAIT, 0, SIG_BURST},
	{M_IDSPELL,	4, 2, WAIT},

	{M_IDSPELL,	10, 12, EQUALi, WVIEWID, 0},
	{M_IDSPELL,	10, 11, LOADVIEW, WVIEWID},
	{M_IDSPELL,	11, 12, CLEAR, WVIEWID},
	{M_IDSPELL,	12, 13, SHOW},
	{M_IDSPELL,	13, 0, SPELL_ME, WOBJECT, SIG_CLEAR},

	{M_IDSPELL,	20, 21, CLEAR, WOBJECT},
	{M_IDSPELL,	21, 22, SHOW},
	{M_IDSPELL,	22,  0, VIDEO, 0, IDS_SMBURST},

	{M_IDSPELL,	100, 101, CLEAR, WVIEWID},
	{M_IDSPELL,	101, 102, IS_A, WOBJECT, IDC_BOMB},
	{M_IDSPELL,	101, 120, IS_A, WOBJECT, IDC_SCROLL},
	{M_IDSPELL,	101, 1},
	{M_IDSPELL,	102, 0, SPELL_ME, 0, SIG_BOMB},

	{M_IDSPELL,	120, 121, MOV, WTEMP1, WOBJECT}, // shared mem to be used by scroll view.
	{M_IDSPELL,	121, 122, MOV, WVIEWID, LVIEW},	  // remember where we are
	{M_IDSPELL,	122, 1, LOADVIEW, 0, IDV_PARCHPAN},

	{M_ID, 0, 20, WAIT,		0,	SIG_HAPPY},
	{M_ID, 0, 21, WAIT,		0,	SIG_HURT},
	{M_ID, 0, 22, WAIT,		0,	SIG_KISS},
	{M_ID, 0, 23, WAIT,		0,	SIG_MAD},
	{M_ID, 0, 24, WAIT,		0,	SIG_SAD},
	{M_ID, 0, 25, WAIT,		0,	SIG_SURPRISED},
	{M_ID, 0, 26, WAIT,		0,	SIG_STRESS},
	{M_ID, 0, 27, WAIT,		0,	SIG_WAVE},
	{M_ID, 0, 50, WAIT,		0,	SIG_BOMB},
	{M_ID, 0,100, WAIT,		0,	SIG_CLEAR},
	{M_ID, 0,  2, WAIT},

	// Compute base sprite ID, place in wParm
	{M_ID, 2, 3, MOV, WPARM, PTR(WIP1)},   // level is based soley on wisdom.
	{M_ID, 3, 4, DIVI, WPARM, MAX_WISDOM/MAX_LEVEL}, // compute player level
	{M_ID, 4, 5, MULI, WPARM, EMOTIONS_PER_LEVEL},
	{M_ID, 5, 6, EQUALi, PTR(WIP2), 1},
	{M_ID, 5, 7, ADDI, WPARM, IDp_MALE},
	{M_ID, 6, 7, ADDI, WPARM, IDp_FEMALE},

	{M_ID, 7, 8, ASSIGN, BPARM, IDp_xxHAPPY}, // just show happy
	{M_ID, 8, 10, ASSIGN, BFRAME, 0},
	{M_ID, 10, 11, MOV, WSPRITE, WPARM},
	{M_ID, 11, 12, ADD, WSPRITE, BPARM},
	{M_ID, 12,0, SHOW, WSPRITE},

	{M_ID, 20,70, ASSIGN, BPARM, IDp_xxHAPPY},
	{M_ID, 21,70, ASSIGN, BPARM, IDp_xxHURT},
	{M_ID, 22,80, ASSIGN, BPARM, IDp_xxKISS},
	{M_ID, 23,70, ASSIGN, BPARM, IDp_xxMAD},
	{M_ID, 24,70, ASSIGN, BPARM, IDp_xxSAD},
	{M_ID, 25,70, ASSIGN, BPARM, IDp_xxSURPRISED},
	{M_ID, 26,80, ASSIGN, BPARM, IDp_xxSTRESS},
	{M_ID, 27,70, ASSIGN, BPARM, IDp_xxWAVE},

	{M_ID, 50,51, VIDEO, 0, IDS_EXPLODE1},
	{M_ID, 51,21, PLAYWAVE, 0, SOUND_EXPLODE}, // explode, and go to hurt.

	{M_ID, 70, 71, MOV, WSPRITE, WPARM},
	{M_ID, 71, 72, ADD, WSPRITE, BPARM},
	{M_ID, 72, 73, SHOW, WSPRITE},
	{M_ID, 73, 0, ANIMATE, 0, V_REWIND}, // animate & rewind without looping

	{M_ID, 80, 81, MOV, WSPRITE, WPARM},
	{M_ID, 81, 82, ADD, WSPRITE, BPARM},
	{M_ID, 82, 83, SHOW, WSPRITE},
	{M_ID, 83, 0, ANIMATE}, // animate without rewind or looping

	{M_ID, 100,101, SHOW},
	{M_ID, 101,  0, SIGNAL, WIP4, SIG_CLEAR},


	//------------------------------------------------------
	// local player pedestal:
#define MAX_PEDS	10

	{M_PED,		0, 2, EPSILON},
	{M_PED,		1, 2, WAIT},
	{M_PED,		2, 3, ASSIGN,		WSPRITE,	MAX_PEDS},
	{M_PED,		3, 4, MUL,			WSPRITE,	LKARMA},
	{M_PED,		4, 5, ASSIGN,		WPARM,		MAX_KARMA},
	{M_PED,		5, 6, DIV,			WSPRITE,	WPARM},
	{M_PED,		6, 7, ASSIGN,		WPARM,		IDS_PED1},
	{M_PED,		7, 8, ADD,			WSPRITE,	WPARM},
	{M_PED,		8, 1, SHOW,			WSPRITE},

	//------------------------------------------------------
	// Local player aura:
#define MAX_AURAS		24

	{M_AURA,	0, 2, EPSILON},
	{M_AURA,	1, 2, WAIT, 0, SIG_ADD},
	{M_AURA,	1, 3, WAIT, 0, SIG_SUB},
	{M_AURA,	1, 2, WAIT},

	{M_AURA,	2, 4, ASSIGN,		WPARM},
	{M_AURA,	3, 4, ASSIGN,		WPARM,	V_REVERSE},

	// determine what image file to use (IDS_AURA1 .. IDS_AURA3)
	{M_AURA,	4, 5, MOV,			WSPRITE,	LENERGY},
	{M_AURA,	5, 6, MODI,			WSPRITE,	MAX_AURAS},
	{M_AURA,	6, 7, ADDI,			WSPRITE,	IDS_AURA1},

	// play the animation
	{M_AURA,	7, 8, SHOW,			WSPRITE},
	{M_AURA,	8, 9, ANIMATE,		WPARM}, // loop, reverse
	
	{M_AURA,	9, 1, SIGNALi,		0,	SID_ID},

	{M_HALO,	0, 1, ASHOW,		0, IDS_HALO01},


	//------------------------------------------------------
	/*
	{M_O_IDWEAR,	0, 1, DROP},
	{M_O_IDWEAR,	1, 0, SHOW, WOBJECT},
	*/

	//------------------------------------------------------
	{M_O_IDSPELL,	0, 1, DROP},
	{M_O_IDSPELL,	1, 0, SPELL_YOU, WOBJECT},
	//------------------------------------------------------
	// other player pedestal:
	{M_O_PED,		0, 1, GRAB},
	{M_O_PED,		0, 1, WAIT},
	COMPUTE_SPRITE(M_O_PED, 1, 0, OKARMA, MAX_KARMA, MAX_PEDS, IDS_PED1),

	//------------------------------------------------------
	// other player aura:
	{M_O_AURA,	0, 2, WAIT, 0, SIG_ADD},
	{M_O_AURA,	0, 3, WAIT, 0, SIG_SUB},
	{M_O_AURA,	0, 30, WAIT, 0, SIG_CLEAR},
	{M_O_AURA,	0, 2, WAIT},

	{M_O_AURA,	2, 4, ASSIGN,		WPARM,	0},
	{M_O_AURA,	3, 4, ASSIGN,		WPARM,	V_REVERSE},

	// determine what image file to use (IDS_AURA1 .. IDS_AURA3)
	{M_O_AURA,	4, 5, MOV,			WSPRITE,	OENERGY},
	{M_O_AURA,	5, 6, MODI,			WSPRITE,	MAX_AURAS},
	{M_O_AURA,	6, 7, ADDI,			WSPRITE,	IDS_AURA1},

	// play the animation
	{M_O_AURA,	7, 8, SHOW,			WSPRITE},
	{M_O_AURA,	8, 9, ANIMATE,		WPARM}, // loop, reverse
	
	{M_O_AURA,	9, 0, SIGNALi,		0,	SOD_ID},

	{M_O_AURA,	30, 0, SHOW},

	// Generic object bin.
	{M_OBJECTBIN,	0, 0, GRAB,	WIP1},

	{M_PLANTBIN,	0, 1, GRAB,	WIP1},
	{M_PLANTBIN,	1, 2, SHOW},
	{M_PLANTBIN,	2, 3, ESTIME, WIP3},
	{M_PLANTBIN,	3, 4, MOV, WSPRITE, WIP2},
	{M_PLANTBIN,	4, 0, SHOW, WSPRITE},

	{M_OBJECTMAP,	0, 1, DRAG,	WIP1},
	{M_OBJECTMAP,	1, 2, HANDOFF,	WIP2},
	{M_OBJECTMAP,	2, 0, PLAYWAVE, WIP3},

	{M_BTN_1phase,	0, 1, CLICK},
	{M_BTN_1phase,	0, 10, DRAGFOCUS, 0, TRUE}, // we are getting the drag focus
	{M_BTN_1phase,	0, 20, DRAGFOCUS, 0, FALSE}, // losing the drag focus
	{M_BTN_1phase, 1, 2, PLAYWAVE, 0, SOUND_BTNPRESS},
	{M_BTN_1phase, 2, 20, LOADVIEW, WIP1},
	{M_BTN_1phase, 10, 11, MOV, WSPRITE, WIP2},
	{M_BTN_1phase, 11, 12, SHOW,WSPRITE},
	{M_BTN_1phase, 12, 0, PLAYWAVE, 0, SOUND_BTNDRAG},
	{M_BTN_1phase, 20, 0, SHOW},

	{M_BTN_2phase,	0, 1, CLICK},
	{M_BTN_2phase,	0, 10, DRAGFOCUS, 0, TRUE}, // we are getting the drag focus
	{M_BTN_2phase,	0, 20, DRAGFOCUS, 0, FALSE}, // losing the drag focus
	{M_BTN_2phase, 1, 2, PLAYWAVE, 0, SOUND_BTNPRESS},
	{M_BTN_2phase, 2, 20, LOADVIEW, WIP1},
	{M_BTN_2phase, 10, 11, MOV, WSPRITE, WIP2},
	{M_BTN_2phase, 11, 12, SHOW,WSPRITE},
	{M_BTN_2phase, 12, 0, PLAYWAVE, 0, SOUND_BTNDRAG},
	{M_BTN_2phase, 20, 21, MOV, WSPRITE, WIP3},
	{M_BTN_2phase, 21, 0, SHOW,WSPRITE},

	//------------------------------------------------------
	// Scene 2: The Vend-o-mat.

	// machine relations for scene 2
	#define WIP_S02_VIAL	WIP1 // used by NUM1, NUM10, UPs and DNs
	#define WIP_S02_LOCK	WIP2 // used by VIAL, UPs and DNs

	#define WIP_DISP01	WIP1  // Used by the VIAL
	#define WIP_DISP10	WIP3  // Used by the VIAL

	#define WIP_S02_EBIN WIP1 // used by BIN.

	// interesting states for scene 2 machines
	// The LOCK:
	#define M02_UNLOCKED	1
	#define M02_KEYED		9
	#define M02_LOCKED		15

	// The BIN
	#define M02_OPENEMPTY	0

	// goose parms for opening and closing locker bins.
	#define M02_CLOSEBIN  1
	#define M02_OPENBIN	  2

	// WPARM - COST
	// BPARM - AMOUNT
	{M02_VIAL, 0, 1, WAIT, 0, SIG_INC1},
	{M02_VIAL, 0, 2, WAIT, 0, SIG_DEC1},
	{M02_VIAL, 0, 3, WAIT, 0, SIG_INC10},
	{M02_VIAL, 0, 4, WAIT, 0, SIG_DEC10},
	{M02_VIAL, 0,10, DRAG, 0, IDD_SCOOPE},
	{M02_VIAL, 0,22, DRAG, 0, IDD_SCOOPF},
	{M02_VIAL, 1, 7, ADDI, WPARM, 1},
	{M02_VIAL, 2, 7, SUBI, WPARM, 1},
	{M02_VIAL, 3, 7, ADDI, WPARM, 10},
	{M02_VIAL, 4, 7, SUBI, WPARM, 10},

	// ensure wparm in range 0..99
	{M02_VIAL, 7, 8, LTEi, WPARM, 99},
	{M02_VIAL, 7, 8, ASSIGN, WPARM, 99},

	{M02_VIAL, 8, 9, SIGNAL, WIP_DISP10, 0},
	{M02_VIAL, 9, 0, SIGNAL, WIP_DISP01, 0},

	// if unlocked, then let them take the nystrom:
 	{M02_VIAL,10, 0, NOTSTATE, M02_KEYED, WIP_S02_LOCK}, // if not keyed then bail
	{M02_VIAL,10, 0, EQUALi, BPARM, 0},
	{M02_VIAL,10,11, SUBI, BPARM, 1},
	{M02_VIAL,11,12, HANDOFF, 0, IDD_SCOOPF},
	{M02_VIAL,12,13, SIGNAL, WIP_DISP10, 0}, // update the counter
	{M02_VIAL,13, 0, SIGNAL, WIP_DISP01, 0}, // update the counter

	// if the lock is locked, then take their nystrom and see if we can open the bin.
 	{M02_VIAL,22, 0, NOTSTATE, M02_LOCKED, WIP_S02_LOCK},
	{M02_VIAL,22,24, GTE, BPARM, WPARM}, // if amount >= cost
	{M02_VIAL,22,23, ADDI, BPARM, 1},
	{M02_VIAL,23,24, HANDOFF, 0, IDD_SCOOPE},
	{M02_VIAL,24,25, SIGNAL, WIP_DISP10, 0}, // update the counter
	{M02_VIAL,25,26, SIGNAL, WIP_DISP01, 0}, // update the counter
	{M02_VIAL,26, 0, LT, BPARM, WPARM},// 
	{M02_VIAL,26,27, MOV, WTEMP1, WTHIS}, // get bin instance
	{M02_VIAL,27,28, ADDI, WTEMP1, S02_BIN___XX-S02_VIAL__XX}, // distance to my bin
	{M02_VIAL,28, 0, SIGNAL, WTEMP1, SIG_OPEN}, // open the bin

  	{M02_LOCK, 0, M02_UNLOCKED, C_ACCEPT, 0, IDC_KEY},
  	{M02_LOCK, M02_UNLOCKED, 2, DROP, 0, 0},
	{M02_LOCK, 2, 3, PLAYWAVE, 0, SOUND_CLICK},
	{M02_LOCK, 3, 4, MOV, WTEMP1, WTHIS}, // get bin instance
	{M02_LOCK, 4, 5, ADDI, WTEMP1, S02_BIN___XX-S02_LOCK__XX}, // distance to my bin
	{M02_LOCK, 5, 7, SIGNAL, WTEMP1, SIG_OPEN}, // make sure the bin is open.
  	{M02_LOCK, 7, 8, O_ACCEPT, WOBJECT}, // set up to accept only this object
	{M02_LOCK, 8, M02_KEYED, SHOW, 0, IDS_LOCKWKEY},

  	{M02_LOCK, M02_KEYED,10, GRAB, 0, 0},
	{M02_LOCK,10,11, MOV, WTEMP1, WTHIS}, // get bin instance
	{M02_LOCK,11,12, ADDI, WTEMP1, S02_BIN___XX-S02_LOCK__XX}, // distance to my bin
  	{M02_LOCK,12,0, IFSTATE, M02_OPENEMPTY, WTEMP1},
	{M02_LOCK,12,14, SIGNAL, WTEMP1, SIG_CLOSE}, // lock the object in the bin
  	{M02_LOCK,14, M02_LOCKED, SHOW, 0, 0,/*IDS_LOCKED*/},
  	{M02_LOCK, M02_LOCKED, 2, DROP, WPARM, 0}, // go to keyed state

	// bin
  	{M02_BIN, M02_OPENEMPTY, 1, DROP, 0, 0}, // open/empty
  	{M02_BIN, M02_OPENEMPTY, 8, WAIT, 0, SIG_CLOSE},
  	{M02_BIN, 1, 2, ASHOW, WOBJECT, 0},
  	{M02_BIN, 2, 3, MOV, WTEMP1, WOBJECT}, // pass the object ot EBIN
  	{M02_BIN, 3, 4, SIGNAL, WIP_S02_EBIN},
  	{M02_BIN, 4, 9, WAIT, 0, SIG_CLOSE},//open/full
  	{M02_BIN, 4, 5, GRAB},  // open/full
  	{M02_BIN, 5, 6, SHOW},
  	{M02_BIN, 6, 7, ASSIGN, WTEMP1, 0}, // pass the object ot EBIN
  	{M02_BIN, 7, 0, SIGNAL, WIP_S02_EBIN},
  	{M02_BIN, 8, 0, WAIT, 0, SIG_OPEN},//closed/empty
  	{M02_BIN, 9, 4, WAIT, 0, SIG_OPEN},//closed/full

	{M02_UP1, 0, 1, CLICK},
	{M02_UP1, 1, 0, IFSTATE, M02_LOCKED, WIP_S02_LOCK},
	{M02_UP1, 1, 2, PLAYWAVE, 0, SOUND_CLICK},
	{M02_UP1, 2, 0, SIGNAL, WIP_S02_VIAL, SIG_INC1}, 

  	{M02_UP10, 0, 1, CLICK},
	{M02_UP10, 1, 0, IFSTATE, M02_LOCKED, WIP_S02_LOCK},
	{M02_UP10, 1, 2, PLAYWAVE, 0, SOUND_CLICK},
	{M02_UP10, 2, 0, SIGNAL, WIP_S02_VIAL, SIG_INC10},

	{M02_DN1, 0, 1, CLICK},
	{M02_DN1, 1, 0, IFSTATE, M02_LOCKED, WIP_S02_LOCK},
	{M02_DN1, 1, 2, PLAYWAVE, 0, SOUND_CLICK},
	{M02_DN1, 2, 0, SIGNAL, WIP_S02_VIAL, SIG_DEC1}, 

	{M02_DN10, 0, 1, CLICK},
	{M02_DN10, 1, 0, IFSTATE, M02_LOCKED, WIP_S02_LOCK},
	{M02_DN10, 1, 2, PLAYWAVE, 0, SOUND_CLICK},
	{M02_DN10, 2, 0, SIGNAL, WIP_S02_VIAL, SIG_DEC10}, 

	// 1's column
	{M02_NUM1, 0, 1, WAIT},
	{M02_NUM1, 1, 2, REF_MACHINE, WIP_S02_VIAL},
	{M02_NUM1, 2, 3, MOV,  BFRAME, R_WPARM},
	{M02_NUM1, 3, 4, SUB,  BFRAME, R_BPARM}, // cost - amount
	{M02_NUM1, 4, 5, MODI, BFRAME, 10},
	{M02_NUM1, 5, 0, SHOW, 0, IDS_CITYNUM},

	// 10's column
	{M02_NUM10, 0, 1, WAIT},
	{M02_NUM10, 1, 2, REF_MACHINE, WIP_S02_VIAL},
	{M02_NUM10, 2, 3, MOV,  BFRAME, R_WPARM},
	{M02_NUM10, 3, 4, SUB,  BFRAME, R_BPARM}, // cost - amount
	{M02_NUM10, 4, 5, DIVI, BFRAME, 10},
	{M02_NUM10, 5, 0, SHOW, 0, IDS_CITYNUM},

	//------------------------------------------------------
	// The Locksmythe: SCENE 3
	{M04_KEYCLAMP,	0,  1, C_ACCEPT,  0,IDC_KEY},
	{M04_KEYCLAMP,	1,  2, DROP},
	// reflect the key setting in the slider bars:
	{M04_KEYCLAMP,	2,  3, SUBI, WOBJECT, IDD_CITY_KEY1},
	{M04_KEYCLAMP,	3,  4, REF_MACHINE, WIP3},
	{M04_KEYCLAMP,	4,  5, MOV, R_BFRAME, WOBJECT},
	{M04_KEYCLAMP,	5,  6, MODI, R_BFRAME, 4},
	{M04_KEYCLAMP,	6,  7, DIVI, WOBJECT,4},
	{M04_KEYCLAMP,	7,  8, REF_MACHINE, WIP2},
	{M04_KEYCLAMP,	8,  9, MOV, R_BFRAME, WOBJECT},
	{M04_KEYCLAMP,	9, 10, MODI, R_BFRAME, 4},
	{M04_KEYCLAMP, 10, 11, DIVI, WOBJECT,4},
	{M04_KEYCLAMP, 11, 12, REF_MACHINE, WIP1},
	{M04_KEYCLAMP, 12, 13, MOV, R_BFRAME, WOBJECT},

	{M04_KEYCLAMP,	13, 14, SHOW,	  0, IDS_KEY_CFGWKEY},
	{M04_KEYCLAMP,	14, 15, CLEAR,  WOBJECT},  // no need to transmit with table request.
	{M04_KEYCLAMP,	15, 16, GRAB},
	{M04_KEYCLAMP,	16, 17, REF_MACHINE, WIP1},
	{M04_KEYCLAMP,	17, 18, MOV,	  WOBJECT, R_BFRAME},
	{M04_KEYCLAMP,	18, 19, CLEAR,	  R_BFRAME},
	{M04_KEYCLAMP,	19, 20, MULI,	  WOBJECT, 4},
	{M04_KEYCLAMP,	20, 21, REF_MACHINE, WIP2},
	{M04_KEYCLAMP,	21, 22, ADD,	  WOBJECT, R_BFRAME},
	{M04_KEYCLAMP,	22, 23, CLEAR,	  R_BFRAME},
	{M04_KEYCLAMP,	23, 24, MULI,	  WOBJECT, 4},
	{M04_KEYCLAMP,	24, 25, REF_MACHINE, WIP3},
	{M04_KEYCLAMP,	25, 26, ADD,	  WOBJECT, R_BFRAME},
	{M04_KEYCLAMP,	26, 27, CLEAR,	  R_BFRAME},
	{M04_KEYCLAMP,	27, 28, ADDI, WOBJECT,IDD_CITY_KEY1},
	{M04_KEYCLAMP,	28, 29, HANDOFF, WOBJECT},
	{M04_KEYCLAMP,	29,  0, SHOW},

	{M04_SLIDER,	0, 1, CLICK},
	{M04_SLIDER,	0, 3, WAIT},
	{M04_SLIDER,	1, 2, ADDI,	  BFRAME,1},
	{M04_SLIDER,	2, 3, LTEi,	  BFRAME,3},
	{M04_SLIDER,	2, 3, ASSIGN, BFRAME,0},
	{M04_SLIDER,	3, 0, SHOW,	  WSPRITE},

	{M04_VIAL, 0,1, DRAG, 0, IDD_SCOOPF},
	{M04_VIAL, 1,2, HANDOFF, 0, IDD_SCOOPE},
	{M04_VIAL, 2,3, DRAG, 0, IDD_SCOOPF},
	{M04_VIAL, 3,4, HANDOFF, 0, IDD_SCOOPE},
	{M04_VIAL, 4,0, SIGNALi, 0, S04_BIN},
	
	{M04_BIN,  0,1, WAIT},
	{M04_BIN,  1,2, ASSIGN, WOBJECT, IDD_CITY_KEY1},
	{M04_BIN,  2,3, SHOW, 0, IDS_KEY_IN_BIN},
	{M04_BIN,  3,4, GRAB},
	{M04_BIN,  4,0, SHOW},

	//------------------------------------------------------
	// The ChemLab: SCENE 5
	{M05_UP,	0, 1, CLICK},
	{M05_UP,	1, 2, PLAYWAVE, 0, SOUND_CLICK},
	{M05_UP,	2, 0, SIGNAL, WIP1, SIG_INC1},

	{M05_DOWN,	0, 1, CLICK},
	{M05_DOWN,	1, 2, PLAYWAVE, 0, SOUND_CLICK},
	{M05_DOWN,	2, 0, SIGNAL, WIP1, SIG_DEC1},

	// selection icon.
	{M05_ICON,	0, 1, WAIT, 0, SIG_INC1},
	{M05_ICON,	0, 7, WAIT, 0, SIG_DEC1},
	// increment:
	{M05_ICON,	1, 2, ADDI, BFRAME, 1},
	{M05_ICON,	2,20, LTi, BFRAME, CHEM_COUNT},
	{M05_ICON,	2,20, ASSIGN, BFRAME, 0},
	// decrement:
	{M05_ICON,	7, 8, EQUALi, BFRAME, 0},
	{M05_ICON,	7,20, SUBI, BFRAME, 1},
	{M05_ICON,	8,20, ASSIGN, BFRAME, CHEM_COUNT-1},
	// update display:
	{M05_ICON,	20,21, SHOW, 0, IDS_CHEMS},
	{M05_ICON,	21,22, SIGNAL, WIP1}, // vial (stores chem cost)
	{M05_ICON,	22,23, SIGNAL, WIP2}, // 1's column
	{M05_ICON,	23, 0, SIGNAL, WIP3}, // 10's column

	// 1's column
	{M05_NUM1, 0, 1, WAIT},
	{M05_NUM1, 1, 2, REF_MACHINE, WIP1},
	{M05_NUM1, 2, 3, MOV,  BFRAME, R_BPARM},
	{M05_NUM1, 3, 4, MODI, BFRAME, 10},
	{M05_NUM1, 4, 0, SHOW, 0, IDS_CITYNUM},

	// 10's column
	{M05_NUM10, 0, 1, WAIT},
	{M05_NUM10, 1, 2, REF_MACHINE, WIP1},
	{M05_NUM10, 2, 3, MOV,  BFRAME, R_BPARM},
	{M05_NUM10, 3, 4, DIVI, BFRAME, 10},
	{M05_NUM10, 4, 0, SHOW, 0, IDS_CITYNUM},

	// Cash in advance.  Use it or lose it:
	{M05_VIAL,  0, 1, DRAG, 0, IDD_SCOOPF},
	{M05_VIAL,  0, 7, CLICK},
	{M05_VIAL,  0,20, WAIT},

	// presented a full scoop ...
	{M05_VIAL,  1, 7, GTE, WPARM, BPARM}, // if amount >= cost
	{M05_VIAL,  1, 2, ADDI, WPARM, 1},
	{M05_VIAL,  2, 3, HANDOFF, 0, IDD_SCOOPE},
	{M05_VIAL,  3, 7, PLAYWAVE, 0, SOUND_SLURP},

	// ... or clicked w/o scoop
	{M05_VIAL,  7, 0, LT, WPARM, BPARM}, // if amount ! >= cost
	{M05_VIAL,  7, 8, SIGNAL, WIP1}, // goose the chem bin
	{M05_VIAL,  8, 9, SIGNAL, WIP2}, // goose the bubbles
	{M05_VIAL,  9, 0, SUB, WPARM, BPARM}, // subtract the cost of the item from the vial

	// the price has changed:
	{M05_VIAL, 20, 21, REF_MACHINE, WIP3}, // reference the icon to get the cost.
	{M05_VIAL, 21, 22, MOV,  BPARM, R_BFRAME},
	{M05_VIAL, 22,  0, MAPi,  BPARM, CHEMCOST}, // get cost of chemical.

	// Show the chem vial,  let player grab it:
	{M05_CHEM, 0, 1, WAIT},
	{M05_CHEM, 1, 2, SHOW, 0, IDS_CHEMFILL},
	{M05_CHEM, 2, 3, ANIMATE}, // animate without looping
	{M05_CHEM, 3, 4, REF_MACHINE, WIP1}, // reference the icon to get the molecule.
	{M05_CHEM, 4, 5, MOV, WOBJECT, R_BFRAME},
	{M05_CHEM, 5, 20, ADDI, WOBJECT, IDD_CHEMV01},
	{M05_CHEM, 20,21, GRAB},
	{M05_CHEM, 21,22, CLEAR, WOBJECT},
	{M05_CHEM, 22,23, CLEAR, WSPRITE},
	{M05_CHEM, 23, 0, SHOW},

	//
	{M05_BUBBLE, 0, 1, WAIT},
	{M05_BUBBLE, 1, 0, VIDEO, 0, IDS_CHEMBUBL},

	//------------------------------------------------------
	// Scene 06: Library and Pez hell.
	{M06_PEZPOP, 0, 1, CLICK},
	{M06_PEZPOP, 1, 2, SHOW, 0, IDS_PEZPOP},
	{M06_PEZPOP, 2, 3, ANIMATE},
	{M06_PEZPOP, 3, 4, GRAB, WIP1},
	{M06_PEZPOP, 4, 5, SHOW},
	{M06_PEZPOP, 5, 6, CLEAR, WSPRITE},
	{M06_PEZPOP, 6, 0, VIDEO, V_REVERSE, IDS_PEZPOP},

	{M06_TRI1, 0, 1, WAIT},
	{M06_TRI1, 1, 2, MOV, WOBJECT, WTEMP1},
	{M06_TRI1, 2, 3, MAPi, WOBJECT, TEMPLATE_OBJECTS},
	{M06_TRI1, 3, 4, ASHOW, WOBJECT},
	{M06_TRI1, 4, 5, SIGNAL, WIP1},
	{M06_TRI1, 5, 6, WAIT},
	{M06_TRI1, 6, 7, CLEAR, WOBJECT},
	{M06_TRI1, 7, 8, SHOW},
	{M06_TRI1, 8, 0, SIGNAL, WIP1},

	{M06_TRI2, 0, 1, WAIT},
	{M06_TRI2, 1, 2, MOV, BFRAME, WTEMP1},
	{M06_TRI2, 2, 3, MAPi, BFRAME, TEMPLATE_COST},
	{M06_TRI2, 3, 4, SHOW, 0, IDS_VIALCNT},
	{M06_TRI2, 4, 5, SIGNAL, WIP1},
	{M06_TRI2, 5, 6, WAIT},
	{M06_TRI2, 6, 7, CLEAR, BFRAME},
	{M06_TRI2, 7, 8, SHOW},
	{M06_TRI2, 8, 0, SIGNAL, WIP1},

	{M06_TRI3, 0, 1, WAIT},
	{M06_TRI3, 1, 2, MOV, BFRAME, WTEMP1},
	{M06_TRI3, 2, 3, MAPi, BFRAME, TEMPLATE_CHEM1},
	{M06_TRI3, 3, 4, SHOW, 0, IDS_CHEMS},
	{M06_TRI3, 4, 5, SIGNAL, WIP1},
	{M06_TRI3, 5, 6, WAIT},
	{M06_TRI3, 6, 7, CLEAR, BFRAME},
	{M06_TRI3, 7, 8, SHOW},
	{M06_TRI3, 8, 0, SIGNAL, WIP1},

	{M06_TRI4, 0, 1, WAIT},
	{M06_TRI4, 1, 2, MOV, BFRAME, WTEMP1},
	{M06_TRI4, 2, 3, MAPi, BFRAME, TEMPLATE_CHEM2},
	{M06_TRI4, 3, 4, SHOW, 0, IDS_CHEMS},
	{M06_TRI4, 4, 5, WAIT},
	{M06_TRI4, 5, 6, CLEAR, BFRAME},
	{M06_TRI4, 6, 0, SHOW},

	{M06_TEMPHOLD, 0, 1, DROP},
	{M06_TEMPHOLD, 1, 2, SHOW, 0, IDS_TMORPH},
	{M06_TEMPHOLD, 2, 3, ANIMATE, 0, V_LOOP|V_REWIND},
	{M06_TEMPHOLD, 3, 4, MOV, WTEMP1, WOBJECT},
	{M06_TEMPHOLD, 4, 5, SUBI, WTEMP1, IDD_TEMPLATE01},
	{M06_TEMPHOLD, 5, 6, SIGNAL, WIP1},
	{M06_TEMPHOLD, 6, 7, GRAB},
	{M06_TEMPHOLD, 7, 8, CLEAR, WOBJECT},
	{M06_TEMPHOLD, 8, 9, SIGNAL, WIP1},
	{M06_TEMPHOLD, 9, 0, SHOW},

	//------------------------------------------------------
	// Scene 08: Manufacturing.
#define HAS_TEMPLATE 6
	// Tempholder stores the template # (not ID)
	// in wParm for the convienence of the hopper
	{M08_TEMPHOLD, 0, 1, DROP},
	{M08_TEMPHOLD, 1, 2, SHOW, 0, IDS_TMORPH},
	{M08_TEMPHOLD, 2, 3, ANIMATE, 0, V_LOOP|V_REWIND},
	{M08_TEMPHOLD, 3, 4, MOV, WPARM, WOBJECT},
	{M08_TEMPHOLD, 4, 5, SUBI, WPARM, IDD_TEMPLATE01},
	{M08_TEMPHOLD, 5, 6, SIGNAL, WIP1},
	{M08_TEMPHOLD, HAS_TEMPLATE, 7, GRAB},
	{M08_TEMPHOLD, 7, 8, CLEAR, WOBJECT},
	{M08_TEMPHOLD, 8, 9, CLEAR, WPARM},
	{M08_TEMPHOLD, 9, 0, SHOW},

	// When testing for proper ingredients, use registers as follows:
	// Nystrom:bParm,  chem1:wParm,  chem2:wObject,
	{M08_HOPPER,  0, 1, DROP},
	{M08_HOPPER,  0, 5, DRAG, 0, IDD_SCOOPF},
	{M08_HOPPER,  1, 2, MOV, WPARM, WOBJECT},
	{M08_HOPPER,  2, 20, CLEAR, WOBJECT},
	// allow user to add nystrom to the machine:
	{M08_HOPPER,  5, 6, HANDOFF, 0, IDD_SCOOPE},
	{M08_HOPPER,  6, 0, ADDI, BPARM, 1},

	// The 1st ingredient has been added:
	{M08_HOPPER, 20, 21, SHOW, WPARM},
	{M08_HOPPER, 21,100, DROP}, // drop the second ingredient, see if we can gen the object!
	{M08_HOPPER, 21, 31, DRAG, 0, IDD_SCOOPF},
	{M08_HOPPER, 21, 22, GRAB, WPARM}, // remove the 1st ingredient
	{M08_HOPPER, 22,  0, SHOW},
	// again, allow user to add nystrom to the machine:
	{M08_HOPPER, 31, 32, HANDOFF, 0, IDD_SCOOPE},
	{M08_HOPPER, 32, 20, ADDI, BPARM, 1},

#define HOPPER_FULL 51
	// The 2nd ingredient has been added:
	{M08_HOPPER, 50, HOPPER_FULL, SHOW, WOBJECT},
	{M08_HOPPER, HOPPER_FULL, 61, DRAG, 0, IDD_SCOOPF},
	{M08_HOPPER, HOPPER_FULL, 20, GRAB}, // remove the 2st ingredient
	{M08_HOPPER, HOPPER_FULL, 100, WAIT}, // Template was changed,  try again!
	// again, allow user to add nystrom to the machine:
	{M08_HOPPER, 61, 62, HANDOFF, 0, IDD_SCOOPE},
	{M08_HOPPER, 62, 100, ADDI, BPARM, 1}, // can we afford it yet?

	{M08_HOPPER, 100,  50, NOTSTATE, HAS_TEMPLATE, WIP1}, // is there a template?
	{M08_HOPPER, 100, 110, REF_MACHINE, WIP1}, // point to the template machime
	// test chem1
	{M08_HOPPER, 110, 111, MOV, WTEMP1, R_WPARM}, // get the template #
	{M08_HOPPER, 111, 112, MAPi, WTEMP1, TEMPLATE_CHEM1}, // get chem1
	{M08_HOPPER, 112, 113, ADDI, WTEMP1, IDD_CHEMV01}, // Add base chem ID for test
	{M08_HOPPER, 113,  50, NEQUAL, WTEMP1, WPARM}, // Wrong chem1 ?
	// test chem2
	{M08_HOPPER, 113, 121, MOV, WTEMP1, R_WPARM}, // get the template #
	{M08_HOPPER, 121, 122, MAPi, WTEMP1, TEMPLATE_CHEM2}, // get chem1
	{M08_HOPPER, 122, 123, ADDI, WTEMP1, IDD_CHEMV01}, // Add base chem ID for test
	{M08_HOPPER, 123,  50, NEQUAL, WTEMP1, WOBJECT}, // Wrong chem2 ?
	// test cost
	{M08_HOPPER, 123, 131, MOV, WTEMP1, R_WPARM}, // get the template #
	{M08_HOPPER, 131, 132, MAPi, WTEMP1, TEMPLATE_COST}, // get cost of object
	{M08_HOPPER, 132,  50, GT, WTEMP1, BPARM}, // Can't afford object

	// they bought it, charge the account
	{M08_HOPPER, 132, 141, SUB, BPARM, WTEMP1},
	{M08_HOPPER, 141, 142, CLEAR, WOBJECT},
	{M08_HOPPER, 142, 143, CLEAR, WPARM},
	{M08_HOPPER, 143, 150, SHOW},

	// Now create it by calling 'BIN'
	{M08_HOPPER, 150, 151, MOV, WTEMP1, R_WPARM}, // get the template #
	{M08_HOPPER, 151, 152, MAPi, WTEMP1, TEMPLATE_OBJECTS}, // get object
	{M08_HOPPER, 152, 153, SIGNAL, WIP2}, // Call the bin with t1 set to create object
	{M08_HOPPER, 153,   0, SIGNAL, WIP3}, // Call the bin with t1 set to create object

	{M08_BEEM, 0, 1, WAIT},
	{M08_BEEM, 0, 1, CLICK},
	{M08_BEEM, 1, 0, VIDEO, 0, IDS_BEAMER},

	{M08_BIN, 0, 1, WAIT},
	{M08_BIN, 1, 2, MOV, WOBJECT, WTEMP1},
	{M08_BIN, 2, 3, ASHOW, WOBJECT},
	{M08_BIN, 3, 4, GRAB},
	{M08_BIN, 4, 0, SHOW},

	//------------------------------------------------------
	// The DRYPIT: SCENE 10

#define MAX_DRYPIT 20

//	COMPUTE_SPRITE(M10_DRYPIT, 0, 10, BPARM, MAX_DRYPIT, 10, IDS_PIT00),
	COMPUTE_SPRITE(M10_DRYPIT, 0, 10, BPARM, MAX_DRYPIT, 9, IDS_PIT00),
	{M10_DRYPIT, 10, 21, DRAG,			0,	IDD_SCOOPE},
	{M10_DRYPIT, 10, 31, DRAG,			0,	IDD_SCOOPF},

	{M10_DRYPIT, 21, 22, GTEi,			BPARM, 1},
	{M10_DRYPIT, 21, 10, EPSILON, 0},
	{M10_DRYPIT, 22, 23, PLAYWAVE,		0, SOUND_SLURP},
	{M10_DRYPIT, 23, 24, SUBI,			BPARM, 1},
	{M10_DRYPIT, 24, 25, HANDOFF,		0, IDD_SCOOPF},
	{M10_DRYPIT, 25, 26, EQUALi,			BPARM, MAX_DRYPIT-1},
	{M10_DRYPIT, 25,  0, EPSILON,		0, 0},
	{M10_DRYPIT, 26, 27, EPSILON,		5, 5}, // timing is broken
	{M10_DRYPIT, 27,  0, SIGNALi,			0, S10_FCLOSE},

	{M10_DRYPIT, 31, 32, LTEi,			BPARM, MAX_DRYPIT-1}, //less than MAX_DRYPIT
	{M10_DRYPIT, 31, 10, EPSILON},
	{M10_DRYPIT, 32, 33, PLAYWAVE,		0, SOUND_SPIT},
	{M10_DRYPIT, 33, 34, ADDI,			BPARM, 1},
	{M10_DRYPIT, 34, 35, HANDOFF,		0, IDD_SCOOPE},
	{M10_DRYPIT, 35, 36, EQUALi,		BPARM, MAX_DRYPIT},
	{M10_DRYPIT, 35,  0, EPSILON},
	{M10_DRYPIT, 36, 37, EPSILON,		5, 5}, // timing is broken
	{M10_DRYPIT, 37,  0, SIGNALi,		0, S10_FOPEN},

	{M10_FOPEN,  0, 1, SHOW,			0, IDS_FLOWER0},
	{M10_FOPEN,  1, 2, WAIT},
	{M10_FOPEN,  2, 3, PLAYWAVE,		0, SOUND_BUZZFUZZ},
	ANIMATE_SPRITES(M10_FOPEN, 3, 20, IDS_FLOWER0, 10, 10),
	{M10_FOPEN, 20, 1, SHOW,		0, IDS_FLOWER9},

	{M10_FCLOSE,  0, 1, WAIT},
	{M10_FCLOSE, 1, 2, PLAYWAVE,	0, SOUND_FUZZBUZZ},
	rANIMATE_SPRITES(M10_FCLOSE,	2, 20, IDS_FLOWER9, 10, 10),
	{M10_FCLOSE, 20, 0, SHOW,		0, IDS_FLOWER0},

	{M11_FIRE,	0, 1, DROP,		0,	IDD_LOG},
	{M11_FIRE,	1, 2, PLAYWAVE,	0,	SOUND_CLUNK},
	{M11_FIRE,	2, 20, SHOW,	0,	IDS_FISHWOOD},

	{M11_FIRE,	20, 0, GRAB},
	{M11_FIRE,	20, 21, DRAG,		0,	IDD_MATCH},
	{M11_FIRE,	21, 22, SHOW,		0,	IDS_BURNWOOD},
	{M11_FIRE,	22, 30, ANIMATE,	0,	V_LOOP|V_REWIND},

// How long does each log burn for?
#define BURN_TIME 30
	{M11_FIRE,	30, 31, ADDI,		WPARM,	BURN_TIME},
	{M11_FIRE,	31, 32, CLEAR,	DETIME}, // start the burn over ...
	{M11_FIRE,	32, 40, PLAYWAVE,		0,	SOUND_FIRE},

#define ON_FIRE	  40
	{M11_FIRE,	ON_FIRE, 50, ESTIME,	WPARM},
	{M11_FIRE,	ON_FIRE, 50, DROP,		0,	IDD_WATER},
	{M11_FIRE,	ON_FIRE, 30, DROP,		0,	IDD_LOG},

	{M11_FIRE,	50, 51, CLEAR, WOBJECT},
	{M11_FIRE,	51, 52, CLEAR, WSPRITE},
	{M11_FIRE,	52, 53, CLEAR, WPARM},
	{M11_FIRE,	53, 54, SHOW},
	{M11_FIRE,	54, 0, STOPWAVE},

// How long does it take to burn a fish?
#define FISH_BURN_TIME 10
	{M11_GRILL,	 0, 10, DROP},
	{M11_GRILL,	10, 20, SHOW,		WOBJECT},
	{M11_GRILL,	20, 50, ANIMATE,	0,V_LOOP},
	{M11_GRILL,	50, 60, ESTIME,		0, FISH_BURN_TIME},
	{M11_GRILL,	50, 80, GRAB},
	{M11_GRILL,	60, 50, NOTSTATE,	ON_FIRE, WIP1},
	{M11_GRILL,	60, 61, MAP_OBJ,	0,	OP_BURN},
	{M11_GRILL,	61, 62, SHOW,		0,	IDS_FISHASH},
	{M11_GRILL,	62, 63, PLAYWAVE,	0,	SOUND_FIRE},
	{M11_GRILL,	63, 70, SIGNAL, WIP2},
	{M11_GRILL,	70, 80, GRAB},
	{M11_GRILL,	80,  0, SHOW},

	{M11_ASCENT,	0,  1, WAIT},
	{M11_ASCENT,	1,  2, SHOW, 0, IDS_SPIRIT0},
	{M11_ASCENT,	2,  3, ANIMATE},
	{M11_ASCENT,	3,  4, GRAB, 0, IDD_SPRINGS},
	{M11_ASCENT,	3,  4, ESTIME, 0, 4},
	{M11_ASCENT,	4,  5, CLEAR, WOBJECT},
	{M11_ASCENT,	5,  6, CLEAR, WPARM},
	{M11_ASCENT,	6,  7, CLEAR, WSPRITE},
	{M11_ASCENT,	7,  0, SHOW},

	{M_BAITSTATION,	0, 1, C_ACCEPT,	0,	IDC_POLE},
	{M_BAITSTATION,	1, 2, SHOW},
	{M_BAITSTATION,	2, 3, DROP},
	{M_BAITSTATION,	3, 8, IS_A,	WOBJECT, ISA_BAITEDPOLE},
	{M_BAITSTATION,	3, 4, SHOW, 0, IDS_POLE1LOL},
	{M_BAITSTATION,	4, 5, MOV, WPARM, WOBJECT},
	{M_BAITSTATION,	5, 6, C_ACCEPT,	0, ISA_BAIT},
	{M_BAITSTATION,	6, 0, GRAB},
	{M_BAITSTATION,	6, 7, DROP},
	{M_BAITSTATION,	7, 8, MIX, WPARM, WOBJECT}, // WPARM(WOBJECT) --> WOBJECT
	{M_BAITSTATION,	8, 9, SHOW, 0, IDS_POLE1B},
	{M_BAITSTATION, 9, 0, GRAB},

// How long does it take to catch a fish?
#define MIN_CATCH_TIME 20
#define ADD_CATCH_TIME 120
	{M_FISHSTATION,	0, 1, C_ACCEPT,	0, IDC_POLE},
	{M_FISHSTATION,	1, 2, SHOW},
	{M_FISHSTATION,	2, 3, DROP},
	{M_FISHSTATION,	2, 30, DRAG,0,IDD_BUCKE},
	{M_FISHSTATION,	3, 4, SHOW,		0,	IDS_POLE1LCL},
	{M_FISHSTATION,	4, 5, IS_A,	WOBJECT, ISA_BAITEDPOLE},
	{M_FISHSTATION,	4,16, EPSILON},
	// wRand = range(MIN_TIME..MIN_TIME+ADD_CATCH_TIME)
	{M_FISHSTATION,	5, 6, RAND,		ADD_CATCH_TIME, MIN_CATCH_TIME},
	{M_FISHSTATION,	6, 7, SYNCPOINT,WRAND, SYNC_FISH1},
	{M_FISHSTATION,	6, 0, GRAB},
	{M_FISHSTATION,	7, 8, PLAYWAVE,	0,	SOUND_HURT},
	{M_FISHSTATION,	8, 9, MOV,		WPARM,	WOBJECT},
	{M_FISHSTATION,	9,10, RAND,		IDD_FISH10-IDD_FISH1, IDD_FISH1}, // wRand = oneof(IDD_FISH1..IDD_FISH10)
	{M_FISHSTATION,10,11, MOV,		WOBJECT, WRAND},
	{M_FISHSTATION,11,12, SHOW,		0,	IDS_POLE1LCT},
	{M_FISHSTATION,12,13, GRAB},
	{M_FISHSTATION,13,14, MOV,		WOBJECT, WPARM},
	{M_FISHSTATION,14,15, XIM,		WOBJECT, WPARM},
	{M_FISHSTATION,15,16, SHOW,		WOBJECT},
	{M_FISHSTATION,16,0,  GRAB},
	{M_FISHSTATION,30,2, HANDOFF,0,IDD_BUCKF},

	{M_POLESTAND,	0, 1, ASSIGN,	WOBJECT, IDD_POLE1},
	{M_POLESTAND,	1, 2, SHOW,		WOBJECT},
	{M_POLESTAND,	2, 3, GRAB},
	{M_POLESTAND,	3, 4, SHOW},
	{M_POLESTAND,	4, 5, C_ACCEPT,	0,	IDC_POLE},
	{M_POLESTAND,	5, 1, DROP},

#define M12_WATER_FULL 4
#define M12_SHELF_FULL 3
	{M12_WATER, 0, 1, DROP, 0, IDD_BUCKF},
	{M12_WATER, 1, 2, HANDOFF,	0, IDD_BUCKE},
	{M12_WATER, 2, 3, SHOW,	0, IDS_VESTH2O},
	{M12_WATER, 3, 4, SIGNAL,	WIP1}, // BLOW OUT THE CANDLE
	{M12_WATER, M12_WATER_FULL, 5, WAIT},
	{M12_WATER, 5, 0, SHOW},

	{M12_CANDLE, 0, 1, DRAG,  0, IDD_MATCH},
	{M12_CANDLE, 0, 6, WAIT},
	{M12_CANDLE, 1, 2, SHOW,  0, IDD_VESTCAN},
	{M12_CANDLE, 2, 5, NOTSTATE, M12_WATER_FULL, WIP1},
	{M12_CANDLE, 2, 5, NOTSTATE, M12_SHELF_FULL, WIP2},
	{M12_CANDLE, 2, 0, SIGNAL,	WIP2}, // start object transformation
	{M12_CANDLE, 5, 6, WAIT},
	{M12_CANDLE, 6, 0, SHOW},

#define M12_SPRING_PRESENT 2
	{M12_ASHSHELF, 0, 1, DROP},
	{M12_ASHSHELF, 1, 2, SHOW, WOBJECT},
	{M12_ASHSHELF, 2, 3, SIGNAL, WIP1}, // blow out the candle
	{M12_ASHSHELF, 3, 4, WAIT},
	{M12_ASHSHELF, 3, 19, GRAB},
	{M12_ASHSHELF, 4, 5, ADDI,	WOBJECT, IDD_SPELL01-IDD_FISHASH1}, // this maps the fishash to a spell w/o table lookup
	{M12_ASHSHELF, 5, 15, NOTSTATE, M12_SPRING_PRESENT, WIP3},
	{M12_ASHSHELF, 5, 15, ADDI, WOBJECT, IDD_SPELx01-IDD_SPELL01},
	{M12_ASHSHELF, 15, 16, ASHOW, WOBJECT},
	{M12_ASHSHELF, 16, 17, SIGNAL,	WIP2}, // empty the water
	{M12_ASHSHELF, 17, 18, SIGNAL,	WIP3}, // toss the rings
	{M12_ASHSHELF, 18, 19, GRAB},
	{M12_ASHSHELF, 19, 0, SHOW},

	{M12_SPRING, 0, 1, DROP, 0, IDD_SPRINGS},
	{M12_SPRING, 1, M12_SPRING_PRESENT, ASHOW, WOBJECT},
	{M12_SPRING, M12_SPRING_PRESENT, 3, WAIT},
	{M12_SPRING, M12_SPRING_PRESENT, 3, GRAB},
	{M12_SPRING, 3, 4, CLEAR, WOBJECT},
	{M12_SPRING, 4, 0, SHOW},


	//------------------------------------------------------------------
	// must keep all the spell machines contiguous & in sequence
	// due to relative referencing:
	{M12_xCANDLE,  0, 20, DRAG,  0, IDD_MATCH},
	{M12_xCANDLE,  0, 10, WAIT, 0, SIG_CLEAR},
	{M12_xCANDLE,  0,200, DRAG,  0, IDD_SCOOPF},
	{M12_xCANDLE,  0, 20, CLICK},

	{M12_xCANDLE, 10, 11, NEQUALi, BPARM, 0},
	{M12_xCANDLE, 10,  0, SHOW},
	{M12_xCANDLE, 11,  0, SHOW, 0, IDS_CANNY1},

	// get id of current scroll:
	{M12_xCANDLE, 20, 21, MOV, WSPRITE, WIP1},
	{M12_xCANDLE, 21, 22, SHOW, WSPRITE},
	{M12_xCANDLE, 22, 23, MOV, WPARM, WTHIS},
	{M12_xCANDLE, 23, 24, ADDI, WPARM, 1}, // id of scroll clamp
	{M12_xCANDLE, 24, 25, REF_MACHINE, WPARM},
	{M12_xCANDLE, 25, 26, MOV, WTEMP1, R_WOBJECT},	  // get scroll id
	{M12_xCANDLE, 26,  0, EQUALi, WTEMP1, 0},		  // no scroll in place
	{M12_xCANDLE, 26, 30, SUBI, WTEMP1, IDD_SCR001},  // normalize scroll id

	// prepare to loop through all hoppers:
	{M12_xCANDLE, 30, 31, MOV, WPARM, WTHIS},
	{M12_xCANDLE, 31, 32, ADDI, WPARM, 2}, // id of first hopper
	{M12_xCANDLE, 32, 40, MOV, WTEMP2, 0},	// count hoppers looked at so far

	// top of loop:
	{M12_xCANDLE, 40, 50, REF_MACHINE, WPARM}, // point to a hopper

	// what object is supposed to be in this machine?
	{M12_xCANDLE, 50, 51, MOV, WOBJECT, WTEMP1},	// scroll id
	{M12_xCANDLE, 51, 52, MAP, WOBJECT, R_WIP1},	// scroll mapping table
	{M12_xCANDLE, 52, 60, EQUALi, WOBJECT, 0},		// no object required
	{M12_xCANDLE, 52, 53, SUBI, WOBJECT, 1},		// normalize index
	{M12_xCANDLE, 53, 54, MAP, WOBJECT, R_WIP2},	// object mapping table
	{M12_xCANDLE, 54, 0, NEQUAL, WOBJECT, R_WOBJECT}, // nope, test failed
	{M12_xCANDLE, 54, 60},

	{M12_xCANDLE, 60, 61, ADDI, WTEMP2, 1}, // count machines referenced
	{M12_xCANDLE, 61, 70, ADDI, WPARM, 1}, // id of next hopper
	{M12_xCANDLE, 70, 40, LTi, WTEMP2, 6}, // looked at all hoppers yet?

	// ingredients are an exact match.  clear 'em and present object:
	// prepare to loop through all hoppers:
	{M12_xCANDLE, 70, 131, MOV, WPARM, WTHIS},
	{M12_xCANDLE, 131, 132, ADDI, WPARM, 2}, // id of first hopper
	{M12_xCANDLE, 132, 140, MOV, WTEMP2, 0},	// count hoppers looked at so far

	// top of loop:
	{M12_xCANDLE, 140, 141, REF_MACHINE, WPARM}, // point to a hopper
	{M12_xCANDLE, 141, 160, SIGNAL, WPARM, SIG_CLEAR}, // clear the object

	{M12_xCANDLE, 160, 161, ADDI, WTEMP2, 1}, // count machines referenced
	{M12_xCANDLE, 161, 170, ADDI, WPARM, 1}, // id of next hopper
	{M12_xCANDLE, 170, 140, LTi, WTEMP2, 6}, // looked at all hoppers yet?
	{M12_xCANDLE, 170, 180},

	{M12_xCANDLE, 180, 181, MOV, WTEMP1, WTEMP1},	// scroll id
	{M12_xCANDLE, 181, 182, MAPi, WTEMP1, SCROLL_SPELL},	// scroll mapping table
	{M12_xCANDLE, 182, 183, ADDI, WTEMP1, IDS_ENCHANT},	// de-normalize spell id
	{M12_xCANDLE, 183, 184, MOV, WPARM, WTHIS},
	{M12_xCANDLE, 184, 185, ADDI, WPARM, 3}, // point to the center plant hopper
	{M12_xCANDLE, 185,   0, SIGNAL, WPARM, SIG_SET}, // make the object appear

	{M12_xCANDLE,200,201, ADDI, BPARM, 1},
	{M12_xCANDLE,201,202, PLAYWAVE, 0, SOUND_SLURP},
	{M12_xCANDLE,202,  0, HANDOFF, 0, IDD_SCOOPE},

	//---------------------------------------------------
	{M12_xSCROLL,  0, 10, DROP}, // reg init = c_accept IDC_SCROLL
	{M12_xSCROLL, 10, 11, SHOW, 0, IDS_SCRHUNG},
	{M12_xSCROLL, 11, 12, MOV, WTEMP1, WOBJECT},
	{M12_xSCROLL, 12, 14, SUBI, WTEMP1, IDD_SCR001},
	{M12_xSCROLL, 14, 15, MOV, WPARM, WTHIS},
	{M12_xSCROLL, 15, 16, ADDI, WPARM, 1},
	{M12_xSCROLL, 16, 17, SIGNAL, WPARM, SIG_OPEN},
	{M12_xSCROLL, 17, 18, ADDI, WPARM, 1},
	{M12_xSCROLL, 18, 19, SIGNAL, WPARM, SIG_OPEN},
	{M12_xSCROLL, 19, 20, ADDI, WPARM, 1},
	{M12_xSCROLL, 20, 21, SIGNAL, WPARM, SIG_OPEN},
	{M12_xSCROLL, 21, 22, ADDI, WPARM, 1},
	{M12_xSCROLL, 22, 23, SIGNAL, WPARM, SIG_OPEN},
	{M12_xSCROLL, 23, 24, ADDI, WPARM, 1},
	{M12_xSCROLL, 24, 25, SIGNAL, WPARM, SIG_OPEN},
	{M12_xSCROLL, 25, 26, ADDI, WPARM, 1},
	{M12_xSCROLL, 26, 30, SIGNAL, WPARM, SIG_OPEN},
	{M12_xSCROLL, 30, 114, GRAB}, // reg init = c_accept IDC_SCROLL
	{M12_xSCROLL, 114, 115, MOV, WPARM, WTHIS},
	{M12_xSCROLL, 115, 116, ADDI, WPARM, 1},
	{M12_xSCROLL, 116, 117, SIGNAL, WPARM, SIG_CLOSE},
	{M12_xSCROLL, 117, 118, ADDI, WPARM, 1},
	{M12_xSCROLL, 118, 119, SIGNAL, WPARM, SIG_CLOSE},
	{M12_xSCROLL, 119, 120, ADDI, WPARM, 1},
	{M12_xSCROLL, 120, 121, SIGNAL, WPARM, SIG_CLOSE},
	{M12_xSCROLL, 121, 122, ADDI, WPARM, 1},
	{M12_xSCROLL, 122, 123, SIGNAL, WPARM, SIG_CLOSE},
	{M12_xSCROLL, 123, 124, ADDI, WPARM, 1},
	{M12_xSCROLL, 124, 125, SIGNAL, WPARM, SIG_CLOSE},
	{M12_xSCROLL, 125, 126, ADDI, WPARM, 1},
	{M12_xSCROLL, 126, 130, SIGNAL, WPARM, SIG_CLOSE},
	{M12_xSCROLL, 130, 0, SHOW},


	{M12_xPLANT, 0, 11, WAIT, 0, SIG_OPEN},
	{M12_xPLANT, 0, 70, WAIT, 0, SIG_SET},
	{M12_xPLANT, 0, 30, DROP},
	// show the ghost of the required ingredient:
	{M12_xPLANT, 11, 12, MOV, BFRAME, WTEMP1},
	{M12_xPLANT, 12, 13, MAP, BFRAME, WIP1},	// scroll frame mapping table
	{M12_xPLANT, 13,  0, EQUALi, BFRAME, 0},
	{M12_xPLANT, 13, 14, MOV, WSPRITE, WIP3},	// filmstrip id
	{M12_xPLANT, 14, 15, SUBI, BFRAME, 1},
	{M12_xPLANT, 15, 20, SHOW, WSPRITE},

	// ghost is visible,  no object present:
	{M12_xPLANT, 20, 30, DROP},
	{M12_xPLANT, 20, 21, WAIT, 0, SIG_CLOSE},
	{M12_xPLANT, 21,  0, SHOW},

	// object was dropped onto ghost:
	{M12_xPLANT, 30, 31, CLEAR, BFRAME},
	{M12_xPLANT, 31, 32, ASHOW, WOBJECT},
	{M12_xPLANT, 32, 40, SIGNAL, WIP4, SIG_CLEAR},

	// object in waiting
	{M12_xPLANT, 40, 41, GRAB},
	{M12_xPLANT, 40, 41, WAIT, 0, SIG_CLEAR},
	{M12_xPLANT, 41, 42, CLEAR, BFRAME},
	{M12_xPLANT, 42, 43, CLEAR, WOBJECT},
	{M12_xPLANT, 43,  0, SHOW},

	{M12_xPLANT, 70, 71, MOV, WOBJECT, WTEMP1},
	{M12_xPLANT, 71, 72, VIDEO, V_REVERSE, IDS_SPEFFECT},
	{M12_xPLANT, 72, 73, PLAYWAVE, 0, SOUND_POPUP},
	{M12_xPLANT, 73, 40, ASHOW, WOBJECT},

	//------------------------------------------------------------------
	// Scene 17: Mine entrance:
	// socket is empty, waiting for cultural lock
	{M17_LOCKSOCKET, 0, 1, DROP},
	{M17_LOCKSOCKET, 1, 40, EQUALi, WOBJECT, IDD_CITLOCK},
	{M17_LOCKSOCKET, 1, 70, EQUALi, WOBJECT, IDD_VILLOCK},
	{M17_LOCKSOCKET, 1, 60},

	// Locked by the City Folk:
	{M17_LOCKSOCKET, 40, 41, SIGNAL, WIP1, SIG_CLOSE},
  	{M17_LOCKSOCKET, 41, 42, C_ACCEPT, 0, IDC_BOMB},
  	{M17_LOCKSOCKET, 42, 50, ASHOW, WOBJECT},
	{M17_LOCKSOCKET, 50, 60, DROP}, // Check for a bomb ...
	{M17_LOCKSOCKET, 50, 51, DRAG, 0, IDD_CITKEY},
	{M17_LOCKSOCKET, 50, 55, CLICK},
	{M17_LOCKSOCKET, 51, 50, SIGNAL, WIP1, SIG_ENTER_1},
	{M17_LOCKSOCKET, 55, 50, SIGNAL, WIP1, SIG_CLOSE},

	{M17_LOCKSOCKET, 60, 61, SIGNAL, WIP1, SIG_OPEN}, // open the door
	{M17_LOCKSOCKET, 61, 62, SIGNAL, WIP2, SIG_BURN}, // show the socket burned
	{M17_LOCKSOCKET, 62, 63, SHOW}, // show no key, go to start state
  	{M17_LOCKSOCKET, 63,  0, C_ACCEPT, 0, ISA_LOCKDROP},

	// Locked by the Village People:
	{M17_LOCKSOCKET, 70, 71, SIGNAL, WIP1, SIG_CLOSE},
  	{M17_LOCKSOCKET, 71, 72, C_ACCEPT, 0, IDC_BOMB},
  	{M17_LOCKSOCKET, 72, 80, ASHOW, WOBJECT},
	{M17_LOCKSOCKET, 80, 60, DROP}, // Check for a bomb ...
	{M17_LOCKSOCKET, 80, 81, DRAG, 0, IDD_VILKEY},
	{M17_LOCKSOCKET, 80, 85, CLICK},
	{M17_LOCKSOCKET, 81, 80, SIGNAL, WIP1, SIG_ENTER_1},
	{M17_LOCKSOCKET, 85, 80, SIGNAL, WIP1, SIG_CLOSE},

	{M17_LOCKBURN, 0, 1, WAIT,0,SIG_BURN},
	{M17_LOCKBURN, 1, 2, SHOW, 0, IDS_MINEBLOW},
	{M17_LOCKBURN, 2, 3, PLAYWAVE, 0, SOUND_EXPLODE},
	{M17_LOCKBURN, 3, 0, VIDEO, 0, IDS_EXPLODE1},

	{M17_DOORWAY, 0, 50, CLICK}, // enter, my friend
	{M17_DOORWAY, 0, 1, WAIT,0,SIG_CLOSE}, // door is open...
	{M17_DOORWAY, 1, 2, SIGNAL, WIP2, SIG_ON},
	{M17_DOORWAY, 2, 3, WAIT,0,SIG_OPEN},
	{M17_DOORWAY, 2, 70, WAIT,0,SIG_ENTER_1},
	{M17_DOORWAY, 3, 0, SIGNAL, WIP2, SIG_OFF},
	{M17_DOORWAY, 50, 0, LOADVIEW, WIP1}, // enter, my friend
	{M17_DOORWAY, 70, 71, SIGNAL, WIP2, SIG_OFF},
	{M17_DOORWAY, 71, 72, CLICK}, // enter, my friend
	{M17_DOORWAY, 71, 1, WAIT, 0, SIG_CLOSE}, // enter, my friend
	{M17_DOORWAY, 72, 1, LOADVIEW, WIP1}, // enter, my friend

	{M17_FFIELD, 0, 1, WAIT,0,SIG_ON},
	{M17_FFIELD, 1, 2, SIGNAL, WIP1, SIG_ON},
	{M17_FFIELD, 2, 3, SHOW,0,IDS_FORCEFLD},
	{M17_FFIELD, 3, 4, ANIMATE, 0, V_REWIND|V_LOOP},
	{M17_FFIELD, 4, 5, WAIT,0,SIG_OFF},
	{M17_FFIELD, 5, 6, SIGNAL, WIP1, SIG_OFF},
	{M17_FFIELD, 6, 0, SHOW},

	// a bottomless Nystrom mine  
	{M17_MINE,	0, 1, DRAG,			0, IDD_SCOOPE},
	{M17_MINE,	0, 3, DRAG,			0, IDD_SCOOPF},
	{M17_MINE,	1, 2, PLAYWAVE,		0, SOUND_SLURP},
	{M17_MINE,	2, 0, HANDOFF,		0, IDD_SCOOPF},
	{M17_MINE,	3, 4, PLAYWAVE,		0, SOUND_SPIT},
	{M17_MINE,	4, 0, HANDOFF,		0, IDD_SCOOPE},

	{M17_FOULWIND,	0, 1, WAIT},
	{M17_FOULWIND,	1, 2, PLAYWAVE,	0, SOUND_BURBLE},
	{M17_FOULWIND,	2, 4, ESTIME,	0, 5},
	{M17_FOULWIND,	4, 0, NEQUALi,	LVIEW, WIP1},
	{M17_FOULWIND,	4, 0, SIGNALi,	0, SID_DEC_ENERGY},

	// scrollman references other instances directly because he is cool.
	{M22_SCROLLMAN, 0,  5, WAIT, 0, SIG_VIEWINIT}, // wait until the view is loaded ...
	{M22_SCROLLMAN, 0, 10, WAIT}, // wait until the view is dumped ...

	{M22_SCROLLMAN, 5, 6, SUBI, WTEMP1, IDD_SCR001},
	{M22_SCROLLMAN, 6, 50, ASSIGN, BPARM, SIG_OPEN},

	{M22_SCROLLMAN, 10, 50, ASSIGN, BPARM, SIG_CLOSE},

	{M22_SCROLLMAN, 50, 51, SIGNALri, BPARM, S22_SHIELD},
	{M22_SCROLLMAN, 51, 52, SIGNALri, BPARM, S22_WORD},
	{M22_SCROLLMAN, 52, 53, SIGNALri, BPARM, S22_PLANT1},
	{M22_SCROLLMAN, 53, 54, SIGNALri, BPARM, S22_PLANT2},
	{M22_SCROLLMAN, 54, 55, SIGNALri, BPARM, S22_PLANT3},
	{M22_SCROLLMAN, 55, 56, SIGNALri, BPARM, S22_FISH1},
	{M22_SCROLLMAN, 56, 57, SIGNALri, BPARM, S22_FISH2},
	{M22_SCROLLMAN, 57, 58, SIGNALri, BPARM, S22_FISH3},
	{M22_SCROLLMAN, 58,	 0,	SIGNALri, BPARM, S22_NYSTROM},

	{M22_SHIELD, 0, 1, WAIT, 0, SIG_OPEN},
	{M22_SHIELD, 0, 6, WAIT, 0, SIG_CLOSE},
	{M22_SHIELD, 1, 2, MOV, BFRAME, WTEMP1},
	{M22_SHIELD, 2, 3, MAP, BFRAME, WIP1},
	{M22_SHIELD, 3, 0, EQUALi, BFRAME, 0},
	{M22_SHIELD, 3, 4, MOV, WSPRITE, WIP2},
	{M22_SHIELD, 4, 5, SUBI, BFRAME, 1},
	{M22_SHIELD, 5, 0, SHOW, WSPRITE},

	{M22_SHIELD, 6, 7, CLEAR, BFRAME},
	{M22_SHIELD, 7, 8, CLEAR, WSPRITE},
	{M22_SHIELD, 8, 0, SHOW},

	/*
	{M22_SHIELD, 0, 1, WAIT, 0, SIG_VIEWINIT},
	{M22_SHIELD, 0, 6, WAIT},
	{M22_SHIELD, 1, 2, MOV, WOBJECT, WTEMP1},
	{M22_SHIELD, 2, 3, MAP, WOBJECT, WIP1},
	{M22_SHIELD, 3, 0, ASHOW, WOBJECT},

	{M22_SHIELD, 6, 7, CLEAR, WOBJECT},
	{M22_SHIELD, 7, 0, SHOW},
	*/

	//------------------------------------------------------
	// Scene 24: Eye of God.
	{M24_EYEPLATE,	0, 1, DROP, 0, IDD_EYEFUSE},
	{M24_EYEPLATE,	1, 2, SIGNALi, 0, S24_BIGEYE},
	{M24_EYEPLATE,	2, 3, C_ACCEPT, 0, IDC_BOMB},
	{M24_EYEPLATE,	3, 4, ASHOW, 0, IDS_EYEPLATE},
	{M24_EYEPLATE,	4, 50, DROP},
	{M24_EYEPLATE, 50, 51, VIDEO, 0, IDS_EXPLODE1},
	{M24_EYEPLATE, 51, 52, PLAYWAVE, 0, SOUND_EXPLODE},
	{M24_EYEPLATE, 52, 53, SHOW},
	{M24_EYEPLATE, 53, 54, O_ACCEPT, 0, IDD_EYEFUSE},
	{M24_EYEPLATE, 54,  0, SIGNALi, SIG_BOMB, S24_BIGEYE},

	{M24_EYESPELL,	0, 1, DROP, 0, IDD_EYESPELL},
	{M24_EYESPELL,	1, 2, SIGNALi, 0, S24_BIGEYE},
	{M24_EYESPELL,	2, 3, C_ACCEPT, 0, IDC_BOMB},
	{M24_EYESPELL,	3, 4, ASHOW, 0, IDS_EYESPELL},
	{M24_EYESPELL,	4, 50, DROP},
	{M24_EYESPELL, 50, 51, VIDEO, 0, IDS_EXPLODE1},
	{M24_EYESPELL, 51, 52, PLAYWAVE, 0, SOUND_EXPLODE},
	{M24_EYESPELL, 52, 53, SHOW},
	{M24_EYESPELL, 53, 54, O_ACCEPT, 0, IDD_EYESPELL},
	{M24_EYESPELL, 54,  0, SIGNALi, SIG_BOMB, S24_BIGEYE},

	{M24_BIGEYE,	0, 10, WAIT},
	{M24_BIGEYE,	10, 50, IFSTATEi, 0, S24_EYEPLATE1},
	{M24_BIGEYE,	10, 50, IFSTATEi, 0, S24_EYEPLATE2},
	{M24_BIGEYE,	10, 50, IFSTATEi, 0, S24_EYEPLATE3},
	{M24_BIGEYE,	10, 50, IFSTATEi, 0, S24_EYESPELL1},
	//{M24_BIGEYE,	10, 50, IFSTATEi, 0, S24_EYESPELL2},
	{M24_BIGEYE,	10, 50, IFSTATEi, 0, S24_EYESPELL3},
	{M24_BIGEYE,	10, 20}, // looks like all is in place ...
	{M24_BIGEYE,	20, 30, ASHOW, 0, IDS_BIGEYE1},
	{M24_BIGEYE,	30, 30, GRAB, 0, IDD_EYEWAND},
	{M24_BIGEYE,	30, 50, WAIT, 0, SIG_BOMB},
	{M24_BIGEYE,	50, 0, SHOW},

	{M24_EYETEXT,	0, 10, DRAG, 0, IDD_EYEWAND},
	{M24_EYETEXT,	0, 20, DRAGFOCUS, 0, FALSE},
	{M24_EYETEXT,	0, 20, WAIT, 0, SIG_INIT},
	{M24_EYETEXT,	10,11, MOV, WSPRITE, WIP1},
	{M24_EYETEXT,	11, 0, SHOW, WSPRITE},
	{M24_EYETEXT,	20, 0, SHOW},

	//------------------------------------------------------
	// Scene 25: Rock 'n Roll at the Hidden Cave
	//-----------------------------------------------------
	{M25_RUMBLE,  0, 10, WAIT, 0, SIG_INIT},	  // clear out all the players
	{M25_RUMBLE,  0, 20, DRAGFOCUS, 0, FALSE},
	{M25_RUMBLE,  0, 30, DROP, 0, IDD_TELEKINESIS},
	{M25_RUMBLE,  0, 40, DRAGFOCUS, 0, TRUE},

	// re-start game:
	{M25_RUMBLE,  10,  0, CLEAR, BPARM},

	// player took his hand off:
	{M25_RUMBLE,  20,  0, SUBI, BPARM, 1},

	// player dropped a levitation spell:
	{M25_RUMBLE,  30, 50, ADDI, BPARM, 1},

	// new player helping out:
	{M25_RUMBLE,  40, 50, ADDI, BPARM, 1},
	// Look for player holding crowbar or wearing a strength spell ...
	// ...
	// ...

	{M25_RUMBLE,  50, 60, GTi, BPARM, 1},
	{M25_RUMBLE,  50,  0, VIDEO, 0, IDS_RUMBLE},

	{M25_RUMBLE,  60, 70, SIGNALi, SIG_OPEN, S25_ROLL},

	{M25_RUMBLE, 70, 71, CLICK},
	{M25_RUMBLE, 71, 70, LOADVIEW, 0, IDV_WALL2EN},

	//-----------------------------------------------------
	{M25_ROLL,	  0, 1, WAIT, 0, SIG_OPEN},
	{M25_ROLL,	  1, 2, VIDEO, 0, IDS_ROLL},
	{M25_ROLL,	  2, 3, ASSIGN, BFRAME, 7},
	{M25_ROLL,	  3, 4, SHOW, 0, IDS_ROLL},

	//-----------------------------------------------------
	{M25_SCATTER, 0, 40, DROP},
	{M25_SCATTER, 40, 41, SET_YOFFSET, ADD, 10},
	{M25_SCATTER, 41, 50, SET_XOFFSET, ADD, 50},
	{M25_SCATTER, 50, 51, VIDEO, 0, IDS_EXPLODE1},
	{M25_SCATTER, 51, 52, PLAYWAVE, 0, SOUND_EXPLODE},
	{M25_SCATTER, 52, 53, SHOW, 0, IDS_SCATTER},
	{M25_SCATTER, 53, 54, CLICK},
	{M25_SCATTER, 54, 53, LOADVIEW, 0, IDV_WALL1EN},

	//-----------------------------------------------------
	{M25_OPNDOOR,  0, 40, DROP},

	// ### check to see what kind of bomb was dropped
	{M25_OPNDOOR, 40, 50},

	{M25_OPNDOOR, 50, 51, VIDEO, 0, IDS_EXPLODE1},
	{M25_OPNDOOR, 51, 52, PLAYWAVE, 0, SOUND_EXPLODE},
	{M25_OPNDOOR, 52, 53, MOV, WSPRITE, WIP1},
	{M25_OPNDOOR, 53, 70, SHOW, WSPRITE},

	{M25_OPNDOOR, 70, 71, CLICK},
	{M25_OPNDOOR, 71, 70, LOADVIEW, WIP2},

	//------------------------------------------------------
	// Scene 26: Banishment
	{M26_BANISH, 0, 10, DRAG, 0, IDD_SCOOPF},
	{M26_BANISH, 0, 20, DRAGFOCUS, 0, TRUE},
	{M26_BANISH, 0, 30, CLICK},

	{M26_BANISH, 10, 20, HANDOFF, 0, IDD_SCOOPE},

	{M26_BANISH, 20, 0, PLAYWAVE,0, SOUND_SPIT},

	{M26_BANISH, 30, 70, EQUALi, LWEALTH, 0},
	{M26_BANISH, 30, 0},

	// go to the view of the scull:
	{M26_BANISH, 70, 0, LOADVIEW, WIP1},

	//------------------------------------------------------
	{M_SQUARE,	0, 1, DROP},
	{M_SQUARE,	0, 30, WAIT, 0, SIG_SET},
	{M_SQUARE,	1, 2, SHOW,	WOBJECT},
	{M_SQUARE,	2, 3, ANIMATE, 0, V_LOOP},
	{M_SQUARE,	3, 4, PLAYWAVE,	0, SOUND_CLICK},
	{M_SQUARE,	4, 40, MOV,	WPARM, WOBJECT},
	{M_SQUARE,	10, 20, DROP},
	{M_SQUARE,	10, 11, GRAB, WPARM},
	{M_SQUARE,	10, 11, WAIT, 0, SIG_CLEAR},
	{M_SQUARE,	11, 12, CLEAR, WOBJECT},
	{M_SQUARE,	12, 13, CLEAR, WPARM},
	{M_SQUARE,	13, 0, SHOW},

	{M_SQUARE,	20, 21, HANDOFF, WPARM},
	{M_SQUARE,	21, 1, CLEAR, WPARM},

	{M_SQUARE,  30, 1, MOV, WOBJECT, WTEMP1},

	// play reversi with the pieces ...
	{M_SQUARE,  40,47, NEQUALi,		WPARM, IDD_REVERSIA},
	{M_SQUARE,  40,10, ASSIGN,		WPARM, IDD_REVERSIB},
	{M_SQUARE,  47,10, NEQUALi,		WPARM, IDD_REVERSIB},
	{M_SQUARE,  47,10, ASSIGN,		WPARM, IDD_REVERSIA},

	// bins for white chess pieces
	{M_WROOK,	0, 0, GRAB,			0,	IDD_WROOK},
	{M_WROOK,	0, 0, DROP},
	{M_WKNIGHT,	0, 0, GRAB,			0,	IDD_WKNIGHT},
	{M_WKNIGHT,	0, 0, DROP},
	{M_WBISHOP,	0, 0, GRAB,			0,	IDD_WBISHOP},
	{M_WBISHOP,	0, 0, DROP},
	{M_WQUEEN,	0, 0, GRAB,			0,	IDD_WQUEEN},
	{M_WQUEEN,	0, 0, DROP},
	{M_WKING,	0, 0, GRAB,			0,	IDD_WKING},
	{M_WKING,	0, 0, DROP},
	{M_WPAWN,	0, 0, GRAB,			0,	IDD_WPAWN},
	{M_WPAWN,	0, 0, DROP},
	{M_WKINGME,	0, 0, GRAB,			0,	IDD_WKINGME},
	{M_WKINGME,	0, 0, DROP},

	// bins for black chess pieces
	{M_BROOK,	0, 0, GRAB,			0,	IDD_BROOK},
	{M_BROOK,	0, 0, DROP,			0,	0},
	{M_BKNIGHT,	0, 0, GRAB,			0,	IDD_BKNIGHT},
	{M_BKNIGHT,	0, 0, DROP,			0,	0},
	{M_BBISHOP,	0, 0, GRAB,			0,	IDD_BBISHOP},
	{M_BBISHOP,	0, 0, DROP,			0,	0},
	{M_BQUEEN,	0, 0, GRAB,			0,	IDD_BQUEEN},
	{M_BQUEEN,	0, 0, DROP,			0,	0},
	{M_BKING,	0, 0, GRAB,			0,	IDD_BKING},
	{M_BKING,	0, 0, DROP,			0,	0},
	{M_BPAWN,	0, 0, GRAB,			0,	IDD_BPAWN},
	{M_BPAWN,	0, 0, DROP,			0,	0},
	{M_BKINGME,	0, 0, GRAB,			0,	IDD_BKINGME},
	{M_BKINGME,	0, 0, DROP,			0,	0},

	{M_CLEAR,	0, 1, CLICK},
	{M_CLEAR,	0, 1, WAIT},
	{M_CLEAR,	1, 2, ASSIGN,		WPARM,	S00_SQUARE63},
	{M_CLEAR,	2, 3, SIGNAL,		WPARM,	SIG_CLEAR}, // clear
	{M_CLEAR,	3, 5, SUBI,			WPARM,	1},
	{M_CLEAR,	5, 2, GTEi,			WPARM,	S00_SQUARE00},
	{M_CLEAR,	5, 0, CLEAR,		WOBJECT},

	{M_CHESS,	0, 50, CLICK},
	{M_CHESS,	1, 2, ASSIGN,		WTEMP1,IDD_WPAWN},
	{M_CHESS,	2, 3, SIGNALi,		SIG_SET, S00_SQUARE00+8},
	{M_CHESS,	3, 4, SIGNALi,		SIG_SET, S00_SQUARE00+9},
	{M_CHESS,	4, 5, SIGNALi,		SIG_SET, S00_SQUARE00+10},
	{M_CHESS,	5, 6, SIGNALi,		SIG_SET, S00_SQUARE00+11},
	{M_CHESS,	6, 7, SIGNALi,		SIG_SET, S00_SQUARE00+12},
	{M_CHESS,	7, 8, SIGNALi,		SIG_SET, S00_SQUARE00+13},
	{M_CHESS,	8, 9, SIGNALi,		SIG_SET, S00_SQUARE00+14},
	{M_CHESS,	9, 10, SIGNALi,		SIG_SET, S00_SQUARE00+15},
	{M_CHESS,	10, 11, ASSIGN,		WTEMP1,IDD_WROOK},
	{M_CHESS,	11, 12, SIGNALi,		SIG_SET, S00_SQUARE00+0},
	{M_CHESS,	12, 13, SIGNALi,		SIG_SET, S00_SQUARE00+7},
	{M_CHESS,	13, 14, ASSIGN,		WTEMP1,IDD_WKNIGHT},
	{M_CHESS,	14, 15, SIGNALi,		SIG_SET, S00_SQUARE00+1},
	{M_CHESS,	15, 16, SIGNALi,		SIG_SET, S00_SQUARE00+6},
	{M_CHESS,	16, 17, ASSIGN,		WTEMP1,IDD_WBISHOP},
	{M_CHESS,	17, 18, SIGNALi,		SIG_SET, S00_SQUARE00+2},
	{M_CHESS,	18, 19, SIGNALi,		SIG_SET, S00_SQUARE00+5},
	{M_CHESS,	19, 20, ASSIGN,		WTEMP1,IDD_WQUEEN},
	{M_CHESS,	20, 21, SIGNALi,		SIG_SET, S00_SQUARE00+3},
	{M_CHESS,	21, 22, ASSIGN,		WTEMP1,IDD_WKING},
	{M_CHESS,	22, 24, SIGNALi,		SIG_SET, S00_SQUARE00+4},

	{M_CHESS,	24, 25, ASSIGN,		WTEMP1,IDD_BPAWN},
	{M_CHESS,	25, 26, SIGNALi,		SIG_SET, S00_SQUARE48},
	{M_CHESS,	26, 27, SIGNALi,		SIG_SET, S00_SQUARE48+1},
	{M_CHESS,	27, 28, SIGNALi,		SIG_SET, S00_SQUARE48+2},
	{M_CHESS,	28, 29, SIGNALi,		SIG_SET, S00_SQUARE48+3},
	{M_CHESS,	29, 30, SIGNALi,		SIG_SET, S00_SQUARE48+4},
	{M_CHESS,	30, 31, SIGNALi,		SIG_SET, S00_SQUARE48+5},
	{M_CHESS,	31, 32, SIGNALi,		SIG_SET, S00_SQUARE48+6},
	{M_CHESS,	32, 33, SIGNALi,		SIG_SET, S00_SQUARE48+7},
	{M_CHESS,	33, 34, ASSIGN,		WTEMP1,IDD_BROOK},
	{M_CHESS,	34, 35, SIGNALi,		SIG_SET, S00_SQUARE56+0},
	{M_CHESS,	35, 36, SIGNALi,		SIG_SET, S00_SQUARE56+7},
	{M_CHESS,	36, 37, ASSIGN,		WTEMP1,IDD_BKNIGHT},
	{M_CHESS,	37, 38, SIGNALi,		SIG_SET, S00_SQUARE56+1},
	{M_CHESS,	38, 39, SIGNALi,		SIG_SET, S00_SQUARE56+6},
	{M_CHESS,	39, 40, ASSIGN,		WTEMP1,IDD_BBISHOP},
	{M_CHESS,	40, 41, SIGNALi,		SIG_SET, S00_SQUARE56+2},
	{M_CHESS,	41, 42, SIGNALi,		SIG_SET, S00_SQUARE56+5},
	{M_CHESS,	42, 43, ASSIGN,		WTEMP1,IDD_BQUEEN},
	{M_CHESS,	43, 44, SIGNALi,		SIG_SET, S00_SQUARE56+3},
	{M_CHESS,	44, 45, ASSIGN,		WTEMP1,IDD_BKING},
	{M_CHESS,	45, 46, SIGNALi,		SIG_SET, S00_SQUARE56+4},
	{M_CHESS,	46,  0, CLEAR,		WTEMP1},
	{M_CHESS,	50, 1, SIGNALi,		SIG_NULL,	S00_CLEAR},
};

TBL_MACHINE_DEF btbl_machine_def(tbl_machine_def, sizeof(tbl_machine_def)/sizeof(TRANSITION));

static MACHINE tbl_machines[] = {
	// m_id,    view-id       def-id  left, top right, bottom REGISTERS
	{NAME(SMP_VIAL),		IDV_MAIN_PANEL,	M_VIAL,			{170,  16, 210,  77}, REG_INIT, LV_INTERNAL, WIPS },
	{NAME(SMP_BACKBUTTON),	IDV_MAIN_PANEL,	M_BACKBUTTON,	{377,75,377+20,75+20}, REG_INIT, LV_INTERNAL, WIPS },
	{NAME(SMP_MAPBUTTON),	IDV_MAIN_PANEL,	M_MAPBUTTON,	{404,75,404+20,75+20}, REG_INIT, LV_INTERNAL, WIPS },
	{NAME(SMP_COMPASS),		IDV_MAIN_PANEL,	M_COMPASS,		{259,  93, 370, 131}, REG_INIT, L_INTERNAL, WIPS },
	{NAME(SMP_INV_LEFT),	IDV_MAIN_PANEL,	M_INV_LEFT,		{  1, 379,  49, 410}, REG_INIT, LV_INTERNAL, WIPS },
	{NAME(SMP_INV_RIGHT),	IDV_MAIN_PANEL,	M_INV_RIGHT,	{ 99, 379, 148, 410}, REG_INIT, LV_INTERNAL, WIPS },
	{NAME(SMP_TEXT_UP),		IDV_MAIN_PANEL,	M_TEXT_UP,		{624, 431, 639, 455}, REG_INIT, LV_INTERNAL, WIPS },
	{NAME(SMP_TEXT_DOWN),	IDV_MAIN_PANEL,	M_TEXT_DOWN,	{624, 456, 639, 479}, REG_INIT, LV_INTERNAL, WIPS },
	{NAME(SMP_LISTEN1),		IDV_MAIN_PANEL,	M_LISTEN,		{480, 380, 515, 406}, REG_INIT, LV_INTERNAL, {0,4,0}},
	{NAME(SMP_LISTEN2),		IDV_MAIN_PANEL,	M_LISTEN,		{515, 380, 550, 406}, REG_INIT, LV_INTERNAL, {1,5,0}},
	{NAME(SMP_LISTEN3),		IDV_MAIN_PANEL,	M_LISTEN,		{550, 380, 585, 406}, REG_INIT, LV_INTERNAL, {2,6,0}},
	{NAME(SMP_LISTEN4),		IDV_MAIN_PANEL,	M_LISTEN,		{585, 380, 626, 406}, REG_INIT, LV_INTERNAL, {3,7,0}},
	{NAME(SMP_TALK1),		IDV_MAIN_PANEL,	M_TALK,			{480, 406, 515, 430}, REG_SHOW(IDS_REDDOT, TRUE), LV_INTERNAL, {SMP_TALK2,SMP_TALK3,SMP_TALK4}},
	{NAME(SMP_TALK2),		IDV_MAIN_PANEL,	M_TALK,			{515, 406, 550, 430}, REG_INIT, LV_INTERNAL, {SMP_TALK1,SMP_TALK3,SMP_TALK4}},
	{NAME(SMP_TALK3),		IDV_MAIN_PANEL,	M_TALK,			{550, 406, 585, 430}, REG_INIT, LV_INTERNAL, {SMP_TALK1,SMP_TALK2,SMP_TALK4}},
	{NAME(SMP_TALK4),		IDV_MAIN_PANEL,	M_TALK,			{585, 406, 626, 430}, REG_INIT, LV_INTERNAL, {SMP_TALK1,SMP_TALK2,SMP_TALK3}},

	{NAME(THE_BARD),	IDV_ID,	M_BARD,  {0, 0, 0, 0}, REG_INIT, INTERNAL, WIPS },
	{NAME(THE_EARTH),	IDV_ID,	M_EARTH, {0, 0, 0, 0}, REG_INIT, INTERNAL, WIPS },
	{NAME(THE_FIRE),	IDV_ID,	M_FIRE,  {0, 0, 0, 0}, REG_INIT, INTERNAL, WIPS },
	{NAME(THE_WIND),	IDV_ID,	M_WIND,  {0, 0, 0, 0}, REG_INIT, INTERNAL, WIPS },
	{NAME(THE_WATER),	IDV_ID,	M_WATER, {0, 0, 0, 0}, REG_INIT, INTERNAL, WIPS },
	{NAME(THE_FISH),	IDV_ID,	M_FISH,  {0, 0, 0, 0}, REG_INIT, INTERNAL, WIPS },
	{NAME(THE_BIRDS),	IDV_ID,	M_BIRDS, {0, 0, 0, 0}, REG_INIT, INTERNAL, WIPS },

	// id stuff goes in here
	//
	{NAME(SID_HALO),	IDV_ID, M_HALO,		{30, 0, 70, 40},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_SPELL),	IDV_ID, M_IDSPELL,	{10, 50, 80, 150},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_ID),		IDV_ID, M_ID,		{0,  0, 101, 171},	REG_INIT, LV_INTERNAL, {LWISDOM, LSEX,0,SID_AURA}},
	{NAME(SID_PED),		IDV_ID, M_PED,		{0,129, 99, 173},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_AURA),	IDV_ID, M_AURA,		{0,  0, 112, 100},	REG_INIT, LV_INTERNAL, WIPS},

	{NAME(SID_DEC_WISDOM),	IDV_ID,	M_DEC_WISDOM,	{0, 0, 0, 0},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_INC_WISDOM),	IDV_ID,	M_INC_WISDOM,	{0, 0, 0, 0},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_DEC_STRENGTH),	IDV_ID,	M_DEC_STRENGTH,	{0, 0, 0, 0},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_INC_STRENGTH),	IDV_ID,	M_INC_STRENGTH,	{0, 0, 0, 0},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_DEC_ENERGY),	IDV_ID,	M_DEC_ENERGY,	{0, 0, 0, 0},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_INC_ENERGY),	IDV_ID,	M_INC_ENERGY,	{0, 0, 0, 0},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_DEC_KARMA),		IDV_ID,	M_DEC_KARMA,	{0, 0, 0, 0},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_INC_KARMA),		IDV_ID,	M_INC_KARMA,	{0, 0, 0, 0},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_DEC_WEALTH),	IDV_ID,	M_DEC_WEALTH,	{0, 0, 0, 0},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SID_INC_WEALTH),	IDV_ID,	M_INC_WEALTH,	{0, 0, 0, 0},	REG_INIT, LV_INTERNAL, WIPS},

	{NAME(SOD_SPELL),	IDV_OTHERID, M_O_IDSPELL,	{10, 50, 80, 150},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SOD_ID),		IDV_OTHERID, M_ID,			{0, 0, 101, 171},	REG_INIT, LV_INTERNAL, {OWISDOM, OSEX,0,SOD_AURA}},
	{NAME(SOD_AURA),	IDV_OTHERID, M_O_AURA,		{0, 0, 112, 100},	REG_INIT, LV_INTERNAL, WIPS},
	{NAME(SOD_PED),		IDV_OTHERID, M_O_PED,		{0,129, 99, 173},	REG_INIT, LV_INTERNAL, WIPS},

#define TOPMENU_BTN(ID, X, Y, VIEW_ID, BTN_IMAGE)\
	{NAME(ID),IDV_TOPMENU, M_BTN_1phase,	{X, Y, X+128, Y+55},REG_INIT, L_INTERNAL, {VIEW_ID, BTN_IMAGE}}

	// to quit, simply load a NULL view!
	TOPMENU_BTN(S00_BTN_QUIT, 390, 216, 0, IDS_BTN_QUIT),
	TOPMENU_BTN(S00_BTN_CREDITS, 793, 221, IDV_CREDITS, IDS_BTN_CREDITS),
	TOPMENU_BTN(S00_BTN_AUDIO, 1438, 224, IDV_CFGAV, IDS_BTN_AUDIO),
	TOPMENU_BTN(S00_BTN_PLAYER, 1916, 222, IDV_CFGPLAYER1, IDS_BTN_PLAYER),
	TOPMENU_BTN(S00_BTN_GAME, 2216, 219, IDV_CFGGAME, IDS_BTN_GAME),
	TOPMENU_BTN(S00_BTN_NETWORK, 2568, 216, IDV_CFGPROV, IDS_BTN_NETWORK),
	TOPMENU_BTN(S00_BTN_HELP, 3000, 215, IDV_HELP, IDS_BTN_HELP),

#define _BTN(ID, VIEW_ID, X, Y, TARGET_VIEW_ID, BTN_IMAGE, BTN_IMAGE_HI)\
	{NAME(ID),VIEW_ID, M_BTN_2phase,	{X, Y, X+80, Y+48},REG_SHOW(BTN_IMAGE,FALSE), L_INTERNAL, {TARGET_VIEW_ID, BTN_IMAGE_HI, BTN_IMAGE}}

	//----------
	// Scene 2:
#define ID(UNIT) ((S02_UNIT__01) + (UNIT) * (S02_MACHINES_PER_UNIT))

#define S02_VENDING_MACHINE(UNIT)	\
	{NAME(ID(UNIT)+S02_VIAL__XX), IDV_LOCKERCU+UNIT, M02_VIAL, {214, 92, 318, 124}, REG_INIT, INTERNAL, {ID(UNIT)+S02_NUM1__XX, ID(UNIT)+S02_LOCK__XX, ID(UNIT)+S02_NUM10_XX}},	\
	{NAME(ID(UNIT)+S02_LOCK__XX), IDV_LOCKERCU+UNIT, M02_LOCK, {44, 165, 137, 243}, REG_INIT, INTERNAL, WIPS},															\
	{NAME(ID(UNIT)+S02_BIN___XX), IDV_LOCKERCU+UNIT, M02_BIN,  {20,  20, 200, 140}, REG_INIT, INTERNAL, {S02_EBIN__00+UNIT, 0, 0}},									\
	{NAME(ID(UNIT)+S02_UP1___XX), IDV_LOCKERCU+UNIT, M02_UP1,  {264,  7, 289,  29}, REG_INIT, INTERNAL, {ID(UNIT)+S02_VIAL__XX, ID(UNIT)+S02_LOCK__XX, 0}},					\
	{NAME(ID(UNIT)+S02_UP10__XX), IDV_LOCKERCU+UNIT, M02_UP10, {230,  8, 257,  29}, REG_INIT, INTERNAL, {ID(UNIT)+S02_VIAL__XX, ID(UNIT)+S02_LOCK__XX, 0}},					\
	{NAME(ID(UNIT)+S02_DN1___XX), IDV_LOCKERCU+UNIT, M02_DN1,  {262, 71, 285,  89}, REG_INIT, INTERNAL, {ID(UNIT)+S02_VIAL__XX, ID(UNIT)+S02_LOCK__XX, 0}},					\
	{NAME(ID(UNIT)+S02_DN10__XX), IDV_LOCKERCU+UNIT, M02_DN10, {231, 72, 257,  89}, REG_INIT, INTERNAL, {ID(UNIT)+S02_VIAL__XX, ID(UNIT)+S02_LOCK__XX, 0}},					\
	{NAME(ID(UNIT)+S02_NUM1__XX), IDV_LOCKERCU+UNIT, M02_NUM1, {263, 39, 286,  62}, REG_INIT, INTERNAL, {ID(UNIT)+S02_VIAL__XX, ID(UNIT)+S02_LOCK__XX, 0}},					\
	{NAME(ID(UNIT)+S02_NUM10_XX), IDV_LOCKERCU+UNIT, M02_NUM10,{232, 40, 255,  63}, REG_INIT, INTERNAL, {ID(UNIT)+S02_VIAL__XX, ID(UNIT)+S02_LOCK__XX, 0}}

	S02_VENDING_MACHINE(0),
	S02_VENDING_MACHINE(1),
	S02_VENDING_MACHINE(2),
	S02_VENDING_MACHINE(3),
	S02_VENDING_MACHINE(4),
	S02_VENDING_MACHINE(5),
	S02_VENDING_MACHINE(6),
	S02_VENDING_MACHINE(7),
	S02_VENDING_MACHINE(8),

  	S02_VENDING_MACHINE(9),
	S02_VENDING_MACHINE(10),
	S02_VENDING_MACHINE(11),
	S02_VENDING_MACHINE(12),
	S02_VENDING_MACHINE(13),
	S02_VENDING_MACHINE(14),
	S02_VENDING_MACHINE(15),
	S02_VENDING_MACHINE(16),
	S02_VENDING_MACHINE(17),

	S02_VENDING_MACHINE(18),
	S02_VENDING_MACHINE(19),
	S02_VENDING_MACHINE(20),
	S02_VENDING_MACHINE(21),
	S02_VENDING_MACHINE(22),
	S02_VENDING_MACHINE(23),
	S02_VENDING_MACHINE(24),
	S02_VENDING_MACHINE(25),
	S02_VENDING_MACHINE(26),

	//----------------------------
	// Scene 4:
	{NAME(S04_KEYCLAMP_1),  IDV_ckeycf01, M04_KEYCLAMP,	{222, 91, 342, 178},	REG_INIT, INTERNAL, {S04_REDSLIDER_1,S04_GRNSLIDER_1,S04_BLUSLIDER_1}},
	{NAME(S04_REDSLIDER_1), IDV_ckeycf01, M04_SLIDER,	{78,  103, 100, 188},	REG_SHOW(IDS_RED_SLIDE,FALSE), INTERNAL},
	{NAME(S04_GRNSLIDER_1), IDV_ckeycf01, M04_SLIDER,	{105, 103, 127, 188},	REG_SHOW(IDS_GRN_SLIDE,FALSE), INTERNAL},
	{NAME(S04_BLUSLIDER_1), IDV_ckeycf01, M04_SLIDER,	{129, 103, 151, 188},	REG_SHOW(IDS_BLU_SLIDE,FALSE), INTERNAL},

	{NAME(S04_KEYCLAMP_2),  IDV_ckeycf02, M04_KEYCLAMP,	{222, 91, 342, 178},	REG_INIT, INTERNAL, {S04_REDSLIDER_2,S04_GRNSLIDER_2,S04_BLUSLIDER_2}},
	{NAME(S04_REDSLIDER_2), IDV_ckeycf02, M04_SLIDER,	{78,  103, 100, 188},	REG_SHOW(IDS_RED_SLIDE,FALSE), INTERNAL},
	{NAME(S04_GRNSLIDER_2), IDV_ckeycf02, M04_SLIDER,	{105, 103, 127, 188},	REG_SHOW(IDS_GRN_SLIDE,FALSE), INTERNAL},
	{NAME(S04_BLUSLIDER_2), IDV_ckeycf02, M04_SLIDER,	{129, 103, 151, 188},	REG_SHOW(IDS_BLU_SLIDE,FALSE), INTERNAL},

	{NAME(S04_KEYCLAMP_3),  IDV_ckeycf03, M04_KEYCLAMP,	{222, 91, 342, 178},	REG_INIT, INTERNAL, {S04_REDSLIDER_3,S04_GRNSLIDER_3,S04_BLUSLIDER_3}},
	{NAME(S04_REDSLIDER_3), IDV_ckeycf03, M04_SLIDER,	{78,  103, 100, 188},	REG_SHOW(IDS_RED_SLIDE,FALSE), INTERNAL},
	{NAME(S04_GRNSLIDER_3), IDV_ckeycf03, M04_SLIDER,	{105, 103, 127, 188},	REG_SHOW(IDS_GRN_SLIDE,FALSE), INTERNAL},
	{NAME(S04_BLUSLIDER_3), IDV_ckeycf03, M04_SLIDER,	{129, 103, 151, 188},	REG_SHOW(IDS_BLU_SLIDE,FALSE), INTERNAL},

	{NAME(S04_VIAL),		IDV_ckeyvend, M04_VIAL,		{130, 130, 200, 200},	REG_INIT, INTERNAL, WIPS},
	{NAME(S04_BIN),		IDV_ckeyvend, M04_BIN,		{121, 212, 223, 285},	REG_INIT, INTERNAL, WIPS},

	//----------
	// Scene 5:
	{NAME(S05_UP),		IDV_chempan, M05_UP,	{7,126,28,153}, REG_INIT, INTERNAL, {S05_ICON}},
	{NAME(S05_DOWN),	IDV_chempan, M05_DOWN,	{7,161,28,191}, REG_INIT, INTERNAL, {S05_ICON}},
	{NAME(S05_ICON),	IDV_chempan, M05_ICON,	{75, 125, 125, 175}, REG_INIT, INTERNAL, {S05_VIAL, S05_NUM1, S05_NUM10}},
	{NAME(S05_NUM1),	IDV_chempan, M05_NUM1,	{234, 96, 258, 120}, REG_INIT, INTERNAL, {S05_VIAL}},
	{NAME(S05_NUM10),	IDV_chempan, M05_NUM10,	{206, 99, 228, 123}, REG_INIT, INTERNAL, {S05_VIAL}},
	{NAME(S05_VIAL),	IDV_chempan, M05_VIAL,	{161,131,221,172    }, REG_INIT, INTERNAL, {S05_CHEM, S05_BUBBLE, S05_ICON}},
	{NAME(S05_CHEM),	IDV_chempan, M05_CHEM,	{283,76,386,238}, REG_INIT, INTERNAL, {S05_ICON}},
	{NAME(S05_BUBBLE),	IDV_chempan, M05_BUBBLE,  {63,0,72,76       }, REG_INIT, INTERNAL},

	//----------
	// Scene 6: Monitor Alley ...
#define S06_MONITOR(INSTANCE_ID, VIEW_ID) \
	{NAME(INSTANCE_ID*5+S06_TRI1), VIEW_ID, M06_TRI1, {168,91,232,155}, REG_INIT, INTERNAL, {INSTANCE_ID*5+S06_TRI2}}, \
	{NAME(INSTANCE_ID*5+S06_TRI2), VIEW_ID, M06_TRI2, {128, 163, 175, 221}, REG_INIT, INTERNAL, {INSTANCE_ID*5+S06_TRI3}}, \
	{NAME(INSTANCE_ID*5+S06_TRI3), VIEW_ID, M06_TRI3, {179, 163, 227, 221}, REG_INIT, INTERNAL, {INSTANCE_ID*5+S06_TRI4}}, \
	{NAME(INSTANCE_ID*5+S06_TRI4), VIEW_ID, M06_TRI4, {230, 163, 278, 221}, REG_INIT, INTERNAL}, \
	{NAME(INSTANCE_ID*5+S06_TEMPHOLD), VIEW_ID, M06_TEMPHOLD, {179,251,220,292}, REG_ACCEPT(TRUE, IDC_TEMPLATE), INTERNAL, {INSTANCE_ID*5+S06_TRI1}}

	S06_MONITOR(0,IDV_CLIBMON1),
	S06_MONITOR(1,IDV_CLIBMON2),
	S06_MONITOR(2,IDV_CLIBMON3),
	S06_MONITOR(3,IDV_CLIBMON4),
	S06_MONITOR(4,IDV_CLIBMON5),
	S06_MONITOR(5,IDV_CLIBMON6),

	// Scene 6 continued: Pez hell ...
#define TEMPLATE(G,X)\
	{NAME(S06_PEZPOP+G*5+X),IDV_CTYPEZ1+G, M06_PEZPOP,  {11+X*77, 5, 84+X*77, 80}, REG_INIT, INTERNAL, {IDD_TEMPLATE01+G*5+X}}
#define TEMPLATE_GROUP(G) \
	TEMPLATE(G,0),\
	TEMPLATE(G,1),\
	TEMPLATE(G,2),\
	TEMPLATE(G,3),\
	TEMPLATE(G,4)

	TEMPLATE_GROUP(0),
	TEMPLATE_GROUP(1),
	TEMPLATE_GROUP(2),
	TEMPLATE_GROUP(3),
	TEMPLATE_GROUP(4),
	TEMPLATE_GROUP(5),
	TEMPLATE_GROUP(6),

	//----------
	// Scene 8 Manufacturing Alley ....
#define S08_MANREP(INST_ID, VIEW_ID) \
	{NAME(INST_ID*INSTS_PER_MANREP+S08_TEMPHOLD), VIEW_ID, M08_TEMPHOLD, {188,174,230,215}, REG_ACCEPT(TRUE, IDC_TEMPLATE), INTERNAL, {INST_ID*INSTS_PER_MANREP+S08_HOPPER}},\
	{NAME(INST_ID*INSTS_PER_MANREP+S08_HOPPER), VIEW_ID, M08_HOPPER, {167,247,255,300}, REG_INIT, INTERNAL, {INST_ID*INSTS_PER_MANREP+S08_TEMPHOLD,INST_ID*INSTS_PER_MANREP+S08_BIN,INST_ID*INSTS_PER_MANREP+S08_BEEM}}, \
	{NAME(INST_ID*INSTS_PER_MANREP+S08_BEEM), VIEW_ID, M08_BEEM, {155,23,160,28}, REG_INIT, INTERNAL}, \
	{NAME(INST_ID*INSTS_PER_MANREP+S08_BIN), VIEW_ID, M08_BIN, {180,80,236,136}, REG_INIT, INTERNAL}

	S08_MANREP(0,IDV_MANREP1),
	S08_MANREP(1,IDV_MANREP2),
	S08_MANREP(2,IDV_MANREP3),
	S08_MANREP(3,IDV_MANREP4),
	S08_MANREP(4,IDV_MANREP5),
	S08_MANREP(5,IDV_MANREP6),
	S08_MANREP(6,IDV_MANREP7),
	S08_MANREP(7,IDV_MANREP8),
	S08_MANREP(8,IDV_MANREP9),
	S08_MANREP(9,IDV_MANREP10),

	//----------
	// Scene 10:
	{NAME(S10_DRYPIT), IDV_SCN10PIT, M10_DRYPIT,   {30, 200, 379,  250}, REG_INIT, INTERNAL, WIPS},
	{NAME(S10_FLOWER), IDV_SCN10PIT, M10_FLOWER,   {139, 31, 273,  127}, REG_INIT, INTERNAL, WIPS},
	{NAME(S10_FOPEN), IDV_SCN10PIT, M10_FOPEN,   {139, 31, 273,  127}, REG_INIT, INTERNAL, WIPS},
	{NAME(S10_FCLOSE), IDV_SCN10PIT, M10_FCLOSE,   {139, 31, 273,  127}, REG_INIT, INTERNAL, WIPS},

	//----------
	// Scene 11:
	{NAME(S11_FIRE),  IDV_FA1PAN, M11_FIRE,   {785, 210, 890,  250}, REG_INIT, INTERNAL, WIPS},
	{NAME(S11_GRILL), IDV_FA1PAN, M11_GRILL,  {774, 163, 890,  200}, REG_ACCEPT(TRUE, IDC_FISH), INTERNAL, {S11_FIRE, S11_ASCENT} },
	{NAME(S11_ASCENT),  IDV_FA1PAN, M11_ASCENT,   {774, 63, 890,  140}, REG_INIT, INTERNAL, WIPS},

	{NAME(S11_LOGBINA), IDV_FH1PTH2, M_OBJECTBIN,  {1200, 37, 2022, 255}, REG_INIT, L_INTERNAL, IDD_LOG },

	{NAME(S11_BAITSTATION), IDV_FH1PAN, M_BAITSTATION,  {2985, 64, 3200, 300}, REG_INIT, INTERNAL, WIPS },
	{NAME(S11_POLESTANDa), IDV_FH1PAN, M_POLESTAND,  {2700,120, 2750, 300}, REG_INIT, INTERNAL, WIPS },
	{NAME(S11_POLESTANDb), IDV_FH1PAN, M_POLESTAND,  {2750,120, 2800, 300}, REG_INIT, INTERNAL, WIPS },
	{NAME(S11_POLESTANDc), IDV_FH1PAN, M_POLESTAND,  {2800,120, 2850, 300}, REG_INIT, INTERNAL, WIPS },
	{NAME(S11_POLESTANDd), IDV_FH1PAN, M_POLESTAND,  {2850,120, 2900, 300}, REG_INIT, INTERNAL },
	{NAME(S11_FISHSTATIONa), IDV_FH1PAN, M_FISHSTATION,  {600,  110, 700, 300}, REG_INIT, INTERNAL, WIPS },
	{NAME(S11_FISHSTATIONb), IDV_FH1PAN, M_FISHSTATION,  {700,  110, 800, 300}, REG_INIT, INTERNAL, WIPS },
	{NAME(S11_FISHSTATIONc), IDV_FH1PAN, M_FISHSTATION,  {800,  110, 900, 300}, REG_INIT, INTERNAL, WIPS },
	{NAME(S11_FISHSTATIONd), IDV_FH1PAN, M_FISHSTATION,  {900,  110, 1000, 300}, REG_INIT, INTERNAL , WIPS},
	{NAME(S11_FISHSTATIONe), IDV_FH1PAN, M_FISHSTATION,  {1000, 110, 1100, 300}, REG_INIT, INTERNAL, WIPS },
	{NAME(S11_FISHSTATIONf), IDV_FH1PAN, M_FISHSTATION,  {1100, 110, 1200, 300}, REG_INIT, INTERNAL , WIPS},
	{NAME(S11_FISHSTATIONg), IDV_FH1PAN, M_FISHSTATION,  {1200, 110, 1300, 300}, REG_INIT, INTERNAL, WIPS },
	{NAME(S11_FISHSTATIONh), IDV_FH1PAN, M_FISHSTATION,  {1300, 110, 1400, 300}, REG_INIT, INTERNAL, WIPS},

	{NAME(S11_GRUBBIN0), IDV_GRBDRT2, M_OBJECTBIN,  {20,8,262,218}, REG_INIT, L_INTERNAL, IDD_BAIT0},
	//{NAME(S11_BUCKETS), IDV_V3PAN, M_OBJECTBIN,  {2170,220,2240,290}, REG_INIT, L_INTERNAL, IDD_BUCKF},

	{NAME(S11_LOGBINB), IDV_PATH1_7, M_OBJECTBIN,  {2833, 168, 3126, 265}, REG_INIT, L_INTERNAL, IDD_LOG },

	//------------------------------------
	// Scene 12: Spells & incantations in the temple...
	{NAME(S12_TCANDLE1), IDV_TM1PAN, M_OBJECTMAP,  {620, 5, 690, 80}, REG_INIT, L_INTERNAL, {IDD_LOG,IDD_MATCH, SOUND_FIRE}},
	{NAME(S12_TCANDLE2), IDV_TM1PAN, M_OBJECTMAP,  {950, 5, 1020, 80}, REG_INIT, L_INTERNAL, {IDD_LOG,IDD_MATCH, SOUND_FIRE}},
	{NAME(S12_TCANDLE3), IDV_TM1PAN, M_OBJECTMAP,  {2070, 5, 2140, 80}, REG_INIT, L_INTERNAL, {IDD_LOG,IDD_MATCH, SOUND_FIRE}},
	{NAME(S12_TCANDLE4), IDV_TM1PAN, M_OBJECTMAP,  {2410, 5, 2480, 80}, REG_INIT, L_INTERNAL, {IDD_LOG,IDD_MATCH, SOUND_FIRE}},

	{NAME(S12_SPRING1), IDV_VESTPAN1, M12_SPRING, {504,131,539,170}, REG_ACCEPT(FALSE, IDD_SPRINGS), INTERNAL},
	{NAME(S12_WATER1), IDV_VESTPAN1, M12_WATER,  {200, 95, 431, 242},REG_INIT,INTERNAL, {S12_CANDLE1}},
	{NAME(S12_CANDLE1), IDV_VESTPAN1, M12_CANDLE, {426,47,554,200}, REG_INIT, INTERNAL, {S12_WATER1,S12_ASHSHELF1}},
	{NAME(S12_ASHSHELF1), IDV_VESTPAN1, M12_ASHSHELF, {95,140,160,210}, REG_ACCEPT(TRUE, IDC_FISHASH), INTERNAL, {S12_CANDLE1,S12_WATER1, S12_SPRING1}},

	{NAME(S12_SPRING2), IDV_VESTPAN2, M12_SPRING, {504,131,539,170}, REG_ACCEPT(FALSE, IDD_SPRINGS), INTERNAL},
	{NAME(S12_WATER2), IDV_VESTPAN2, M12_WATER,  {200, 95, 431, 242},REG_INIT,INTERNAL, {S12_CANDLE2}},
	{NAME(S12_CANDLE2), IDV_VESTPAN2, M12_CANDLE, {426,47,604,241}, REG_INIT, INTERNAL, {S12_WATER2,S12_ASHSHELF2}},
	{NAME(S12_ASHSHELF2), IDV_VESTPAN2, M12_ASHSHELF, {95,140,160,210}, REG_ACCEPT(TRUE, IDC_FISHASH), INTERNAL, {S12_CANDLE2,S12_WATER2, S12_SPRING2}},

	{NAME(S12_aCANDLE), IDV_TMCU1, M12_xCANDLE, {262,59,299,113}, REG_INIT, INTERNAL, {IDS_CAN1}},
	{NAME(S12_aSCROLL), IDV_TMCU1, M12_xSCROLL, {132,201, 250,271}, REG_INIT, INTERNAL},
	{NAME(S12_aPLANT1), IDV_TMCU1, M12_xPLANT,  {65,80,115,130}, REG_INIT, INTERNAL, {SCROLL_PLANT1,PLANT_FILMSTRIP_MAPPING, IDS_PLANTXX, S12_aCANDLE}},
	{NAME(S12_aPLANT2), IDV_TMCU1, M12_xPLANT,  {115,80,165,130}, REG_INIT, INTERNAL, {SCROLL_PLANT2,PLANT_FILMSTRIP_MAPPING, IDS_PLANTXX, S12_aCANDLE}},
	{NAME(S12_aPLANT3), IDV_TMCU1, M12_xPLANT,  {165,80,215,130}, REG_INIT, INTERNAL, {SCROLL_PLANT3,PLANT_FILMSTRIP_MAPPING, IDS_PLANTXX, S12_aCANDLE}},
	{NAME(S12_aFISH1),  IDV_TMCU1, M12_xPLANT,  {65,130,115,180}, REG_INIT, INTERNAL, {SCROLL_FISH1,FISHASH_FILMSTRIP_MAPPING, IDS_FISHXX, S12_aCANDLE}},
	{NAME(S12_aFISH2),  IDV_TMCU1, M12_xPLANT,  {115,130,165,180}, REG_INIT, INTERNAL, {SCROLL_FISH2,FISHASH_FILMSTRIP_MAPPING, IDS_FISHXX, S12_aCANDLE}},
	{NAME(S12_aFISH3),  IDV_TMCU1, M12_xPLANT,  {165,130,215,180}, REG_INIT, INTERNAL, {SCROLL_FISH3,FISHASH_FILMSTRIP_MAPPING, IDS_FISHXX, S12_aCANDLE}},

#define SCROLL_BIN(S12_SCROLLXX, IDD, X) \
	{NAME(S12_SCROLLXX), IDV_TMCUT, M_OBJECTBIN,  {X,100,X+30,187}, REG_INIT, L_INTERNAL, {IDD}}
	SCROLL_BIN(S12_SCROLL01, IDD_SCR001, 30),
	SCROLL_BIN(S12_SCROLL02, IDD_SCR002, 60),
	SCROLL_BIN(S12_SCROLL03, IDD_SCR003, 90),
	SCROLL_BIN(S12_SCROLL04, IDD_SCR004, 130),
	SCROLL_BIN(S12_SCROLL05, IDD_SCR005, 160),
	SCROLL_BIN(S12_SCROLL06, IDD_SCR006, 190),
	SCROLL_BIN(S12_SCROLL07, IDD_SCR007, 260),
	SCROLL_BIN(S12_SCROLL08, IDD_SCR008, 330),
	SCROLL_BIN(S12_SCROLL09, IDD_SCR009, 366),
	SCROLL_BIN(S12_SCROLL10, IDD_SCR010, 400),
	SCROLL_BIN(S12_SCROLL11, IDD_SCR011, 435),
	SCROLL_BIN(S12_SCROLL12, IDD_SCR012, 500),
	SCROLL_BIN(S12_SCROLL13, IDD_SCR013, 570),

	//------------------------------------
	// Scene 17: The MINES are the Quarry:
	{NAME(S17_aLOCKSOCKET),	IDV_aMINEPAN, M17_LOCKSOCKET,{177,116,232,163}, REG_ACCEPT(TRUE, ISA_LOCKDROP), INTERNAL, {S17_aDOORWAY, S17_aLOCKBURN}},
	{NAME(S17_aLOCKBURN),	IDV_aMINEPAN, M17_LOCKBURN,	 {128,20,128,20}, REG_INIT, INTERNAL},
	{NAME(S17_aDOORWAY),	IDV_aMINEPAN, M17_DOORWAY,	 {280,75,480,274}, REG_INIT, INTERNAL, {IDV_aCV1aPAN, S17_aFFIELD1}},
	{NAME(S17_aFFIELD1),	IDV_aMINEPAN, M17_FFIELD,	 {280,82,280,82}, REG_INIT, INTERNAL, {S17_aFFIELD2}},
	{NAME(S17_aFFIELD2),	IDV_aMINEPAN, M17_FFIELD,	 {280,156,280,156}, REG_INIT, INTERNAL, {S17_aFFIELD3}},
	{NAME(S17_aFFIELD3),	IDV_aMINEPAN, M17_FFIELD,	 {280,223,280,223}, REG_INIT, INTERNAL},
	{NAME(S17_aMINE),		IDV_aCV1bPAN, M17_MINE,		 {2900, 200, 3200, 300}, REG_INIT, INTERNAL},
	{NAME(S17_aFOULWIND),	IDV_aCV1bPAN, M17_FOULWIND,	 {0, 0, 0, 0}, REG_INIT, INTERNAL, {IDV_aCV1bPAN}},

	{NAME(S17_bLOCKSOCKET),	IDV_bMINEPAN, M17_LOCKSOCKET,{177,116,232,163}, REG_ACCEPT(TRUE, ISA_LOCKDROP), INTERNAL, {S17_bDOORWAY, S17_bLOCKBURN}},
	{NAME(S17_bLOCKBURN),	IDV_bMINEPAN, M17_LOCKBURN,	 {128,20,128,20}, REG_INIT, INTERNAL},
	{NAME(S17_bDOORWAY),	IDV_bMINEPAN, M17_DOORWAY,	 {280,75,480,274}, REG_INIT, INTERNAL, {IDV_bCV1aPAN, S17_bFFIELD1}},
	{NAME(S17_bFFIELD1),	IDV_bMINEPAN, M17_FFIELD,	 {280,82,280,82}, REG_INIT, INTERNAL, {S17_bFFIELD2}},
	{NAME(S17_bFFIELD2),	IDV_bMINEPAN, M17_FFIELD,	 {280,156,280,156}, REG_INIT, INTERNAL, {S17_bFFIELD3}},
	{NAME(S17_bFFIELD3),	IDV_bMINEPAN, M17_FFIELD,	 {280,223,280,223}, REG_INIT, INTERNAL},
	{NAME(S17_bMINE),		IDV_bCV1bPAN, M17_MINE,		 {2900, 200, 3200, 300}, REG_INIT, INTERNAL},
	{NAME(S17_bFOULWIND),	IDV_bCV1bPAN, M17_FOULWIND,	 {0, 0, 0, 0}, REG_INIT, INTERNAL, {IDV_bCV1bPAN}},

	{NAME(S17_cLOCKSOCKET),	IDV_cMINEPAN, M17_LOCKSOCKET,{177,116,232,163}, REG_ACCEPT(TRUE, ISA_LOCKDROP), INTERNAL, {S17_cDOORWAY, S17_cLOCKBURN}},
	{NAME(S17_cLOCKBURN),	IDV_cMINEPAN, M17_LOCKBURN,	 {128,20,128,20}, REG_INIT, INTERNAL},
	{NAME(S17_cDOORWAY),	IDV_cMINEPAN, M17_DOORWAY,	 {280,75,480,274}, REG_INIT, INTERNAL, {IDV_cCV1aPAN, S17_cFFIELD1}},
	{NAME(S17_cFFIELD1),	IDV_cMINEPAN, M17_FFIELD,	 {280,82,280,82}, REG_INIT, INTERNAL, {S17_cFFIELD2}},
	{NAME(S17_cFFIELD2),	IDV_cMINEPAN, M17_FFIELD,	 {280,156,280,156}, REG_INIT, INTERNAL, {S17_cFFIELD3}},
	{NAME(S17_cFFIELD3),	IDV_cMINEPAN, M17_FFIELD,	 {280,223,280,223}, REG_INIT, INTERNAL},
	{NAME(S17_cMINE),		IDV_cCV1bPAN, M17_MINE,		 {2900, 200, 3200, 300}, REG_INIT, INTERNAL},
	{NAME(S17_cFOULWIND),	IDV_cCV1bPAN, M17_FOULWIND,	 {0, 0, 0, 0}, REG_INIT, INTERNAL, {IDV_cCV1bPAN}},

	{NAME(S17_dLOCKSOCKET),	IDV_dMINEPAN, M17_LOCKSOCKET,{177,116,232,163}, REG_ACCEPT(TRUE, ISA_LOCKDROP), INTERNAL, {S17_dDOORWAY, S17_dLOCKBURN}},
	{NAME(S17_dLOCKBURN),	IDV_dMINEPAN, M17_LOCKBURN,	 {128,20,128,20}, REG_INIT, INTERNAL},
	{NAME(S17_dDOORWAY),	IDV_dMINEPAN, M17_DOORWAY,	 {280,75,480,274}, REG_INIT, INTERNAL, {IDV_dCV1aPAN, S17_dFFIELD1}},
	{NAME(S17_dFFIELD1),	IDV_dMINEPAN, M17_FFIELD,	 {280,82,280,82}, REG_INIT, INTERNAL, {S17_dFFIELD2}},
	{NAME(S17_dFFIELD2),	IDV_dMINEPAN, M17_FFIELD,	 {280,156,280,156}, REG_INIT, INTERNAL, {S17_dFFIELD3}},
	{NAME(S17_dFFIELD3),	IDV_dMINEPAN, M17_FFIELD,	 {280,223,280,223}, REG_INIT, INTERNAL},
	{NAME(S17_dMINE),		IDV_dCV1bPAN, M17_MINE,		 {2900, 200, 3200, 300}, REG_INIT, INTERNAL},
	{NAME(S17_dFOULWIND),	IDV_dCV1bPAN, M17_FOULWIND,	 {0, 0, 0, 0}, REG_INIT, INTERNAL, {IDV_dCV1bPAN}},

	{NAME(S17_eLOCKSOCKET),	IDV_eMINEPAN, M17_LOCKSOCKET,{177,116,232,163}, REG_ACCEPT(TRUE, ISA_LOCKDROP), INTERNAL, {S17_eDOORWAY, S17_eLOCKBURN}},
	{NAME(S17_eLOCKBURN),	IDV_eMINEPAN, M17_LOCKBURN,	 {128,20,128,20}, REG_INIT, INTERNAL},
	{NAME(S17_eDOORWAY),	IDV_eMINEPAN, M17_DOORWAY,	 {280,75,480,274}, REG_INIT, INTERNAL, {IDV_eCV1aPAN, S17_eFFIELD1}},
	{NAME(S17_eFFIELD1),	IDV_eMINEPAN, M17_FFIELD,	 {280,82,280,82}, REG_INIT, INTERNAL, {S17_eFFIELD2}},
	{NAME(S17_eFFIELD2),	IDV_eMINEPAN, M17_FFIELD,	 {280,156,280,156}, REG_INIT, INTERNAL, {S17_eFFIELD3}},
	{NAME(S17_eFFIELD3),	IDV_eMINEPAN, M17_FFIELD,	 {280,223,280,223}, REG_INIT, INTERNAL},
	{NAME(S17_eMINE),		IDV_eCV1bPAN, M17_MINE,		 {2900, 200, 3200, 300}, REG_INIT, INTERNAL},
	{NAME(S17_eFOULWIND),	IDV_eCV1bPAN, M17_FOULWIND,	 {0, 0, 0, 0}, REG_INIT, INTERNAL, {IDV_eCV1bPAN}},

	// Scene 21: Berry trees & agriculture
#define BERRYBIN(ID, VIEW_ID, X,Y,H,W, IDD)\
	{NAME(ID), VIEW_ID, M_OBJECTBIN,  {X,Y,X+W,Y+H}, REG_INIT, L_INTERNAL, {IDD}}
#define PLANTBIN(ID, VIEW_ID, X,Y,H,W, IDD, IDS, GROWTIME) \
	{NAME(ID), VIEW_ID, M_PLANTBIN,  {X,Y,X+W,Y+H}, REG_SHOW(IDS,FALSE), L_INTERNAL, {IDD, IDS, GROWTIME}}

#include "data\database\plants.h"

	// Scene 22: Reading scrolls. ----------------------------------------------
	{NAME(S22_SCROLLMAN),	IDV_PARCHPAN, M22_SCROLLMAN, {0, 0, 0, 0}, REG_INIT, L_INTERNAL},
	{NAME(S22_SHIELD),	IDV_PARCHPAN, M22_SHIELD, {49, 24, 118, 113}, REG_INIT, L_INTERNAL, {SCROLL_SHIELD, IDS_SHLDSCRL}},
	{NAME(S22_WORD),	IDV_PARCHPAN, M22_SHIELD, {133, 62, 354, 89}, REG_INIT, L_INTERNAL, {SCROLL_SPELL,IDS_WORDSCRL}},
	{NAME(S22_PLANT1),	IDV_PARCHPAN, M22_SHIELD, {213, 105, 274, 165}, REG_INIT, L_INTERNAL, {SCROLL_PLANT1,IDS_PLNTSCRL}},
	{NAME(S22_PLANT2),	IDV_PARCHPAN, M22_SHIELD, {287, 105, 348, 165}, REG_INIT, L_INTERNAL, {SCROLL_PLANT2,IDS_PLNTSCRL}},
	{NAME(S22_PLANT3),	IDV_PARCHPAN, M22_SHIELD, {367, 105, 423, 165}, REG_INIT, L_INTERNAL, {SCROLL_PLANT3,IDS_PLNTSCRL}},
	{NAME(S22_FISH1),	IDV_PARCHPAN, M22_SHIELD, {165, 185, 268, 226}, REG_INIT, L_INTERNAL, {SCROLL_FISH1,IDS_FISHSCRL}},
	{NAME(S22_FISH2),	IDV_PARCHPAN, M22_SHIELD, {273, 185, 376, 226}, REG_INIT, L_INTERNAL, {SCROLL_FISH2,IDS_FISHSCRL}},
	{NAME(S22_FISH3),	IDV_PARCHPAN, M22_SHIELD, {381, 185, 483, 226}, REG_INIT, L_INTERNAL, {SCROLL_FISH3,IDS_FISHSCRL}},
	{NAME(S22_NYSTROM),	IDV_PARCHPAN, M22_SHIELD, {302, 231, 259, 280}, REG_INIT, L_INTERNAL, {SCROLL_NYSTROM,IDS_VIALSCRL}},

	// Scene 24: Eye of God. ----------------------------------------------
	{NAME(S24_EYEPLATE1), IDV_EYEC1P, M24_EYEPLATE, {200,28, 260,63}, REG_INIT, INTERNAL},
	{NAME(S24_EYEPLATE2), IDV_EYEC1P, M24_EYEPLATE, {266,32, 324,68}, REG_INIT, INTERNAL},
	{NAME(S24_EYEPLATE3), IDV_EYEC1P, M24_EYEPLATE, {333,38, 390,73}, REG_INIT, INTERNAL},

	{NAME(S24_EYESPELL1), IDV_EYEC1P, M24_EYESPELL, {0,  64, 59,105}, REG_INIT, INTERNAL},
	//{NAME(S24_EYESPELL2), IDV_EYEC1P, M24_EYESPELL, {310,71,368,112}, REG_INIT, INTERNAL},
	{NAME(S24_EYESPELL3), IDV_EYEC1P, M24_EYESPELL, {542,79,603,120}, REG_INIT, INTERNAL},

	{NAME(S24_BIGEYE), IDV_EYEC1P, M24_BIGEYE, {310,71,368,112}, REG_SHOW(IDS_EYESPELL,TRUE), INTERNAL},

	{NAME(S24_EYETEXT), IDV_EYEC1P, M24_EYETEXT, {48,155, 553,193}, REG_INIT, L_INTERNAL, {IDS_EYENGLSH}},

	// Scene 25: Rock 'n Roll at the hidden caves -------------------------------
	{NAME(S25_RUMBLE), IDV_W4LOCK, M25_RUMBLE, {415,53,520,171}, REG_INIT, L_INTERNAL},
	{NAME(S25_ROLL),  IDV_W4LOCK, M25_ROLL, {346,59,516,180}, REG_INIT, L_INTERNAL},
	{NAME(S25_SCATTER), IDV_W4LOCK, M25_SCATTER, {25,55,326,180}, REG_ACCEPT(TRUE, IDC_BOMB), L_INTERNAL},

	{NAME(S25_OPN1), IDV_WALL1IN, M25_OPNDOOR, {95,126,404,299}, REG_ACCEPT(TRUE, IDC_BOMB), L_INTERNAL, {IDS_OPN1, IDV_HIDDENR1}},
	{NAME(S25_OPN2), IDV_WALL2IN, M25_OPNDOOR, {2717,138,3028,298}, REG_ACCEPT(TRUE, IDC_BOMB), L_INTERNAL, {IDS_OPN2, IDV_HIDDENR1}},

	{NAME(S25_WALLCU1E), IDV_PL1P, M24_EYETEXT, {162,70,476,158}, REG_INIT, L_INTERNAL, {IDS_WALL1CUE}},
	{NAME(S25_WALLCU2E), IDV_PL2P, M24_EYETEXT, {165,78,479,218}, REG_INIT, L_INTERNAL, {IDS_WALL2CUE}},

	//SCENE 26: BANISHMENT
#define BANISH(ID,X,Y, VIEW_ID) \
	{NAME(S26_BANISH+ID),IDV_BANISH, M26_BANISH, {X, Y, X+40, Y+40}, REG_INIT, L_INTERNAL, {VIEW_ID}}

	BANISH(0,1438,237,IDV_WALL1IN),
	BANISH(1,836,227,IDV_WALL2IN),
	BANISH(2,42,219,IDV_aCV1bPAN),
	BANISH(3,3153,249,IDV_eCV1bPAN),

	// Checker board   ----------------------------------------------------------
	{NAME(S00_REVERSIA),IDV_BOARD,M_OBJECTBIN,{500, 60,500+50,100},REG_SHOW(IDS_REVERSIA,FALSE),L_INTERNAL,{IDD_REVERSIA}},
	{NAME(S00_REVERSIB),IDV_BOARD,M_OBJECTBIN,{990, 60, 990+50,100},REG_SHOW(IDS_REVERSIB,FALSE),L_INTERNAL,{IDD_REVERSIB}},

	{NAME(S00_GOBIN_B),IDV_BOARD,M_OBJECTBIN,{500, 100, 500+50,140},REG_SHOW(IDS_GOBTRFLY,FALSE),L_INTERNAL,{IDD_GOBTRFLY}},
	{NAME(S00_GOBIN_D),IDV_BOARD,M_OBJECTBIN,{990, 100, 990+50,140},REG_SHOW(IDS_GODRAGON,FALSE),L_INTERNAL,{IDD_GODRAGON}},

	// WHITE chess piece bins:
#define BIN_OFFSET(X) {450+X*25,143,450+X*25+25,143+35}
	{NAME(S00_WKING),	IDV_BOARD, M_CHESS,		BIN_OFFSET(0),	REG_SHOW(IDS_WKING,FALSE), INTERNAL, WIPS},
	{NAME(S00_WQUEEN),	IDV_BOARD, M_WQUEEN,	BIN_OFFSET(1),	REG_SHOW(IDS_WQUEEN,FALSE), L_INTERNAL, WIPS},
	{NAME(S00_WROOK),	IDV_BOARD, M_WROOK,		BIN_OFFSET(2),	REG_SHOW(IDS_WROOK,FALSE), L_INTERNAL, WIPS},
	{NAME(S00_WBISHOP),	IDV_BOARD, M_WBISHOP,	BIN_OFFSET(3),	REG_SHOW(IDS_WBISHOP,FALSE), L_INTERNAL, WIPS},
	{NAME(S00_WKNIGHT),	IDV_BOARD, M_WKNIGHT,	BIN_OFFSET(4),	REG_SHOW(IDS_WKNIGHT,FALSE), L_INTERNAL, WIPS},
	{NAME(S00_WPAWN),	IDV_BOARD, M_WPAWN,		BIN_OFFSET(5),	REG_SHOW(IDS_WPAWN,FALSE), L_INTERNAL, WIPS},

	// BLACK chess piece bins:
#undef BIN_OFFSET
#define BIN_OFFSET(X) {915+X*25,143,915+X*25+25,143+35}
	{NAME(S00_BPAWN),	IDV_BOARD, M_BPAWN,		BIN_OFFSET(0),	REG_SHOW(IDS_BPAWN,FALSE), L_INTERNAL, WIPS},
	{NAME(S00_BKNIGHT),	IDV_BOARD, M_BKNIGHT,	BIN_OFFSET(1),	REG_SHOW(IDS_BKNIGHT,FALSE), L_INTERNAL, WIPS},
	{NAME(S00_BBISHOP),	IDV_BOARD, M_BBISHOP,	BIN_OFFSET(2),	REG_SHOW(IDS_BBISHOP,FALSE), L_INTERNAL, WIPS},
	{NAME(S00_BROOK),	IDV_BOARD, M_BROOK,		BIN_OFFSET(3),	REG_SHOW(IDS_BROOK,FALSE), L_INTERNAL, WIPS},
	{NAME(S00_BQUEEN),	IDV_BOARD, M_BQUEEN,	BIN_OFFSET(4),	REG_SHOW(IDS_BQUEEN,FALSE), L_INTERNAL, WIPS},
	{NAME(S00_BKING),	IDV_BOARD, M_CHESS,		BIN_OFFSET(5),	REG_SHOW(IDS_BKING,FALSE), INTERNAL, WIPS},

	{NAME(S00_CLEAR),	IDV_BOARD, M_CLEAR,		{560, 10,600,50},REG_SHOW(IDS_REDEAR,FALSE), INTERNAL, WIPS},

	// Checker/Chess board.  x is column, y is row
#define SQUARE_X		618
#define SQUARE_Y		  5
#define SQUARE_WIDTH	35+2
#define SQUARE_HEIGHT	36+2

#define SQUARE_OFFSET(left, top, right, bottom) {left+SQUARE_X, top+SQUARE_Y, right+SQUARE_X, bottom+SQUARE_Y}
#define SQUARE(x,y,y_adjust) SQUARE_OFFSET(x*SQUARE_WIDTH,y*SQUARE_HEIGHT+y_adjust,(x+1)*SQUARE_WIDTH,(y+1)*SQUARE_HEIGHT+y_adjust)

	// a square on a checker board:
#define S00_SQUARE(ROW,COL,Y_ADJUST)	\
	{NAME(S00_SQUARE00 + ROW*8 + COL), IDV_BOARD, M_SQUARE,  SQUARE(ROW,COL,Y_ADJUST),REG_ACCEPT(TRUE, IDC_TOKEN), INTERNAL, WIPS}

	// a row on a checker board (8 squares):
#define S00_COL(COL, Y_ADJUST)  \
	S00_SQUARE(COL,0,3/Y_ADJUST), \
	S00_SQUARE(COL,1,2/Y_ADJUST), \
	S00_SQUARE(COL,2,1/Y_ADJUST), \
	S00_SQUARE(COL,3,0), \
	S00_SQUARE(COL,4,0), \
	S00_SQUARE(COL,5,-1/Y_ADJUST), \
	S00_SQUARE(COL,6,-2/Y_ADJUST), \
	S00_SQUARE(COL,7,-3/Y_ADJUST)

	// a checker board (8 rows):
	// 1st row
	S00_COL(0,1),
	// 2nd row
	S00_COL(1,2),
	// 3rd row
	S00_COL(2,3),
	// 4th row
	S00_COL(3,4),
	// 5th row
	S00_COL(4,4),
	// 6th row
	S00_COL(5,3),
	// 7th row
	S00_COL(6,2),
	// 8th row
	S00_COL(7,1),
};

GDBT_MACHINES gdbt_machines(tbl_machines, sizeof(tbl_machines)/sizeof(MACHINE));

static ALTVIEW altviews[] = {

  // these alt views link us from the central locker room LOCKERR to the individual lockers.
#define S02_EXTERNAL_BIN_Na(UNIT) \
  	{IDV_LOCKERR, ID(UNIT)+S02_BIN___XX, {3050+(UNIT%3)*80,  120+(UNIT/3)*82, 3050+(UNIT%3+1)*80,  202+(UNIT/3)*82},ALT_LINK_STATIC}
  S02_EXTERNAL_BIN_Na(0),
  S02_EXTERNAL_BIN_Na(1),
  S02_EXTERNAL_BIN_Na(3),
  S02_EXTERNAL_BIN_Na(4),

#define S02_EXTERNAL_BIN_Nb(UNIT) \
  	{IDV_LOCKERR, ID(UNIT)+S02_BIN___XX, {10,  120+(UNIT/3)*83, 105,  202+(UNIT/3)*82},ALT_LINK_STATIC}
  S02_EXTERNAL_BIN_Nb(2),
  S02_EXTERNAL_BIN_Nb(5),

#define S02_EXTERNAL_BIN_W(UNIT) \
  	{IDV_LOCKERR, ID(UNIT+9)+S02_BIN___XX, {2295+(UNIT%3)*83,  120+(UNIT/3)*82, 2295+(UNIT%3+1)*83,  202+(UNIT/3)*82},ALT_LINK_STATIC}
  S02_EXTERNAL_BIN_W(0),
  S02_EXTERNAL_BIN_W(1),
  S02_EXTERNAL_BIN_W(2),
  S02_EXTERNAL_BIN_W(3),
  S02_EXTERNAL_BIN_W(4),
  S02_EXTERNAL_BIN_W(5),

#define S02_EXTERNAL_BIN_S(UNIT) \
  	{IDV_LOCKERR, ID(UNIT+18)+S02_BIN___XX, {1496+(UNIT%3)*90,  120+(UNIT/3)*88, 1496+(UNIT%3+1)*90,  208+(UNIT/3)*88},ALT_LINK_STATIC}
  S02_EXTERNAL_BIN_S(0),
  S02_EXTERNAL_BIN_S(1),
  S02_EXTERNAL_BIN_S(2),
  S02_EXTERNAL_BIN_S(3),
  S02_EXTERNAL_BIN_S(4),
  S02_EXTERNAL_BIN_S(5),

  // these alt views link us from one of the north, west, or south locker panels to the individual lockers.
#define S02_EXTERNAL_BIN(UNIT) \
  	{IDV_LOCKERN, ID(UNIT)+S02_BIN___XX, {66+(UNIT%3)*93,  17+(UNIT/3)*93, 129+(UNIT%3)*93,  79+(UNIT/3)*93},ALT_LINK}
  S02_EXTERNAL_BIN(0),
  S02_EXTERNAL_BIN(1),
  S02_EXTERNAL_BIN(2),
  S02_EXTERNAL_BIN(3),
  S02_EXTERNAL_BIN(4),
  S02_EXTERNAL_BIN(5),
  S02_EXTERNAL_BIN(6),
  S02_EXTERNAL_BIN(7),
  S02_EXTERNAL_BIN(8),
#undef S02_EXTERNAL_BIN
#define S02_EXTERNAL_BIN(UNIT) \
  	{IDV_LOCKERW, ID(UNIT+9)+S02_BIN___XX, {66+(UNIT%3)*93,  17+(UNIT/3)*93, 129+(UNIT%3)*93,  79+(UNIT/3)*93},ALT_LINK}
  S02_EXTERNAL_BIN(0),
  S02_EXTERNAL_BIN(1),
  S02_EXTERNAL_BIN(2),
  S02_EXTERNAL_BIN(3),
  S02_EXTERNAL_BIN(4),
  S02_EXTERNAL_BIN(5),
  S02_EXTERNAL_BIN(6),
  S02_EXTERNAL_BIN(7),
  S02_EXTERNAL_BIN(8),
#undef S02_EXTERNAL_BIN
#define S02_EXTERNAL_BIN(UNIT) \
  	{IDV_LOCKERS, ID(UNIT+18)+S02_BIN___XX, {66+(UNIT%3)*93,  17+(UNIT/3)*93, 129+(UNIT%3)*93,  79+(UNIT/3)*93},ALT_LINK}
  S02_EXTERNAL_BIN(0),
  S02_EXTERNAL_BIN(1),
  S02_EXTERNAL_BIN(2),
  S02_EXTERNAL_BIN(3),
  S02_EXTERNAL_BIN(4),
  S02_EXTERNAL_BIN(5),
  S02_EXTERNAL_BIN(6),
  S02_EXTERNAL_BIN(7),
  S02_EXTERNAL_BIN(8),
};

TBL_ALTVIEWS tbl_altviews(altviews, sizeof(altviews)/sizeof(ALTVIEW));

void GDBT_MACHINES::MachInit()
{
  Signal(SID_ID, SIG_NULL);
  Signal(SOD_ID, SIG_CLEAR);
}

