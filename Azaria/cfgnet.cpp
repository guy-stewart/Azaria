/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       cfgview.cpp
;
; Description:
;	Implements the panel style puzzle.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 07/31/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#ifdef _MSC_VER // Windows
#include <windows.h>
#include <direct.h>
#include <process.h>
#else // Linux
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif /*_MSC_VER*/

#include "maya.h"
#include "controls.h"
#include "viewMast.h"
#include "cfgview.h"
#include "viewman.h"
#include "player.h"
#include "teletype.h"

#include "Data/Database/Ids.h"
#include "Data/Database/idv.h"

#include "netctrl.h"
#include "display.h"

char *provStrings(int position);
BOOL provSelect(int item);

char *sesStrings(int position);
BOOL sesSelect(int item);

int CFGNET::lastProvider = 0;
HANDLE CFGNET::Die;
HANDLE CFGNET::Dead;

#ifdef _MSC_VER // Windows
//----------------------------------------------------------------------------
char *provStrings(int position)
{
	return netman.GetProviderDesc(position);
}

//----------------------------------------------------------------------------
char *sesStrings(int position)
{
	return netman.GetSessionDesc(position);
}

//----------------------------------------------------------------------------
void CFGNET::EnumThread(CFGNET *thing)
{
  int count;

  for (;;)
  {
	// go ahead and enumerate all the sessions:
	// display.FlipToGdi(FALSE);
	count = netman.EnumSessions();
	// display.FlipToGdi(TRUE);

	if (WAIT_OBJECT_0 == WaitForSingleObject(Die,1000))
	  break;

	thing->listbox->SetCount(count);
	thing->listbox->Refresh();
  }

  ReleaseSemaphore(Dead,1,0);
}

//----------------------------------------------------------------------------
void CFGNET::OpenEnumThread()
{
  Die = CreateSemaphore(NULL, 0, 1, NULL);
  Dead = CreateSemaphore(NULL, 0, 1, NULL);

	_beginthreadex(NULL, 0, (Entry)EnumThread, this, 0, &thid);
}

//----------------------------------------------------------------------------
void CFGNET::CloseEnumThread()
{

	if (thid)
	{
		ReleaseSemaphore(Die,1,0);
		netman.LostInterest();
		WaitForSingleObject(Dead,5000);
	}
}

//----------------------------------------------------------------------------
CFGNET::CFGNET(int view_id):
CFGVIEW(view_id)
{
	int count;
	thid = 0;

	switch (view_id)
	{
	case IDV_CFGPROV:
	  // go ahead and enumerate all the service providers:
	  display.FlipToGdi(FALSE);
	  count = netman.Enumerate();
	  display.FlipToGdi(TRUE);

	  control[0] = listbox = new LISTBOX(this, IDS_STD_LISTBOXL, 160, 50, LB_BORDER, LB_BORDER, count, provStrings);
	  listbox->ChangeSel(lastProvider);
	  control[1] = new BUTTON(this, 1, 160, 250, IDS_BTN_OK, IDS_BTN_OK_HI);
	  control[2] = new BUTTON(this, 2, 240, 250, IDS_BTN_CANCEL, IDS_BTN_CANCEL_HI);
	  control_count = 3;

	  break;
	case IDV_CFGSESS:
	  control[0] = listbox = new LISTBOX(this, IDS_STD_LISTBOXM, 160, 50, LB_BORDER, LB_BORDER, 0, sesStrings);
	  control[1] = new BUTTON(this, 1, 160, 250, IDS_BTN_OK, IDS_BTN_OK_HI);
	  control[2] = new BUTTON(this, 2, 240, 250, IDS_BTN_CANCEL, IDS_BTN_CANCEL_HI);
	  control[3] = new BUTTON(this, 3, 300, 250, IDS_BTN_NEW, IDS_BTN_NEW_HI);
	  control_count = 4;

	  OpenEnumThread();
	  break;
	case IDV_CFGNEWSESS:
	  char gamename[128];
	  unsigned long len = 128;
	  GetComputerName(gamename, &len);

	  control[0] = new EDITBOX(this, IDS_STD_EDITBOXM, 160, 50, LB_BORDER, LB_BORDER, gamename, 16);
	  control[1] = new BUTTON(this, 1, 160, 250, IDS_BTN_OK, IDS_BTN_OK_HI);
	  control[2] = new BUTTON(this, 2, 240, 250, IDS_BTN_CANCEL, IDS_BTN_CANCEL_HI);

	  control_count = 3;
	  break;
	}
}

//----------------------------------------------------------------------------
CFGNET::~CFGNET()
{
  switch (view_id)
  {
  case IDV_CFGPROV:
	lastProvider = listbox->GetIntValue();
	break;
  case IDV_CFGSESS:
	CloseHandle(Die);
	CloseHandle(Dead);
	break;
  case IDV_CFGNEWSESS:
	break;
  }
}

static void Report(int hr)
{
	TELETYPE::Msg(0, "Network: %s.", netman.error(hr, TRUE));
}

//----------------------------------------------------------------------------
void CFGNET::OnEvent(int control_id, int event_id)
{
  HRESULT hr;

  switch (view_id)
  {
  case IDV_CFGPROV:
	switch (control_id)
	{
	case 1: // OK
	  {
	  display.FlipToGdi(FALSE);
	  hr = netman.Create(control[0]->GetIntValue());
	  display.FlipToGdi(TRUE);
	  if (hr == DP_OK)
			viewman.Load(IDV_CFGSESS);
	  else
		Report(hr);
	  }
	  break;
	case 2: // Cancel
	  viewman.Load(IDV_TOPMENU);
	  break;
	}
	break;
  case IDV_CFGSESS:
	switch (control_id)
	{
	case 1: // OK
	  {
		CloseEnumThread();

		display.FlipToGdi(FALSE);
		hr = netman.Open(control[0]->GetIntValue(), "", DPOPEN_OPENSESSION);
		display.FlipToGdi(TRUE);

		if (hr == DP_OK)
		{
			netman.CreatePlayer(local_player->GetName(), "formal name");
			if (hr == DP_OK)
			{
				if (local_player->GetCult() == CULT_CITY)
					viewman.Load(IDV_ctysmith);
				else
					viewman.Load(IDV_V1PAN);
				break;
			}
		}
		// getting here means there was an error above
		Report(hr);
		OpenEnumThread();
	  }
	  break;
	case 2: // Cancel
	  CloseEnumThread();
	  viewman.Load(IDV_CFGPROV);
	  break;
	case 3: // New
	  CloseEnumThread();
	  viewman.Load(IDV_CFGNEWSESS);
	  break;
	}
	break;
  case IDV_CFGNEWSESS:
	switch (control_id)
	{
	case 1: // OK
	  {
		HRESULT hr;

		CloseEnumThread();

		display.FlipToGdi(FALSE);
		hr = netman.Open(0, control[0]->GetStrValue(), DPOPEN_CREATESESSION);
		display.FlipToGdi(TRUE);

		if (hr == DP_OK)
		{
			hr = netman.CreatePlayer(local_player->GetName(), "formal name");
			if (hr == DP_OK)
			{
				if (local_player->GetCult() == CULT_CITY)
					viewman.Load(IDV_ctysmith);
				else
					viewman.Load(IDV_V1PAN);
			}
			else
			  Report(hr);
		}
		else
		  Report(hr);
	  }
	  break;
	case 2: // Cancel
	  viewman.Load(IDV_CFGSESS);
	  break;
	}
	break;
  }
  return;
}
#endif
