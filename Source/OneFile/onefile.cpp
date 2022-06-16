/*----------------------------------------------------------------------------
; onefile.cpp - Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in V  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  V document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       onefile.cpp
;
; Description:
;
; TO BE DONE:
;		- put the offset to each frame in an array following the header.
;		- non-niave compressed blit onto buffer.
;		- incorporate blit descriptor, with y-offset table.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 05/26/96  gas  Added one file compression.
;
----------------------------------------------------------------------------*/

#include "jpegio.h"
#include "mask.h"
#include "xfile.h"
#include "fcaseopen.h"

#include <assert.h>
#include <memory.h>     // for memcpy, etc, for debugging.
#include <stdio.h>
#include "one.h"
#include "platform.h"

void WarnUser( const LPSTR fmt, ... );

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void dumpPaletteEntries(PALETTEENTRY *e)
{
	int i;

	printf("  Red Green  Blue");
	for (i=0;i<256;i++)
	{
		printf("%3d: %5d %5d %5d", i, e[i].peRed, e[i].peGreen, e[i].peBlue);
	}		
}

void splitpath(const char *path, char * drive, char * dir, char * fname, char * ext)
{
    char *dirc, *basec, *bname, *dname;
    dirc = strdup(path);
    basec = strdup(path);
    dname = dirname(dirc);
    bname = basename(basec);
    
    if (drive)
        drive[0] = '\0';
    if (dir)
        dir[0] = '\0';
    if (fname)
        fname[0] = '\0';
    if (ext)
        ext[0] = '\0';
    if (dir)
        strcpy(dir, dname);
    char * dot = strrchr(bname, '.');

    if (dot) {
        *dot = '\0';
        dot++;
        if (ext)
            strcpy(ext, dot);
    }
    if (fname)
        strcpy(fname, bname);
    
    free(dirc);
    free(basec);
}

void makepath(char* Path,const char* Drive,const char* Directory, const char* File,const char* Extension)
{
    if (Path) {
        if (Drive)
            while(*Drive != '\0')
                *Path++ = *Drive++;
        if (Directory) {
            while(*Directory != '\0')
                *Path++ = *Directory++;
            *Path++ = '/';
        }
        if (File)
            while(*File != '\0')
                *Path++ = *File++;
        if (Extension) {
            *Path++ = '.';
            while(*Extension != '\0')
                *Path++ = *Extension++;
        }
        *Path = '\0';
    }
    return;
}

//----------------------------------------------------------------------------
void ONE::ConvertTo32bit(BYTE *raw_bits, PALETTEENTRY *_palette) {

  if (allocBits())
  {
	  BYTE *a = raw_bits;
	  PIXEL *b = m_bits;

	  if (sizeof(PIXEL) == 1) {
		  memcpy(m_bits, raw_bits, m_hdr.height*m_hdr.pitch);
	  } else {
	    RGBA8 rgbv;

	    PALETTEENTRY *palette = GetPalette(); // palette from image file
		if (!palette)
			palette = _palette; // some palette passed

	    for (int r=0; r<m_hdr.height; r++) {
	        for (int c=0; c<m_hdr.pitch; c++) {
				int palette_index = a[(r*m_hdr.pitch)+c];
	            PALETTEENTRY * p = &palette[palette_index];
	            rgbv.peRed = p->peRed;
	            rgbv.peGreen = p->peGreen;
	            rgbv.peBlue = p->peBlue;

				if (palette_index == m_hdr.colorkey) {
	            	rgbv.peAlpha = 0x00;
				} else {
	            	rgbv.peAlpha = 0xFF;
				}

	            b[(r*m_hdr.pitch)+c] = *(uint32_t*)(&rgbv);
        	}
	  	}
	  }
  }
  // free the old bits:
  free(raw_bits);


  // allocate a 4x buffer to convert from 8 bit palette to 32bit RGBA

  // copy buffer

  // free the old buffer

  // update the header
}



//----------------------------------------------------------------------------
static void oneUnCompress(
  unsigned int pitch,
  unsigned int width, 
  unsigned int height,
  unsigned char *source,
  unsigned char *target
)
{
  unsigned int ntrots;
  unsigned short count;
  unsigned int i;
  unsigned short x;
  unsigned short y;
  unsigned int offset = 0;
  unsigned int sz = pitch * height;

  // decompress the bits:
  ntrots = *((unsigned int *)source);
  source += sizeof(int);
  for (i=0; i<ntrots; i++)
  {
    x = *((unsigned short *)source);
    source += sizeof(short);
    y = *((unsigned short *)source);
    source += sizeof(short);
    count = *((unsigned short *)source);
    source += sizeof(short);
    offset = (y * pitch) + x;

    if (offset < sz) 
	{
		if (count & 0x8000)
		{
			count &= ~0x8000;
			memset(target+offset, *source, count);
			source += sizeof(BYTE);
		}
		else
		{
			memcpy(target+offset, source, count);
			source += count;
		}
	}
  }
}

//----------------------------------------------------------------------------
BYTE *ONE::UnCompress(BYTE  *byte_bits)
{
  int i;

  assert(IsCompressed());

  BYTE *old_bits = byte_bits;
  BYTE *new_bits = NULL;

  int frame_size = m_hdr.pitch*(m_hdr.height/m_hdr.frames);

  /* alloc bits: */
  new_bits = (BYTE*)malloc(m_hdr.height*m_hdr.pitch);
  if (new_bits)
  {
	memset(new_bits, m_hdr.colorkey, m_hdr.height*m_hdr.pitch);

	for (i=0;i<m_hdr.frames; i++)
	{
	  // printf("m_frame_offset[%d] = %d\n", i, m_frame_offset[i]);
	  oneUnCompress(
			m_hdr.pitch, m_hdr.width, m_hdr.height,
			old_bits+m_frame_offset[i], new_bits+(frame_size*i));
	  m_frame_offset[i] = i*frame_size;
	}

	// free the old bits:
	free(old_bits);

	m_hdr.m_flags &= ~ONE_IS_COMPRESSED;
	m_hdr.m_size = m_hdr.height*m_hdr.pitch;
  }
  return new_bits;
}

//----------------------------------------------------------------------------
ONE::ONE(PIXEL *_bits, int _width, int _height, int _pitch)
{
	INIT();

	m_bits = (PIXEL *)_bits;

	m_hdr.width = _width;
	m_hdr.height = _height;
	m_hdr.pitch = _pitch;
	m_hdr.m_size = m_hdr.pitch * m_hdr.height;
}

//----------------------------------------------------------------------------
ONE::~ONE()
{
  if (m_bits && (m_hdr.m_flags & ONE_OWNS_BITS))
	freeBits();

// TODO: why aren't we freeing the palette?
 // if (m_pal && (m_hdr.m_flags & ONE_OWNS_PAL))
//	free(m_pal);

  if (m_frame_offset)
	free(m_frame_offset);
}

//----------------------------------------------------------------------------
/*
*	There are magic numbers around the malloced block
*	so that I can test for blits that exceed the image bounds.
*/
static PIXEL *_allocBits(int height, int pitch) {
	int *magic;

	int size = height*pitch;
	PIXEL *bits = NULL;

	magic = (int*)malloc(sizeof(PIXEL)*size + sizeof(int)*2);
	assert(magic);

	if (!magic)
		return NULL;

	bits = (PIXEL*)(magic + 1);
	*magic = ONE_MAGIC;
	magic = (int*)(bits+size);
	*magic = ONE_MAGIC;

	return bits;
}

BOOL ONE::allocBits()
{
	m_bits = _allocBits(m_hdr.height,m_hdr.pitch);

	// I will need to free the bits.
	m_hdr.m_flags |= ONE_OWNS_BITS;

	return TRUE;
}

//----------------------------------------------------------------------------
// pass in a previously allocated one structure w/o bits.
// This routine will allocate memory for the bits.
BOOL ONE::ReadOne(const char *filename, PALETTEENTRY *default_palette)
{
	XFile xfile(filename);
	BOOL rc = false;
	int pc;
	BYTE *raw_bits = NULL;

	freeBits();

	xfile.SetExtension("one");

	/* Set the bits to null in case the open or read hdr fails */
	m_bits = NULL;

	/* Open file for input: */
	if(xfile.Open(XFile::modeRead))	{
	  /* Read in file header */
	  if( xfile.Read( &m_hdr, sizeof(ONE_HEADER) ) == sizeof(ONE_HEADER) ) {
		if (m_hdr.frames == 82) {
			printf("**** probably a bad file???  %s\n", filename);
			m_hdr.frames = 1;
		}
		if (m_hdr.frames == m_hdr.width) {
			printf("%s:%d frames(%d)==width(%d)- probably a bad file???  %s\n",
			          __FILE__, __LINE__, m_hdr.frames, m_hdr.width, filename);
			// m_hdr.frames = 1;
		}
		if (m_hdr.frames == 0) {
			printf("**** probably a bad file???  %s\n", filename);
			m_hdr.frames = 1;
		}
		if (IsCompressed())	{
		  m_frame_offset = (int32_t*)malloc(sizeof(int32_t)*m_hdr.frames);
		  xfile.Read(m_frame_offset, sizeof(int32_t)*m_hdr.frames);
		}

		/* alloc bits: */
		raw_bits = (BYTE*)malloc(m_hdr.m_size);
		assert(raw_bits);
		if (raw_bits) {
            size_t count = xfile.Read( raw_bits, m_hdr.m_size);
            rc = (count == m_hdr.m_size);
            if (count != m_hdr.m_size) {
                printf("%s:%d ERROR xfile.Read(\"%s\", %d) --> %zu\n", __FILE__, __LINE__, xfile.GetPath(), m_hdr.m_size, count);
            }
		}
	  }
	  if (m_hdr.m_flags & CUSTOM_PALETTE) {
		m_pal = (PALETTEENTRY*)malloc(sizeof(PALETTEENTRY)*256);
		if (m_pal) {
			pc = xfile.Read( m_pal, sizeof(PALETTEENTRY)*256);
			if (pc < sizeof(PALETTEENTRY)*256) {
				free(m_pal);
				m_pal = NULL;
			} else {
				m_hdr.m_flags |= ONE_OWNS_PAL;
				// dumpPaletteEntries(m_pal);
			}
		}
	  }
	}

	if (IsCompressed()) {
		raw_bits = UnCompress(raw_bits);
	}
	ConvertTo32bit(raw_bits, default_palette);
	return rc;
}

typedef struct {
	int height;
	int width;
	int pitch;
	int rows_read;
	PIXEL *bits;
} BASE_IMAGE;

int read_JPEG_callback(BASE_IMAGE *bim,
                         struct jpeg_decompress_struct *cinfo,
						 JSAMPROW buffer,
						 int rowstride,
						 const char *filename) {
    if (!bim->bits) {
		bim->rows_read = 0;
		bim->height = cinfo->image_height;
		bim->width  = cinfo->image_width;
		bim->pitch = (rowstride*4)/3;
		bim->bits = _allocBits(bim->height, bim->pitch);
	}

	PIXEL *bits = bim->bits + bim->rows_read * bim->pitch;
	for (int i=0; i<rowstride; i++) {
		int k=i*3;
		RGBA8 *pel = (RGBA8*)bits + i;
		pel->peRed   = buffer[k];
		pel->peGreen = buffer[k+1];
		pel->peBlue  = buffer[k+2];
		pel->peAlpha = 0;
	}

	bim->rows_read++;

	return 0;
}

//----------------------------------------------------------------------------
BOOL ONE::Read(const char *path, const char *filename, PALETTEENTRY *default_palette )
{
   char filepath[_MAX_FNAME];
   char ext[_MAX_EXT];

   printf("%s:%d:ONE::Read(\"%s\", \"%s\")\n", __FILE__, __LINE__, path, filename);

	if (!stricmp("flower9", filename)) {
		printf("the file you are looking for?\n");
	}
   // switch on extension for various file types:
   splitpath( filename, NULL, NULL, NULL, ext );

	{
		BASE_IMAGE bim;
		memset(&bim, 0, sizeof(bim));
		struct jpeg_decompress_struct cinfo;
    	makepath( filepath, NULL, path, filename, "JPG");
		do_read_JPEG_file(&cinfo, filepath, (fn_read_JPEG_callback)read_JPEG_callback, &bim);

		if (bim.bits) {
			// got something, turn it into a ONE structure ...
			m_hdr.height = bim.height;
			m_hdr.width = bim.width;
			m_hdr.pitch = bim.pitch;
			m_hdr.m_size = bim.height * bim.pitch;
			m_hdr.frames = 1;

			m_bits = bim.bits;
			//		free(bim.bits);
			printf("READ AS JPEG !!!!!!!!!\n");
			return TRUE;
		}
	}

	if (!stricmp("bigeye1", filename)) {
		printf("the file you are looking for?\n");
	}

   // build complete path the the file
   makepath( filepath, NULL, path, filename, NULL);

   BOOL rc =  ReadOne(filepath, default_palette);

   	if (!stricmp("bigeye1", filename)) {
		printf("Inspecting %s?\n", filename);
		printf("height:%d, width:%d, pitch:%d, size:%d, frames:%d",
				m_hdr.height, m_hdr.width, m_hdr.pitch, m_hdr.m_size, m_hdr.frames);
				m_hdr.width = m_hdr.pitch;
	}
	return rc;
}

//----------------------------------------------------------------------------
int ONE::Init(int width, int height)
{
	INIT(); // zero out structure.

	m_hdr.pitch = width;
	m_hdr.height = height;
	m_hdr.width = width;
	m_hdr.frames = 1; // at least one frame.

	m_hdr.m_size = m_hdr.height * m_hdr.pitch;

	return allocBits(); //m_hdr.m_size
}

//----------------------------------------------------------------------------
// Ensure that the image is not corrupt from blitting, etc
BOOL ONE::Validate()
{
	int *magic;

	// make sure there is something here:
	if (!m_bits)
		return FALSE;

	// majic numbers around the bits for bounds checking. 
	// check the bottom right:
	magic = (int*)(m_bits + m_hdr.m_size);
	if (*magic != ONE_MAGIC)
	{
		printf("Image file is corrupt");
		assert(*magic == ONE_MAGIC);
		return FALSE;
	}

	// check the top left:
	magic = (int*)m_bits;
	magic--;
	if (*magic != ONE_MAGIC)
	{
		printf("Image file is corrupt");
		assert(*magic == ONE_MAGIC);
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL ONE::freeBits()
{
	int *majic;

	if (m_bits == NULL) return false;
	if (!Validate()) return false;
	majic = (int*)m_bits;
	majic--; // Tis where'es freed at
	*majic = ONE_EVIL; // Set to evil bits so we can recognize a freed image
	free(majic);
	m_bits = NULL;
	m_hdr.m_flags = 0;
	return true;
}

//----------------------------------------------------------------------------
// util to free bits on behalf of a ONE method
BOOL ONE::freeBits(BYTE *bits)
{
	int *majic;

	majic = (int*)bits;
	majic--; // Tis where'es freed at
	*majic = ONE_EVIL; // Set to evil bits so we can recognize a freed image
	free(majic);
	return true;
}
