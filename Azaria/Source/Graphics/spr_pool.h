/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
;
; File:       spr_pool.h
;
; Description:
;	Storage of currently active sprites.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/31/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#pragma once

typedef struct {
	char name[16];
	ONE sprite;
	short sprite_id;
	short refCount;
} SPR_OBJECT;

typedef int OBJ_HANDLE;

OBJ_HANDLE sprSelect(int id);
SPR_OBJECT *sprGet(OBJ_HANDLE handle);
int sprRelease(OBJ_HANDLE hObject);
