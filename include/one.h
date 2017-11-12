/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in V  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  V document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       one.h
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/23/95  gas  Created.
; 05/26/96  gas  Added one file compression.
; 
----------------------------------------------------------------------------*/
#include "ptypes.h"
#include "xfile.h"

class FBLT_DSCR;

// majic numbers are used to test for out-of-bounds writes to images.
#define ONE_MAJIC	0xA55A
#define ONE_EVIL	0xCEFA

// The one destructor will only attempt to free the bits or pal
// if they were allocated by a ONE constructor.
#define CUSTOM_PALETTE		0x01
#define ONE_OWNS_BITS		0x02
#define ONE_OWNS_PAL		0x04
#define ONE_IS_COMPRESSED	0x08

//----------------------------------------------------------------------------
// DO NOT CHANGE HEADER DATA ELEMENTS. FIXED FILE I/O USED.
typedef struct {
public:
	long pitch;
	long width;
	long height;
public:
	unsigned char version;		// Read/Written to/from file
	unsigned char m_flags;		// Private flags for use by ONE methods
	unsigned char frames;		// number of frames for animation
	unsigned char fp10s;		// frames per 10 seconds. (250 = 25 fps)
	unsigned char colorkey;		// transparency color used in compression.

private:
	unsigned char reserved_for_future_use_0;
	unsigned short reserved_for_future_use_1;
public:
	unsigned int m_size;
	WORD reserved_for_future_use_2;
	WORD user_flags;
} ONE_HEADER;

//----------------------------------------------------------------------------
// all images used in the game are referenced through class ONE:
class ONE {
  friend FBLT_DSCR;

	// data elements.
public:
	ONE_HEADER m_hdr;
	PALETTEENTRY *m_pal;
	unsigned char *m_bits;
private:
	long *m_frame_offset;	// array of offsets into an animation sequence.
	BOOL m_AllDirty;		// the entire image is dirty.
	BOOL m_Dirty;			// Just specific rows are dirty.
	long *m_DirtyRows;		// bitwise list of dirty rows.

public: // constructors, destructors.
	void INIT() {
		memset(&m_hdr, 0, sizeof(ONE_HEADER));

		m_Dirty = FALSE;
		m_DirtyRows = NULL;

  		m_frame_offset = NULL;
		m_pal=NULL;
		m_bits=0;
	};

	ONE(int width, int height) {Init(width, height);}
	ONE(void *bits, int _width, int _height, int _pitch);
	ONE() {INIT();}
	~ONE();

private:
	BOOL allocBits(int size);
	static BOOL freeBits(BYTE *bits);
public:
	BOOL freeBits();

public:
	inline unsigned char * getBits() { return m_bits;}
	inline unsigned int getSize() { return m_hdr.m_size;}
	inline long GetWidth() {return m_hdr.width;}
	inline long GetPitch() {return m_hdr.pitch;}
	inline long GetHeight() {return m_hdr.height;}
	inline BYTE GetFrames() {return m_hdr.frames;}
	inline BYTE GetFP10S() {return (m_hdr.fp10s)?m_hdr.fp10s:100;}

	void SetCustomPalette(PALETTEENTRY *pal) {m_hdr.m_flags|=CUSTOM_PALETTE; m_pal = pal;}
	inline PALETTEENTRY *GetPalette() {return (m_hdr.m_flags&CUSTOM_PALETTE)?m_pal:NULL;}

	inline void Clear(int color) {memset(m_bits, color, m_hdr.m_size);}
	// tools for compressed images:

	BOOL IsCompressed() {return m_hdr.m_flags & ONE_IS_COMPRESSED;}
	BOOL UnCompress();
    void Compress(XFile *xfile);

	void DoTheDirty();
	void SetClean();
	void SetAllDirty();

private:
    int CompressFrame(
				XFile *xfile,
				unsigned char *bits,
				unsigned int height
				);
    void CompressRow(
				int ofh, 
				unsigned char *bits,
				unsigned char xcolor,
				unsigned int height,
				unsigned int y,
				unsigned int width, 
				unsigned int *nbytes,
				unsigned int *ntrots
				);

public:
	// Handy for testing images for corruption after blitting onto.
	BOOL Validate();
	int Init(int width, int height);
	int Read(const char * filename);
	int Read(const char *path, const char *filename);
	int ReadSprite(const char *path, const char *filename);
	int Write(const char * lpszPathName, BOOL bCompressed, BOOL fromBMP, int frames, int FP10S);
};

//----------------------------------------------------------------------------
int oneWriteBMP(const char *filename, unsigned char *bits, BOOL bCompressed, int width, int height, int pitch, int frames, int FP10S);
int oneWriteCustomBMP(const char *filename, unsigned char *bits, int width, int height, int pitch, PALETTEENTRY *pal);

//----------------------------------------------------------------------------
typedef struct {
public:
	int x;
	int y;
	int length;
} MASK;

MASK * maskCreate(unsigned char *bits, int h, int w, int pitch, unsigned char ck);

int maskWrite(const char *lpszPathName, MASK *vector);
extern int maskRead(const char *path, const char * lpszPathName, MASK **vector);
int maskFree(MASK **vector);
void maskGetBoundingRect(MASK *pvect, RECT *rect);
BOOL maskPtInRect(MASK *pvect, POINT p);
void maskMove(MASK *mask, int x_delta, int y_delta);

