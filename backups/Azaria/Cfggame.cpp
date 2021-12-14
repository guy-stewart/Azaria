/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       cfgplayer.cpp
;
; Description:
;	Implements the panel style puzzle.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 08/03/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#ifdef _MSC_VER // Windows
#include <io.h>
#include <direct.h>
#endif

#include "maya.h"
#include "paths.h"

#include "controls.h"
#include "viewMast.h"
#include "cfgview.h"
#include "viewman.h"
#include "gdb.h"
#include "machterp.h"
#include "gameman.h"

#include "Data/Database/Ids.h"
#include "Data/Database/idv.h"

#include "netctrl.h"

#include "user.h"

//----------------------------------------------------------------------------
CFGGAME::CFGGAME(int view_id):
CFGVIEW(view_id)
{
  int count;

	switch (view_id)
	{
	case IDV_CFGGAME:
	  count = CfgDir(0, gamePath, gameExt);
	  control[0] = games = new LISTBOX(this, IDS_STD_LISTBOXM, 320, 50, LB_BORDER, LB_BORDER, count, CfgStrings);
	  control[1] = new EDITBOX(this, IDS_STD_EDITBOXM, 20, 100, LB_BORDER, LB_BORDER, Game::GetGameName(), MAX_NAME_LEN-1);
	  control[2] = new BUTTON(this, 2, 60, 230, IDS_BTN_OK, IDS_BTN_OK_HI);
	  control[3] = new BUTTON(this, 3, 80, 150, IDS_BTN_SAVE, IDS_BTN_SAVE_HI);
	  control[4] = new BUTTON(this, 4, 320, 250, IDS_BTN_LOAD, IDS_BTN_LOAD_HI);
	  control[5] = new BUTTON(this, 5, 380, 250, IDS_BTN_DELETE, IDS_BTN_DELETE_HI);
	  control[6] = new BUTTON(this, 6, 20, 150, IDS_BTN_NEW, IDS_BTN_NEW_HI);

	  control_count = 7;
	  break;
	}
}

//----------------------------------------------------------------------------
void CFGGAME::OnEvent(int control_id, int event_id)
{
  char path[_MAX_PATH];
  const char *name;

  switch (view_id)
  {
  case IDV_CFGGAME:
	switch (control_id)
	{
	case 2: // OK
	  viewman.Load(IDV_TOPMENU);
	  Game::SetGameName(control[1]->GetStrValue());
	  break;
	case 3: // SAVE
	  Game::Save(control[1]->GetStrValue());
	  games->SetCount(CfgDir(0, gamePath, gameExt));
	  games->Refresh();
	  break;
	case 4: // LOAD
	  name = CfgStrings(control[0]->GetIntValue());
	  if (Game::Load(name))
	  {
		control[1]->SetStrValue(name);
		control[1]->Refresh();
	  }
	  break;
	case 5: // DELETE
	  name = CfgStrings(control[0]->GetIntValue());
	  makepath(path, NULL, gamePath, name, gameExt);
	  unlink(path);

	  games->SetCount(CfgDir(0, gamePath, gameExt));
	  games->Refresh();
	  break;
	case 6: // NEW
	  Game::New();
	  Game::SetGameName("New");
	  control[1]->SetStrValue("New");
	  break;
	}
	break;
  }
  return;
}

