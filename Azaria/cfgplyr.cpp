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

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#endif

#include "maya.h"
#include "paths.h"
#include "controls.h"
#include "viewMast.h"
#include "cfgview.h"
#include "viewman.h"
#include "player.h"

#include "Data/Database/Ids.h"
#include "Data/Database/idv.h"

#include "netctrl.h"

#include "user.h"

//----------------------------------------------------------------------------
CFGPLYR::CFGPLYR(int view_id):
CFGVIEW(view_id)
{
  int count;

	switch (view_id)
	{
	case IDV_CFGPLAYER1:
	  count = CfgDir(0, plyrPath, plyrExt);
	  control[0] = egos = new LISTBOX(this, IDS_STD_LISTBOXM, 320, 50, LB_BORDER, LB_BORDER, count, CfgStrings);
	  control[1] = new EDITBOX(this, IDS_STD_EDITBOXM, 20, 100, LB_BORDER, LB_BORDER, local_player->GetName(), MAX_NAME_LEN-1);
	  control[2] = new BUTTON(this, 2, 60, 230, IDS_BTN_OK, IDS_BTN_OK_HI);
	  control[3] = new BUTTON(this, 3, 80, 150, IDS_BTN_SAVE, IDS_BTN_SAVE_HI);
	  control[4] = new BUTTON(this, 4, 320, 250, IDS_BTN_LOAD, IDS_BTN_LOAD_HI);
	  control[5] = new BUTTON(this, 5, 380, 250, IDS_BTN_DELETE, IDS_BTN_DELETE_HI);
	  control[6] = new BUTTON(this, 6, 20, 150, IDS_BTN_NEW, IDS_BTN_NEW_HI);
	  control_count = 7;
	  break;

	case IDV_CFGPLAYER2:
	  control[0] = new BUTTON(this, 0, 60, 230, IDS_BTN_OK, IDS_BTN_OK_HI);
	  control[1] = btn_vill = new CHECKBOX(this, 1, 220, 60, IDS_BTN_VILCULT, IDS_BTN_VILCULT_HI);
	  control[2] = btn_city = new CHECKBOX(this, 2, 420, 60, IDS_BTN_CITCULT, IDS_BTN_CITCULT_HI);
	  control[3] = btn_fem  = new CHECKBOX(this, 3, 220, 120, IDS_BTN_FEMALE, IDS_BTN_FEMALE_HI);
	  control[4] = btn_male = new CHECKBOX(this, 4, 420, 120, IDS_BTN_MALE, IDS_BTN_MALE_HI);
	  control_count = 5;

	  BOOL cult_city = CULT_CITY == local_player->GetCult();
	  BOOL female = FEMALE == local_player->GetSex();
  	  btn_city->SetChecked(cult_city);
	  btn_vill->SetChecked(!cult_city);
  	  btn_fem->SetChecked(female);
	  btn_male->SetChecked(!female);

	  break;
	}
}

//----------------------------------------------------------------------------
void CFGPLYR::OnEvent(int control_id, int iParm)
{
  char path[_MAX_PATH];
  char *name;

  switch (view_id)
  {
  case IDV_CFGPLAYER1:
	switch (control_id)
	{
	case 2: // OK
	  local_player->SetName(control[1]->GetStrValue());
	  viewman.Load(IDV_TOPMENU);
	  break;
	case 3: // SAVE
	  makepath(path, NULL, plyrPath, control[1]->GetStrValue(), plyrExt);
	  local_player->Save(path);

	  egos->SetCount(CfgDir(0, plyrPath, plyrExt));
	  egos->Refresh();
	  break;
	case 4: // LOAD
	  name = CfgStrings(control[0]->GetIntValue());
	  makepath(path, NULL, plyrPath, name, plyrExt);
	  local_player->Load(path);
	  local_player->SetName(name);
	  control[1]->SetStrValue(name);
	  control[1]->Refresh();
	  break;
	case 5: // DELETE
	  name = CfgStrings(control[0]->GetIntValue());
	  makepath(path, NULL, plyrPath, name, plyrExt);
	  unlink(path);

	  egos->SetCount(CfgDir(0, plyrPath, plyrExt));
	  egos->Refresh();
	  break;
	case 6: // NEW
	  viewman.Load(IDV_CFGPLAYER2);
	  break;
	}
	break;
  case IDV_CFGPLAYER2:
	switch (control_id)
	{
	case 0: // OK
	  viewman.Load(IDV_CFGPLAYER1);
	  break;
	case 1: // 
	  btn_city->SetChecked(FALSE);
	  btn_vill->SetChecked(TRUE);
	  local_player->SetCult(CULT_VILLAGE);
	  break;
	case 2: // 
	  btn_city->SetChecked(TRUE);
	  btn_vill->SetChecked(FALSE);
	  local_player->SetCult(CULT_CITY);
	  break;
	case 3: // 
	  btn_male->SetChecked(FALSE);
	  btn_fem->SetChecked(TRUE);
	  local_player->SetSex(FEMALE);
	  break;
	case 4: // 
	  btn_male->SetChecked(TRUE);
	  btn_fem->SetChecked(FALSE);
	  local_player->SetSex(MALE);
	  break;
	}
	break;
  }
  return;
}

