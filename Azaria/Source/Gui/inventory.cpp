/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       inventory.c
;
; Description:
;	Implements the inventory view.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 02/01/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "maya.h"
#include "gdb.h"
#include "inventory.h"
#include "user.h"
#include "Data/Database/Ids.h"

#define Y_POS 10

//----------------------------------------------------------------------------
void INVENTORY::Paint(int etime, int flags)
{
  int i;

 	if (m_NewSpirit)
	{
	  for (i=0;i<3;i++)
		if (m_Spirit[i])
		{
			delete m_Spirit[i];
			m_Spirit[i] = NULL;
		}

	  for (i=0;i<3;i++)
	  {
		int position = m_position+i-1;
		if (position>=0 && position<m_count && m_objects[position])
		{
			m_Spirit[i] = new SPIRIT(&surface, objdefGetLookIconId(m_objects[position]));
			if (m_Spirit[i])
			{
				m_Spirit[i]->Move(70*i, Y_POS, 0);
  				m_Spirit[i]->Animate(view_id, TRUE);
			}
		}
	  }
	}

  for (i=0;i<3;i++)
	if (m_Spirit[i])
	{
		m_Spirit[i]->ON();
	}

	m_NewSpirit = FALSE;

	VIEW::Paint(etime, flags);
}

//----------------------------------------------------------------------------
int INVENTORY::Keycmd(int input, int flags)
{
	if (m_count<=1)
		return input;

	if (!(flags & KEY_PRESSED))
		return input;

	/// show the next/prev object:
    switch( input )
    {
    case KEY_RIGHT:
	case KEY_DOWN:
		m_position++;
		if (m_position >= m_count)
			m_position = 0;
		input = 0;
		break;
    case KEY_LEFT:
    case KEY_UP:
		m_position--;
		if (m_position < 0)
			m_position = m_count-1;
		input = 0;
        break;
    }

	m_NewSpirit = TRUE;
	Invalidate();
	return input;

} /* ProcessInput */

//----------------------------------------------------------------------------
WORD INVENTORY::QueryCursor(SRECT r)
{
  return IDS_HANDMACH;
}

//----------------------------------------------------------------------------
WORD INVENTORY::Grab(POINT p, BOOL bClickOnly, POINT *obj_pos)
{
	WORD obj_def_id = 0;

	if (!m_count || bClickOnly)
		return 0;

	obj_def_id = m_objects[m_position];
	m_objects[m_position] = 0;

	// if this was not the last object in the inventory,
	// then collapse the array:
	if (m_position < (m_count-1))
		memmove(&m_objects[m_position], &m_objects[m_position+1], (m_count-(m_position+1))*sizeof(WORD));

	m_count--;
	if (m_position >= m_count && m_position > 0)
		m_position--;

	// repaint inventory next time around:
	m_NewSpirit = TRUE;

	obj_pos->x = 70;
	obj_pos->y = Y_POS;
	*obj_pos = MapToScreen(*obj_pos);

	return obj_def_id;
}

//----------------------------------------------------------------------------
BOOL INVENTORY::Drop(WORD *obj_def_id, POINT p)
{
	if (m_count >= MAX_INV_OBJECTS)
	{
		return FALSE;
	}

	// shift the other objects out of the way:
	if (m_count)
		memmove(&m_objects[m_position+1], &m_objects[m_position], (m_count-m_position)*sizeof(WORD));

	m_objects[m_position] = *obj_def_id;
	*obj_def_id = 0;

	m_count++;

	m_NewSpirit = TRUE;
	Invalidate();
	return TRUE;
}

//----------------------------------------------------------------------------
static void invFree(INVENTORY *inventory)
{
	if (inventory)
	{
		delete inventory;
	}
}

//----------------------------------------------------------------------------
INVENTORY::INVENTORY(int view_id):
VIEW(view_id, "Inventory", BLT_PORTAL)
{
  int i;
  for (i=0;i<3;i++)
	m_Spirit[i] = NULL;

	m_NewSpirit = FALSE;
	m_count = 0;
	m_position = 0;

	fbd.m_src_x = 32; // half the left image is off the display.
}

//----------------------------------------------------------------------------
INVENTORY *invLoad(int view_id)
{
	INVENTORY *inv = new INVENTORY(view_id);
	return inv;
}

