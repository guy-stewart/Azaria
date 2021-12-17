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

//#include <ddraw.h>
#include "one.h"
#include "fblt.h"
#include "spirit.h"

//----------------------------------------------------------------------------
FBLT_DSCR::FBLT_DSCR()
{
  ZeroInit();
}

//----------------------------------------------------------------------------
FBLT_DSCR::~FBLT_DSCR()
{
	if (m_mask)
		MASK::Free(&m_mask);
	if (m_lens)
	  free(m_lens);
}


//----------------------------------------------------------------------------
void FBLT_DSCR::ZeroInit()
{
  m_mask = NULL;
  m_lens = NULL;

  m_frame = 0;
  m_etime = 0;
  m_flags = 0;
  m_dst_x = 0;
  m_dst_y = 0;
  m_src_x = 0;
  m_src_y = 0;
  m_dx = 0;
  m_dy = 0;
}

//----------------------------------------------------------------------------
void FBLT_DSCR::SpriteInit(ONE *sprite)
{
  setSprite();

  m_dx = sprite->GetWidth();
  m_dy = sprite->GetHeight();

  // If it a multi-framed image,  then assume they want to use it as such.
  if (sprite->GetFrameCount() > 1)
  {
	m_dy = sprite->GetHeight()/sprite->GetFrameCount();
	setFramed();

	// if they set a frame rate in the one file:
	m_mpf = 10000 / sprite->GetFP10S();
  }
}

//----------------------------------------------------------------------------
void FBLT_DSCR::MaskInit(ONE *sprite)
{
  // free any prior created mask:
  if (m_mask)
		MASK::Free(&m_mask);
  m_mask = NULL;

  // can't create a mask for a compressed sprite.
  if (sprite->IsCompressed())
	return;

  m_masks = MASK::Create(
		sprite->getBits(),
		sprite->GetFrameCount(),
		sprite->GetHeight(),
		sprite->GetWidth(),
		sprite->GetPitch(),
		sprite->getBits()[0]
		);

  m_mask = m_masks->mask(0);
  m_mask->GetBoundingRect(&m_brect);
}

//----------------------------------------------------------------------------
/*
  blit_compressed(
			m_dst_x,m_dst_y,
			dest->m_hdr.pitch, dest->m_hdr.width, dest->m_hdr.height,
			src->m_bits+src->m_frame_offset[m_frame], dest->m_bits);
*/
void FBLT_DSCR::BltCompressed(ONE *dest, ONE *src)
{
  int clipped_length, length;
  unsigned int run_count; // number of runs and walks in the image
  unsigned short count;   // number of pels in the current run/walk
  unsigned int i;
  unsigned short x;
  unsigned short y;
  unsigned int offset = 0;
  unsigned int sz = dest->m_hdr.pitch * dest->m_hdr.height;

  unsigned char *source = src->m_bits+src->m_frame_offset[m_frame];
  unsigned char *target = dest->m_bits;

  int this_pitch;
  m_font_pitch = 0;

  // decompress the bits:
  run_count = *((unsigned int *)source);
  source += sizeof(int);
  for (i=0; i<run_count; i++)
  {
    x = m_dst_x + *((unsigned short *)source);
    source += sizeof(short);
    y = m_dst_y + *((unsigned short *)source);
    source += sizeof(short);
    count = *((unsigned short *)source);
    source += sizeof(short);
    offset = (y * dest->m_hdr.pitch) + x;

	// highbit of count declares whether it is a walk or run:
	clipped_length = length = count & ~0x8000;

	// compute font pitch:
	this_pitch = x-m_dst_x + length;
	if (m_font_pitch < this_pitch)
	  m_font_pitch = this_pitch;

	// clip to bottom of destination:
	if (y >= dest->m_hdr.height)
	  break;

	// clip to right side:
	if (x+length >= dest->m_hdr.pitch)
	{
	  // the whole thing is off the page:
	  if (x >= dest->m_hdr.pitch)
	  {
		if (count & 0x8000)
			source += sizeof(BYTE);
		else
			source += length;
		continue;
	  }
	  // shorten up the lenght of the line:
	  clipped_length = dest->m_hdr.pitch - x;
	}

	{
		if (count & 0x8000)
		{
			aligned_memset(target+offset, *source, clipped_length);
			source += sizeof(BYTE);
		}
		else
		{
			aligned_memcpy(target+offset, source, clipped_length);
			source += length;
		}
	}
  }
}

void FBLT_DSCR::BltFont(ONE *dest, ONE *src)
{
  int clipped_length, length;
  unsigned int run_count; // number of runs and walks in the data
  unsigned short count;
  unsigned int i;
  unsigned short x;
  unsigned short y;
  unsigned int offset = 0;

  unsigned char *source = src->m_bits+src->m_frame_offset[m_frame];
  unsigned char *target = dest->m_bits;

  int this_pitch;
  m_font_pitch = 0;

  // decompress the bits:
  run_count = *((unsigned int *)source);
  source += sizeof(int);
  for (i=0; i<run_count; i++)
  {
    x = m_dst_x + *((unsigned short *)source);
    source += sizeof(short);
    y = m_dst_y + *((unsigned short *)source);
    source += sizeof(short);
    count = *((unsigned short *)source);
    source += sizeof(short);
    offset = (y * dest->m_hdr.pitch) + x;

	// highbit of count declares whether it is a walk or run:
	clipped_length = length = count & ~0x8000;

	// compute font pitch:
	this_pitch = x-m_dst_x + length;
	if (m_font_pitch < this_pitch)
	  m_font_pitch = this_pitch;

	// clip to bottom of destination:
	if (y >= dest->m_hdr.height)
	  break;

	// clip to right side:
	if (x+length >= dest->m_hdr.pitch)
	{
	  // the whole thing is off the page:
	  if (x >= dest->m_hdr.pitch)
	  {
		if (count & 0x8000)
			source += sizeof(BYTE);
		else
			source += length;
		continue;
	  }
	  // shorten up the lenght of the line:
	  clipped_length = dest->m_hdr.pitch - x;
	}

	{
		if (count & 0x8000)
		{
			aligned_memset(target+offset, m_font_color, clipped_length);
			source += sizeof(BYTE);
		}
		else
		{
			aligned_memset(target+offset, m_font_color, clipped_length);
			source += length;
		}
	}
  }
}

//----------------------------------------------------------------------------
// print a message at x,y on the given canvas:
int FBLT_DSCR::Print(ONE *canvas, ONE *font, const char *msg, int color)
{
  int i;
  int x = m_dst_x; // save
  int total_pitch;
  int left, right;

  m_font_color = color;

  for (i=0;msg[i];i++)
  {
	m_frame = msg[i] - ' ';

	if (m_frame<0 || m_frame>font->m_hdr.frames)
	  m_frame = ' ';
	
	//  printf("%s:%d:CFONT TODO ...\n", __FILE__, __LINE__);

	GetCharBound(&left,&right,font);
	if (m_dst_x > left-1)
	  m_dst_x -= left-1;

	BltFont(canvas, font);
	// Blt(canvas, font);

	switch (m_frame) {
	case 0: // space
	  m_dst_x += font->GetWidth()/4;
	  break;
	default:
	  m_dst_x += right;
	  break;
	}
  }

  // restore
  total_pitch = m_dst_x - x;
  m_dst_x = x;

  return total_pitch;
}


//----------------------------------------------------------------------------
// print a message at x,y on the given canvas:
int FBLT_DSCR::XPrint(ONE *canvas, SPIRIT *xfont, const char *msg, int color)
{
  int i;
  int x = m_dst_x; // save
  int total_pitch;
  int left, right;
  RECT rect;

  m_font_color = color;

  for (i=0;msg[i];i++)
  {
	m_frame = msg[i] - ' ';

	if (m_frame<0 || m_frame> 96) // TODO: restore this test font->m_hdr.frames)
	  m_frame = ' ';
	
	GetBoundingRect(&rect, m_frame);
	if (m_dst_x > rect.left-1)
	  m_dst_x -= rect.left-1;

	xfont->Move(m_dst_x, m_dst_y, 0);
	xfont->SetFrame(m_frame);
	xfont->ON();

	switch (m_frame) {
	case 0: // space
	  m_dst_x += xfont->GetWidth()/4;
	  break;
	default:
	  m_dst_x += rect.right;
	  break;
	}
  }

  // restore
  total_pitch = m_dst_x - x;
  m_dst_x = x;

  return total_pitch;
}

//----------------------------------------------------------------------------
void FBLT_DSCR::SetFramesPerSecond(int fps)
{
  m_mpf = 1000/fps;
}

//----------------------------------------------------------------------------
BOOL FBLT_DSCR::NextFrame(ONE *sprite, int etime, BOOL bLoop)
{
  m_etime -= etime;
  if (m_etime <= 0)
  {
	m_etime = m_mpf;

    m_frame++;

	if (m_frame >= sprite->GetFrameCount())
	{
	  if (bLoop)
		m_frame = 0;
	  else
	  {
		m_frame--;
		return FALSE; // no more frames
	  }
	}
  }

  return TRUE;
}

//----------------------------------------------------------------------------
BOOL FBLT_DSCR::PrevFrame(ONE *sprite, int etime, BOOL bLoop)
{
  m_etime -= etime;
  if (m_etime <= 0)
  {
	m_etime = m_mpf;

	if (m_frame == 0)
	{
	  if (bLoop)
		m_frame = sprite->GetFrameCount();
	  else
	  {
		return FALSE; // no more frames
	  }
	}
	else
	  m_frame--;
  }

  return TRUE;
}

//----------------------------------------------------------------------------
void FBLT_DSCR::BonesOn(int x, int y)
{
	assert(false);
	m_src_y = 0;
	m_src_x = 0;
	switch (m_style)
	{
	case BLT_RECTFLIP:
		m_dst_x = y;
		m_dst_y = x;
		break;
	default:
		m_dst_x = x;
		m_dst_y = y;
	}
}

//----------------------------------------------------------------------------
void FBLT_DSCR::BonesOff(int x, int y)
{
	assert(false);
	m_dst_x = 0;
	m_dst_y = 0;
	{
		switch (m_style)
		{
		case BLT_RECTFLIP:
			m_src_y = x;
			m_src_x = y;
			break;
		default:
			m_src_x = x;
			m_src_y = y;
		}
	}
}

//----------------------------------------------------------------------------
void FBLT_DSCR::GetCharBound(int *left, int *right, ONE *font)
{
  int length;
  unsigned int run_count; // number of runs and walks in the data
  unsigned short count;
  unsigned int i;
  unsigned short x;
  unsigned short y;
    // UNUSED: unsigned int offset = 0;

  unsigned char *source = font->m_bits+font->m_frame_offset[m_frame];

  *left = font->GetWidth();
  *right = 0;

  // decompress the bits:
  run_count = *((unsigned int *)source);
  source += sizeof(int);
  for (i=0; i<run_count; i++)
  {
    x = *((unsigned short *)source);
    source += sizeof(short);
    y = *((unsigned short *)source);
    source += sizeof(short);
    count = *((unsigned short *)source);
    source += sizeof(short);

	// highbit of count declares whether it is a walk or run:
	length = count & ~0x8000;

	// compute font pitch:
	if (x<*left)
	  *left = x;

	if ((x+length)>*right)
	  *right = x+length;

	if (count & 0x8000)
		source += sizeof(BYTE);
	else
		source += length;
  }

  // happens in the case of a space ' '
  if (*left>*right)
	*left=*right;
}
