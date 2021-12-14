/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       provider.c
;
; Description:
;	generates a list of service providers,  allows the player to select one,
;	and loads the selected service provider.
;	
;	This thing is based on a standard panel view.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 02/26/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#include "maya.h"

#include "viewman.h"
#include "controls.h"
#include "player.h"
#include "netctrl.h"
#include "display.h"

#include "data/database/idv.h"

//----------------------------------------------------------------------------
int provSelect(ZLISTBOX *panel, int item)
{
	HRESULT hr;
	hr = netman.Create(panel->Selected);
	if (hr != DP_OK)
	{
		panel->message("Unable to load that service provider");
		return 0;
	}

	viewman.Load(IDV_SESSIONS);

	return 0;
}

//----------------------------------------------------------------------------
char *provStrings(ZLISTBOX *panel, int position)
{
	return netman.GetProviderDesc(position);
}

//----------------------------------------------------------------------------
ZLISTBOX *provLoad(int view_id)
{
	int count;	
	ZLISTBOX *panel;
	panel = new ZLISTBOX(view_id);

	panel->StrVal = provStrings;
	panel->Select = provSelect;

	display.FlipToGdi(FALSE);
	count = netman.Enumerate();
	display.FlipToGdi(TRUE);

	panel->Init(count, "Select Network");
	panel->ChangeSel(0);

	return panel;
}

//----------------------------------------------------------------------------
int sesSelect(ZLISTBOX *panel, int item)
{
  HRESULT hr;

	if (panel->MaxSel == 0)
	{
		viewman.Load(IDV_NEWSESSION);
	}
	else
	{
		display.FlipToGdi(FALSE);
	  	hr = netman.Open(panel->Selected, "", DPOPEN_OPENSESSION);
		display.FlipToGdi(TRUE);

		if (hr == DP_OK)
			viewman.Load(IDV_NEWPLAYER);
		else
		  panel->message("Session open failed");
	}

	return 0;
}

//----------------------------------------------------------------------------
char *sesStrings(ZLISTBOX *panel, int position)
{
	return netman.GetSessionDesc(position);
}

//----------------------------------------------------------------------------
void sesNew(EDITBOX *panel)
{
	display.FlipToGdi(FALSE);
	netman.Open(0, panel->BUFF, DPOPEN_CREATESESSION);
	display.FlipToGdi(TRUE);
	viewman.Load(IDV_NEWPLAYER);
}

//----------------------------------------------------------------------------
void playerNew(EDITBOX *panel)
{
	netman.CreatePlayer(panel->BUFF, "formal name");
	viewman.Load(IDV_MENUSYSTEM);
}

//----------------------------------------------------------------------------
EDITBOX *newsesLoad(int view_id)
{
	EDITBOX *panel;
	panel = new EDITBOX(view_id);

	switch (view_id)
	{
	case IDV_NEWSESSION:
	  {
		char username[128];
		unsigned long len = 128;
		GetComputerName(username, &len);
		panel->Init(username, DPSESSIONNAMELEN, "Session Name:");
		panel->Enter = sesNew;
	  }
	  break;
	case IDV_NEWPLAYER:
	  {
		panel->Init(local_player->friendly_name, DPSESSIONNAMELEN, "Player Name:");
		panel->Enter = playerNew;
	  }
	  break;
	}

	return panel;
}

//----------------------------------------------------------------------------
int playerSelect(ZLISTBOX *panel, int item)
{
	return 0;
}

//----------------------------------------------------------------------------
ZLISTBOX *sesLoad(int view_id)
{
	int count;	
	char *title = "sesLoad";
	ZLISTBOX *panel;
	// panel = panelLoad(view_id);
	panel = new ZLISTBOX(view_id);

	switch (view_id) 
	{
	case IDV_SESSIONS:
		// Function LB calls to fill in list box:
		panel->StrVal = sesStrings;
		// Function LB calls when an item is selected:
		panel->Select = sesSelect;
		count = netman.EnumSessions();
		title = "Select a game";
		break;
	}


	panel->Init(count, title);
	panel->ChangeSel(0);

	return panel;
}



