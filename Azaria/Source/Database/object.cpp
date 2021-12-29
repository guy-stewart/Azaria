/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       object.cpp
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/25/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "includes.h"
#include "tables.h"

#include "Data/Database/Idents.h"
#include "Data/Database/Ids.h"
#include "Data/Database/tbl_objects.h"

static int tbl_look_rows = sizeof(tbl_look)/sizeof(tbl_look[0]);
static int count_objects_defs = sizeof(tbl_object_def)/sizeof(tbl_object_def[0]);

//----------------------------------------------------------------------------
LOOK *lookGet(int look_id)
{
	int i;

	for (i=0;i<tbl_look_rows;i++)
		if (tbl_look[i].look_id == look_id)
			return &tbl_look[i];

	return NULL;
}

//----------------------------------------------------------------------------
OBJECT_DEF *objdefGet(WORD object_id)
{
	int i;

	for (i=0; i<count_objects_defs; i++)
		if (tbl_object_def[i].object_id == object_id)
			return &tbl_object_def[i];

	return NULL;
}

//----------------------------------------------------------------------------
WORD objdefGetIDByName(char *name)
{
	int i;

	for (i=0; i<count_objects_defs; i++)
		if (!strcasecmp(tbl_object_def[i].name, name))
			return tbl_object_def[i].object_id;

	return 0;
}

//----------------------------------------------------------------------------
const char * objdefGetName(WORD object_id)
{
	OBJECT_DEF *object;

	object = objdefGet(object_id);

	if (object)
		return object->name;

	return "0";
}


//----------------------------------------------------------------------------
int objdefGetClass(WORD object_id)
{
	OBJECT_DEF *object;

	object = objdefGet(object_id);

	if (object)
		return object->class_id;

	return 0;
}

//----------------------------------------------------------------------------
int objdefGetLookIconId(WORD object_id)
{
	OBJECT_DEF *object;

	object = objdefGet(object_id);

	if (object)
		return lookGet(object->object_look_id)->icon_id;

	return 0;
}

//----------------------------------------------------------------------------
int objdefGetLookActorId(WORD object_id)
{
	OBJECT_DEF *object;

	object = objdefGet(object_id);

	if (object)
		return lookGet(object->object_look_id)->actor_id;

	return 0;
}

//----------------------------------------------------------------------------
int objdefGetLookCursorId(WORD object_id)
{
	OBJECT_DEF *object;

	object = objdefGet(object_id);

	if (object)
		return lookGet(object->object_look_id)->cursor_id;

	return 0;
}
