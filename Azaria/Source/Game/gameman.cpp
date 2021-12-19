
/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; 
; File:       gameman.cpp
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "includes.h"
#include "netctrl.h"
#include "tables.h"
#include "spr_pool.h"
#include "spirit.h"

#include "spell.h"

#include "user.h"
#include "music.h"
#include "gdb.h"
#include "viewman.h"
#include "machterp.h"
#include "teletype.h"

#include "player.h"
#include "gameman.h"
#include "display.h"
#include "cursor.h"

#include "Data/Database/idv.h"
#include "Data/Database/Ids.h"

const char *plyrPath = "Egos";
const char *plyrExt = "ego";

const char *gamePath = "Games";
const char *gameExt = "Mx7";
#define NewGameName	"New"
#define NewGameExt	""

char ViewsPath[MAX_PATH];
char ObjectsPath[MAX_PATH];
char PalettesPath[MAX_PATH];
char MasksPath[MAX_PATH];
char WavePath[MAX_PATH];

char AltViewsPath[MAX_PATH];
char AltObjectsPath[MAX_PATH];
char AltPalettesPath[MAX_PATH];
char AltMasksPath[MAX_PATH];
char AltWavePath[MAX_PATH];

Game gameman; // The one and only game manager.

DWORD Game::c_owner;
int Game::sfxVolume = 80;
int Game::musicVolume = 80;
char Game::gameName[MAX_GAME_NAME];

static DWORD m_last_view_time;    // last time we updated the views
static DWORD m_last_agenda_time;    // last time we updated the agenda
static DWORD m_last_poll_time;

//----------------------------------------------------------------------------
Game::Game()
{
}

//----------------------------------------------------------------------------
void Game::InitPaths(const char * datapath)
{
    strcpy(ViewsPath, datapath);
    strcpy(ObjectsPath, datapath);
    strcpy(PalettesPath, datapath);
    strcpy(MasksPath, datapath);
    strcpy(WavePath, datapath);
    
    strcat(ViewsPath, __ViewsPath);
    strcat(ObjectsPath, __ObjectsPath);
    strcat(PalettesPath, __PalettesPath);
    strcat(MasksPath, __MasksPath);
    strcat(WavePath, __WavePath);
    
    strcpy(AltViewsPath, datapath);
    strcpy(AltObjectsPath, datapath);
    strcpy(AltPalettesPath, datapath);
    strcpy(AltMasksPath, datapath);
    strcpy(AltWavePath, datapath);
    
    strcat(AltViewsPath, __ViewsPath);
    strcat(AltObjectsPath, __ObjectsPath);
    strcat(AltPalettesPath, __PalettesPath);
    strcat(AltMasksPath, __MasksPath);
    strcat(AltWavePath, __WavePath);
}

//----------------------------------------------------------------------------
Game::~Game()
{
  char path[_MAX_PATH];

/*
  { // Set the last player for the current user
	  Result = RegSetValueEx(Key, "PlayerName", NULL, REG_SZ, (LPBYTE)local_player->GetName(), strlen(local_player->GetName())+1);
	  Result = RegSetValueEx(Key, "GameName", NULL, REG_SZ, (LPBYTE)gameName, strlen(gameName)+1);
	  Result = RegSetValueEx(Key, "prefsMusicVolume", NULL, REG_DWORD, (LPBYTE)&musicVolume, sizeof(DWORD));
	  Result = RegSetValueEx(Key, "prefsSfxVolume", NULL, REG_DWORD, (LPBYTE)&sfxVolume, sizeof(DWORD));
  }
*/
  makepath(path, NULL, plyrPath, local_player->GetName(), plyrExt);
  local_player->Save(path);

  makepath(path, NULL, gamePath, gameName, gameExt);
  gdbt_machines.Save(path);
}

//----------------------------------------------------------------------------
void Game::SetMusicVolume(int volume)
{
	musicVolume = volume;
	Wave::SetMusicVolume(volume);
	Music::SetVolume(volume);
}

//----------------------------------------------------------------------------
void Game::SetSfxVolume(int volume)
{
	sfxVolume = volume;
	Wave::SetFxVolume(volume);
}

//----------------------------------------------------------------------------
void Game::LoadLRU()
{
#define ERROR_SUCCESS = 0
  char path[_MAX_PATH];
  { // get the last player for the current user
	  char name[MAX_FRIENDLY_NAME];
	  DWORD DataSize = MAX_FRIENDLY_NAME;

	  //------------------------------------------
	  // load the last player used:
      printf("TODO: get user name\n");
      strcpy(name, "Shilla");
	  local_player->SetName(name);
	  makepath(path, NULL, plyrPath, name, plyrExt);
	  WORD recent_view = local_player->Load(path);
	  if (recent_view)
		viewman.Load(recent_view);

	  //------------------------------------------
	  // Save 'new' game, load LRU game:
	  // for starters, save the c_init game so they can start a 'New' game:
	  makepath(path, NULL, gamePath, NewGameName, NewGameExt);
	  gdbt_machines.Save(path);

	  DataSize = MAX_GAME_NAME;
	  // load the last game played:
      // TODO: load last game played
      strcpy(gameName, "Azaria");

	  // Load the game they were playing last
	  makepath(path, NULL, gamePath, gameName, gameExt);
	  gdbt_machines.Load(path);

      // TODO: get saved volume:
      musicVolume = 50; //?????
	  SetMusicVolume(musicVolume);
      sfxVolume = 50; //???
	  SetSfxVolume(sfxVolume);
  }
}

// Returns the system time, in milliseconds
long long print_current_time_with_ms (void)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    //do stuff
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    
    return delta_us;
}

// TODO: this should be 64 bit.
DWORD timeGetTime()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    
    uint64_t us = (t.tv_sec) * 1000000 + (t.tv_nsec) / 1000;
    
    return us / 1000;
}

//----------------------------------------------------------------------------
BOOL Game::Init()
{
	// init elapsed time vars for viewman and agenda
	m_last_view_time = 
	m_last_agenda_time = 
    m_last_poll_time = timeGetTime();

	// present the initial views to the player:
	viewman.Load(IDV_MAIN_PANEL);
	viewman.Load(IDV_PLAYERNAME);
	viewman.Load(IDV_OTHERNAME);
	viewman.Load(IDV_INVENTORY);
	viewman.Load(IDV_ID);
	viewman.Load(IDV_OTHERID);
	viewman.Load(IDV_S_TELETYPE); // the small teletype.
	viewman.Load(IDV_TOPMENU);
	viewman.Load(IDV_COMPASS);

	LoadLRU();

	// give the player a cursor:
	cursorman.Init();

	// Let there be light:
	gdbt_machines.Init();

	// and a little mood music:
	Music::Init();

	return TRUE;
}

//----------------------------------------------------------------------------
void Game::LoadView(WORD view_id)
{
	viewman.Load(view_id);
}

//----------------------------------------------------------------------------
void Game::Cleanup()
{
}

//----------------------------------------------------------------------------
void Game::RestoreDisplay()
{
	display.SetAllDirty();
	cursorman.RestoreDisplay();
}

//----------------------------------------------------------------------------
void Game::Tick(BOOL display_available)
{

	DWORD time, etime;
	time = timeGetTime();

	// compute elapsed time for views
	etime = time-m_last_view_time;
	// ### This is to protect us from overwriting the display 
	// for vga cards that cannot tell us the backbuffer has not yet flipped.
	//if (etime > 30) 
	{
		if (etime > 250) etime = 250;

		m_last_view_time = time;

		if (display_available)
		{
			CheckCursor();

			viewman.Tick(etime);
  			SPIRIT::Tick(etime);
			display.Flip();
		}
	}

	// compute elapsed time for agenda
	etime = time-m_last_agenda_time;
	if (etime > 500)
	{
		m_last_agenda_time = time;
		gdbt_machines.Tick(etime);

		WIZARD::Tick(etime);

		// By the way,  update remote nodes as to player orientation:
		local_player->Reflect();
	}

	// see what the rest of the world has to say
	etime = time-m_last_poll_time;
	if (etime > 100) // poll every 100 milliseconds
	{
	  m_last_poll_time = time;
	  GDB_TABLE::Poll();
	}
}

//----------------------------------------------------------------------------
// This gets called when a new player joins the game
void Game::AddPlayer(DPID pid, BOOL bOwner)
{
  gdbt_players.AddPlayer(pid, bOwner);
}

//----------------------------------------------------------------------------
void Game::DelPlayer(DPID pid)
{
  PLAYER *player = gdbt_players.Get(pid);

  TELETYPE::DelPlayer(pid);

  gdbt_players.DelPlayer(pid);

  if (pid == GetOwner())
  {
	SessionLost();
  }
}

//----------------------------------------------------------------------------
void Game::JoinNetGame(DPID pid, char *name, BOOL bOwner)
{
	local_player->SetPid(pid);

	if (bOwner)
	{
		// set the game owners pid to us.
		Msg("Setting database owner to %d", pid);
		SetOwner(pid);
	}
	else
	{
		// initialize the player table:
		netman.EnumPlayers();

		Msg("Requesting player stats from all other players ...");
		gdbt_players.SendStartupInfo(0, TRUE, bOwner);
	}
}

//----------------------------------------------------------------------------
void Game::SendState(DPID to)
{
  Msg("Sending game state to %d", to);
  gdbt_machines.SendAllStates(to);
}

//----------------------------------------------------------------------------
void Game::SessionLost()
{
	DWORD pid = local_player->GetPid();
	const char *msg = "The game owner has left the game";

	Msg(msg);
    TELETYPE::Msg(0, msg);

	SetOwner(pid);
}

//----------------------------------------------------------------------------
int Game::KeyReleased(int key)
{
  VIEW *pView;
  if (pView = viewman.GetFocus())
	  key = pView->Keycmd(key, KEY_RELEASED);

  // see if the teletype wants to handle it:
  if (key && (pView = viewman.GetView(IDV_S_TELETYPE)))
	  return pView->Keycmd(key, KEY_RELEASED);
  else
	return FALSE;
}

//----------------------------------------------------------------------------
int Game::KeyPressed(int key)
{
  VIEW *pView;

  if (pView = viewman.GetFocus())
	  key = pView->Keycmd(key, KEY_PRESSED);

  // see if the teletype wants to handle it:
  if (key && (pView = viewman.GetView(IDV_S_TELETYPE)))
	  return pView->Keycmd(key, KEY_PRESSED);
  else
	return FALSE;
}

//----------------------------------------------------------------------------
BOOL SetSRect(SRECT *srect, int left, int top, int right, int bottom)
{
  srect->left   = left;
  srect->top    = top;
  srect->right  = right;
  srect->bottom = bottom;

  return TRUE;
}

//----------------------------------------------------------------------------
void Game::CheckCursor()
{
	WORD obj_def_id;
	VIEW *pview = viewman.Hit(user.mousePos);
	WORD new_obj_def_id;

	if (pview)
	{
		// If the cursor is over a different view, then set new view focus.
		if (user.current_view != pview->view_id)
			viewman.SetFocus(pview->view_id);
		
		// Drag testing only occurs if we are holding something:
		//if (obj_def_id = cursorman.HeldObject())
		obj_def_id = cursorman.HeldObject();
		//{
			new_obj_def_id = pview->Drag(obj_def_id, user.mousePos);
			if (new_obj_def_id != obj_def_id)
			{
				//### cursorman.DropObject();
				//cursorman.LoadObject(new_obj_def_id);
			}
		//}

		WORD current_cursor = cursorman.GetCursor();
		SRECT cursor_rect;
		SetSRect(&cursor_rect, user.mousePos.x, user.mousePos.y, user.mousePos.x+48, user.mousePos.y+48);
		WORD new_cursor = pview->QueryCursor(cursor_rect);
		if (new_cursor != current_cursor)
		{
		  cursorman.SetCursor(new_cursor);
		}
	}
	else
	{
	  // mouse if outside of the application, set focus arbitrarily.
	  viewman.SetFocus(IDV_MAIN_PANEL);
	}

	local_player->SetPos(user.mousePos);
}

//----------------------------------------------------------------------------
void Game::MouseMove(POINT p)
{
}

//----------------------------------------------------------------------------
void Game::LButtonDown(POINT p)
{
	POINT px;
	WORD obj_def_id = 0;
	VIEW *pView = viewman.GetFocus();

	WORD ids = cursorman.GetCursor();

	PLAYER *player = gdbt_players.GetByTouching(pView->GetViewID(), pView->MapToCanvas(p));
	if (player)
	{
		gdbt_players.SendIntimate(player->GetPid(), TRUE);
	}

	// if on a view and we are not already holding something:
	if (pView)
	{
		switch (ids)
		{
		case IDS_HANDIDLE:
		  break;
		case IDS_HANDMACH:
		  if (!cursorman.HeldObject())
			  obj_def_id = pView->Grab(p, FALSE, &px); // grab something, or click
		  else
			  pView->Grab(p, TRUE, &px); // just click on a machine

		  if (obj_def_id)
		  {
			  cursorman.LoadObject(obj_def_id, px);
			  // jink the object so it displays at the correct place.
			  // cursorman.cursorMove(p); 
		  }
		  break;

		default:
  		  pView->Navigate(p, ids);
		}
	}
}

//----------------------------------------------------------------------------
void Game::LButtonUp(POINT p)
{
	WORD obj_def_id;
	VIEW *pView = viewman.GetFocus();
	POINT pCenter, pPos;

	// if on a view that accepts objects
	// and we are holding something:
	if (pView && pView->AcceptsObjects() && cursorman.HeldObject())
	{
		obj_def_id = cursorman.DropObject(&pCenter, &pPos);
		if (obj_def_id)
		{
			// try to drop the object into a machine
			p.x = pPos.x + pCenter.x;
			p.y = pPos.y + pCenter.y;
			if (!pView->Drop(&obj_def_id, p) && obj_def_id)
			{
				cursorman.LoadObject(obj_def_id, pPos); // so hang on to it
				// jink the object so it displays at the correct place.
				//cursorman.cursorMove(p); 
			}
		}
	}
}

//----------------------------------------------------------------------------
void Game::RButtonDown(POINT p)
{
}

//----------------------------------------------------------------------------
void Game::RButtonUp(POINT p)
{
}

//----------------------------------------------------------------------------
char* Game::GetGameName()
{
  return gameName;
}

//----------------------------------------------------------------------------
BOOL Game::SetGameName(const char *name)
{
  if (name && name[0])
  {
	strncpy(gameName, name, MAX_GAME_NAME);
	gameName[MAX_GAME_NAME-1] = 0;
	return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
BOOL Game::Save(const char *name)
{
  char path[_MAX_PATH];
  if (SetGameName(name))
  {
	makepath(path, NULL, gamePath, name, gameExt);
	gdbt_machines.Save(path);
	return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
BOOL Game::Load(const char *name)
{
  char path[_MAX_PATH];
  makepath(path, NULL, gamePath, name, gameExt);
  return gdbt_machines.Load(path);
}

//----------------------------------------------------------------------------
BOOL Game::New()
{
  char path[_MAX_PATH];
  makepath(path, NULL, gamePath, NewGameName, NewGameExt);
  return gdbt_machines.Load(path);
}
#ifdef _MSC_VER // Windows

#endif
