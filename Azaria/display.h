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
; File:       display.h
; Description:    Display Manager - the games sole interface to DD.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/

#ifndef __DISPLAY_INCLUDED__
#define __DISPLAY_INCLUDED__

#include "includes.h"

#include "palette.h"

// Video Adapter Direct Draw Extenstion object.
class Display {
private:
	HWND hWndMain;       // the Maya window
	SIZE m_Size;
	UINT m_bPP;
	int m_flipping_pages;		// how many flipping pages are there?
	int m_current_buffer;		// what is the current buffer.
	int m_DirtyFrameMask;		// used by apps to dermine when all buffers are refreshed
	BOOL  bFullscreen;    // run in fullscreen mode
	BOOL  bExclusive;     // run in exclusive mode, we own palette
	BOOL m_Paused;

	BOOL m_PalChanged;			// set true whenever SetPalette is called, cleared by Flip
    PALETTEENTRY    m_palette[256];  // current palette entries,  used for SetPalette

	int 	maintRestBfrs;

	IDirectDrawSurface     *lpFrontBuffer;
	IDirectDrawSurface     *lpBackBuffer;
	IDirectDrawPalette     *lpDDPalette;

	IDirectDraw            *lpDD;
	IDirectDrawClipper     *lpClipper;

	int     nBufferCount;

	//	WS_SYSMENU		-	so we get a icon in in our tray button
	//	WS_POPUP		-	no border, no title
	//	WS_POPUPWINDOW	-	border, title, close button
	//	WS_CAPTION		-	title bar

#ifdef _MSC_VER // Windows
	enum {
		FULLSCREEN_STYLE = WS_SYSMENU|WS_POPUP,
		WINDOWED_STYLE = WS_POPUPWINDOW|WS_CAPTION,
	};
#else
    enum {
        FULLSCREEN_STYLE = 0,
        WINDOWED_STYLE = 1,
    };
#endif
    
private:
	BOOL _Restore();

	void Restore();
	BOOL Enable( void );
	BOOL CreateFlippingSurface( void );
    void ReadPalFile( const char *path, const char *filename, PALETTE *pal);

	BOOL SwapBuffers(void);
	BOOL RestoreAll(void);

	IDirectDrawSurface *CreateSurface( DWORD width, DWORD height, BOOL sysmem, BOOL trans );

	//-------------------------------------------------------
public: // interface for application configuration:
	BOOL Init(HWND hWnd, SIZE Size, UINT bPP);

	inline int BufferCount() {return nBufferCount;}
	inline BOOL IsFullscreen() {return bFullscreen;};
	inline void setFullscreen(BOOL mode) {bFullscreen = mode;};
	inline BOOL IsExclusive()	{return bExclusive;};
	inline void setExclusive(BOOL mode) {bExclusive = mode;};

	void FlipToGdi(BOOL bIsActive);

	BOOL ReInit();
	BOOL Begin(VOID);
	BOOL Disable( BOOL );
	BOOL Restore(int active_app);
	BOOL Clear();

	BOOL CreatePalette();
	BOOL AttachPalette();
	void ReleasePalette();
	// for debug of palette:
    void PrintPalette();

	// All backbuffers need repainting
	inline void SetAllDirty()
	{
		maintRestBfrs = 0xFF;	// force repaint of all buffers
		m_PalChanged=TRUE;		// force restore of palette.
	}

	inline int GetFrameMask() {return m_DirtyFrameMask;}

	//-------------------------------------------------------
public: // interface for game run-time:

	// If the backbuffers need to be restored for any reason,
	// such as application startup (buffers are clear and never been painted)
	// are screen mode switch (buffers are lost and then restored)
	BOOL BackBufferNeedsPainting() {return maintRestBfrs & (1<<m_current_buffer);}
	void SetBackBufferPainted() {maintRestBfrs &= ~(1<<m_current_buffer);}

	// the game likes to change the palette on the fly:
	BOOL SetPalette(PALETTEENTRY *pal, BOOL bAll);
	inline PALETTEENTRY *GetPalette() {return m_palette;};

	inline int GetBackBufferId() {return m_current_buffer;}
	inline IDirectDrawSurface * GetBackBuffer() {return lpBackBuffer;}
	int Flip();				// returns new backubuffer number.

	const char *GetDdError(HRESULT hr);

	// FUN STUFF - for use by games
private:
	int blindness;
    PALETTEENTRY    m_blind_palette[256]; // current palette with blindness gamma applied

public:
	int SetBlind(int delta);
};

extern Display display;	// The one and only video display adapter manager.

extern RECT                     rcWindow;       // where the Maya window is.

#endif
