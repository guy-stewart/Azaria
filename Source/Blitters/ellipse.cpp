/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       ellipse.cpp
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "../../include/stdapp.h"
#include "../../include/one.h"
#include "../../include/fblt.h"

#include <math.h>

//----------------------------------------------------------------------------
#if 0

#else
//----------------------------------------------------------------------------
void FBLT_DSCR::fbltPortal(
				ONE *oneDest,
				ONE *oneSrc) // uses src_x, src_y
{
	FBLT_DSCR * fbd = this;

	int i, x, y, dest_y, src_y;
	int src_x;
    PIXEL * bDest;
	PIXEL * bSrc;
	int length, wrap;
	MASK *mask = fbd->m_mask;
	int dst_pitch = oneDest->GetPitch();
	int src_pitch = oneSrc->GetPitch();
	int dst_height = oneDest->GetHeight();
	int src_height = oneSrc->GetHeight();
	int dst_width = oneDest->GetWidth();

	bDest = oneDest->getBits()
		 + fbd->m_dst_x
		 + (fbd->m_dst_y * dst_pitch);  // offset +/- vertically

	// set the source x in case the blit is offset:
	if (fbd->isSprite())
		src_x = fbd->m_src_x;
	else
	{
		src_x = fbd->m_src_x;
		// line up the source with the bounding rectangle, don't over-wrap!
		src_x -= fbd->m_brect.left;
		if (src_x > src_pitch)
				src_x -= src_pitch;
	}
    //unsigned char *source = src->m_bits + src->m_frame_offset[m_frame];

	bSrc = oneSrc->getBits()
		 + src_x        // the source can be offset +/- horizontally
		 + (fbd->m_src_y * src_pitch);  // offset +/- vertically
/*
	if (isFramed()) {
		printf("\n%s:%d:working here:\n", __FILE__, __LINE__);
		printf("m_frame:%d, m_dy = %d, src_height:%d\n", m_frame, m_dy, src_height);
		printf("frames:%d\n", oneSrc->GetFrameCount());
		printf("src_height/frames:%d\n", src_height/oneSrc->GetFrameCount());
		printf("mask[0].length = %d\n", mask[0].length);
	}
*/

	// for (i=1; i<=mask[0].length; i++)
	for (i=1; i<=mask[0].length; i++)
	{
		length = mask[i].length;
		x = mask[i].x;
		y = mask[i].y;

		dest_y = y * dst_pitch;

		// clip to bottom of destination
		if (y >= dst_height - fbd->m_dst_y)
			break;

		// clip to right side of destination
		if ((fbd->m_dst_x + x + length) >= dst_width)
		{
			length = dst_width - (fbd->m_dst_x + x);
			if (length <=0)
				continue;
		}

		if (fbd->isSprite())
		{
			// use standard positioning:
			src_y = y * src_pitch;
		}
		else
		{
			// compensate for portal offset into screen:
			src_y = (y-mask[1].y) * src_pitch;
		}

		/* the source is wrapping ... */
		if (x > (src_pitch - src_x))
		{
		  FBLT_DSCR::aligned_memcpy(bDest+dest_y+x, bSrc+src_y+x-src_pitch, length);
		}
		else if ((x + length) > (src_pitch - src_x))
		{
			wrap = (src_pitch - src_x) - x;
			FBLT_DSCR::aligned_memcpy(bDest+dest_y+x, bSrc+src_y+x, wrap);
			FBLT_DSCR::aligned_memcpy(bDest+dest_y+x+wrap, bSrc+src_y+x+wrap-src_pitch, length-wrap);
		}
		else
			FBLT_DSCR::aligned_memcpy(bDest+dest_y+x, bSrc+src_y+x, length);
	}
}
#endif
//----------------------------------------------------------------------------
void FBLT_DSCR::BltRectangle(ONE *oneDest,ONE *oneSrc)
{
	int y;
	int dy;
	int dx; // used to clip to right side of source and target
	int src_y;
	int dst_y;
    PIXEL * bDest;
	PIXEL * bSrc;
	int dst_pitch = oneDest->GetPitch();
	int src_pitch = oneSrc->GetPitch();
	int dst_height = oneDest->GetHeight();
	int src_height = oneSrc->GetHeight();
	int src_width = oneSrc->GetWidth();
	int dst_width = oneDest->GetWidth();

	dst_y = m_dst_y;
	src_y = m_src_y;
    //unsigned char *source = src->m_bits+src->m_frame_offset[m_frame];

	dx = m_dx;

	// check for valid parms:
	if ((src_y >= src_height)||(m_src_x >= src_width)
	 || (dst_y >= dst_height)||(m_dst_x >= dst_width))
	  return;

	// clip to right side of destination
	if (dx + m_src_x > src_width)
		dx = src_width - m_src_x;

	// clip to right side of source
	if (dx + m_dst_x > dst_width)
		dx = dst_width - m_dst_x;

	bSrc  = oneSrc->getBits()  + m_src_x + (src_y * src_pitch);
	bDest = oneDest->getBits() + m_dst_x + (dst_y * dst_pitch);

	dy = m_dy;
	if (m_style == BLT_RECTFLIP)
	{
		for (y=0; y<dy; y++)
		{
			// wrap around seam in source:
			if (y >= src_height - src_y)
			{
				src_y = 0;
				bSrc = oneSrc->getBits() + m_src_x;
			}
			// wrap around seam in destination
			if (y >= dst_height - dst_y)
			{
				dst_y = 0;
				bDest = oneDest->getBits() + m_dst_x;
			}

			aligned_memcpy(bDest, bSrc, dx);
			bSrc += src_pitch;
			bDest += dst_pitch;
		}
	}
	else
	{
		// clip to bottom of source:
		if ((dy) >= (src_height - src_y))
			dy = (src_height - src_y);

		// clip to bottom of destination:
		if (dy >= (dst_height - dst_y))
			dy = (dst_height - dst_y);

		for (y=0; y<dy; y++)
		{
			aligned_memcpy(bDest, bSrc, dx);
			bSrc += src_pitch;
			bDest += dst_pitch;
		}
	}
}

//----------------------------------------------------------------------------
void FBLT_DSCR::Blt(ONE *oneDst, ONE *oneSrc)
{
	// protect ourselves from misframed items:
	if (m_frame >= oneSrc->GetFrameCount()) {
		m_frame = 0;
		// printf("%s:%d:TODO: misframed item detected.\n", __FILE__, __LINE__);
	}

	// automatically pick out the frame and set *fbd correctly
	if (isFramed())
	{
		m_src_y = m_dy * m_frame;
		if (m_masks) {
			m_mask = m_masks->mask(m_frame);
		}
	}

    if (oneSrc == NULL || oneDst == NULL)
        return;

		switch (m_style)
		{
		case BLT_LENSED:
			if (m_mask) {
				LensPortal(oneDst, oneSrc);
			}
			break;
		case BLT_FLIPPED:
			BltFlip(oneDst, oneSrc);
			break;
		case BLT_PORTAL:
			fbltPortal(oneDst, oneSrc);
			break;
		case BLT_RECTFLIP:
		case BLT_RECT:
		default:
			BltRectangle(oneDst, oneSrc);
			break;
		}
}

//----------------------------------------------------------------------------
/*
 *  DDoneBlt
 *
 *  copy bits into a DirectDrawSurface
 *
 */
HRESULT DDoneBlt(RECT *rect, IDirectDrawSurface *pdds, ONE *oneSrc, FBLT_DSCR *fbd)
{
    HRESULT     ddrval;

    DDSURFACEDESC       ddsd;

    if (oneSrc == NULL || pdds == NULL)
        return E_FAIL;

    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    ddrval = pdds->GetSurfaceDesc(&ddsd);

	// ### This called frequently has to wait for the surface, esp when in
	// non-exclusive mode.
    ddrval = pdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

	if (ddrval == DD_OK)
	{
		ONE oneDst((PIXEL*)ddsd.lpSurface, ddsd.dwWidth, ddsd.dwHeight, ddsd.lPitch);

		try
		{
			fbd->Blt(&oneDst, oneSrc);
		}
		catch(...)
		{
			pdds->Unlock(NULL);
			errorCriticalStop("exception in oneDDBlt");
		}
		ddrval = pdds->Unlock(NULL);
    }

    return ddrval;
}

//----------------------------------------------------------------------------
HRESULT oneDDBlt(RECT *rect, ONE *oneDst, IDirectDrawSurface *pdds, FBLT_DSCR *fbd)
{
    HRESULT     ddrval;

    DDSURFACEDESC ddsd;

    if (oneDst == NULL || pdds == NULL)
        return E_FAIL;

    //
    // get size of surface.
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    pdds->GetSurfaceDesc(&ddsd);

    ddrval = pdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

	if (ddrval == DD_OK)
	{
		ONE oneSrc((PIXEL*)ddsd.lpSurface, ddsd.dwWidth, ddsd.dwHeight, ddsd.lPitch);

		try
		{
			fbd->Blt(oneDst, &oneSrc);
		}
		catch(...)
		{
			pdds->Unlock(NULL);
			errorCriticalStop("exception in oneDDBlt");
		}
		pdds->Unlock(NULL);
    }

    return ddrval;
}

