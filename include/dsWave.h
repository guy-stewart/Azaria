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
#ifndef __dsWave_INCLUDED__
#define __dsWave_INCLUDED__
#include "includes.h"
#include "tbl_wave.h"

//typedef enum WAVE_RAMID;
//typedef enum WAVE_FILEID;

typedef  struct {
    int nada;
} HMMIO;

typedef  struct {
    int cksize;
} MMCKINFO;

typedef  struct {
    int nSamplesPerSec;
} WAVEFORMATEX;

typedef  struct {
    int nada;
} MMIOINFO;

typedef  struct {
    int nada;
} DIRECTSOUND;

typedef  struct {
    int nada;
} DIRECTSOUNDBUFFER;


typedef MMCKINFO * LPMMCKINFO;

typedef DIRECTSOUND * LPDIRECTSOUND;

typedef DIRECTSOUNDBUFFER * LPDIRECTSOUNDBUFFER;

class Wave
{
public:
    Wave();
    ~Wave();

	static HRESULT SetCooperativeLevel( HWND hWndMain, DWORD flags );
	static HRESULT Release();
	static HRESULT DirectSoundCreate(GUID* guID);
	static void SetFxVolume(int volume);
	static void SetMusicVolume(int volume);

public: // interface
  // static audio files:
	static BOOL Load(int count, char *Path, char *altPath, char *filenames[]);
	static void Play(WAVE_RAMID ram_id);
	static void Play(WAVE_FILEID file_id);

  // dynamic files.
	BOOL Play(char *filename, BOOL bPaused); // play file in seperate thread.
	int StreamBufferSetup( char * filename, BOOL bLoop );
	BOOL Play(WORD bFadeOutTime);
	void Stop();
	void Pause(WORD wFadeOutTime);

    HRESULT SetVolume(LONG volume);
    HRESULT SetPan(LONG pan);
    HRESULT SetFrequency(DWORD frequency);

	void TimeFunc();
	void ResetWavePlayer( void );

public:
	// cksize
	// Size, in bytes, of the data member of the chunk. 
	// The size of the data member does not include the 4-byte chunk identifier, 
	//	the 4-byte chunk size, or the optional pad byte at the end of the data member.
	DWORD GetCkSize() {return mmckInRIFF.cksize;}
	BOOL IsLooping() {return bLoopFile;}
	BOOL IsPlaying() {return bPlaying;}
	void SetLooping(BOOL bLoop) {bLoopFile = bLoop;}
	DWORD SamplesPerSec() {return pwfx->nSamplesPerSec;}
	DWORD GetProgress() {return dwProgress;}

private:
	static void StartThread(Wave *thing);
	void PlayThread();

private: // file I/O stuff
	int OpenFile(char *);
	int StartDataRead();
	int ReadFile(UINT, BYTE *, UINT *);
	int CloseReadFile();

private:
	int CreateFile(char *, HMMIO *, WAVEFORMATEX *, MMCKINFO *, MMCKINFO *);
	int StartDataWrite(HMMIO *, MMCKINFO *, MMIOINFO *);
	int WriteFile(HMMIO, UINT, BYTE *, MMCKINFO *, UINT *, MMIOINFO *);
	int CloseWriteFile(HMMIO *, MMCKINFO *, MMCKINFO *, MMIOINFO *, DWORD);

	int LoadFile(char *filename);
	BOOL TransferDataToDirectSoundBuffer();
	int SaveFile(char *, UINT, DWORD, WAVEFORMATEX *);

	static int CopyUselessChunks(HMMIO *, MMCKINFO *, MMCKINFO *, HMMIO *,
									MMCKINFO *, MMCKINFO *);
	static BOOL riffCopyChunk(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck);

  typedef unsigned int (__stdcall* Entry)(void*);

private:
  static LPDIRECTSOUND  lpDS;
  static int			dbSfx;
  static int			dbMusic;
  static int			ThreadCount;
  static BOOL			bThreadsMustDie;
  static Wave			**ramWave;
private:
    WAVEFORMATEX         *pwfx;             /* Wave Format data structure */
    HMMIO                hmmio;             /* MM I/O handle for the WAVE */
    MMCKINFO             mmck;              /* Multimedia RIFF chunk */
    MMCKINFO             mmckInRIFF;        /* Use in opening a WAVE file */
    LPDIRECTSOUNDBUFFER  lpDSBStreamBuffer; /* Points to DirectSoundBuffer */
    DWORD                dwBufferSize;      /* Size of the entire buffer */
    DWORD                dwNextWriteOffset; /* Offset to next buffer segment */
    DWORD                dwProgress;        /* Used with above to show prog. */
    DWORD                dwBytesRemaining;  /* Bytes 'til timer shutdown */
    BOOL                 bDonePlaying;      /* Signals early abort to timer */
    BOOL                 bLoopFile;         /* Should we loop playback? */
    BOOL                 bFoundEnd;         /* Timer found file end */
	BOOL				 bFileOpen;
	BOOL				 bPlaying;
	WORD				 sleeptime;			/* fade time remaining in ms */
	short				 deltaVolume;
	short				 currentVolume;
	short				 finalVolume;
	unsigned			 thid;				/* NULL if thread is not active */
	// used specifically for static wave files:
	UINT				 cbSize;
	BYTE				 *pbData;
	BOOL				 bHardware;
};

#endif /* __dsWave_INCLUDED__ */

