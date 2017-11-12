/*==========================================================================
 ***************************************************************************/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <process.h>
#include <dsound.h>
#include <memory.h>
#include <cderr.h>

#include "dsWave.h"
#include "dsConst.h"

LPDIRECTSOUND Wave::lpDS = NULL;
int Wave::ThreadCount = 0;
BOOL Wave::bThreadsMustDie = TRUE;
Wave** Wave::ramWave;
int Wave::dbSfx;
int Wave::dbMusic;

//----------------------------------------------------------------------------
Wave::Wave()
{
  thid = 0;
  lpDSBStreamBuffer = NULL;
}

//----------------------------------------------------------------------------
Wave::~Wave()
{
  if (thid)
  {
	Stop();
	while (thid)
	{
	  Sleep(100);
	}
  }
}

//----------------------------------------------------------------------------
int atten[] = {-10000, -1600, -800, -600, -300, -200, -100, -75, -50, -25, 0};
void Wave::SetFxVolume(int volume)
{
  volume /=10;
  dbSfx = atten[volume];
}

//----------------------------------------------------------------------------
void Wave::SetMusicVolume(int volume)
{
  volume /=10;
  dbMusic = atten[volume];
}

//----------------------------------------------------------------------------
void Wave::Play(WAVE_RAMID ram_id)
{
  if (ramWave[ram_id] && ramWave[ram_id]->lpDSBStreamBuffer)
  {
	ramWave[ram_id]->lpDSBStreamBuffer->SetCurrentPosition(0 );
	ramWave[ram_id]->lpDSBStreamBuffer->SetVolume(dbSfx);
	ramWave[ram_id]->lpDSBStreamBuffer->Play( 0, 0, 0 );
  }
}

//----------------------------------------------------------------------------
void Wave::Play(WAVE_FILEID file_id)
{
}
  

//----------------------------------------------------------------------------
#define FADE_TIME_DELTA 100
BOOL Wave::Play(WORD fadetime) // fade time in milliseconds
{
  if( bFileOpen && lpDSBStreamBuffer )
  {
      // Ensure that position is at 0, ready to go
	  if (fadetime)
	  {
		currentVolume = (MIN_VOLUME-dbMusic)/2;
		deltaVolume = (((dbMusic-MIN_VOLUME)*FADE_TIME_DELTA)/fadetime);
		finalVolume = dbMusic;
		sleeptime = FADE_TIME_DELTA;
		lpDSBStreamBuffer->SetVolume(currentVolume);
	  }

      lpDSBStreamBuffer->Play( 0, 0, DSBPLAY_LOOPING);
	  bPlaying = TRUE;
  }

  return bPlaying;
}

//----------------------------------------------------------------------------
BOOL Wave::Play(char *filename, BOOL bPaused)
{
	if (StreamBufferSetup(filename, FALSE) == 0)
		bFileOpen = TRUE;
	else
		return FALSE;

	_beginthreadex(NULL, 0, 
	  (Entry)StartThread,
	  this, 0, 
	  &thid);

	if (!bPaused)
	  Play(0);

	return TRUE;
}

//----------------------------------------------------------------------------
void Wave::PlayThread()
{
  ThreadCount++;

  while (!bDonePlaying && !bThreadsMustDie)
  {
	TimeFunc();

	if (deltaVolume)
	{
		currentVolume += deltaVolume;
		if ((deltaVolume > 0) && (currentVolume >= finalVolume)) // fading in
		{
		  currentVolume = finalVolume;
		  deltaVolume = 0; // we are done fading in ...
		  sleeptime = PLAYBACK_TIMER_PERIOD;
		}
		else if ((deltaVolume < 0) && (currentVolume <= finalVolume)) // fading out
		{
		  currentVolume = finalVolume;
		  deltaVolume = 0; // we are done fading in ...
		  sleeptime = PLAYBACK_TIMER_PERIOD;
		  bPlaying = FALSE;
		  lpDSBStreamBuffer->Stop();
		}

		lpDSBStreamBuffer->SetVolume(currentVolume);
	}

    Sleep(sleeptime);
  }

  if (bPlaying)
	Stop();

  thid = 0;

  ThreadCount--;
}

//----------------------------------------------------------------------------
void Wave::StartThread(Wave *thing)
{
  thing->PlayThread();
}

//----------------------------------------------------------------------------
void Wave::Stop()
{
  bDonePlaying = TRUE;

  if( bPlaying )
  {
	bPlaying = FALSE;
	lpDSBStreamBuffer->Stop();
  }

  if( bFileOpen )
  {
	CloseReadFile();
	if( lpDSBStreamBuffer )
		lpDSBStreamBuffer->Release();

	lpDSBStreamBuffer = NULL;

	bFileOpen = FALSE;
  }
}

//----------------------------------------------------------------------------
void Wave::Pause(WORD fadetime)
{
  // bDonePlaying = TRUE;

  if( bPlaying )
  {
	if (fadetime)
	{
	  deltaVolume = (((MIN_VOLUME-currentVolume)*FADE_TIME_DELTA)/fadetime);
	  finalVolume = MIN_VOLUME;
	  sleeptime = FADE_TIME_DELTA;
	  lpDSBStreamBuffer->SetVolume(currentVolume);
	}
	else
	{
	  bPlaying = FALSE;
	  lpDSBStreamBuffer->Stop();
	}
  }

  //ResetWavePlayer();
}

//----------------------------------------------------------------------------
HRESULT Wave::SetVolume(LONG value)
{
  value /= 10;

  if (lpDSBStreamBuffer) 
    return lpDSBStreamBuffer->SetVolume( atten[value]);
  else
	return DSERR_GENERIC;
}

//----------------------------------------------------------------------------
HRESULT Wave::SetPan(LONG value)
{
  if (lpDSBStreamBuffer) 
    return lpDSBStreamBuffer->SetPan( value );
  else
	return DSERR_GENERIC;
}

//----------------------------------------------------------------------------
HRESULT Wave::SetFrequency(DWORD value)
{
  if (lpDSBStreamBuffer) 
    return lpDSBStreamBuffer->SetFrequency( value );
  else
	return DSERR_GENERIC;
}


/*****************************************************************************/
/* StreamBufferSetup()                                                       */
/*                                                                           */
/*    This function uses the filename stored in the global character array to*/
/* open a WAVE file. Then it creates a secondary DirectSoundBuffer object    */
/* which will later be used to stream that file from disk during playback.   */
/*                                                                           */
/*****************************************************************************/
int Wave::StreamBufferSetup( char * filename, BOOL bLoop )
{
	DSBUFFERDESC dsbd;
	HRESULT      dsRetVal;
	LPBYTE       lpWrite1, lpWrite2;
	DWORD        dwLen1, dwLen2;
	UINT         uChkErr;
      
	int nChkErr;


	if (!lpDS)
	{
		return( DSERR_NODRIVER );
	}

	if(( nChkErr = OpenFile( filename )) != 0 )
	{
		return( ERR_WAVE_OPEN_FAILED );
	}

	if( pwfx->wFormatTag != WAVE_FORMAT_PCM )
	{
		CloseReadFile();
		return( ERR_WAVE_INVALID_FORMAT );
	}

	/* Seek to the data chunk */
	if(( nChkErr = StartDataRead()) != 0 )
	{
		CloseReadFile();
		return( ERR_WAVE_CORRUPTED_FILE );
	}
	/* As a side note, mmck.ckSize will be the size of all the data in this file.
	 * That's something which might be handy when calculating the length... */

	/* Calculate a buffer length, making sure it is an exact multiple of the
	 * buffer segment size.
	 */
	dwBufferSize = ((DWORD)pwfx->nAvgBytesPerSec
							* (((NUM_BUFFER_SEGMENTS * PLAYBACK_TIMER_PERIOD * PLAYBACK_OVERSAMPLE)
							/ 10)) / 100);

	/*
	 * Create the secondary DirectSoundBuffer object to receive our sound data.
	 */
	memset( &dsbd, 0, sizeof( DSBUFFERDESC ));
	dsbd.dwSize = sizeof( DSBUFFERDESC );
	dsbd.dwFlags = DSBCAPS_CTRLDEFAULT;
	dsbd.dwBufferBytes = dwBufferSize;

	/* Set Format properties according to the WAVE file we just opened */
	dsbd.lpwfxFormat = pwfx;
	dsRetVal = lpDS->CreateSoundBuffer( &dsbd,&lpDSBStreamBuffer,NULL );
	if( dsRetVal != DS_OK )
	{
		return dsRetVal;
	}

	bFoundEnd = FALSE;
	dwBytesRemaining = 0;

	dsRetVal = lpDSBStreamBuffer->Lock( 0, dwBufferSize,
										&lpWrite1, &dwLen1,
										&lpWrite2, &dwLen2,
										0 );
	if( dsRetVal != DS_OK )
	{
		return dsRetVal;
	}

	if( dwLen1 )
		{
		nChkErr = ReadFile( (UINT)dwLen1, lpWrite1, &uChkErr );
		if( uChkErr < dwLen1 )
			{
			if( bLoopFile )
				{
	/* If the file is shorter than the buffer and we're looping, we need to
	 * read the file in again so that we don't get a block of silence before
	 * the timer loops playback.
	 */
				LPBYTE lpTemp = lpWrite1;

				do
					{
					/* Continue decrementing our count and moving our temp
					 * pointer forward until we've read the file enough times
					 * to fill the buffer.  NOTE: It's probably not efficient
					 * to bother with the overhead of streaming a file that's
					 * not at least as large as the buffer... */
					lpTemp += uChkErr;
					dwLen1 -= uChkErr;
					nChkErr = StartDataRead();
					nChkErr = ReadFile((UINT)dwLen1,lpTemp, &uChkErr );
					} while( uChkErr < dwLen1 );
				}
			else
				{
				bFoundEnd = TRUE;
				dwBytesRemaining = (DWORD)uChkErr;
				_fmemset( (lpWrite1+uChkErr),
								pwfx->wBitsPerSample == 8 ? 128 : 0,
								(dwLen1 - uChkErr));
				}
			}
		}
	dsRetVal = lpDSBStreamBuffer->Unlock((LPVOID)lpWrite1, dwLen1,
										(LPVOID)lpWrite2, 0 );
	dwNextWriteOffset = dwProgress = 0;

	lpDSBStreamBuffer->SetCurrentPosition(0);

	bDonePlaying = FALSE;
	bLoopFile = bLoop;
	bFileOpen = TRUE;

	return( 0 );
}


/*****************************************************************************/
/* ResetWavePlayer()                                                         */
/*                                                                           */
/*  Performs a subset of the above operations (in StreamBufferSetup). Things */
/* not done include creating a DSB and opening the file (it's already open). */
/*                                                                           */
/*****************************************************************************/
void Wave::ResetWavePlayer( void )
    {
    LPBYTE      lpWrite1, lpWrite2;
    DWORD       dwLen1, dwLen2;
    UINT        uChkErr;
    int         nChkErr;
    HRESULT     dsrval;

    StartDataRead();
    bFoundEnd = FALSE;
    dwBytesRemaining = 0;

    dsrval = lpDSBStreamBuffer->Lock(	0, dwBufferSize,
                                        &lpWrite1, &dwLen1,
                                        &lpWrite2, &dwLen2,
                                        0 );

    if( dwLen1 )
        {
        nChkErr = ReadFile( (UINT)dwLen1, lpWrite1, &uChkErr );
        if( uChkErr < dwLen1 )
            {
            if( bLoopFile )
                {
    /* If the file is shorter than the buffer and we're looping, we need to
     * read the file in again so that we don't get a block of silence before
     * the timer loops playback.
     */
                LPBYTE lpTemp = lpWrite1;

                do
                    {
                    /* Continue decrementing our count and moving our temp
                     * pointer forward until we've read the file enough times
                     * to fill the buffer.  NOTE: It's probably not efficient
                     * to bother with the overhead of streaming a file that's
                     * not at least as large as the buffer... */
                    lpTemp += uChkErr;
                    dwLen1 -= uChkErr;
                    nChkErr = StartDataRead();
                    nChkErr = ReadFile( (UINT)dwLen1,lpTemp,&uChkErr );
                    } while( uChkErr < dwLen1 );
                }
            else
                {
                bFoundEnd = TRUE;
                dwBytesRemaining = (DWORD)uChkErr;

                // Cover ourselves by filling the rest of the buffer space
                // with 8 or 16 bit silence, in case we can't stop the playback
                // exactly on a block boundary and we run a bit into NULL data
                _fmemset(( lpWrite1 + uChkErr),
                                pwfx->wBitsPerSample == 8 ? 128 : 0,
                                dwLen1 - uChkErr);
                }
            }
        }
    dsrval = lpDSBStreamBuffer->Unlock( 
                                        (LPVOID)lpWrite1, dwLen1,
                                        (LPVOID)lpWrite2, 0 );
    dwNextWriteOffset = dwProgress = 0;
    bDonePlaying = FALSE;
    }

//----------------------------------------------------------------------------
HRESULT Wave::DirectSoundCreate(GUID* guID)
{
  HRESULT hr = ::DirectSoundCreate( guID, &lpDS, NULL );
  if (hr == 0)
	bThreadsMustDie = FALSE;

  return hr;
}

//----------------------------------------------------------------------------
HRESULT Wave::Release()
{
  if( !lpDS )
	return DSERR_GENERIC;

  bThreadsMustDie = TRUE;
  while (ThreadCount) Sleep(100);

  HRESULT hr = lpDS->Release();
  lpDS = NULL;
  return hr;
}

//----------------------------------------------------------------------------
HRESULT Wave::SetCooperativeLevel( HWND hWnd, DWORD flags )
{
  if( !lpDS )
	return DSERR_GENERIC;

  return lpDS->SetCooperativeLevel( hWnd, flags );
}

