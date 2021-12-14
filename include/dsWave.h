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
#include "Data/Database/tbl_wave.h"
#include <pthread.h>

//typedef enum WAVE_RAMID;
//typedef enum WAVE_FILEID;

#define DS_OK 1
#define DSERR_GENERIC 2
#define DSERR_NODRIVER 3
#define DSERR_INVALIDCALL 4

#define DSBCAPS_PRIMARYBUFFER       0x00000001
#define DSBCAPS_STATIC              0x00000002
#define DSBCAPS_LOCHARDWARE         0x00000004
#define DSBCAPS_LOCSOFTWARE         0x00000008
#define DSBCAPS_CTRLFREQUENCY       0x00000020
#define DSBCAPS_CTRLPAN             0x00000040
#define DSBCAPS_CTRLVOLUME          0x00000080
#define DSBCAPS_CTRLDEFAULT  DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY
#define DSBCAPS_CTRLALL             0x000000E0  // All control capabilities
#define DSBCAPS_STICKYFOCUS         0x00004000
#define DSBCAPS_GETCURRENTPOSITION2 0x00010000  // More accurate play cursor under emulation

#define DSBCAPS_STATIC 0x00000002

#define DSBPLAY_LOOPING 1

#define WAVE_FORMAT_PCM     1

//https://msdn.microsoft.com/en-us/library/microsoft.directx_sdk.reference.waveformatex(v=vs.85).aspx
typedef struct WAVEFORMATEX {
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    WORD wBitsPerSample;
    WORD cbSize;
} WAVEFORMATEX, *LPWAVEFORMATEX;

// https://msdn.microsoft.com/en-us/library/microsoft.directx_sdk.reference.dsbufferdesc(v=vs.85).aspx
typedef struct DSBUFFERDESC {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    LPWAVEFORMATEX lpwfxFormat;
    GUID guid3DAlgorithm;
} DSBUFFERDESC,  *LPDSBUFFERDESC;

typedef const DSBUFFERDESC *LPCDSBUFFERDESC;

class IDirectSoundBuffer : public IUnknown {
    // https://msdn.microsoft.com/en-us/library/mt708923(v=vs.85).aspx
public:
    HRESULT SetCurrentPosition(
                               DWORD dwNewPosition
                               ){return 0;};
    HRESULT GetCurrentPosition(
                               LPDWORD lpdwCurrentPlayCursor,
                               LPDWORD lpdwCurrentWriteCursor
                               ) {return 0;};
    HRESULT SetVolume(
                      LONG lVolume
                      ){return 0;};
    HRESULT Play(
                 DWORD dwReserved1,
                 DWORD dwReserved2,
                 DWORD dwFlags
                 ){return 0;};
    HRESULT Stop(){return 0;};
    HRESULT SetPan(
                   LONG lPan
                   ){return 0;};
    HRESULT SetFrequency(
                         DWORD dwFrequency
                         ){return 0;};
    HRESULT Lock(
                 DWORD dwWriteCursor,
                 DWORD dwWriteBytes,
                 LPVOID lplpvAudioPtr1,
                 LPDWORD lpdwAudioBytes1,
                 LPVOID lplpvAudioPtr2,
                 LPDWORD lpdwAudioBytes2,
                 DWORD dwFlags
                 ) {return 0;};
    HRESULT Unlock(
                   LPVOID lpvAudioPtr1,
                   DWORD dwAudioBytes1,
                   LPVOID lpvAudioPtr2,
                   DWORD dwAudioBytes2
                   ){return 0;};
};
typedef IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;
typedef LPDIRECTSOUNDBUFFER *LPLPDIRECTSOUNDBUFFER;

class IDirectSound  : public IUnknown {
    // https://msdn.microsoft.com/en-us/library/mt708922(v=vs.85).aspx
public:
    HRESULT CreateSoundBuffer(
                              LPCDSBUFFERDESC lpcDSBufferDesc,
                              LPLPDIRECTSOUNDBUFFER lplpDirectSoundBuffer,
                              IUnknown FAR* pUnkOuter
                              ){return 0;};
    HRESULT SetCooperativeLevel(
                                HWND hwnd,
                                DWORD dwLevel
                                ){return 0;};
};
typedef IDirectSound *LPDIRECTSOUND;

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
#ifdef _WIN32
	DWORD GetCkSize() {return mmckInRIFF.cksize;}
#endif
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
#ifdef _WIN32
	int CreateFile(char *, HMMIO *, WAVEFORMATEX *, MMCKINFO *, MMCKINFO *);
	int StartDataWrite(HMMIO *, MMCKINFO *, MMIOINFO *);
	int WriteFile(HMMIO, UINT, BYTE *, MMCKINFO *, UINT *, MMIOINFO *);
	int CloseWriteFile(HMMIO *, MMCKINFO *, MMCKINFO *, MMIOINFO *, DWORD);
    int SaveFile(char *, UINT, DWORD, WAVEFORMATEX *);
    static int CopyUselessChunks(HMMIO *, MMCKINFO *, MMCKINFO *, HMMIO *,
                                 MMCKINFO *, MMCKINFO *);
    static BOOL riffCopyChunk(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck);
#endif
	int LoadFile(char *filename);
	BOOL TransferDataToDirectSoundBuffer();

    typedef void * (*Entry)(void *);
private:
  static LPDIRECTSOUND  lpDS;
  static int			dbSfx;
  static int			dbMusic;
  static int			ThreadCount;
  static BOOL			bThreadsMustDie;
  static Wave			**ramWave;
private:
    WAVEFORMATEX         *pwfx;             /* Wave Format data structure */
#ifdef _WIN32
    HMMIO                hmmio;             /* MM I/O handle for the WAVE */
    MMCKINFO             mmck;              /* Multimedia RIFF chunk */
    MMCKINFO             mmckInRIFF;        /* Use in opening a WAVE file */
#endif
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
	pthread_t			 thid;				/* NULL if thread is not active */
	// used specifically for static wave files:
	UINT				 cbSize;
	BYTE				 *pbData;
	BOOL				 bHardware;
};

#endif /* __dsWave_INCLUDED__ */

