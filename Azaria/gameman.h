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
; File:       gameman.h
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifndef __GAMEMAN_INCLUDED__
#define __GAMEMAN_INCLUDED__


#define __ViewsPath		"Data\\Ones\\Views"
#define __ObjectsPath	"Data\\Ones\\Objects"
#define __PalettesPath	"Data\\Palettes"
#define __MasksPath		"Data\\Masks"
#define __WavePath		"Data\\Sounds\\Wave"
#define __MidiPath		"Data\\Sounds\\Midi"

#define MAX_GAME_NAME 80

// Game Manager.
class Game {
  static char m_InstallDir[_MAX_PATH];
  static DPID c_owner;
  static int musicVolume;
  static int sfxVolume;
  static char gameName[MAX_GAME_NAME];
public:
	Game();
	~Game();

private:
  void CheckCursor();
  static void  LoadLRU();

public: // interface
	BOOL Init();		// Called once, Loads all the initial views.
	void Cleanup();		// Called before exit.
	void Tick(BOOL display_available);	// called by main program loop.
	void RestoreDisplay();	// restore the display pages and the cursor.

	void LoadView(WORD view_id);	// allows outside entity to load views.

	//--------------
	// network play:

	// Notify the game manager that local player is joining a network game.
	void JoinNetGame(
					DPID pid,		// local players PID
					char *name,		// local players name
					BOOL bOwner);	// true if local player is the game creator

	void AddPlayer( // Another player is joining the game
				DPID pid,		// pid of the new player
				BOOL bOwner);	// This player owns the game
	void DelPlayer(DPID pid);		// player is dropping out of the game
	void SetGameOwner(DPID pid);	// Game ownership is being set or transferred.
	void SessionLost();			// session connection is dropped.

	// set/get the game owner:
	static inline void SetOwner(DPID pid) {c_owner = pid;};
	static inline DWORD GetOwner() {return c_owner;};
	static inline BOOL IsOwner(DPID pid) {return c_owner == pid;};
	static void SendState(DPID to);

	// user keystrokes and mouse movements:
	int KeyReleased(int key);
	int KeyPressed(int key);
	void MouseMove(POINT p);
	void LButtonDown(POINT p);
	void LButtonUp(POINT p);
	void RButtonDown(POINT p);
	void RButtonUp(POINT p);

	// music and sfx settings
	static void SetMusicVolume(int volume);
	static void SetSfxVolume(int volume);
	static int GetSfxVolume() {return sfxVolume;}
	static int GetMusicVolume() {return musicVolume;}

	static BOOL SetGameName(char *name);
	static char* GetGameName();

	static BOOL Save(char *name);
	static BOOL Load(char *name);
	static BOOL New();
};

extern Game gameman;	// The one and only game manager.

#endif
