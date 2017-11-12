/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       tables.h
;
; Description:
;	Definitions of database tables.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/31/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#define MAX_PLAYERNAME		30
#define MAX_FILENAME		30
#define MAX_ENTRYNAME		30
#define MAX_DESCR			30

// Object image relations.  STATIC.
typedef struct {
	WORD look_id;
	WORD icon_id;
	WORD cursor_id;
	WORD actor_id;
} LOOK;

// Object physics relations. STATIC.
typedef struct {
	WORD physics_id;
	WORD density;
	WORD mass;
	WORD color;
} PHYSICS;

// Object behaviors relations. STATIC.
typedef struct {
	WORD behavior_id;
} BEHAVIOR;

// OBJECT:  Object definitions.  STATIC.
#define NAME(X) #X,X
typedef struct {
	const char *name;
	WORD object_id;
	WORD unique_flags;
	WORD class_id;
	WORD object_look_id;
	WORD physics_id;
} OBJECT_DEF;

