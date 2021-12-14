/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:               Dstrtime.c
 *  Content:    This source file contains a Multimedia Timer Callback
 *             procedure which is used by the DirectSound Stream Sample
 *             application to update the playback buffer at regular intervals
 *
 ***************************************************************************/
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <dsound.h>
#include <cderr.h>
#endif

#include "../../include/ptypes.h"
#include <memory.h>

#include "dsWave.h"
#include "dsConst.h"

void __cdecl Msg( LPSTR fmt, ... );

extern char  szDebug[];

void Wave::TimeFunc()
{
    LPBYTE      lpWrite1, lpWrite2, lpTemp;
    DWORD       dwLen1, dwLen2, dwPlay, dwWrite, dwPlayedLength;
    DWORD       dwLeftToRead;
    int         nChkErr;
    UINT        uChkErr;

	if (!bPlaying)
	  return;

    /* Get and print the current position of the play cursor */
    lpDSBStreamBuffer->GetCurrentPosition( &dwPlay, &dwWrite );
    
    /* If the play cursor is at the same spot as the last call, there are two
     * possibilities.  The first is that we were called extremely late and
     * happened to land on an integer number of complete buffer cycles.  This
     * is not very likely.  The other is that the play cursor didn't move.
     * Since we're oversampling, this is very likely.  In this case, we should
     * bail.
     */

    if( dwPlay == dwNextWriteOffset )
        {
        return;
        }

    /* Have we found the end of the file and passed the buffer end? */
    if( bFoundEnd && !dwBytesRemaining )
        {
        if( !bDonePlaying )
            {
            bDonePlaying = TRUE;
            }
        return;
        }

    if( dwPlay < dwNextWriteOffset )
        {
        /* Calculate how many writeable bytes there are behind the play cursor */
        dwPlayedLength = (dwPlay + dwBufferSize - dwNextWriteOffset);
        }
    else
        {
        /* Calculate how many writeable bytes there are behind the play cursor */
        dwPlayedLength = (dwPlay - dwNextWriteOffset);
        }

    dwProgress += dwPlayedLength;

    /*
     *  If bFoundEnd == TRUE, then we've finished reading in the file,
     * but we need to wait until the buffer's play cursor passes the point we
     * were at when we found out we were done reading.
     */
    if( bFoundEnd && dwBytesRemaining )
        {
        // Decrement the count of how many bytes we have to wait for before
        // we can kill the timer procedure safely
        if( dwPlayedLength > dwBytesRemaining )
            dwBytesRemaining = 0;
        else
            dwBytesRemaining -= dwPlayedLength;

        if( lpDSBStreamBuffer->Lock(dwNextWriteOffset,
                                    dwPlayedLength,
                                    &lpWrite1, &dwLen1,
                                    &lpWrite2, &dwLen2,
                                    0 ) != 0 )
            {
            OutputDebugString( "TimeFunc(X) could not lock DirectSoundBuffer" );
            return;
            }

        /* Silence out both parts of the locked buffer */
        memset( lpWrite1, pwfx->wBitsPerSample == 8 ? 128 : 0, dwLen1 );

        if( lpWrite2 && dwLen2 )
            memset( lpWrite2,
                    pwfx->wBitsPerSample == 8 ? 128 : 0,
                    dwLen2 );

        lpDSBStreamBuffer->Unlock((LPVOID)lpWrite1, dwLen1,
                                         (LPVOID)lpWrite2, dwLen2 );
    /*
     * This code is stolen from the end of the routine -- we need to keep
     * zeroing out buffer segments while we're waiting for the play cursor to
     * catch up to the end of the WAVE data.
     */
        dwNextWriteOffset += dwPlayedLength;
        if( dwNextWriteOffset >= dwBufferSize )
            dwNextWriteOffset -= dwBufferSize;

        return;
        }

    /* Lock a segment of memory that is behind the play cursor */
	HRESULT hr;
	hr = lpDSBStreamBuffer->Lock(dwNextWriteOffset,
                                dwPlayedLength,
                                &lpWrite1, &dwLen1,
                                &lpWrite2, &dwLen2,
                                0 );
    if( hr != 0 )
    {
        OutputDebugString( "TimeFunc(B) could not lock DirectSoundBuffer" );
		switch (hr) {
		case DSERR_INVALIDCALL:
			OutputDebugString( " Already locked" );
			break;
		}
		Msg("ThreadID=%X", thid);

        return;
    }
        
    if( dwLen1 && !bDonePlaying )
        {
        nChkErr = ReadFile((UINT)dwLen1, lpWrite1, &uChkErr );
        if( uChkErr < (UINT)dwLen1 )
            {
            if( !bLoopFile )
                {
                /* Zero out the rest of this block */
                if( pwfx->wBitsPerSample == 8 )
                    memset( lpWrite1+uChkErr, 128, ((UINT)dwLen1-uChkErr));
                else if( pwfx->wBitsPerSample == 16 )
                    memset( lpWrite1+uChkErr, 0, ((UINT)dwLen1-uChkErr));

/* Enable play completion detection code at the beginning of the next call */

                bFoundEnd = TRUE;
                if( dwPlay > dwNextWriteOffset )
                    dwBytesRemaining = (dwNextWriteOffset + dwBufferSize - dwPlay);
                else
                    dwBytesRemaining = (dwNextWriteOffset - dwPlay);
                }
             else
                {
                lpTemp = lpWrite1;
                dwLeftToRead = dwLen1;
                do
                    {
                    /* Continue decrementing our count and moving our temp
                     * pointer forward until we've read the file enough times
                     * to fill the buffer.  NOTE: It's probably not efficient
                     * to bother with the overhead of streaming a file that's
                     * not at least as large as the buffer... */
                    lpTemp += uChkErr;
                    dwLeftToRead -= uChkErr;
                    nChkErr = StartDataRead();
                    nChkErr = ReadFile( (UINT)dwLeftToRead,lpTemp, &uChkErr );
                    } while( uChkErr < dwLeftToRead );
                }
            }
        }
    /*
    * The bDonePlaying flag is set by the caller if the user stops playback
    * before the end of the WAVE file is encountered.  It tells us to cut this
    * racket out and play nothing in case it takes the caller a couple
    * interrupts to shut off the timer.
    */
    else if( dwLen1 && bDonePlaying )
        {
        // Set the appropriate silence value
        memset( lpWrite1,
                    pwfx->wBitsPerSample == 8 ? 128 : 0,
                    dwLen1);
        }

    if( dwLen2 && !bDonePlaying )
        {
        nChkErr = ReadFile( (UINT)dwLen2, lpWrite2, &uChkErr );
        if( uChkErr < (UINT)dwLen2 )
            {
            if( !bLoopFile )
                {
                /* Zero out the rest of this block */
                if( pwfx->wBitsPerSample == 8 )
                    memset( lpWrite2+uChkErr, 128, ((UINT)dwLen2-uChkErr));
                else if( pwfx->wBitsPerSample == 16 )
                    memset( lpWrite2+uChkErr, 0, ((UINT)dwLen2-uChkErr));
                /* Enable play completion detection code at the beginning
                 * of the next call
                 */
                bFoundEnd = TRUE;
                if( dwPlay > dwNextWriteOffset )
                    dwBytesRemaining = (dwNextWriteOffset
                                            + dwBufferSize - dwPlay);
                else
                    dwBytesRemaining = (dwNextWriteOffset
                                            - dwPlay);
                }
            else
                {
                lpTemp = lpWrite2;
                dwLeftToRead = dwLen2;
                do
                    {
                    /* Continue decrementing our count and moving our temp
                     * pointer forward until we've read the file enough times
                     * to fill the buffer.  NOTE: It's probably not efficient
                     * to bother with the overhead of streaming a file that's
                     * not at least as large as the buffer... */
                    lpTemp += uChkErr;
                    dwLeftToRead -= uChkErr;
                    nChkErr = StartDataRead();
                    nChkErr = ReadFile( (UINT)dwLeftToRead,lpTemp,&uChkErr );
                    } while( uChkErr < dwLeftToRead );
                }
            }
        }
    else if( lpWrite2 && dwLen2 && bDonePlaying )
        {
        // Set the appropriate silence value
        memset( lpWrite2,
                    pwfx->wBitsPerSample == 8 ? 128 : 0,
                    dwLen2 );
        }

    lpDSBStreamBuffer->Unlock((LPVOID)lpWrite1, dwLen1,
                                     (LPVOID)lpWrite2, dwLen2 );

    dwNextWriteOffset += dwPlayedLength;
    if( dwNextWriteOffset >= dwBufferSize )
        dwNextWriteOffset -= dwBufferSize;

    return;
}
