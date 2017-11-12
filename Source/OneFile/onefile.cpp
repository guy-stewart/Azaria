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

#include <XFile.h>

#include <assert.h>
#include <memory.h>     // for memcpy, etc, for debugging.

#include <one.h>

void WarnUser( LPSTR fmt, ... );

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
  _write(ofh, &x, sizeof(short)); 
  _write(ofh, &y, sizeof(short));
  _write(ofh, &wlen, sizeof(short));  // note hi-bit is clear.
  _write(ofh, wi, (long) wlen);
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
  _write(ofh, &x, sizeof(short));
  _write(ofh, &y, sizeof(short));
  _write(ofh, &rlen, sizeof(short));  // note hi-bit is set.
  _write(ofh, &pel, sizeof(BYTE));
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
  unsigned int startx,
  unsigned int starty,
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
  BYTE *old_bits = m_bits;
  int frame_size = m_hdr.pitch*(m_hdr.height/m_hdr.frames);

  /* alloc bits: */
  if (allocBits(m_hdr.height*m_hdr.pitch))
  {
	memset(m_bits, m_hdr.colorkey, m_hdr.height*m_hdr.pitch);

	for (i=0;i<m_hdr.frames; i++)
	{
	  oneUnCompress(
			0,0,
			m_hdr.pitch, m_hdr.width, m_hdr.height,
			old_bits+m_frame_offset[i], m_bits+(frame_size*i));
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

  if (m_pal && (m_hdr.m_flags & ONE_OWNS_PAL))
	free(m_pal);

  if (m_frame_offset)
	free(m_frame_offset);

  if (m_DirtyRows)
	free(m_DirtyRows);
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
*	There are majic numbers around the malloced block
*	so that I can test for blits that exceed the image bounds.
*/
BOOL ONE::allocBits(int size)
{
	int *majic;

	majic = (int*)malloc(size + sizeof(int)*2);
	assert(majic);

	if (!majic)
		return FALSE;

	m_bits = (unsigned char*)(majic + 1);
	*majic = ONE_MAJIC;
	majic = (int*)(m_bits+size);
	*majic = ONE_MAJIC;

	// I will need to free the bits.
	m_hdr.m_flags |= ONE_OWNS_BITS;

	return TRUE;
}

//----------------------------------------------------------------------------
// pass in a previously allocated one structure w/o bits.
// This routine will allocate memory for the bits.
int ONE::Read(char *filename)
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
		if (IsCompressed())
		{
		  m_frame_offset = (long*)malloc(sizeof(long)*m_hdr.frames);
		  xfile.Read(m_frame_offset, sizeof(long)*m_hdr.frames);
		}

		/* alloc bits: */
		if (allocBits(m_hdr.m_size))
		{
		  rc = (xfile.Read( m_bits, m_hdr.m_size ) == m_hdr.m_size);
		}
	  }
	  if (m_hdr.m_flags & CUSTOM_PALETTE)
	  {
		m_pal = (PALETTEENTRY*)malloc(sizeof(PALETTEENTRY)*256);
		if (m_pal)
		{
			pc = xfile.Read( m_pal, sizeof(PALETTEENTRY)*256);
			if (pc < sizeof(PALETTEENTRY)*256)
			{
				free(m_pal);
				m_pal = NULL;
			}
			else
				m_hdr.m_flags |= ONE_OWNS_PAL;
		}
	  }
	}

	return rc;
}

//----------------------------------------------------------------------------
int ONE::Read(char *path, char *filename)
{
   char filepath[_MAX_FNAME];
   char ext[_MAX_EXT];

   // switch on extension for various file types:
   _splitpath( filename, NULL, NULL, NULL, ext );


   // build complete path the the file
   _makepath( filepath, NULL, path, filename, NULL);

   return Read(filepath);
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
void ONE::DoTheDirty()
{
  // long for every 2^5 rows.  (32 bits per long). 
  // 1 extra long for the fiddly bits at the end.
  m_DirtyRows = (long*)calloc(1+(m_hdr.height>>5), sizeof(long));
}

//----------------------------------------------------------------------------
void ONE::SetClean()
{
  if (m_Dirty && m_DirtyRows)
	memset(m_DirtyRows,0,(1+(m_hdr.height>>5)) * sizeof(long));

  m_Dirty = FALSE;
  m_AllDirty = FALSE;
}

//----------------------------------------------------------------------------
void ONE::SetAllDirty()
{
  m_AllDirty = TRUE;
}

//----------------------------------------------------------------------------
// Ensure that the image is not corrupt from blitting, etc
BOOL ONE::Validate()
{
	int *majic;

	// make sure there is something here:
	if (!m_bits)
		return FALSE;

	// majic numbers around the bits for bounds checking. 
	// check the bottom right:
	majic = (int*)(m_bits + m_hdr.m_size);
	if (*majic != ONE_MAJIC)
	{
		WarnUser("Image file is corrupt");
		assert(*majic == ONE_MAJIC);
		return FALSE;
	}

	// check the top left:
	majic = (int*)m_bits;
	majic--;
	if (*majic != ONE_MAJIC)
	{
		WarnUser("Image file is corrupt");
		assert(*majic == ONE_MAJIC);
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
	m_bits == NULL;
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

//----------------------------------------------------------------------------
int maskRead(const char *path, const char *filename, MASK **vector)
{
	int fh;
	int rc = 0;
	MASK header;
	char lpszPathName[_MAX_PATH];

	// build complete path the the file
	_makepath( lpszPathName, NULL, path, filename, NULL);

	/* Open file for input: */
	if( (fh = _open( lpszPathName, _O_RDONLY )) != -1 )
	{
		/* Read in size of the mask */
		if( _read( fh, &header, sizeof(MASK) ) == sizeof(MASK) )
		{
			/* allocate memory for the mask */
			*vector = (MASK*)malloc((header.length+1) * sizeof(MASK));
			if (vector)
			{
				/* copy the header */
				*vector[0] = header;

				/* Read the rest of the mask */
				// ### For osme reason,  the masks are not the correct length according to header.length???
				rc = _read( fh, *vector+1, sizeof(MASK)*header.length);
				rc = rc > 0;
			}
		}
		_close( fh );
	}

	return rc;
}

//----------------------------------------------------------------------------
MASK * maskCreate(unsigned char *x_bits, int h, int w, int pitch, unsigned char ck)
{
	int runs; // how many runs in the image.
	int index;
	MASK *mask;
	int x, y;
	int top=0, left=0;

	unsigned char *lm_bits;
	
	runs = 0;
	lm_bits = x_bits;
	// figure out the size of the mask:
	for (y=0; y<h; y++)
	{
		x=0;
		while (x<w)
		{
			/* find start of run (or end of row) */
			while (x<w && (*lm_bits==ck))
			{
				lm_bits++;
				x++; 
			}
			if (x<w) // found a run
			{
				while (x<w && (*lm_bits!=ck))
				{ 
					lm_bits++;
					x++;
				}  /* find end of run */
				runs++;
			}
		}

		while (x<pitch)
		{
			lm_bits++;
			x++;
		}
	}

	mask = (MASK*)malloc(sizeof(MASK)*(runs+1));
	if (!mask)
		return 0;

	lm_bits = x_bits;
	// Now do it for real:
	index = 1;
	for (y=0; y<h; y++)
	{
		x=0;
		while (x<w)
		{
			/* find start of run (or end of row) */
			while (x<w && (*lm_bits==ck)) 
			{ 
				x++;
				lm_bits++;
			}
			if (x<w) // found a run
			{
				if (y<top)	top = y;
				if (x<left)	left=x;
				mask[index].y = y;
				mask[index].x = x;
				while (x<w && (*lm_bits!=ck))  /* find end of run */
				{ 
					x++;
					lm_bits++;
				}
				mask[index].length = x - mask[index].x;
				index++;
			}
		}

		while (x<pitch)
		{
			lm_bits++;
			x++;
		}
	}

	mask[0].length = runs;
	mask[0].x = left;
	mask[0].y = top;
	return mask;
}

//----------------------------------------------------------------------------
/*  returns true if the vector could be freed */
int maskFree(MASK **vector)
{
	if (vector && *vector)
		free(*vector);
	else
		return FALSE;
	*vector = NULL;
	return TRUE;
}

//----------------------------------------------------------------------------
int maskWrite(const char *lpszPathName, MASK *vector)
{
	XFile xfile(lpszPathName);
	int rc = 0;

	xfile.SetExtension("vct");

	/* Open file for output: */
	if(xfile.Open(XFile::modeWrite))
	{
		rc = xfile.Write(vector, sizeof(MASK)*(vector[0].length + 1));
	}

	return rc;	
}

//----------------------------------------------------------------------------
void maskGetBoundingRect(MASK *mask, RECT *rect)
{
	int i, x, y, length;

	/* keep track of dirty rectangle */
	rect->left = mask[1].x;
	rect->right = mask[1].x + mask[1].length;
	rect->top = mask[1].y;
	rect->bottom = mask[1].y;

	for (i=1; i<=mask[0].length; i++)
	{
		length = mask[i].length;
		x = mask[i].x;
		y = mask[i].y;

		/* keep track of dirty rectangle */
		if (x < rect->left) rect->left = x;
		else if ((x+length) > rect->right) rect->right = x+length;
		if (y < rect->top) rect->top = y;
		else if (y > rect->bottom) rect->bottom = y;
	}

}

//----------------------------------------------------------------------------
// ### This is certainly the least efficient method of doing a hit test.
// This function gets called every time the mouse moves.  It would
// be nice if I could create an 'inner bounding box' that could be tested
// before doing all this work:

BOOL maskPtInRect(MASK *mask, POINT p)
{
  int i, x;

  for (i=1; i<=mask[0].length; i++)
  {
	x = mask[i].x;
	if ((p.y == mask[i].y) && (p.x > x) && (p.x < x+mask[i].length))
	  return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
void maskMove(MASK *mask, int x_delta, int y_delta)
{
  int i;

  for (i=1; i<=mask[0].length; i++)
  {
	mask[i].x += x_delta;
	mask[i].y += y_delta;
  }
}
