/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       triggers.h
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 02/26/95  gas  Created.
; 06/02/96  gas  Add table of cardinals for view navigation.
; 07/15/96  gas  Add view queries to see players in linked views.
----------------------------------------------------------------------------*/

BOOL Cardinal(int view_id, int theta, BOOL bBackup, BOOL bFire);
WORD trgQueryCursor(int view_id, POINT p);
BOOL trgFire(int view_id, POINT p);

//----------------------------------------------------------------------------
class CARDINALS {
public:
	WORD m_view_id;
	WORD m_cardinal[8];
public:
	inline WORD GetPKey() {return m_view_id;}
	inline WORD GetGroupKey() {return m_view_id;}
};

//----------------------------------------------------------------------------
#define BTC_TABLE B_TABLE<CARDINALS, WORD, WORD, WORD> 
class TBL_CARDINALS : public BTC_TABLE {
public:
	TBL_CARDINALS(CARDINALS *table, int count):
	  BTC_TABLE(table, count, count) {}
public:
	static BOOL Fire(WORD view_id, WORD cardinal, BOOL bFire);

	// are there any hands in this direction?
	static void TBL_CARDINALS::CardHands(int view_id, CARDINALS *hands);
	static BOOL TBL_CARDINALS::InView(int from_view_id, int to_view_id);
};

