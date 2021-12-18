
#include <math.h>
#include "one.h"
#include "fblt.h"
#include "palette.h"

#define SCALEFACT 0x0000FFFF

//----------------------------------------------------------------------------
void FBLT_DSCR::lensInit(int height, int width)
{
	int x;
	double fx;
	double X;
	// The radius of a circle is C=2*pi*R
	// the image is rotated 90 deg, so height is width.
	double R = (height)/(2*3.1416);

	m_lens = (unsigned short*)malloc(width*sizeof(m_lens[0]));

	for (x=0;x<width; x++)
	{
		X = x - (width/2);
		fx = sin(acos(X/R));
		m_lens[x] = (unsigned short) (fx * SCALEFACT);
	}
}

//----------------------------------------------------------------------------
void FBLT_DSCR::lensInit(int height, int width, int effect)
{
	int x;
	double fx;
	double X;
	// The radius of a circle is C=2*pi*R
	// the image is rotated 90 deg, so height is width.
	double R = (height)/(2*3.1416);

	m_lens = (unsigned short*)malloc(width*sizeof(m_lens[0]));

	for (x=0;x<width; x++)
	{
		X = x - width/2;
		fx = cos(asin(X/R));
		//m_lens[x] = (unsigned short) (fx * SCALEFACT);
		m_lens[x] = SCALEFACT/4;
	}
}

//----------------------------------------------------------------------------
static void sliver_up(unsigned char *tgt,
                      unsigned char *src,
                      int th, // height
                      int tp, // pitch
                      unsigned short frac  )
{
    // note: center pixel of this sliver will be blitted by 'sliver'
    int32_t accum = 0;
    int32_t cf;
    int esi = th;
    src--;

    for (esi=th;esi;esi--) {
        *tgt = *src;
        // accumulate fractional part
        accum += frac;
        cf = (accum&0xFFFF0000)>>16;
        accum = (accum&0x0000FFFF);
        //src -= 0 + overflow from fractional part
        src -= cf;
        tgt -= tp;
    }
    // assert(false);
}

//----------------------------------------------------------------------------
// ### unroll the inner loops to make faster
static void sliver(unsigned char *tgt, unsigned char *src, int th, int tp, int accum_init, int frac_part  )
{
    // printf("TODO: fix this blitter: lens.cpp::sliver ******************\n");
    // assert(false);
    int si;
    int accum = 0; //accum_init;
	int frac = frac_part;
    int32_t cf;

    for (si=th;si;si--) {
        *tgt = *src;
        
        // accumulate fractional part
        accum += frac;
        cf = (accum&0xFFFF0000)>>16;
        accum = (accum&0x0000FFFF);

        src += cf;
        tgt += tp;
    }
}

//----------------------------------------------------------------------------
/*
Description:
	Used to render surround VIEWS to the display memory.

	this routine expects the source image to be Flipped Diagonally Left

	The blit is done in 2 parts to prevent vertical shearing from appearing due
	to the lens being shifted down by odd/even amounts. Instead,  the lens is
	shifted outwards from the center.
*/
void FBLT_DSCR::LensPortal(ONE *oneDest,ONE *oneSrc)
{
	int x, dx;
    unsigned char * bDest;
	unsigned char * bSrc;

	// Speeds access to the vector.
	MASK *mask;
	PLENS lens;

	int upLength;
	int downLength;
	int biFocal, biFocalSrc, biFocalDest; // Where the lens seam sits on src or dest.

	int accum_init; // to compensate for bifocal seam in lens.

	int src_y; // used to compensate for seam in source

	int32_t dst_pitch = oneDest->GetPitch();
	int32_t src_pitch = oneSrc->GetPitch();
	int32_t src_height = oneSrc->GetHeight();

	// The lens is cut in half and blitted outwards from the center
	// to prevent vertical artifacts from appearing.  biFocal is a
	// horizontal line along the center of the view mask. NOTE: The portal 
	// is flipped and rotated,  but the bounding rectangle is not.
	biFocalSrc = ((m_brect.bottom - m_brect.top)/2);
	biFocal = (biFocalSrc + m_brect.top);
	biFocalDest = biFocal * dst_pitch;

	dx = m_mask[0].length;

	// The lens shifts the upper half of the image by half the lens.
	// This is compensated by adjusting the lens applied to the lower
	// half.  The value from the vertical center of the lens is used
	// so that I don't recompute throughout the outer loop.
	accum_init = biFocalSrc * m_lens[dx/2];

	src_y = m_src_x;
	bSrc  = oneSrc->getBits()  
		+ m_src_y
		+ (src_y * src_pitch)
		+biFocalSrc;

	bDest = oneDest->getBits()
		+ m_dst_x + m_brect.left
		+ ((m_dst_y) * dst_pitch)
		+ biFocalDest;

	// make assembly listing of the outer loop easier to read by moving init
	// outside the loop:
	lens = m_lens;
	mask=&m_mask[1]; // speed access to the vector.
	x=0;
	for (; x<dx; x++, mask++, lens++)
	{
		// Compensate for seam in source image.  This is easy since we are blitting
		// in vertical slivers.
		if (src_y >= src_height)
		{
			src_y = 0;
			bSrc  = oneSrc->getBits()  + m_src_y + biFocalSrc;
		}

		// Each entry in the portal mask describes a vertical sliver.
		upLength = biFocal - mask->x;
		downLength = mask->length - upLength;

		if (upLength > 0)
			sliver_up(bDest-dst_pitch, bSrc, upLength, dst_pitch, *lens);

		if (downLength > 0)
			sliver(bDest, bSrc, downLength, dst_pitch, accum_init, *lens);

		bDest++;
		src_y++;
		bSrc += src_pitch;
	}
}

//----------------------------------------------------------------------------
static void sliver_R90Fh(unsigned char *tgt, unsigned char *src, int len, int bpsl)
{
	// Msg("sliver_R90Fh");
    // TODO: fix this blitter
    //assert(false);

	for (int si=len; si; si--) {
		*tgt = *src++;
		tgt+= bpsl;
	}
}

//----------------------------------------------------------------------------
static void sliver_memset(unsigned char *tgt, unsigned char color, int len, int bpsl)
{
    // TODO: fix this blitter
    //assert(false);

	for (int si=len;si;si--) {
		*tgt = color;
		tgt+= bpsl;
	}
}

//----------------------------------------------------------------------------
// used to blit a sprite onto a surround view canvas (surface).
// The sprite is rotated and flipped as it is blitted onto the canvas.
void FBLT_DSCR::BltFlip(ONE *oneDest, ONE *oneSrc)
{
	int i, y, x, dx;
	int length;
	int init_x, src_y;
	int dest_x;

    unsigned char * bDest;
	unsigned char * bSrc;

	int dst_pitch = oneDest->GetPitch();
	int src_pitch = oneSrc->GetPitch();
	int dst_width = oneDest->GetWidth();
	int src_width = oneSrc->GetWidth();
	int dst_height = oneDest->GetHeight();
	int src_height = oneSrc->GetHeight();

	// check for valid parms:
	if ((m_src_y >= src_height)||(m_src_x >= src_width)
	 || (m_dst_x >= dst_height)||(m_dst_y >= dst_width)) // rotated.
	  return;

	src_y = m_src_y;
	bSrc  = oneSrc->getBits()  
		+ m_src_x 
		+ (src_y * src_pitch);

	init_x = m_dst_x;
	bDest = oneDest->getBits()
		+ m_dst_y
		+ (init_x * dst_pitch);	// rotated.

	dx = m_mask[0].length;
	for (i=1; i<=dx; i++)
	{
		length = m_mask[i].length;
		x = m_mask[i].x;
		y = m_mask[i].y;

		src_y = y * src_pitch;

		dest_x = x * dst_pitch;

		// clip to the bottom of the surface:
		if ((y+m_dst_y) >= dst_width)
		{
			continue;
		}

		// wrap around the seam of the target bitmap.
		if ((init_x + x + length) > dst_height)
		{
			// the whole thing is on the other side:
			if ((init_x + x) >= dst_height)
			{
				dest_x -= oneDest->getSize();
				sliver_R90Fh(bDest+dest_x+y, bSrc+src_y+x, length, dst_pitch);
			}
			// the line stradles the seam:
			else
			{
				int wrap = (init_x + x + length) - dst_height;
				sliver_R90Fh(bDest+dest_x+y, bSrc+src_y+x, length-wrap, dst_pitch);
				dest_x += (length-wrap) * dst_pitch;
				dest_x -= oneDest->getSize();
				src_y += (length-wrap);
				sliver_R90Fh(bDest+dest_x+y, bSrc+src_y+x, wrap, dst_pitch);
			}
		}
		else
			sliver_R90Fh(bDest+dest_x+y, bSrc+src_y+x, length, dst_pitch);
	}
}

