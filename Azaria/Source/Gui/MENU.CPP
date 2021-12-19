/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       menu.c
;
; Description:
;	implements menus.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 03/04/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "maya.h"
#include "viewman.h"
#include "controls.h"

#include "Data/Database/idv.h"

extern BOOL bPostQuitMessage;

class MENU {
public:
	int menu_id;
	int parent_menu_id;
	char title[20];
	char background[20];
};

class MENU_ITEM {
public:
	int menu_id;
	char string[20];
	int (*function)(ZLISTBOX *panel, int parm1);
	int parm1;
};

int menuCallMenu(ZLISTBOX *panel, int menu_id);
int menuCallView(ZLISTBOX *panel, int view_id);
int menuEsc(ZLISTBOX *panel, int parm1);

#include "Data/Database/MENU.H"

static int tbl_menus_count = sizeof(tbl_menus) / sizeof(MENU);
static int tbl_menu_items_count = sizeof(tbl_menu_items) / sizeof(MENU_ITEM);

int menuCallView(ZLISTBOX *panel, int view_id)
{
	return viewman.Load(view_id) != NULL;
}

int menuEsc(ZLISTBOX *panel, int parm1)
{
	bPostQuitMessage = TRUE;
	return FALSE;
}

MENU *menuGet(int menu_id)
{
	int i;
	for (i=0;i<tbl_menus_count;i++)
	{
		if (tbl_menus[i].menu_id == menu_id)
			return &tbl_menus[i];
	}
	return NULL;
}

int menuGetItemCount(int menu_id)
{
	int i;
	int count=0;
	for (i=0;i<tbl_menu_items_count;i++)
	{
		if (tbl_menu_items[i].menu_id == menu_id)
			count++;
	}
	return count;
}

char *menuGetItemName(ZLISTBOX *panel, int position)
{
	int i;
	int item_id = 0;

	for (i=0;i<tbl_menu_items_count;i++)
	{
		if (tbl_menu_items[i].menu_id == panel->menu_id)
		{
			if (item_id == position)
				return tbl_menu_items[i].string;
			else item_id++;
		}
	}
	return "------------";
}

int menuSelect(ZLISTBOX *panel, int position)
{
	int i;
	int item_id = 0;

	for (i=0;i<tbl_menu_items_count;i++)
	{
		if (tbl_menu_items[i].menu_id == panel->menu_id)
		{
			if (item_id == position)
			{
				return tbl_menu_items[i].function(panel, tbl_menu_items[i].parm1);
			}
			else item_id++;
		}
	}
	return FALSE;
}

static int CurrentMenu = IDM_MAINMENU;

menuCallMenu(ZLISTBOX *panel, int menu_id)
{
	MENU *menu = menuGet(menu_id);

	if (!menu)
		return FALSE;

	panel->menu_id = menu_id;

	CurrentMenu = menu_id;

	panel->ChangeSurface(menu->background);

	panel->Init(menuGetItemCount(menu_id), menu->title);
	panel->ChangeSel(0);

	return TRUE;
}

// loads a menu heirarchy.  User must traverse down
// the menu heirarchy to get to all other menus.
ZLISTBOX *menuLoad(int view_id)
{
	ZLISTBOX *lb;
	int count;
	int menu_id = CurrentMenu;
	MENU *menu = menuGet(menu_id);

	if (!menu)
		return NULL;

	// ### lb = panelLoad(view_id);
	lb = new ZLISTBOX(view_id);
	if (!lb)
		return NULL;

	lb->menu_id = menu_id;

	// Function LB calls to fill in list box:
	lb->StrVal = menuGetItemName;
	// Function LB calls when an item is selected:
	lb->Select = menuSelect;
	// Function LB calls when ECS is hit:
	lb->Esc = menuEsc;

	count = menuGetItemCount(menu_id);

	lb->Init(count, menu->title);
	lb->ChangeSel(0);

	return lb;
}
