/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
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
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/

#include <fcntl.h>      /* Needed only for _O_RDONLY definition */
//#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include <wtypes.h>
#include <one.h>

void WarnUser( LPSTR fmt, ... );

int ONE::Write(char * filename)
{
	int fh;
	int rc = 0;

	/* Open file for output: */
	if( (fh = _open( filename, _O_RDWR | _O_BINARY | _O_CREAT | _O_TRUNC, 
                               _S_IREAD | _S_IWRITE )) != -1 )
	{
		if( _write( fh, &m_hdr, sizeof( ONE_HEADER )) == sizeof(ONE_HEADER) )
		{
			rc = _write( fh, m_bits, m_hdr.m_size);
		}
		_close( fh );
	}
	return rc;
}

ONE::ONE(void *_bits, int _width, int _height, int _pitch)
{
	INIT();

	m_bits = (unsigned char *)_bits;
	m_hdr.width = _width;
	m_hdr.height = _height;
	m_hdr.pitch = _pitch;
	m_hdr.m_size = m_hdr.pitch * m_hdr.height;
}

ONE::~ONE()
{
	if (m_bits && (m_hdr.m_flags & ONE_OWNS_BITS))
		freeBits();

	if (m_pal && (m_hdr.m_flags & ONE_OWNS_PAL))
		free(m_pal);
}

int oneWriteBMP(char *filename, unsigned char *bits, int width, int height, int pitch)
{
	int fh;
	int rc = 0;
	int i;

	/* Open file for output: */
	if( (fh = _open( filename, _O_RDWR | _O_BINARY | _O_CREAT | _O_TRUNC, 
                               _S_IREAD | _S_IWRITE )) != -1 )
	{
		ONE one(bits, width, height, pitch);
		if( _write( fh, &one.m_hdr, sizeof( ONE_HEADER )) == sizeof(ONE_HEADER) )
		{
			bits = one.getBits()+one.getSize();
			for (i=0;i<one.m_hdr.height; i++)
			{
				bits-=one.m_hdr.pitch;
				rc = _write( fh, bits, one.GetPitch());
			}
		}
		_close( fh );
	}
	return rc;
}

int oneWriteCustomBMP(char *filename, unsigned char *bits, int width, int height, int pitch, PALETTEENTRY *pal)
{
	int fh;
	int rc = 0;
	int i;

	/* Open file for output: */
	if( (fh = _open( filename, _O_RDWR | _O_BINARY | _O_CREAT | _O_TRUNC, 
                               _S_IREAD | _S_IWRITE )) != -1 )
	{
		ONE one(bits, width, height, pitch);
		if (pal)
		{
			one.SetCustomPalette(pal);
		}

		if( _write( fh, &one.m_hdr, sizeof( ONE_HEADER )) == sizeof(ONE_HEADER) )
		{
			bits = one.getBits()+one.getSize();
			for (i=0;i<one.GetHeight(); i++)
			{
				bits-=one.GetPitch();
				rc = _write( fh, bits, one.GetPitch());
			}
		}

		if (pal)
		{
			rc = _write( fh, pal, sizeof(PALETTEENTRY) * 256);
		}

		_close( fh );
	}
	return rc;
}

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

// pass in a previously allocated one structure w/o bits.
// This routine will allocate memory for the bits.
int ONE::Read(char *filename)
{
	int fh;
	int rc = FALSE;
	int pc;

	/* Set the bits to null in case the open or read hdr fails */
	m_bits = NULL;

	/* Open file for input: */
	if( (fh = _open( filename, _O_RDONLY | _O_BINARY )) != -1 )
	{
		/* Read in file header */
		if( _read( fh, &m_hdr, sizeof(ONE_HEADER) ) == sizeof(ONE_HEADER) )
		{
			/* alloc bits: */
			if (allocBits(m_hdr.m_size))
			{
				rc = _read( fh, m_bits, m_hdr.m_size );
			}
		}
		if (m_hdr.m_flags & CUSTOM_PALETTE)
		{
			m_pal = (PALETTEENTRY*)malloc(sizeof(PALETTEENTRY)*256);
			if (m_pal)
			{
				pc = _read( fh, m_pal, sizeof(PALETTEENTRY)*256);
				if (pc < sizeof(PALETTEENTRY)*256)
				{
					free(m_pal);
					m_pal = NULL;
				}
				else
					m_hdr.m_flags |= ONE_OWNS_PAL;
			}
		}
		_close( fh );
	}
	else
	{
		WarnUser("Unable to open: %s", filename);
	}
	return rc;
}

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

int ONE::Init(int _width, int _height)
{
	INIT(); // zero out structure.

	m_hdr.pitch = _width;
	m_hdr.height = _height;
	m_hdr.width = _width;

	m_hdr.m_size = m_hdr.height * m_hdr.pitch;

	return allocBits(m_hdr.m_size);
}

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

BOOL ONE::freeBits()
{
	int *majic;

	if (!Validate())
		return FALSE;

	majic = (int*)m_bits;
	majic--; // Tis where'es freed at
	*majic = ONE_EVIL; // Set to evil bits so we can recognize a freed image

	free(majic);
	m_bits == NULL;
	m_hdr.m_flags = 0;

	return TRUE;
}

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
				rc = _read( fh, *vector+1, sizeof(MASK)*header.length);
			}
		}
		_close( fh );
	}
	else
	{
		WarnUser("Unable to open: %s", lpszPathName);
	}
	return rc;
}


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

int maskWrite(const char *lpszPathName, MASK *vector)
{
	int fh;
	int rc = 0;

	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];

	_splitpath( lpszPathName, drive, dir, fname, NULL );
	_makepath( path_buffer, drive, dir, fname, "vct");

	/* Open file for output: */
	if( (fh = _open( path_buffer, _O_RDWR | _O_BINARY | _O_CREAT | _O_TRUNC, 
                               _S_IREAD | _S_IWRITE )) != -1 )
	{
		rc = _write( fh, vector, sizeof(MASK)*(vector[0].length + 1));
		_close( fh );
	}

	return rc;	
}

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

