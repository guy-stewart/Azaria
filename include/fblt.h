/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       fblt.h
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/23/95  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifndef _fblt_h
#define _fblt_h
#include "mask.h"
#include "ddraw.h"
#include "ptypes.h"
#include "palette.h"

typedef unsigned short *PLENS;	/* an array of scale factors */

#define BLT_RECT		0x01	// blit a rectangle
#define BLT_RECTFLIP	0x02	//
#define BLT_PORTAL		0x03
#define BLT_LENSED		0x04
#define BLT_LENSPORT	0x05
#define BLT_FLIPPED		0x06

#define FLG_SPRITE		0x01 // blit as a sprite onto a canvas.
#define FLG_VIEW		0x02 // blit as a view canvas.
#define FLG_FRAMED		0x04 // blitter should pick a specific frame from the image.

class SPIRIT;

class FBLT_DSCR {
private:
	short m_flags;
	BYTE m_font_color;
	BYTE m_font_pitch;	// pitch of last character printed.
public:
	short m_frame;		// current frame of animation
	int m_etime;		// milliseconds remaining for current frame
	int m_mpf;			// milliseconds per frame
	short m_style;		/* uses rect or portal for target clipping */
	MASKS *m_masks;
	MASK  *m_mask;       // clipping region
	RECT  m_brect;			/* rectangle clipping region,  or portal bounding box */
	PLENS m_lens;			/* optional lens used for rendering */
	int m_dst_x, m_dst_y;	/* target offset */
	int m_src_x, m_src_y;	/* source offset */
	int m_dx, m_dy;

	enum {
		DONTLOOP = 0,
		LOOP = 1
	};

public:
    FBLT_DSCR();
    ~FBLT_DSCR();

    void ZeroInit();

	// initialize the blit descriptor as appropriate for the
	// ONE sprite asssuming it is to be blitted onto a ONE canvas.
    void SpriteInit(ONE *sprite);
    void MaskInit(ONE *sprite);

    BOOL NextFrame(ONE *sprite, int etime, BOOL bLoop);
    BOOL PrevFrame(ONE *sprite, int etime, BOOL bLoop);
    void SetFramesPerSecond(int fps);
    WORD GetFrame() {return m_frame;}

    void lensInit(int height, int width);
    void lensInit(int height, int width, int effect);

    void BonesOn(int x, int y);
    void BonesOff(int x, int y);

    void Blt(ONE *dst, ONE *hbm);
    void BltCompressed(ONE *dest, ONE *src);
    void BltCompressedFlip(ONE *dest, ONE *src);
    void BltFont(ONE *dest, ONE *src);
    void BltFlip(ONE *oneDest, ONE *oneSrc);
    void BltRectangle(ONE *oneDest,ONE *oneSrc);
	void fbltPortal(ONE *oneDest,ONE *oneSrc);

    void LensPortal(ONE *oneDest,ONE *oneSrc);

    int GetFontPitch() {return m_font_pitch;}
    void GetCharBound(int *left, int *right, ONE *src);
	void GetXCharBounds(int *left, int *right);
	void GetBoundingRect(RECT *rect, int frame) {m_masks->mask(frame)->GetBoundingRect(rect);};

	//----------------------------------------------------------------------------
	// print a message at x,y on the given canvas:
    int Print(ONE *canvas, ONE *font, const char *msg, int color);
    int XPrint(ONE *canvas, SPIRIT *xfont, const char *msg, int color);

	static inline void aligned_memcpy( UCHAR *dst, const UCHAR *src, size_t count )
	{
        // TODO: implement this blitter
        memcpy(dst, src, count);
	}

	static inline void aligned_memset( void *dst, int color, size_t count )
	{
        // TODO: implement this blitter
        memset(dst, color, count);
	}


public: // This stuff is not Scottish. (if it ain't Scottish, it's crap)
	inline int isSprite() {return m_flags & FLG_SPRITE;};
	inline void setSprite() {m_flags |= FLG_SPRITE;};

	inline int isView() {return m_flags & FLG_VIEW;};
	inline void setView() {m_flags |= FLG_VIEW;};

	inline int isFramed() {return m_flags & FLG_FRAMED;};
	inline void setFramed() {m_flags |= FLG_FRAMED;};
};

#define restore_debug "ck.>=m!"
extern HRESULT DDoneBlt(RECT *rect, IDirectDrawSurface *pdds, ONE *one, FBLT_DSCR *fbd);
extern HRESULT HBoneBlt(RECT *rect, HBITMAP dst, ONE *one, FBLT_DSCR *fbd);
extern HRESULT oneDDBlt(RECT *rect, ONE *oneDst, IDirectDrawSurface *pdds, FBLT_DSCR *fbd);
extern HRESULT oneHBBlt(RECT *rect, ONE *one, HBITMAP hbm, FBLT_DSCR *fbd);
#endif
