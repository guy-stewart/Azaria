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

#include "mask.h"
#include "xfile.h"

#include <assert.h>
#include <memory.h>     // for memcpy, etc, for debugging.

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

	Msg("  Red Green  Blue");
	for (i=0;i<256;i++)
	{
		Msg("%3d: %5d %5d %5d", i, e[i].peRed, e[i].peGreen, e[i].peBlue);
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
static void write_walk(
  int ofh, 
  unsigned short x, 
  unsigned short y, 
  unsigned short wlen, 
  unsigned char *wi
)
{
  if (!wlen) return;
  write(ofh, &x, sizeof(short));
  write(ofh, &y, sizeof(short));
  write(ofh, &wlen, sizeof(short));  // note hi-bit is clear.
  write(ofh, wi, (long) wlen);
}

//----------------------------------------------------------------------------
static void write_run(
  int ofh, 
  unsigned short x, 
  unsigned short y, 
  unsigned short rlen, 
  BYTE pel
)
{
  if (!rlen) return;
  rlen |= 0x8000;                   // set the hi-bit, indicating a run.
  write(ofh, &x, sizeof(short));
  write(ofh, &y, sizeof(short));
  write(ofh, &rlen, sizeof(short));  // note hi-bit is set.
  write(ofh, &pel, sizeof(BYTE));
}

//----------------------------------------------------------------------------
void ONE::ConvertTo32bit() {

  // allocate a 4x buffer to convert from 8 bit palette to 32bit RGBA

  // copy buffer

  // free the old buffer

  // update the header
}

//----------------------------------------------------------------------------
void ONE::CompressRow(
  int ofh, 
  unsigned char *bits,
  unsigned char xcolor,
  unsigned int height,
  unsigned int y,
  unsigned int width, 
  unsigned int *nbytes,
  unsigned int *ntrots
)
{
  unsigned short wlen;     // for scanning.
  unsigned char *wi;       // write index - start of next write.
  unsigned char *la;       // look ahead pointer.
  unsigned short x;        // coordinate in the row.

  // run stuff:
  BYTE run_color;			// set to color of run,  if it is a run.
  BOOL isa_run;				// set TRUE if it is a run
  int new_run_len;

  // skip over any leading transparencies.
  wi = bits;     // keep track of which pels to write.
  la = bits;     // start examining the image from the beginning of the row.
  x = 0;         // remember where we are in the row.    

SKIP_XCOLOR:
  while (width && (*la == xcolor))
  {
    wi++;        // not going to write this pel.
    la++;        // look ahead one.
    x++;         // remember where we are in the row.    
    width--;     // one less pel to worry about.
  }
  
  // hit the end of the row w/ out finding (another) trot?
  if (!width) return;

  // must learn to walk before learning to run. start out walking ...
  wlen = 0;      // empty walk.
  run_color = *la;	// lets see if we have a run
  isa_run = TRUE;	// be optimystic

  // walk until end of row, or xparency is detected.
  while (width && (*la != xcolor))
  {

#define MIN_RUN_LEN 12 // seems to be a good number, in general

	if (isa_run)
	{
	  if (run_color != *la)
	  {
		if (wlen >= MIN_RUN_LEN)
		  break;	// write the run, come back later to work on the next set.
		else
		{
		  run_color = *la;  // try to find an appended run
		  new_run_len = 0;
		  isa_run = FALSE;
		}
	  }
	}
	else	  // try to pick a run out of a waltz:
	{
	  if (run_color != *la)
	  {
		run_color = *la;  // try to find an appended run
		new_run_len = 0;
	  }
	  else
	  {
		new_run_len++;

	    if (new_run_len >= MIN_RUN_LEN)
		{
		  // write the walk, come back later to work on the run:
		  wlen -= new_run_len;
		  la -= new_run_len;
		  width += new_run_len;
	  	  break;
		}
	  }
	}

	wlen++;
	la++;
	width--;
  }

  // must have hit end of row, or an xparency.
  if (wlen)
  {
	  if (isa_run)
	  {
		write_run(ofh, x, y, wlen, run_color);
		*nbytes += (3 * sizeof(short)) + sizeof(BYTE);
	  }
	  else
	  {
		write_walk(ofh, x, y, wlen, wi);
		*nbytes += (3 * sizeof(short)) + wlen;
	  }

	x += wlen;
    wi += wlen;
    (*ntrots)++;
  }

  // hit the end of row?
  if (!width) return;   

  // handled the run, or walk ended with a transparency.
  goto SKIP_XCOLOR;
}

//----------------------------------------------------------------------------
// Writes a compressed image frame to the file handle ofh.
// It starts wherever the current position of the file happens to be, 
// and leaves the current file position at the start of the frame.
int ONE::CompressFrame(
		XFile *xfile,
		unsigned char *bits,
		unsigned int height)
{
  unsigned int ntrots;     // the number of walks and runs found.
  unsigned short y;          // cartesion coordinate within the dib.
  long fpos;               // file position where number of walks/runs must be stored.
  unsigned int size = 0;

  // accounting.
  ntrots = 0;  

  // remember where in the file the walk/run count must go. (where it is now).
  fpos = xfile->GetPosition();
  xfile->Write(&ntrots, sizeof(long));  // store a dummy place holder.

  // next cruise through the dib adding walks and runs to the output file.
  for (y=0; y<height; y++)
    CompressRow(xfile->GetHandle(), bits+(y*m_hdr.pitch), *bits, height, y, m_hdr.width, &size, &ntrots);

  // store the walk/run count.
  xfile->Seek(fpos, XFile::Begin);
  xfile->Write(&ntrots, sizeof(long));

  // account for storage of ntrots in the image bits:
  size += sizeof(long);

  return size;
}

//----------------------------------------------------------------------------
void ONE::Compress(XFile *xfile)
{
  int i;
  long fpos;
  long size=0;
  int frame_height = m_hdr.height/m_hdr.frames;
  int frame_size = frame_height * m_hdr.pitch;

  // reserve space following the header for the frame offsets:
  fpos = xfile->GetPosition();
  xfile->Write(m_bits, sizeof(long)*m_hdr.frames);

  for (i=0;i<m_hdr.frames;i++)
  {
	// write the offset of the frame into the bits:
	xfile->Seek(fpos+(sizeof(long)*i), XFile::Begin);
	xfile->Write(&size, sizeof(long));

	// add the next frame to the end of the file:
	xfile->SeekToEnd();
	size += CompressFrame(xfile, m_bits+(frame_size*i), frame_height);
  }

  // put the size into the header and re-write the header.
  m_hdr.m_size = size;
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
BOOL ONE::UnCompress()
{
  int i;

  if (!IsCompressed()) {
	  return TRUE;
  }
  BYTE *old_bits = m_bits;
  int frame_size = m_hdr.pitch*(m_hdr.height/m_hdr.frames);

  /* alloc bits: */
  if (allocBits(m_hdr.height*m_hdr.pitch))
  {
	memset(m_bits, m_hdr.colorkey, m_hdr.height*m_hdr.pitch);

	for (i=0;i<m_hdr.frames; i++)
	{
	  // printf("m_frame_offset[%d] = %d\n", i, m_frame_offset[i]);
	  oneUnCompress(
			m_hdr.pitch, m_hdr.width, m_hdr.height,
			old_bits+m_frame_offset[i], m_bits+(frame_size*i));
	  m_frame_offset[i] = i*frame_size;
	}

	// free the old bits:
	freeBits(old_bits);

	m_hdr.m_flags &= ~ONE_IS_COMPRESSED;
	m_hdr.m_size = m_hdr.height*m_hdr.pitch;
	return TRUE;
  }
  else
	return FALSE;
}

//----------------------------------------------------------------------------
int ONE::Write(const char *filename, BOOL bCompressed, BOOL fromBMP, int frames, int FP10S)
{
  XFile xfile(filename);

  int rc = 0;
  long i;
  unsigned char *bits;

  xfile.SetExtension("one");

  if (!frames)	  // must be at least 1 frame.
	frames = 1;
  if (!FP10S)	  // must be at least 1 frame per second.
	FP10S = 1;

  m_hdr.frames = frames;
  m_hdr.fp10s = FP10S;

  if (bCompressed)
  {
	// user wants to write it compressed:
	m_hdr.m_flags |= ONE_IS_COMPRESSED;

	if (!xfile.Open(XFile::modeWrite))
    {
      WarnUser("Unable to open: %s", filename);
      return FALSE;
    }

	if (xfile.Write(&m_hdr, sizeof(ONE_HEADER)) == sizeof(ONE_HEADER))
	{
		m_hdr.colorkey = *m_bits;

		// compress all the frames into xfile:
		Compress(&xfile);

		xfile.SeekToBegin();
		xfile.Write(&m_hdr, sizeof(ONE_HEADER)) == sizeof(ONE_HEADER);
	}
    return rc;
  }
  else
  {
    // its uncompressed.
    if (!xfile.Open(XFile::modeWrite))
    {
      // add fancy-schmancy high-falootin' error handling here.
      WarnUser("Unable to open: %s", filename);
      return FALSE;
    }

    if (xfile.Write(&m_hdr, sizeof(ONE_HEADER)) == sizeof(ONE_HEADER))
	{
      if (fromBMP)
      {
			  bits = m_bits + m_hdr.m_size;
			  for (i=0; i<m_hdr.height; i++)
			  {
				  bits -= m_hdr.pitch;
				  rc = xfile.Write(bits, m_hdr.pitch);
			  }
      }
      else
        rc = xfile.Write(m_bits, m_hdr.m_size);
	}
	return rc;
  }
}

//----------------------------------------------------------------------------
ONE::ONE(void *_bits, int _width, int _height, int _pitch)
{
	INIT();

	m_bits = (unsigned char *)_bits;

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
int oneWriteBMP(const char *filename, unsigned char *bits, BOOL bCompressed, int width, int height, int pitch, int frames, int FP10S)
{
	ONE one(bits, width, height, pitch);
	one.Write(filename, bCompressed, TRUE, frames, FP10S);
	return 0;
}

//----------------------------------------------------------------------------
int oneWriteCustomBMP(const char *filename, unsigned char *bits, int width, int height, int pitch, PALETTEENTRY *pal)
{
	XFile xfile(filename);
	int rc = 0;
	int i;

	xfile.SetExtension("one");

	/* Open file for output: */
	if(xfile.Open(XFile::modeWrite))
	{
		ONE one(bits, width, height, pitch);
		if (pal)
		{
			one.SetCustomPalette(pal);
		}

		if( xfile.Write(&one.m_hdr, sizeof( ONE_HEADER )) == sizeof(ONE_HEADER) )
		{
			bits = one.getBits()+one.getSize();
			for (i=0;i<one.GetHeight(); i++)
			{
				bits-=one.GetPitch();
				rc = xfile.Write(bits, one.GetPitch());
			}
		}

		if (pal)
		{
			rc = xfile.Write(pal, sizeof(PALETTEENTRY) * 256);
		}
	}
	return rc;
}

//----------------------------------------------------------------------------
/*
*	There are magic numbers around the malloced block
*	so that I can test for blits that exceed the image bounds.
*/
BOOL ONE::allocBits(int size)
{
	int *magic;

	magic = (int*)malloc(size + sizeof(int)*2);
	assert(magic);

	if (!magic)
		return FALSE;

	m_bits = (unsigned char*)(magic + 1);
	*magic = ONE_MAGIC;
	magic = (int*)(m_bits+size);
	*magic = ONE_MAGIC;

	// I will need to free the bits.
	m_hdr.m_flags |= ONE_OWNS_BITS;

	return TRUE;
}


//----------------------------------------------------------------------------
// pass in a previously allocated one structure w/o bits.
// This routine will allocate memory for the bits.
int ONE::Read(const char *filename)
{
	XFile xfile(filename);
	int rc = FALSE;
	int pc;

	xfile.SetExtension("one");

	/* Set the bits to null in case the open or read hdr fails */
	m_bits = NULL;

	/* Open file for input: */
	if(xfile.Open(XFile::modeRead))
	{
	  /* Read in file header */
	  if( xfile.Read( &m_hdr, sizeof(ONE_HEADER) ) == sizeof(ONE_HEADER) )
	  {
		// if compressed, then read the offsets array:
		printf("width:%d, height:%d, pitch:%d, frames:%d\n", m_hdr.width, m_hdr.height, m_hdr.pitch, m_hdr.frames);
		if (m_hdr.frames == 82) {
			printf("**** probably a bad file???  %s\n", filename);
			m_hdr.frames = 1;
		}
		if (IsCompressed())
		{
		  printf("IsCompressed()\n");
		  m_frame_offset = (int32_t*)malloc(sizeof(int32_t)*m_hdr.frames);
		  xfile.Read(m_frame_offset, sizeof(int32_t)*m_hdr.frames);
		}

		/* alloc bits: */
		if (allocBits(m_hdr.m_size))
		{
            size_t count = xfile.Read( m_bits, m_hdr.m_size);
            rc = (count == m_hdr.m_size);
            if (count != m_hdr.m_size) {
                printf("xfile.Read(\"%s\", %d) --> %zu\n", xfile.GetPath(), m_hdr.m_size, count);
            }
		}
	  }
	  if (m_hdr.m_flags & CUSTOM_PALETTE)
	  {
		printf("CUSTOM_PALETTE\n");
		m_pal = (PALETTEENTRY*)malloc(sizeof(PALETTEENTRY)*256);
		if (m_pal)
		{
			pc = xfile.Read( m_pal, sizeof(PALETTEENTRY)*256);
			if (pc < sizeof(PALETTEENTRY)*256)
			{
				free(m_pal);
				m_pal = NULL;
			}
			else {
				m_hdr.m_flags |= ONE_OWNS_PAL;
				// dumpPaletteEntries(m_pal);
			}
		}
	  }
	}

	if (IsCompressed()) {
		UnCompress();
	}
	return rc;
}

//----------------------------------------------------------------------------
int ONE::Read(const char *path, const char *filename)
{
   char filepath[_MAX_FNAME];
   char ext[_MAX_EXT];

   printf("%s:%d:ONE::Read(\"%s\", \"%s\")\n", __FILE__, __LINE__, path, filename);
    
   // switch on extension for various file types:
   splitpath( filename, NULL, NULL, NULL, ext );

   // build complete path the the file
   makepath( filepath, NULL, path, filename, NULL);

   int rc =  Read(filepath);
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

	return allocBits(m_hdr.m_size);
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
		WarnUser("Image file is corrupt");
		assert(*magic == ONE_MAGIC);
		return FALSE;
	}

	// check the top left:
	magic = (int*)m_bits;
	magic--;
	if (*magic != ONE_MAGIC)
	{
		WarnUser("Image file is corrupt");
		assert(*magic == ONE_MAGIC);
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL ONE::freeBits()
{
	int *majic;

	if (!Validate()) return FALSE;
	majic = (int*)m_bits;
	majic--; // Tis where'es freed at
	*majic = ONE_EVIL; // Set to evil bits so we can recognize a freed image
	free(majic);
	m_bits = NULL;
	m_hdr.m_flags = 0;
	return TRUE;
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
	return TRUE;
}
