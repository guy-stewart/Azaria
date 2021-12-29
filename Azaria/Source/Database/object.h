/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
;
; File:       obj_list.h
;
; Description:
;	Storage of objects for views.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 02/09/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#pragma once

int objdefGetClass(WORD object_id);
WORD objdefGetIDByName(char *name);
int objdefGetLookIconId(WORD object_id);
int objdefGetLookActorId(WORD object_id);
int objdefGetLookCursorId(WORD object_id);
const char *objdefGetName(WORD object_id);
