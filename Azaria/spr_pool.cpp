/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       spr_pool.c
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

#include "maya.h"
#include "paths.h"

#define MAX_RAM_SPRITES		100

// The object table is private.
static SPR_OBJECT spr_pool[MAX_RAM_SPRITES];
static int lastObject = 0;

typedef struct {
	int sprite_id;
	const char *filename;
} STR_ENTRY;

#include "Data/Database/Ids.h"
#include "Data/Database/tbl_sprnames.h"

static int names_count = sizeof(tbl_spr_names)/sizeof(STR_ENTRY);

const char *sprGetName(int sprite_id)
{
  int i;
  for (i=0;i<names_count; i++)
  {
	  if (tbl_spr_names[i].sprite_id == sprite_id)
		  return tbl_spr_names[i].filename;
  }
  return tbl_spr_names[0].filename;
}

int sprSelect(int sprite_id)
{
	int nextFreeObject = 0;
	int i;

	// see if the object is already loaded:
	for (i=0;i<lastObject;i++)
	{
		if (spr_pool[i].sprite_id == sprite_id)
		{
			spr_pool[i].refCount++;
			return i+1;
		}
		else if (!nextFreeObject && !spr_pool[i].refCount)
		{ // if we cant find the object, save first slot to load it into.
			nextFreeObject = i+1;
		}
	}

	// we can either insert the sprite into the middle of the sprite pool ...
	if (nextFreeObject)
		i = nextFreeObject-1;
	else
	{	// ... or append it to the end of the pool:
		i = lastObject;
		lastObject++;
	}

	if (i >= MAX_RAM_SPRITES)
	{
		Msg("Sprite pool overflow loading %s", sprGetName(sprite_id));
		throw(ERROR_OUT_OF_STRUCTURES);
	}

	// load the sprite:
	if (!spr_pool[i].sprite.Read(AltObjectsPath, sprGetName(sprite_id))
	  &&!spr_pool[i].sprite.Read(ObjectsPath, sprGetName(sprite_id)))
	{
		strncpy(missing_file_name, sprGetName(sprite_id), _MAX_FNAME);
		strncpy(missing_file_path1, ObjectsPath, _MAX_PATH);
		strncpy(missing_file_path2, AltObjectsPath, _MAX_PATH);
		throw(ERROR_FILE_NOT_FOUND);
	}

	// TODO: uncompress spr sprites ...
	spr_pool[i].sprite.UnCompress();

	spr_pool[i].sprite_id = sprite_id;

	// for debugging only -
	strncpy(spr_pool[i].name, sprGetName(sprite_id),15);
	spr_pool[i].name[15] = 0;

	spr_pool[i].refCount = 1;

	return i+1;
}

SPR_OBJECT *sprGet(int hSprite)
{
	hSprite--; // normalize object handle.

	return &spr_pool[hSprite];
}

int sprRelease(int hSprite)
{
	// check for bad sprite handle:
	assert(hSprite>0);
	if (hSprite <=0) return FALSE;

	hSprite--; // normalize object handle.

	assert(spr_pool[hSprite].refCount>0);

	if (!--spr_pool[hSprite].refCount)
	{
		spr_pool[hSprite].sprite.freeBits();
		memset(&spr_pool[hSprite], 0, sizeof(SPR_OBJECT));
		if (hSprite == lastObject)
			lastObject--;
	}

	return TRUE;
}


