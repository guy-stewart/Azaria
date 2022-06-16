/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
;
; File:       dsWave.h
;
; Description:
;	Implements a high level Direct Sound Interface.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 07/01/96  gas  Created.
----------------------------------------------------------------------------*/
#pragma once

#include "includes.h"
#include <SDL2/SDL_mixer.h>

//typedef enum WAVE_RAMID;
//typedef enum WAVE_FILEID;

class Wave
{
   	Mix_Chunk* chunk;
  static SDL_AudioSpec wav_spec; // the specs of our piece of music
  static Wave			**ramWave;

public:
    Wave();
    Wave(const char *filename);
    ~Wave();

    static bool Init();
	static HRESULT Release();
	static void SetFxVolume(int volume);
	static void SetMusicVolume(int volume);

public: // interface
  // static audio files:
	static BOOL Load(int count, char *Path, char *altPath, char *filenames[]);
    static void AudioCallback(void *userdata, Uint8 *buffer, int len);

  // dynamic files.
	BOOL Play(char *filename, BOOL bPaused); // play file in seperate thread.
	BOOL Play(WORD bFadeOutTime);
	void Stop();
	void Pause(WORD wFadeOutTime);

    HRESULT SetVolume(LONG volume);
    HRESULT SetPan(LONG pan);
    HRESULT SetFrequency(DWORD frequency);

	void TimeFunc();


private:
	static void StartThread(Wave *thing);
	void PlayThread();
	int OpenFile(char *);
	int StartDataRead();
	int ReadFile(UINT, BYTE *, UINT *);
	int CloseReadFile();
	int LoadFile(char *filename);
	BOOL TransferDataToDirectSoundBuffer();
};
