/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       ctrlview.cpp
;
; Description:
;	Implements the control view style puzzle.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 08/20/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifdef _MSC_VER // Windows
#include <io.h>
#include <direct.h>
#endif

#include "maya.h"
#include "dsWave.h"
#include "gameman.h"
#include "music.h"
#include "controls.h"
#include "viewMast.h"
#include "cfgview.h"
#include "viewman.h"

#include "gdb.h"
#include "player.h"

#include "Data/Database/Ids.h"
#include "Data/Database/idv.h"

#include "netctrl.h"

#include "user.h"

CTRL_VIEW* CTRL_VIEW::myname;
CTRL_VIEW* CTRL_VIEW::yourname;

//----------------------------------------------------------------------------
static const char *PlayerName(int index)
{
  PLAYER *player = gdbt_players.GetPlayer(index);

  if (player)
	return player->GetName();
  else
	return "(dead)";
}

//----------------------------------------------------------------------------
CTRL_VIEW::CTRL_VIEW(int view_id):
CFGVIEW(view_id)
{
  int count;

	switch (view_id)
	{
	case IDV_PLAYERNAME:
	  myname = this;
	  control[0] = new EDITBOX(this, IDS_NAMEPLATE, 0, 0, 0, 0, local_player->GetName(), MAX_NAME_LEN-1);
  	  control_count = 1;
	  break;
	case IDV_OTHERNAME:
	  yourname = this;
	  control[0] = new EDITBOX(this, IDS_NAMEPLATE, 0, 0, 0, 0, "", MAX_NAME_LEN-1);
  	  control_count = 1;
	  break;
	case IDV_STRATMON1:
	  {
		POINT p;
		VIEW *view = viewman.Load(IDV_STRATMAP);
		p.x=0;
		p.y=0;
		p = MapToScreen(p);
		view->Move(p.x,p.y);
  		control_count = 0;
	  }
	  break;
	case IDV_STRATMON2:
	  count = gdbt_players.Count();
	  control[0] = new LISTBOX(this, IDS_STRAT_LB, 162, 27, 0, 0, count, PlayerName);
	  control[1] = new BUTTON(this, 2, 218, 250, IDS_BTN_UP, IDS_BTN_UP_HI);
	  control[2] = new BUTTON(this, 3, 417, 250, IDS_BTN_DOWN, IDS_BTN_DOWN_HI);
	  //control[4] = new BUTTON(this, 4, 320, 250, IDS_BTN_LOAD, IDS_BTN_LOAD_HI);
  	  control_count = 3;
	  break;
	}
}

//----------------------------------------------------------------------------
CTRL_VIEW::~CTRL_VIEW()
{
	switch (view_id)
	{
	case IDV_STRATMON1:
	  viewman.Release(IDV_STRATMAP);
	  break;
	}
}

//----------------------------------------------------------------------------
void CTRL_VIEW::SetName(const char *name)
{
  CONTROL *ctrl = control[0];

  ctrl->SetStrValue(name);
  ctrl->Refresh();
}

//----------------------------------------------------------------------------
void CTRL_VIEW::OnEvent(int control_id, int iParm)
{
}

void CTRL_VIEW::Paint(int etime, int flags) {
  CONTROL *ctrl = control[0];
  ctrl->Refresh();
}
