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

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>

static const char *provStrings(int position);
static BOOL provSelect(int item);

static const char *sesStrings(int position);
static BOOL sesSelect(int item);

int CFGNET::lastProvider = 0;
HANDLE CFGNET::Die;
HANDLE CFGNET::Dead;

//----------------------------------------------------------------------------
const char *provStrings(int position)
{
	return netman.GetProviderDesc(position);
}

//----------------------------------------------------------------------------
const char *sesStrings(int position)
{
	return netman.GetSessionDesc(position);
}

#define LPSECURITY_ATTRIBUTES WORD
#define RC_OBJECT_NOT_CREATED 0
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t      condition;
    int         semCount;
}sem_struct;

HANDLE CloseSemaphore(HANDLE handle)
{
    sem_struct *token = (sem_struct *) handle;
    pthread_mutex_destroy(&(token->mutex));
    pthread_cond_destroy(&(token->condition));
    free(token);
    return 0;
}
HANDLE CreateSemaphore (LPSECURITY_ATTRIBUTES   lpSemaphoreAttributes,
                        LONG            lInitialCount,
                        LONG            lMaximunCount,
                        const char *   lpName)
{
    int rc;
    sem_struct *token;
    token = (sem_struct *) malloc(sizeof(sem_struct));
    rc = pthread_mutex_init(&(token->mutex), NULL);
    if(rc)
    {
        free(token);
        return RC_OBJECT_NOT_CREATED;
    }
    rc = pthread_cond_init(&(token->condition), NULL);
    if(rc)
    {
        pthread_mutex_destroy( &(token->mutex) );
        free(token);
        return RC_OBJECT_NOT_CREATED;
    }
    token->semCount = 0;
    return (void *)token;
}
BOOL WINAPI ReleaseSemaphore(HANDLE hSemaphore,
                             LONG   lReleaseCount,
                             LPLONG lpPreviousCount)
{
    return true;
}
int WaitForSingleObject(HANDLE handle, int timeInMs)
{
    struct timespec timeToWait;
    struct timeval now;
    int rt;
    gettimeofday(&now,NULL);
    timeToWait.tv_sec = now.tv_sec+5;
    timeToWait.tv_nsec = (now.tv_usec+1000UL*timeInMs)*1000UL;
    sem_struct *token = (sem_struct *) handle;
    pthread_mutex_lock(&token->mutex);
    rt = pthread_cond_timedwait(&(token->condition), &(token->mutex), &timeToWait);
    pthread_mutex_unlock(&token->mutex);
    return rt;
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

	if (0 == WaitForSingleObject(Die,1000))
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

    pthread_create(&thid, NULL, (Entry)EnumThread, this);
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
         //   char buffer[128];
            const char *gamename = "gethostname";
	  //size_t len = 128;
            // TODO: gethostname(gamename, &len);

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
	CloseSemaphore(Die);
	CloseSemaphore(Dead);
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

