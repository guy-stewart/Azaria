/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       cfgAV.cpp
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

#include <io.h>
#include <direct.h>

#include "maya.h"
#include "dsWave.h"
#include "gameman.h"
#include "music.h"
#include "controls.h"
#include "viewMast.h"
#include "cfgview.h"
#include "viewman.h"
#include "player.h"

#include "data\database\ids.h"
#include "data\database\idv.h"

#include "netctrl.h"

#include "user.h"

//----------------------------------------------------------------------------
CFGAV::CFGAV(int view_id):
CFGVIEW(view_id)
{
  int x;

	switch (view_id)
	{
	case IDV_CFGAV:
	  control[0] = new BUTTON(this, 0, 120, 240, IDS_BTN_OK, IDS_BTN_OK_HI);

	  x = (Game::GetMusicVolume()*11)/10;
	  control[1] = new LABEL(this, 1, 70, 80, IDS_TXT_MUSIC);
	  control[2] = new SLIDER(this, 2, 190, 90, x, 300,10,0,110, IDS_BTN_LEFT, IDS_BTN_LEFT_HI);

	  x = (Game::GetSfxVolume()*11)/10;
	  control[3] = new LABEL(this, 3, 70, 160, IDS_TXT_SFX);
	  control[4] = new SLIDER(this, 4, 190, 170, x, 300,10,0,110, IDS_BTN_LEFT, IDS_BTN_LEFT_HI);

	  control_count = 5;
	  break;
	}
}

//----------------------------------------------------------------------------
void CFGAV::OnEvent(int control_id, int iParm)
{
  CONTROL *ctrl = control[control_id];

  switch (view_id)
  {
  case IDV_CFGAV:
	switch (control_id)
	{
	case 0: // OK
	  viewman.Load(IDV_TOPMENU);
	  break;

	case 2: // music volume was changed
	  Game::SetMusicVolume((ctrl->GetIntValue()*10)/11); // "but this one goes to 11"
	  break;

	case 4: // sfx volume was changed
	  Game::SetSfxVolume((ctrl->GetIntValue()*10)/11); // "but this one goes to 11"
	  break;
	}
	break;
  }
  return;
}

