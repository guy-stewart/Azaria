/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       machines.h
;
; Description:
;	Storage of objects for views.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 04/03/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#ifndef __MACHINES_INCLUDED__
#define __MACHINES_INCLUDED__

//#include "data\database\idents.h"
#include "btable.h"

// Foreward declaration:
class MACHINE;

#define MAKELONGKEY(a,b) MAKELONG(b,a)
#define MAKELONG(a, b)  ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))

#define MAKEDWORDKEY(a,b) MAKEDWORD(b,a)
#define MAKEDWORD(a, b)  ((DWORD)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))


typedef struct {
	BYTE signaled:1;
} SYNC;

class TRANSITION {
public:
	WORD	m_machine_id;
	BYTE	m_state;
	BYTE	m_new_state;
	BYTE	m_input;
	BYTE	m_bParm;
	WORD	m_wParm;

public:
	inline int GetGroupKey() {return MAKELONGKEY(m_machine_id,m_state);}
	BOOL MatchTransition(BYTE input, BYTE bParm, WORD wParm, WORD obj_def_id);
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Input operations:	BYTE

#ifdef __MACHINES_CPP__
char *strAutoOps[] = {
	"EPSILON",
	"PLAYWAVE",
	"WTOPWAVE",
	"PLAYMIDI",
	"STOPMIDI",
	"ESTIME",
	"EMTIME",
	"IFSTATE",
	"IFSTATEi",
	"NOTSTATE",
	"NOTSTATEi",
	"MAP_OBJ",
	"MIX",
	"XIM",
	"SET_YOFFSET",
	"SET_XOFFSET",
	"SHOW",
	"ASHOW",
	"ANIMATE",
	"VIDEO",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"MOD",
	"ADDI",
	"SUBI",
	"MULI",
	"DIVI",
	"MODI",
	"ASSIGN",
	"MOV",

	"GTi",
	"GTEi",
	"LTi",
	"LTEi",
	"EQUALi",
	"NEQUALi",

	"GT",
	"GTE",
	"LT",
	"LTE",
	"EQUAL",
	"NEQUAL",

	"IS A",
	"NOT A",

	"LOADVIEW",
	"SPELL_ME",
	"SPELL_YOU",
	"SIGNALri",
	"SIGNALi",
	"SIGNAL",
	"HANDOFF",
	"C_ACCEPT",
	"O_ACCEPT",
	"SYNCPOINT",
	"RAND",
	"REF_MINST",
	"SENDKEYI",
	"SENDKEY",
	"SENDMSGI",
	"SENDMSG",
	"MAPi",
	"MAP",
};
#endif

enum {
  // Actionless input operations come first...
  // NOTE: If you add any actionless operations, then you must change the
  //		 LAST_AOP manifest constant.
   FIRST_AOP	= 0,	// 1st Actionless Operation

   EPSILON		= 0,	// Just do it.

   PLAYWAVE	,	// sound id is stored in transition::wAmbient
   STOPWAVE	,
   PLAYMIDI	,
   STOPMIDI	,

   ESTIME		,	// ESTIME 
							  // Elapsed time in seconds.  the time remaining is
							  // stored in the machine::dEtime.
   EMTIME		,	// Elapsed time in milliseconds.  time remaining is
							  // stored in the machine::dEtime.

   IFSTATE		,	// IFSATE state, machine_id
   IFSTATEi	,	// IFSATE state, machine_id
   NOTSTATE	,	// NOTSTATE state, machine_id
   NOTSTATEi	,	// NOTSTATE state, machine_id
   MAP_OBJ		,	// MAP_OBJ map_operation_id
							  // wObject = map_operation(wObject)
							  // if the object cannot be mapped
							  // then the transition occurs w/o mapping the object.
   MIX			,	// wObject = MAP_OBJ(r1, r2) r1 = operator, r2 = operand.
   XIM			,	//

	SET_YOFFSET,
	SET_XOFFSET,

   SHOW		,	// SHOW register, <cursor | icon | actor>
							  // Show as sprite of the stored object.
   ASHOW		,	// Just like show, but play looped animate
   ANIMATE		,	// ANIMATE [LOOP]
   VIDEO		,	// Show a video clip. Remove the image after the last frame.

  // Arithmetic operations:
   ADD			,	// ADD register, register --> register
   SUB			,	// SUB register, register --> register
   MUL			,	// MUL register, register --> register
   DIV			,	// DIV register, register --> register
   MOD			,	// MOD register, register --> register

   ADDI		,	// ADD register, value (imm) --> register
   SUBI		,	// SUB register, value (imm) --> register
   MULI		,	// MUL register, value (imm) --> register
   DIVI		,	// DIV register, value (imm) --> register
   MODI		,	// MOD register, value (imm) --> register

  // Move operations:
   ASSIGN,		// ASSIGN register, value (imm) --> register
#define   CLEAR		ASSIGN	// clear a register so unused value is not 
							  // transmitted with table updates.

   MOV		,	// MOV source register,  dest register

  // Comparison operations:
   GTi		,	// GTE register, value - Greater Than or Equal to.
   GTEi		,	// GTE register, value - Greater Than or Equal to.
   LTi		,	// GTE register, value - Greater Than or Equal to.
   LTEi		,	// LTE register, value - Less Than or Equal to
   EQUALi	,	// EQUAL register, value - EQUAL to
   NEQUALi	,	// NEQUAL register, value - Not EQUAL to

   GT	  	,	// GTE register, value - Greater Than or Equal to.
   GTE	  	,	// GTE register, value - Greater Than or Equal to.
   LT		,	// LTE register, value - Less Than or Equal to
   LTE		,	// LTE register, value - Less Than or Equal to
   EQUAL	,	// EQUAL register, value - EQUAL to
   NEQUAL	,	// NEQUAL register, value - Not EQUAL to

   IS_A,
   NOT_A,

   LOADVIEW,	// LOADVIEW view_id

  // SPELL	(bParm, wParm)
   SPELL_ME,	// dropped a spell on me!

  // GRABFROM:
  // The grabbed object is always stored in wObject.
  // If a register is specified int the first argument,
  // then the machine id is taken from that register.
  // otherwise the second argument specifies the machine id.
   SPELL_YOU,	// Dropped a spell on you!

  // signal another machine to go.  
   SIGNALri,	// (imm) wParm is the id of the machine to be signaled.
							  // (bParm) stores the signal id
   SIGNALi,	// (imm) wParm is the id of the machine to be signaled.
							  // bParm is the signal id.
   SIGNAL,	// (reg) bParm is the register holding the machine_id of
							  // of the machine to be signaled. wParm is the parm
							  // to the signaled call.
   HANDOFF,
   C_ACCEPT,	// Accept objects of a specific class.
   O_ACCEPT,	// Accept specific object.
   SYNCPOINT,
   RAND,

   REF_MACHINE,	// reference another machines registers.

   SENDKEYI,
   SENDKEY,
   SENDMSGI,
   SENDMSG,
   MAPi,
   MAP,
#define LAST_AOP	MAP	// <- MUST BE EQUAL TO LAST AUTOTRANSITION OP
};

//----------------------------------------------------------------------------
// System events:
#define WAIT		0x40	// wait for a signal from another machine.

// User events:

#define DROP		0x80	// DROP [SWAP], [IDD_OBJECT] --> wObject
#define SWAP TRUE // drop can take SWAP as a bParm argument

#define GRAB		0x81	// object_def retrieved from wObject,
							// OR from transitions wParm if specified.
#define CLICK		0x82	// 
#define DRAG		0x83	// object_def stored in wObject
#define DRAGFOCUS	0x84

//----------------------------------------------------------------------------
// Registers:
#define IMMEDIATE	0x00	// Not a register at all, the value is stored in 
							// the transitions wParm.
#define BPARM		0x01
#define WPARM		0x02
#define WOBJECT		0x03
#define WSPRITE		0x04
#define BFRAME		0x05
#define WACCEPT		0x06
#define DETIME		0x07

#define WTHIS		0x08	// instance id of the caller. Read only.

// instance constant registers:
// The values of these register are set in the instance decl.  The value
// should not change at run-time because the register are not syncronized
// across the global database.
#define WIP1		0x10
#define WIP2		0x11
#define WIP3		0x12
#define WIP4		0x13

// Referenced Registers:
// A machine instance can look at the registers of another machine instance.
#define R_OFFSET	0x20  // IMPORTANT NUMBER: what do I subtract from the referenced
						  // registers values to get the non-referenced register values.
						  // I.E.  R_BPARM - R_OFFSET = BPARM.
#define R_BPARM		0x21
#define R_WPARM		0x22
#define R_WOBJECT	0x23
#define R_WSPRITE	0x24
#define R_BFRAME	0x25
#define R_WACCEPT	0x26
#define R_DETIME	0x27

#define R_WIP1		0x30
#define R_WIP2		0x31
#define R_WIP3		0x32
#define R_WIP4		0x33

// shared registers.
// These registers are shared by all the machines on a local computer.
// A machine can use these registers to pass parms to other machines,
// as well as to store intermediate results.
// The contents of the register is not guaranteed to be preserved if the
// machine is pre-empted for any reason.
#define WRAND		0x40
#define WTEMP1		0x41
#define WTEMP2		0x42
#define WTEMP3		0x43
#define WVIEWID		0x44

// database psuedo-registers
// These can be acted on by arithmetic, compare, and move operations
#define LKARMA		0x70
#define LSTRENGTH	0x71
#define LWISDOM		0x72
#define LENERGY		0x73
#define LSEX		0x74
#define LWEALTH		0x75
#define LVIEW		0x76

#define OKARMA		0x77
#define OSTRENGTH	0x78
#define OWISDOM		0x79
#define OENERGY		0x7A
#define OSEX		0x7B
#define OWEALTH		0x7C
#define OVIEW		0x7D

#define PTR(REGISTER) (REGISTER|0x80)
//----------------------------------------------------------------------------
typedef struct {
  WORD *table;
  WORD sizeoftable;
} TBL_MAP_ENTRY;
extern TBL_MAP_ENTRY tbl_map_tables[];
extern int tbl_map_tables_count;
//----------------------------------------------------------------------------
typedef struct {
	WORD op;
	WORD obj_def;
public:
	DWORD GetPKey() {return MAKELONGKEY(op, obj_def);}
	WORD GetGroupKey() {return op;}
} ISA;

#define TISA_TABLE B_TABLE<ISA, DWORD, DWORD, WORD>
class TBL_ISA : TISA_TABLE  {
public:
	TBL_ISA(ISA *table, int records):
  		TISA_TABLE(table, records, records) {};

		ISA *Find(DWORD key) {return TISA_TABLE::PGet(key);}
};

//----------------------------------------------------------------------------

typedef struct {
	WORD op;
	WORD src_obj_def;
	WORD dst_obj_def;
public:
	DWORD GetPKey() {return MAKEDWORDKEY(op, src_obj_def);}
	WORD GetGroupKey() {return op;}
	WORD GetUKey() {return dst_obj_def;}
} OBJECT_OP;

#define TOPS_TABLE B_TABLE<OBJECT_OP, DWORD, WORD, WORD>
class TBL_OBJECT_OPS : TOPS_TABLE  {
public:
	TBL_OBJECT_OPS(OBJECT_OP *table, int records):
  		TOPS_TABLE(table, records, records) {};

		OBJECT_OP *Find(DWORD key) {return TOPS_TABLE::PGet(key);}
		OBJECT_OP *UFind(WORD key) {return TOPS_TABLE::UGet(key);}
};

//----------------------------------------------------------------------------

#define BMT_TABLE B_TABLE<TRANSITION, int, int, int>
class TBL_MACHINE_DEF : public BMT_TABLE {
public:
	TBL_MACHINE_DEF(TRANSITION *table, int count):
	  BMT_TABLE (table, count, count) {}

	TRANSITION *TBL_MACHINE_DEF::FindTransition(MACHINE *machine, BYTE state, BYTE input, BYTE bParm, WORD wParm);
	TRANSITION *TBL_MACHINE_DEF::FindFirstAutoTrans(WORD machine_id, BYTE state, int *position);
	TRANSITION *TBL_MACHINE_DEF::FindNextAutoTrans(WORD machine_id, BYTE state, int *position);
};

//----------------------------------------------------------------------------
// Registers for each machine instance are syncronized across the
// global database.
#define REG_INIT {0,0,0,0,0,0,0,0,0,0,0,0,0}
#define REG_SHOW(SPRITE, ANIMATE) {0,0,0,0,SPRITE,(ANIMATE==TRUE),(ANIMATE==TRUE),0,0,0,0,0,0}
#define REG_ACCEPT(CLASS, ID) {0,0,0,0,0,0,0,0,0,0,CLASS,0,ID,0} // accept a specific class of object.
typedef struct {
	BYTE state;
	BYTE bParm;
	WORD wParm;
	WORD wObject;
	WORD wSprite;
	BYTE Animate:1;
#define V_LOOP		0x0001 // loop the video clip.
#define V_REWIND	0x0002 // reverse direction at the end of the clip.
#define V_LASTFRAME 0x0004 // start running from the last frame.
#define V_REVERSE	(V_REWIND|V_LASTFRAME)
	BYTE Loop:1;
	BYTE PlayReverse:1;
	BYTE bStartFromEnd:1;
	BYTE bRewind:1;
	BYTE AcceptClass:1;
	BYTE bFrame;
	WORD wAccept;
	long dEtime;
} REGISTERS;

// Word Instance Parameter are constant.
typedef struct {
	WORD wIP1;
	WORD wIP2;
	WORD wIP3;
	WORD wIP4;
} WIP;

//----------------------------------------------------------------------------
#define NAME(X) #X,X

class MACHINE {
public:
	// registers shared by all the machines.
	static WORD c_Rand;		  // current shared random value.
	static WORD c_Temp1;	  // temp reg for computations.
	static WORD c_Temp2;	  // temp reg for computations.
	static WORD c_Temp3;	  // temp reg for computations.
	static WORD c_ViewId;	  // store current view id for scrolls, etc
	static DPID c_Pid;		  // PID of player responsible for current transaction.
	static WORD c_RefMachine; // machine being referenced by current transaction.
public:
	char *m_name;
	WORD m_id;
	WORD m_view_id;
	WORD m_machine_id;
	SRECT m_rect;

	REGISTERS m_reg;

#define INTERNAL 0,0,0,0,0,0,0,0
#define L_INTERNAL 1,0,0,0,0,0,0,0
#define LV_INTERNAL 1,0,0,0,0,0,1,0
	WORD m_Local:1;
	WORD m_onAgenda:1;
	WORD m_Dirty:1;
	WORD m_NewSprite:1;
	WORD m_NewFrame:1;
	WORD m_Timing:1;		// 0:not timing (load dEtime on timed op,  1:already timing)
	WORD m_AlwaysVisible:1;

	SPIRIT *m_Spirit;

#define WIPS {0,0,0,0}
  WIP m_wip;

public: // should be private
	WORD MACHINE::GetRegVal(BYTE reg_id);
	void MACHINE::SetRegVal(BYTE reg_id, WORD value);
	BOOL MACHINE::Tick(int etime);

public:
	void SendState(DPID pid, WORD group);
	void SendTrans(WORD group, WORD op, WORD obj_def_id, DPID pid);

public:	// callbacks for B_TABLE:
	inline int GetGroupKey() {return m_view_id;}
	inline int GetUKey() {return m_id;}
	inline SRECT *GetRegion() {return &m_rect;}

public:
	BOOL Drop(WORD *obj_def, BOOL local, BOOL autoTrans);
	WORD Grab(BOOL local, BOOL autoTrans, BOOL bClickOnly); // return object definition if object is generated
	WORD Drag(WORD obj_def, BOOL local, BOOL autoTrans);
	void Signal(WORD wParm); // signal the machine to continue;
	void DragFocus(BOOL bHasFocus); // signal the machine to continue;

	void Paint(ONE *canvas);
	void Erase();
	void Release();

	BOOL isDirtySprite() {return m_Dirty;}

	// execute the given transition
	InitiateTransition(WORD proc, WORD obj_def, BOOL local, BOOL autoTrans);
	void Transition(TRANSITION *trans, BYTE bParm);
	BOOL AutoTransition(int etime);
	void SlamState(REGISTERS *reg); // slam the machine into a new state. Deal with it!
};

//----------------------------------------------------------------------------
#ifdef __GDB_INCLUDED__

#define BMW_TABLE B_TABLE<MACHINE, WORD, WORD, WORD>

class GDBT_MACHINES : public BMW_TABLE,  public GDB_TABLE {
private:
	WORD m_AgendaCount; // how many machines are on our agenda
	WORD m_MaxAgenda;	// max size of agenda list
	MACHINE **m_Agenda;
	MACHINE *m_machine_last_drag; // The last machine to rx a drag operation.
public:
	GDBT_MACHINES(MACHINE *tbl_machines, int records):
  		BMW_TABLE (tbl_machines, records, records),
		GDB_TABLE(IDT_MACHINES) 
		{
			m_Agenda = (MACHINE**)malloc(records * sizeof(MACHINE*));
			m_AgendaCount = 0;
			m_MaxAgenda = records;
		}

	void Init();		// engine initialization
	void MachInit();	// game specific initialization

	int GDBT_MACHINES::CompressMachine(BYTE *output, REGISTERS *r);
	int GDBT_MACHINES::UncompressMachine(BYTE *input, REGISTERS *r);
	void GDBT_MACHINES::SendAllStates(DWORD pid);
	BOOL GDBT_MACHINES::UncompressAllStates(BYTE *pkt);

	BOOL GDBT_MACHINES::Load(char *filename);
	BOOL GDBT_MACHINES::Save(char *filename);

	// Callbacks for the global database engine:
private:
	virtual void ReceiveUpdate(DPID from, DPID to, WORD proc, UDATA *data);

public:
	void SendObject(DWORD to, WORD obj_def_id);
	void SendSync(WORD machine_id, WORD syncpoint);

	MACHINE *GDBT_MACHINES::Get(WORD id) {return UGet(id);}  // get a machine by unique machine id.
	BOOL GDBT_MACHINES::Drop(WORD view_id, WORD *obj_def_id, POINT p);
	BOOL GDBT_MACHINES::Drop(WORD machine_sid, WORD *obj_def_id);
	WORD GDBT_MACHINES::Grab(WORD view_id, POINT p, BOOL bClickOnly, POINT *obj_pos);
	WORD GDBT_MACHINES::Drag(WORD view_id, WORD obj_def_id, POINT p);
	void GDBT_MACHINES::Signal(WORD machine_sid, WORD wParm);

	WORD GDBT_MACHINES::QueryCursor(WORD view_id, WORD obj_def_id, POINT p);

	void GDBT_MACHINES::Paint(ONE *canvas, WORD view_id);
	void GDBT_MACHINES::Init(WORD view_id);
	void GDBT_MACHINES::Release(WORD view_id);
	void GDBT_MACHINES::Tick(int etime);
	void GDBT_MACHINES::AddAgenda(MACHINE *machine);
};

extern GDBT_MACHINES gdbt_machines;
extern TBL_MACHINE_DEF btbl_machine_def;
extern TBL_OBJECT_OPS tbl_object_ops;
extern SYNC tbl_syncpoints[];
extern TBL_ISA tbl_isa;

#endif // __GDB_INCLUDED__
//----------------------------------------------------------------------------

class ALTVIEW {
public:
  WORD m_view_id;
  WORD m_machine_id;
  SRECT m_region;
#define ALT_LINK 0,0,1,1,1
#define ALT_LINK_STATIC	0,0,0,0,1
  WORD m_sprite;
  SPIRIT *m_Spirit;
  WORD m_Animate:1;
  WORD m_Loop:1;
  WORD m_Link:1;

public:	// callbacks for B_TABLE:
  inline int GetGroupKey() {return m_view_id;}

  void ALTVIEW::Paint(ONE *canvas);
  void ALTVIEW::Erase();
  void Release();
  inline SRECT *GetRegion() {return &m_region;}
};

#define ALT_TABLE B_TABLE<ALTVIEW, WORD, WORD, WORD>

class TBL_ALTVIEWS : public ALT_TABLE {
public:
	TBL_ALTVIEWS(ALTVIEW *table, int records):
  		ALT_TABLE(table, records, records)
		{}

	void Paint(ONE *canvas, WORD view_id);
	BOOL TBL_ALTVIEWS::Link(WORD view_id, POINT p, BOOL bFire);
	void Release(WORD view_id);
};

extern TBL_ALTVIEWS tbl_altviews;
#endif // __MACHINES_INCLUDED__

