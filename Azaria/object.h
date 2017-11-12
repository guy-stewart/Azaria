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

#ifndef __OBJECT_INCLUDED__
#define __OBJECT_INCLUDED__

int objdefGetClass(WORD object_id);
WORD objdefGetIDByName(char *name);
int objdefGetLookIconId(WORD object_id);
int objdefGetLookActorId(WORD object_id);
int objdefGetLookCursorId(WORD object_id);

#endif // __OBJECT_INCLUDED__

